#pragma once
#include"median_selection.h"
#include<numeric>

namespace Standard::Algorithms::Numbers::Inner
{
    template<class long_int_t, class data_t>
    [[nodiscard]] constexpr auto compute_total_cost(const data_t &data) -> long_int_t
    {
        static_assert(sizeof(decltype(data[0].second)) <= sizeof(long_int_t));

        require_positive(data.size(), "data size");

        constexpr long_int_t zero{};

        auto summer = [] [[nodiscard]] (const long_int_t &presum, const auto &item)
        {
            const auto &item_cost = require_positive(item.second, "item cost");
            auto sum = static_cast<long_int_t>(presum + item_cost);
            require_greater(sum, presum, "partial sum");

            assert(!(presum < zero) && !(sum < item_cost));

            return sum;
        };

        auto total_cost = std::accumulate(data.cbegin(), data.cend(), zero, summer);

        require_positive(total_cost, "total cost");

        return total_cost;
    }
} // namespace Standard::Algorithms::Numbers::Inner

namespace Standard::Algorithms::Numbers
{
    // todo(p4): partial_sort_copy


    // Given a non-empty bag of pairs with unique keys and positive costs (weights),
    // the weighted median is a pair such that:
    // 1) (sum of costs of items with keys LESS than median key) <= TC/2.
    // 2) (sum of costs of items with keys GREATER than median key) <= TC/2.
    // Where TC = total cost of all items.
    // O(n) time using binary prune search.
    // The long_int_t must be large enough not to cause an overflow.
    template<class long_int_t, class data_t, class item_t = typename data_t::value_type,
        std::size_t extent = std::dynamic_extent>
    [[nodiscard]] constexpr auto weighted_median_selection(data_t &data) -> item_t &
    {
        // Given {L, m, G}, each > 0.
        // Let TC = L + m + G.
        // W.median conditions: (2*L <= TC) and (2*G <= TC)
        //   (2*L <= L + m + G) and (2*G <= L + m + G)
        //   (L <= m + G) and (G <= L + m)
        //   (L - G <= m) and (G - L <= m)
        // can be replaced with one condition: (|L - G| <= m).
        static_assert(sizeof(decltype(data[0].second)) <= sizeof(long_int_t));

        constexpr long_int_t zero{};
        constexpr long_int_t two = 2;

        auto total_cost = Inner::compute_total_cost<long_int_t, data_t>(data);

        const auto key_less = [] [[nodiscard]] (const item_t &aaa, const item_t &bbb)
        {
            return aaa.first < bbb.first;
        };

        using less_t = decltype(key_less);

        auto *first = &data[0];
        auto *last_incl = &data.back();

        for (;;)
        {
            assert(!(last_incl < first) && zero < total_cost);

            if (first == last_incl)
            {
                return *first;
            }

            const auto left_size = static_cast<std::size_t>(last_incl - first + 1);
            const auto statistic_order = left_size >> 1U;

            std::span<item_t> spa(first, left_size);

            auto &plain_median =
                median_selection<item_t, less_t, extent>::select_order_statistic(spa, statistic_order, key_less);

            assert(!(&plain_median < first) && !(last_incl < &plain_median));

            long_int_t less1{};
            std::size_t lesser_size{};

            for (auto *index = first; index <= last_incl; ++index)
            {// todo(p3): join with smaller_to_beginning, greater_to_ending?
                if (index->first < plain_median.first)
                {
                    const auto &item_cost = index->second;
                    less1 += item_cost;
                    ++lesser_size;

                    assert(zero < item_cost && zero < less1 && 0U < lesser_size);
                }
            }

            const auto median_index = static_cast<std::size_t>(&plain_median - first);
            const auto stop_inclusive = static_cast<std::size_t>(last_incl - first);

            if (const auto two_less = less1 * two; total_cost < two_less) // Discard all greater or equal.
            {
                assert(zero < less1 && 0U < lesser_size && first + lesser_size <= last_incl);

                std::span<item_t, extent> spa_rta(first, stop_inclusive + 1U);

                Inner::smaller_to_beginning<item_t, less_t, extent>(lesser_size, spa_rta, median_index, key_less);

                last_incl = first + lesser_size - 1U;
            }
            else if (const auto two_less_eq = (less1 + plain_median.second) * two;
                     two_less_eq < total_cost) // Discard all less or equal
            {
                ++lesser_size; // Count equal.
                assert(lesser_size < left_size);

                const auto greater_size = left_size - lesser_size;

                std::span<item_t, extent> spa_rta(first, stop_inclusive + 1U);

                Inner::greater_to_ending<item_t, less_t, extent>(greater_size, spa_rta, median_index, key_less);

                total_cost -= two_less_eq;
                first += lesser_size;
            }
            else
            {
                return plain_median;
            }
        }
    }

    // Return both lower and upper medians; the lower might be equal to the upper.
    // Slow time O(n*log(n)).
    template<class long_int_t, class data_t, class item_t = typename data_t::value_type>
    [[nodiscard]] constexpr auto weighted_median_selection_slow(data_t &data) -> std::pair<item_t &, item_t &>
    {
        static_assert(sizeof(decltype(data[0].second)) <= sizeof(long_int_t));

        constexpr long_int_t zero{};
        constexpr long_int_t two = 2;

        {
            auto key_less = [] [[nodiscard]] (const auto &aaa, const auto &bbb)
            {
                return aaa.first < bbb.first;
            };
            std::sort(data.begin(), data.end(), key_less);
        }

        for (std::size_t index = 1; index < data.size(); ++index)
        {
            const auto &pre = data[index - 1U];
            const auto &cur = data[index];

            if (!(pre.first < cur.first)) [[unlikely]]
            {
                throw std::runtime_error(
                    "The key at [" + std::to_string(index) + "] must be unique in weighted_median_selection_slow.");
            }
        }

        const auto total_cost = Inner::compute_total_cost<long_int_t, data_t>(data);
        assert(zero < total_cost);

        long_int_t partial_cost{};

        for (std::size_t index{}; index < data.size(); ++index)
        {
            auto &cur = data[index];
            const auto &item_cost = cur.second;

            {
                const auto old_cost = partial_cost;
                partial_cost += static_cast<long_int_t>(item_cost * two);

                require_greater(partial_cost, old_cost, "slow partial cost");
            }
            assert(zero < item_cost && item_cost < partial_cost);

            if (partial_cost < total_cost)
            {
                continue;
            }

            if (total_cost < partial_cost || index + 1U == data.size())
            {
                return { cur, cur };
            }

            auto &next = data[index + 1U];

            return { cur, next };
        }

        throw std::runtime_error("No median error in weighted_median_selection_slow, data size " +
            std::to_string(data.size()) + ". The costs and their sum must be positive.");
    }
} // namespace Standard::Algorithms::Numbers
