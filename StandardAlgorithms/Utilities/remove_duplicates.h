#pragma once
#include"is_debug.h"
#include<algorithm>
#include<cassert>
#include<utility>
#include<vector>

namespace Standard::Algorithms
{
    // The input must be sorted.
    constexpr void remove_duplicates(auto &values)
    {
        if constexpr (::Standard::Algorithms::is_debug)
        {
            assert(std::is_sorted(values.begin(), values.end()));
        }

        auto iter = std::unique(values.begin(), values.end());
        values.erase(iter, values.end());
    }

    constexpr void sort_remove_duplicates(auto &values)
    {
        std::sort(values.begin(), values.end());
        remove_duplicates(values);
    }
} // namespace Standard::Algorithms
