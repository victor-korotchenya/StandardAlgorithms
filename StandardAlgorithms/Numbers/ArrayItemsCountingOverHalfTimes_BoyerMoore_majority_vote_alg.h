#pragma once

namespace Privet
{
    namespace Algorithms
    {
        namespace Numbers
        {
            // Boyer–Moore majority vote (array maority element) algorithm.
            // Given an array, return the index of the major element,
            // appearing > n/2 times in the array, or 'stop' if none exists.
            template <typename iterator_t>
            iterator_t Boyer_Moore_majority_vote_algorithm(const iterator_t& start, const iterator_t& stop)
            {
                if (start == stop)
                    return stop; // empty.

                auto it = start, major = start;
                auto total_count = 1u, count = 1u;
                while (++it != stop)
                {
                    ++total_count;
                    count += (*it == *major) ? 1 : -1;
                    if (count == 0)
                    {
                        count = 1u;
                        major = it;
                    }
                }
                if (total_count <= 1u)
                    return stop;

                if (total_count / 2u < count)
                    return major;

                count = 0u;
                for (it = start; it != stop; ++it)
                    count += *it == *major;

                return total_count / 2 < count ? major : stop;
            }
        }
    }
}