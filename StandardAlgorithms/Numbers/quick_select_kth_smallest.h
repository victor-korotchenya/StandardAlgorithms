#pragma once
#include"../Utilities/random.h"
#include"../Utilities/require_utilities.h"
#include"dutch_flag_split.h"
#include<vector>

namespace Standard::Algorithms::Numbers::Inner
{
    template<class value_t>
    constexpr void swap_rand_with_right(const std::size_t left,
        Standard::Algorithms::Utilities::random_t<std::size_t> &rnd, std::vector<value_t> &arr, const std::size_t right)
    {
        const auto dist = right - left + 1U;

        assert(left <= right && right < arr.size() && 0U < dist);

        const auto dumb = rnd();
        const auto index = dumb % dist + left;

        std::swap(arr.at(index), arr.at(right));
    }

    // Return the inclusive range of values equal to a random item in [left, right].
    template<class value_t>
    [[nodiscard]] constexpr auto eq_incl(const std::size_t left,
        Standard::Algorithms::Utilities::random_t<std::size_t> &rnd, std::vector<value_t> &arr, const std::size_t right)
        -> std::pair<std::size_t, std::size_t>
    {
        assert(left <= right && right < arr.size());

        swap_rand_with_right<value_t>(left, rnd, arr, right);

        auto &pivot = arr.at(right);

        const auto [less_end, equ_end] = // The pivot is excluded from the partition.
            dutch_flag_split_into_less_equal_greater_3way_partition(arr.begin() + left, pivot, arr.begin() + right);

        std::swap(*equ_end, pivot); // Make the pivot as the rightmost item of the returned range.

        auto res = std::pair<std::size_t, std::size_t>(less_end - arr.begin(), equ_end - arr.begin());

        assert(left <= res.first && res.first <= res.second && res.second <= right);

        return res;
    }
} // namespace Standard::Algorithms::Numbers::Inner

namespace Standard::Algorithms::Numbers
{
    // Values are not unique (may repeat).
    // Can be used to compute quantiles.
    // See also "median_selection".
    // As std::nth_element, it is about 3x faster in immense practice but not always linear as it uses quick select.
    // Often linear time; worst time O(n*n).
    template<class value_t>
    [[nodiscard]] constexpr auto quick_select_kth_smallest(std::vector<value_t> &arr,
        // Starts from 0.
        std::size_t rank) -> std::size_t
    {
        require_greater(arr.size(), rank, "rank");

        Standard::Algorithms::Utilities::random_t<std::size_t> rnd{};
        std::size_t left{};
        auto right = arr.size() - 1U;

        for (;;)
        {
            assert(left <= right && rank <= right - left && right < arr.size());

            const auto [start, stop] = Inner::eq_incl<value_t>(left, rnd, arr, right);

            assert(left <= start && start <= stop && stop <= right && right < arr.size());

            if constexpr (::Standard::Algorithms::is_debug)
            {
                for (auto index = left; index < start; ++index)
                {
                    assert(arr[index] < arr[start]);
                }

                for (auto index = start; index <= stop; ++index)
                {
                    assert(arr[start] == arr[index]);
                }

                for (auto index = stop + 1LLU; index <= right; ++index)
                {
                    assert(arr[start] < arr.at(index));
                }
            }

            if (rank < start - left)
            {// Take the branch [left, start).
                right = start - 1U;
            }
            else if (stop - left < rank)
            {// Go to (stop, right]. Decrease the rank by skipped elements.
                const auto skipped = stop + 1U - left;
                assert(skipped <= rank);

                rank -= skipped, left = stop + 1U;
            }
            else
            {// The rank element is inside [start, stop].
                // E.g. rank=0; left = start = stop = right = 7.
                return start;
            }
        }
    }
} // namespace Standard::Algorithms::Numbers
