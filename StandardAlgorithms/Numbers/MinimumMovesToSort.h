#pragma once

#include <vector>
#include "../Utilities/StreamUtilities.h"
#include "LongestIncreasingSubsequence.h"

namespace Privet
{
    namespace Algorithms
    {
        namespace Numbers
        {
            //Given an array, probably unsorted,
            // return the minimum number of moves to sort it.
            //E.g. given { 4, 1, 2, 3, }, the "4" must be moved to the end,
            // thus there is 1 move.
            template <typename Element, typename TComparer = std::less<Element>>
            size_t MinimumMovesToSort(const std::vector<Element>& data, TComparer comparer = {})
            {
                const size_t size = data.size();
                if (size <= 1)
                {
                    return 0;
                }

                const auto longestIncreasing =
                    LongestIncreasingSubsequence<Element, std::vector<Element>, TComparer>::Fast(
                        data, comparer);

                const auto increasingSize = longestIncreasing.size();
                if (size < increasingSize)
                {
                    std::ostringstream ss;
                    ss << "Error: size(" << size
                        << ") < increasingSize(" << increasingSize << ").";
                    StreamUtilities::throw_exception(ss);
                }

                const size_t result = size - increasingSize;
                return result;
            }
        }
    }
}