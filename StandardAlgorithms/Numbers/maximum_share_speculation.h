#pragma once
#include"../Utilities/require_utilities.h"
#include<array>
#include<tuple>
#include<vector>

namespace Standard::Algorithms::Numbers::Inner
{
    inline constexpr void max_sh_spec_check_size(const std::size_t size)
    {
        constexpr auto max_size = std::numeric_limits<std::size_t>::max() - 2U;

        require_less_equal(size, max_size, "size");
    }

    // Given {8,7,7,3,5,7,3,9,9,3,1}
    // return {3,5,7,3,9}.
    template<class int_t>
    [[nodiscard]] constexpr auto skip_non_increasing_ends(const std::vector<int_t> &data)
        -> std::tuple<std::size_t, std::size_t, bool>
    {
        std::size_t start{};
        auto size = data.size();

        while (start + 1U < size && !(data[start] < data[start + 1U]))
        {
            ++start;
        }

        const auto size_at_least_two = start + 1U < size;

        if (size_at_least_two)
        {
            while (!(data[size - 2U] < data[size - 1U]))
            {
                assert(2U < size);
                --size;
            }
        }

        return { start, size, size_at_least_two };
    }

    /*template<std::signed_integral int_t, std::signed_integral long_int_t>
    requires(sizeof(int_t) <= sizeof(long_int_t))
    [[nodiscard]]  constexpr auto maximum_share_one_tran(std::size_t min_index,
        const std::vector<int_t>& prices,
        const std::size_t size,
        std::vector<std::pair<std::size_t, std::size_t>>& indexes) -> long_int_t
    {
        assert(min_index < size && min_index + 1U < size && prices[min_index] < prices[min_index + 1U]);

        long_int_t profit{};
        indexes.assign(1, {});

        for (auto index = min_index + 1U; index < size; ++index)
        {
            const auto& prev = prices[min_index];
            const auto& cur = prices[index];

            if (cur < prev)
            {
                min_index = index;
            }
            else if (const auto cand = static_cast<long_int_t>(static_cast<long_int_t>(cur) - prev);
                profit < cand)
            {
                profit = cand;
                indexes[0] = { min_index, index };
            }
        }

        assert(long_int_t{} < profit && indexes[0].first < indexes[0].second);

        return profit;
    }*/

    // Time O(n).
    template<std::signed_integral int_t, std::signed_integral long_int_t>
    requires(sizeof(int_t) <= sizeof(long_int_t))
    [[nodiscard]] constexpr auto find_goingup_segments(const std::vector<int_t> &prices, std::size_t index,
        std::vector<std::pair<std::size_t, std::size_t>> &indexes, const std::size_t size)
        -> std::pair<long_int_t, std::vector<int_t>>
    {
        assert(2U <= size && size <= prices.size() && prices[size - 2U] < prices[size - 1U]);

        long_int_t profit{};

        std::vector<int_t> goingup_segments;
        goingup_segments.reserve(size - index);

        for (;;)
        {
            // Wave up.
            assert(index < size && index + 1U < size && prices[index] < prices[index + 1U]);

            auto max_index = index;

            do
            {// Find a local maximum.
                ++max_index;
            } while (max_index + 1U < size && !(prices[max_index + 1U] < prices[max_index]));

            const auto dif = static_cast<long_int_t>(static_cast<long_int_t>(prices[max_index]) - prices[index]);

            assert(prices[index] < prices[max_index] && long_int_t{} <= profit && profit < profit + dif);

            indexes.emplace_back(index, max_index);
            goingup_segments.push_back(prices[index]);
            goingup_segments.push_back(prices[max_index]);

            profit += dif;

            assert(long_int_t{} < profit);

            if (size <= max_index + 1U)
            {
                return { std::move(profit), std::move(goingup_segments) };
            }

            // Wave down.
            index = max_index;

            assert(!(prices[index] < prices[index + 1U]));

            do
            {// Find a local minimum.
                assert(index < size && index + 1U < size);

                ++index;

                // At the end there is an increment (prices[size - 2] < prices[size - 1]).
                // Thus, the size check can be safely omitted.
            } while (!(prices[index] < prices[index + 1U]));
        }
    }
} // namespace Standard::Algorithms::Numbers::Inner

namespace Standard::Algorithms::Numbers
{
    // Share prices for a period of n days are given.
    // 0 or 1 operation a day can be conducted: a share is either bought or sold, or nothing happens.
    // A transaction occurs when a share is bought on a day i, and then sold on a day j > i.
    // Determine how much profit can you make if up to m transactions are allowed?
    // Time O(n*m).
    template<std::signed_integral int_t, std::signed_integral long_int_t>
    requires(sizeof(int_t) <= sizeof(long_int_t))
    [[nodiscard]] constexpr auto maximum_share_speculation_simple(const std::vector<int_t> &prices,
        std::size_t allowed_transactions, std::vector<std::pair<std::size_t, std::size_t>> &indexes) -> long_int_t
    {
        Inner::max_sh_spec_check_size(prices.size());

        allowed_transactions = std::min(allowed_transactions, prices.size() >> 1U);

        indexes.clear(); // todo(p3): fill indexes.

        if (0U == allowed_transactions)
        {
            return {};
        }

        const auto [start, size, has] = Inner::skip_non_increasing_ends(prices);

        if (!has)
        {// Cannot make even 1 profitable transaction.
            return {};
        }

        assert(2U <= size && prices[size - 2U] < prices[size - 1U]);

        using vec_t = std::vector<long_int_t>;

        std::array<vec_t, 2> buffers{ vec_t(size + 1U), vec_t(size + 1U) };
        std::uint32_t parity{};

        for (std::size_t tran{}; tran < allowed_transactions; ++tran)
        {
            const auto &prev = buffers[parity];
            auto &cur = buffers[parity ^ 1U];

            auto max_dif = std::numeric_limits<long_int_t>::min();

            for (auto index = start; index < size - 1U; ++index)
            {
                const auto dif = static_cast<long_int_t>(prev[index] - static_cast<long_int_t>(prices[index]));

                max_dif = std::max(max_dif, dif);

                const auto cand = static_cast<long_int_t>(static_cast<long_int_t>(prices[index + 1U]) + max_dif);

                cur[index + 1U] = std::max(cur[index], cand);
            }

            parity ^= 1U;
        }

        const auto &pre = buffers[parity];
        auto profit = *std::max_element(pre.cbegin(), pre.cend());
        return profit;
    }

    // Usually it is slightly faster than the simple code above.
    // Idea: leave only 2 edge points from each going up segment
    // because any other segment point, excluding the beginning and the ending, is never better.
    template<std::signed_integral int_t, std::signed_integral long_int_t>
    requires(sizeof(int_t) <= sizeof(long_int_t))
    [[nodiscard]] constexpr auto maximum_share_speculation(const std::vector<int_t> &prices,
        std::size_t allowed_transactions, std::vector<std::pair<std::size_t, std::size_t>> &indexes) -> long_int_t
    {
        Inner::max_sh_spec_check_size(prices.size());

        indexes.clear();

        if (0U == allowed_transactions)
        {
            return {};
        }

        const auto [start, size, has] = Inner::skip_non_increasing_ends(prices);

        if (!has)
        {// Cannot make even 1 profitable transaction.
            return {};
        }

        assert(2U <= size && prices[size - 2U] < prices[size - 1U]);

        allowed_transactions = std::min(allowed_transactions, size >> 1U);

        /* // Less code - easier to support.
        if (allowed_transactions == 1U)
        {
            auto profit1 = Inner::maximum_share_one_tran<int_t, long_int_t>(start,
                prices, size, indexes);

            return profit1;
        }*/

        auto [profit, goingup_segments] = Inner::find_goingup_segments<int_t, long_int_t>(prices, start, indexes, size);

        assert(!indexes.empty());

        if (const auto possible_transactions = indexes.size(); possible_transactions <= allowed_transactions)
        {
            return profit;
        }

        profit = maximum_share_speculation_simple<int_t, long_int_t>(goingup_segments, allowed_transactions, indexes);

        return profit;
    }

    // Slow time O(n*n*transactions).
    template<std::signed_integral int_t, std::signed_integral long_int_t>
    requires(sizeof(int_t) <= sizeof(long_int_t))
    [[nodiscard]] constexpr auto maximum_share_speculation_slow(const std::vector<int_t> &prices,
        const std::size_t allowed_transactions, std::vector<std::pair<std::size_t, std::size_t>> &indexes) -> long_int_t
    {
        Inner::max_sh_spec_check_size(prices.size());

        indexes.clear();

        const auto size = prices.size();
        if (0U == allowed_transactions || size < 2U)
        {
            return {};
        }

        // todo(p3): fill indexes.
        using vec_t = std::vector<long_int_t>;

        std::array<vec_t, 2> buffers{ vec_t(size + 1U), vec_t(size + 1U) };
        std::uint32_t parity{};

        for (std::size_t tran{}; tran < allowed_transactions; ++tran)
        {
            const auto &prev = buffers[parity];
            auto &cur = buffers[parity ^ 1U];
            auto has_changed = false;

            for (std::size_t tod = 1; tod < size; ++tod)
            {
                auto &val = cur[tod + 1U];
                val = std::max(prev[tod + 1U], cur[tod]);

                for (std::size_t from{}; from < tod; ++from)
                {
                    if (!(prices[from] < prices[tod]))
                    {
                        continue;
                    }

                    const auto delta = static_cast<long_int_t>(prices[tod]) - static_cast<long_int_t>(prices[from]);

                    const auto cand = static_cast<long_int_t>(prev[from] + delta);

                    if (val < cand)
                    {
                        val = cand;
                        has_changed = true;
                    }
                }
            }

            parity ^= 1U;

            if (!has_changed)
            {
                break;
            }
        }

        const auto &pre = buffers[parity];
        auto profit = *std::max_element(pre.cbegin(), pre.cend());
        return profit;
    }
} // namespace Standard::Algorithms::Numbers
