#pragma once

namespace Privet::Algorithms::Numbers
{
    // Given an array, find the longest alternative subsequence where:
    // - either the items first increase, then decrease, again increase, and so on e.g 5,7,2,4;
    // - or decrease, increase, decrease, increase, decrease, ... like 9,3,4,3,50.
    // No two adjacent integers are the same.
    // Time O(n), space O(1).
    template<class iter_t>
    size_t longest_alternative_subsequence(iter_t start, const iter_t stop)
    {
        if (start == stop)
            return 0;

        size_t len = 1;
        auto sign = 0;
        for (;;)
        {
            const auto prev = start;
            if (++start == stop)
                return len;

            if (*prev == *start)
                continue;

            const auto sign2 = *prev < *start ? 1 : -1;
            if (sign == sign2)
                continue;

            sign = sign2;
            ++len;
        }
    }
}