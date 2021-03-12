#pragma once
#include<cstddef>

namespace Standard::Algorithms::Numbers
{
    // Boyer-Moore majority vote (array majority element) algorithm.
    // Given an array, return the index of the major element,
    // appearing > n/2 times in the array, or 'stop' if none exists.
    template<class iterator_t>
    constexpr auto array_item_count_over_half_majority_vote(iterator_t start, iterator_t stop) -> iterator_t
    {
        if (const auto empty = start == stop; empty)
        {
            return stop;
        }

        constexpr std::size_t one = 1;

        auto total_count = one;
        auto count = one;
        auto major = start;

        for (auto iter = start; ++iter != stop;)
        {
            ++total_count;
            count += (*iter == *major) ? one : -one;

            if (count == 0U)
            {
                count = 1U;
                major = iter;
            }
        }

        if (total_count == 1U)
        {
            return stop;
        }

        if (total_count / 2U < count)
        {
            return major;
        }

        count = 0U;

        for (auto iter = start; iter != stop; ++iter)
        {
            count += *iter == *major ? 1U : 0U;
        }

        return total_count / 2U < count ? major : stop;
    }
} // namespace Standard::Algorithms::Numbers
