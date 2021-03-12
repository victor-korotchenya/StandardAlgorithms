#pragma once
#include"../Utilities/require_utilities.h"
#include"gcd.h"
#include<array>
#include<vector>

namespace Standard::Algorithms::Numbers
{
    // {3,7,100,5,9,8} array and shift=2 given, return
    // {100,5,9,8,3,7}.
    template<class ar_t>
    constexpr void rotate_array_left(ar_t &arr, const std::int32_t shift)
    {
        const auto size = static_cast<std::int32_t>(arr.size());
        assert(!(size < 0));

        if (0 == shift || size <= 1)
        {
            return;
        }

        require_positive(shift, "shift");
        require_greater(size, shift, "shift");

        const auto gr_div = gcd_int(size, shift);
        assert(0 < gr_div && gr_div < size && gr_div <= shift);

        std::int32_t index{};

        do
        {
            const auto old = arr[index];

            for (auto cur = index, next = index + shift;;)
            {
                assert(cur != next && !(cur < 0) && cur < size && !(next < 0) && next < size);
                arr[cur] = arr[next];

                if (next == index)
                {
                    break;
                }

                cur = next;
                next += shift;

                if (!(next < size))
                {
                    next -= size;
                }
            }

            const auto pos = size - shift + index;
            assert(!(pos < 0) && pos < size && pos != index);

            arr[pos] = old;
        } while (++index < gr_div);
    }

    // Slow.
    // todo(p3): Could rotate_array_left be actually slower?
    template<class ar_t, class t>
    constexpr void rotate_array_left_slow(ar_t &arr, const std::int32_t shift, std::vector<t> &temp)
    {
        const auto size = static_cast<std::int32_t>(arr.size());
        assert(!(size < 0));

        if (0 == shift || size <= 1)
        {
            return;
        }

        require_positive(shift, "shift");
        require_greater(size, shift, "shift");
        assert(0 < shift && shift < size);

        temp.resize(size);

        std::copy(arr.cbegin(), arr.cbegin() + shift, temp.begin() + size - shift);
        std::copy(arr.cbegin() + shift, arr.cend(), temp.begin());

        std::copy(temp.cbegin(), temp.cend(), arr.begin());
    }

    // The cycle of distinct elements with min first item.
    // E.g. return {3,7,100,5,9,8} for any of:
    //   {3,7,100,5,9,8};
    //   {8,3,7,100,5,9};
    //   {9,8,3,7,100,5};
    //   {5,9,8,3,7,100};
    //   {100,5,9,8,3,7};
    //   {7,100,5,9,8,3}.
    template<class cycle_t>
    constexpr void normalize_cycle(cycle_t &cycle)
    {
        if (cycle.size() <= 1U)
        {
            return;
        }

        const auto min_it = std::min_element(cycle.cbegin(), cycle.cend());
        const auto shift = static_cast<std::int32_t>(min_it - cycle.cbegin());
        if (0 == shift)
        {
            return;
        }

        rotate_array_left<cycle_t>(cycle, shift);
    }

    // Slow.
    template<class cycle_t, class t>
    constexpr void normalize_cycle_slow(cycle_t &cycle, std::vector<t> &temp)
    {
        if (cycle.size() <= 1U)
        {
            return;
        }

        const auto min_it = std::min_element(cycle.cbegin(), cycle.cend());
        const auto shift = static_cast<std::int32_t>(min_it - cycle.cbegin());
        if (0 == shift)
        {
            return;
        }

        rotate_array_left_slow<cycle_t, t>(cycle, shift, temp);
    }
} // namespace Standard::Algorithms::Numbers
