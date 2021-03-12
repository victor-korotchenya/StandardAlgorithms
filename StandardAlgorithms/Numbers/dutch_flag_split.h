#pragma once
#include<utility>

namespace Standard::Algorithms::Numbers
{
    // 3 way, three way, three-way or 3-way split (less, equal, greater) can be used in quick sort.
    template<class iter_t, class value_t>
    [[nodiscard]] constexpr auto dutch_flag_split_into_less_equal_greater_3way_partition(iter_t start,
        // Passing by value for now to avoid a mess.
        const value_t pivot, iter_t stop) -> std::pair<iter_t, iter_t>
    {
        // <            =              >
        //
        // [initial start, start)  [start, stop)  [stop, initial stop)

        while (start != stop && (*start) < pivot)
        {// Skip initial lesser.
            ++start;
        }

        auto equ = start;

        while (equ != stop && (*equ) == pivot)
        {// Skip initial equal.
            ++equ;
        }

        while (equ != stop && pivot < *(stop - 1))
        {// Skip ending greater.
            --stop;
        }

        while (equ != stop)
        {
            auto &item = *equ;

            if (pivot < item)
            {// todo(p3): <=> faster?
                --stop;

                std::swap(item, *stop);
            }
            else
            {
                if (item < pivot)
                {
                    std::swap(item, *start);
                    ++start;
                }
                // else (pivot == item)
                ++equ;
            }
        }

        return std::make_pair(start, equ);
    }
} // namespace Standard::Algorithms::Numbers
