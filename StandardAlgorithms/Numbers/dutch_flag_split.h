#pragma once
#include <utility>

namespace Privet::Algorithms::Numbers
{
    template<class iter_t, class value_t>
    std::pair<iter_t, iter_t> dutch_flag_split_into3_less_equal_greater_3way_partition(iter_t start, iter_t stop, value_t pivot)
    {
        iter_t i = start, eq = start, greater = stop;
        // <            =              >
        // [start, eq)  [eq, greater)  [greater, end)
        while (i != greater)
        {
            if (*i < pivot)
            {
                std::swap(*i, *eq);
                ++i, ++eq;
            }
            else if (pivot < *i)
            {
                --greater;
                std::swap(*i, *greater);
            }
            else // equal.
            {
                ++i;
            }
        }

        return std::make_pair(eq, greater);
    }
}