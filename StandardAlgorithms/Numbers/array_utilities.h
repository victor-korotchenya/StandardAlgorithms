#pragma once
#include <algorithm>
#include <cassert>
#include <vector>
#include "NumberUtilities.h"

namespace Privet::Algorithms::Numbers
{
    // {3,7,100,5,9,8} array and shift=2 given, return
    // {100,5,9,8,3,7}.
    template <class ar_t>
    void rotate_array_left(ar_t& ar, const int shift)
    {
        const auto size = static_cast<int>(ar.size());
        assert(size >= 0);
        if (!shift || size <= 1)
            return;

        assert(shift > 0 && shift < size);

        const auto g = gcd(size, shift);
        assert(g > 0 && g < size&& g <= shift);

        auto i = 0;
        do
        {
            auto f = ar[i];
            auto i0 = i, i1 = i + shift;
            for (;;)
            {
                assert(i0 != i1 && i0 >= 0 && i0 < size&& i1 >= 0 && i1 < size);
                ar[i0] = ar[i1];
                if (i1 == i)
                    break;

                i0 = i1;
                i1 += shift;
                if (i1 >= size)
                    i1 -= size;
            }

            const auto i2 = size - shift + i;
            assert(i2 >= 0 && i2 < size&& i2 != i);
            ar[i2] = f;
        } while (++i < g);
    }

    // Slow.
    template <class ar_t, class t>
    void rotate_array_left_slow(ar_t& ar, std::vector<t>& temp, const int shift)
    {
        const auto size = static_cast<int>(ar.size());
        assert(size >= 0);
        if (!shift || size <= 1)
            return;

        assert(shift > 0 && shift < size);

        temp.clear();
        temp.resize(size);

        //   {9,8,  3,7,100,5};  size = 6, shift = 2.
        for (auto i = shift; i < size; ++i)
            temp[i - shift] = ar[i];

        for (auto i = 0; i < shift; ++i)
            temp[i + (size - shift)] = ar[i];

        std::copy(temp.begin(), temp.end(), ar.begin());
    }

    // The cycle of distinct elements with min first item.
    // E.g. return {3,7,100,5,9,8} for any of:
    //   {3,7,100,5,9,8};
    //   {8,3,7,100,5,9};
    //   {9,8,3,7,100,5};
    //   {5,9,8,3,7,100};
    //   {100,5,9,8,3,7};
    //   {7,100,5,9,8,3}.
    template <typename cycle_t>
    void normalize_cycle(cycle_t& cycle)
    {
        if (cycle.size() <= 1)
            return;

        const auto min_it = std::min_element(cycle.begin(), cycle.end());
        const auto shift = static_cast<int>(min_it - cycle.begin());
        if (!shift)
            return;

        rotate_array_left<cycle_t>(cycle, shift);
    }

    // Slow.
    template <class cycle_t, class t>
    void normalize_cycle_slow(cycle_t& cycle, std::vector<t>& temp)
    {
        if (cycle.size() <= 1)
            return;

        const auto min_it = std::min_element(cycle.begin(), cycle.end());
        const auto shift = static_cast<int>(min_it - cycle.begin());
        if (!shift)
            return;

        rotate_array_left_slow<cycle_t, t>(cycle, temp, shift);
    }
}