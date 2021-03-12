#pragma once
#include <vector>
#include "NumberUtilities.h"

namespace Privet::Algorithms::Numbers
{
    // Values must be small.
    // Time O(n*max_value).
    template <typename t2, typename t>
    t max_gcd_pair_in_array(const t* const ar, const size_t n)
    {
        static_assert(std::is_unsigned_v<t> && std::is_unsigned_v<t2> && sizeof(t) <= sizeof(t2));
        assert(1 < n);

        const auto& max_val = *std::max_element(ar, ar + n);

        //std::map<int, int> counts;
        std::vector<size_t> counts(max_val + 1llu);
        for (size_t i{}; i < n; i++)
            ++counts[ar[i]];

        if (counts[0])
            return max_val;

        for (auto i = max_val; 0 < i; --i)
        {
            t2 j = i;
            auto cnt = 0;
            do
            {
                if (counts[j] > 1)
                    return i;

                if (counts[j] == 1)
                {
                    if (2 == ++cnt)
                        return i;
                }
            } while ((j += i) <= max_val);
        }

        return 0;
    }

    // Time O(n*n).
    template <typename t>
    t max_gcd_pair_in_array_slow(const t* const ar, const size_t n)
    {
        static_assert(std::is_unsigned_v<t>);
        assert(1 < n);

        t result{};
        for (size_t i{}; i < n; ++i)
        {
            for (size_t j = i + 1; j < n; ++j)
            {
                result = std::max<t>(result, gcd_unsigned(ar[i], ar[j]));
            }
        }

        return result;
    }
}