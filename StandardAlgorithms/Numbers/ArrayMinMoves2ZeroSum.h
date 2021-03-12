#pragma once

#include <algorithm>
#include <cassert>
#include <type_traits>
#include <unordered_map>
#include <vector>

namespace Privet
{
    namespace Algorithms
    {
        namespace Numbers
        {
            //Given a signed circular ring, having zero total sum,
            // return minimum number of moves to make all values zero.
            //In every step, either positive or negative sum can be carried between neighbor points.
            //E.g. given {1, 0, -1}, there is one move of 1 from 0-th to second position.
            //Example2: {10, 1, -4, 2, 4, -3, -6, -4} requires 6 moves: 10 to -4 and to -6;
            //  then 1 to -4; -3 to 2; -1 to 4; 3 to -3.
            //
            //Warning: here is no test that the total sum of all numbers is zero.
            template <typename Number>
            size_t ArrayMinMoves2ZeroSum(const std::vector<Number>& ring)
            {
                static_assert(std::is_signed<Number>::value, "Number must be of signed type.");
                const auto size = ring.size();

                size_t result{};
                Number sum{};

                std::unordered_map<Number, size_t> prefix_sums;

                for (size_t i = 0; i < size; ++i)
                {
                    sum += ring[i];

                    const auto cnt = ++(prefix_sums[sum]);
                    result = std::max(result, cnt);
                }

                assert(sum == 0);
                return size - result;
            }
        }
    }
}