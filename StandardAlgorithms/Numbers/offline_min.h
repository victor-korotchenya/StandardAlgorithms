#pragma once
#include"../Utilities/iota_vector.h"
#include"../Utilities/require_utilities.h"
#include"disjoint_set.h"
#include<set>

namespace Standard::Algorithms::Numbers::Inner
{
    constexpr std::int64_t always_existing_extra_group = 1;

    template<std::signed_integral int_t>
    requires(sizeof(int_t) <= sizeof(std::uint64_t))
    constexpr void check_input(const std::vector<int_t> &data, const int_t &perm_size) noexcept(false)
    {
        require_positive(perm_size, "permutation size");
        {
            const auto unsigned_size = static_cast<std::uint64_t>(perm_size);
            assert(0U < unsigned_size);

            require_greater(data.size(), unsigned_size, "input vs permutation sizes");
        }

        constexpr int_t zero{};

        require_less_equal(zero, data.at(0), "data[0]");
        require_greater(zero, data.back(), "data.back");
    }

    // {5,8,4,-1} without the closing -1 will form a group, possibly empty.
    // In {1,0,2,-1,-1}, there are 2 groups: {1,0,2}, and {}.
    template<std::signed_integral int_t, class iter_t>
    constexpr auto split_into_consecutive_groups(iter_t first, std::vector<int_t> &group_values, const iter_t last,
        disjoint_set<int_t> &dsu, std::vector<int_t> &value_groups) -> int_t
    {
        assert(first != last);

        constexpr int_t zero{};

        auto count = zero;

        while (first != last)
        {
            if (const auto is_group_closing = *first < zero; is_group_closing)
            {
                ++first, ++count;

                assert(zero < count);

                continue;
            }

            const auto &first_flood = *first;
            value_groups[first_flood] = count;

            while (++first != last && !(*first < zero))
            {
                const auto &curva = *first;

                value_groups[curva] = count;
                dsu.unite(curva, first_flood);
            }

            group_values[count] = dsu.parent(first_flood);
        }

        return count;
    }

    template<std::signed_integral int_t>
    constexpr void extracted_smallest_first(const int_t &perm_size, std::vector<int_t> &group_values,
        disjoint_set<int_t> &dsu, std::vector<int_t> &value_groups, const int_t &count, std::vector<int_t> &results)
    {
        require_positive(count, "group count");
        require_less_equal(count, perm_size, "permutation size");

        constexpr int_t zero{};
        constexpr int_t one = 1;

        // To find the next slot in O(1) average time, not O(n).
        auto next_available =
            ::Standard::Algorithms::Utilities::iota_vector<int_t>(perm_size + always_existing_extra_group, one);

        // An extra group is always available to be ignored.
        std::vector<bool> available(perm_size + always_existing_extra_group, true);

        for (int_t value{}; value < perm_size; ++value)
        {
            const auto par = dsu.parent(value);
            const auto &group = value_groups.at(par);
            assert(!(group < zero) && group <= count);

            if (group == count)
            {// Ignore the extra group.
                continue;
            }

            // When possible, 0 will be extracted the earliest, then 1, et. cetera.
            results[group] = value;
            available[group] = false;

            auto &next_group = next_available[group];
            assert(group < next_group && next_group <= count);

            while (!available[next_group])
            {
                const auto &cand = next_available[next_group];
                assert(next_group < cand && cand <= count);

                next_group = cand;
            }

            const auto &cur_val = group_values[group];
            assert(!(cur_val < zero));

            auto &next_val = group_values[next_group];

            if (next_val < zero)
            {// When not all values are popped, or the extra group.
                next_val = cur_val;
            }
            else
            {
                dsu.unite(next_val, cur_val);

                // Given {2,0,-1, 1, ..}, the parent of 1 after uniting {0,2} and {1}
                // could actually be group 0 - ensure it is group 1.
                dsu.self_parent(next_val);
            }

            value_groups[next_val] = next_group;
        }
    }
} // namespace Standard::Algorithms::Numbers::Inner

namespace Standard::Algorithms::Numbers
{
    // Given a zero-based permutation, possibly randomly shuffled, with additional -1s,
    // where -1 denotes a min extraction and otherwise a non-negative value insertion,
    // the offline-minimum problem is to compute the extracted minimums.
    // The input must have all the numbers between 0 and (n - 1) inclusively, where n>0, start with an insertion, and
    // stop with an extraction. Any prefix of the input data cannot have more -1s than non-negative values.
    // Time complexity O(n*inv_ackermann(n)).
    // Here, in a faster version, fewer checks are performed.
    template<std::signed_integral int_t>
    requires(sizeof(int_t) <= sizeof(std::uint64_t))
    constexpr void offline_min(const std::vector<int_t> &data, const int_t &perm_size, std::vector<int_t> &results)
    {
        Inner::check_input<int_t>(data, perm_size);

        constexpr int_t minus_one = -static_cast<int_t>(1);
        static_assert(minus_one < int_t{});

        std::vector<int_t> group_values(perm_size + Inner::always_existing_extra_group, minus_one);

        std::vector<int_t> value_groups(perm_size);
        disjoint_set<int_t> dsu(perm_size);

        const auto count =
            Inner::split_into_consecutive_groups<int_t>(data.cbegin(), group_values, data.cend(), dsu, value_groups);

        results.assign(perm_size, minus_one);

        Inner::extracted_smallest_first<int_t>(perm_size, group_values, dsu, value_groups, count, results);

        std::erase(results, minus_one);
    }

    // O(n * log(n)) time when using a heap or a set.
    template<std::signed_integral int_t>
    requires(sizeof(int_t) <= sizeof(std::uint64_t))
    [[nodiscard]] constexpr auto offline_min_slow(const std::vector<int_t> &data, const int_t &perm_size)
        -> std::vector<int_t>
    {
        Inner::check_input<int_t>(data, perm_size);

        constexpr int_t zero{};

        std::vector<int_t> result;
        std::set<int_t> sito;
        std::vector<bool> used(perm_size, false);

        for (const auto &datum : data)
        {
            if (datum < zero)
            {
                throw_if_empty("Too many pop operations.", sito);

                auto iter = sito.begin();
                result.push_back(*iter);
                sito.erase(iter);
                continue;
            }

            require_greater(perm_size, datum, "Too large input datum");

            if (used[datum]) [[unlikely]]
            {
                auto err = "The number " + std::to_string(datum) + " cannot repeat.";
                throw std::runtime_error(err);
            }

            used[datum] = true;
            sito.insert(datum);
        }

        if (const auto ite = std::find(used.cbegin(), used.cend(), false); ite != used.cend()) [[unlikely]]
        {
            const auto missing = ite - used.cbegin();

            throw std::runtime_error("A number (" + std::to_string(missing) + ") must have been used in the input.");
        }

        return result;
    }
} // namespace Standard::Algorithms::Numbers
