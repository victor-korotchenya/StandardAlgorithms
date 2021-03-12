#pragma once
#include <queue>
#include <type_traits>
#include <vector>
#include "../Utilities/ExceptionUtilities.h"

namespace Privet
{
    namespace Algorithms
    {
        namespace Numbers
        {
            //Given an array, any element can be increased by 0,1,-1.
            //The goal is the make the array NOT strictly increasing
            // by making the minimum possible number of operations.
            //E.g. given
            // {5, 9, 1, 4, 10, 8}, the answer is 11:
            // {5, 5, 5, 5,  8, 8}
            //     4 +4 +1  +2 == 11.
            //Note. To make it strictly increasing,
            // one must subtract the index from each element,
            // then run this (N*logN) alg.
            template <typename Number>
            Number ArrayMinMovesToMakeNonDecreasing(const std::vector<Number>& data)
            {
                static_assert(std::is_signed<Number>::value, "Number must be of signed type.");

                const auto size = static_cast<Number>(data.size());
                RequirePositive(size, "There must be at least one number in the array.");

                Number result{};
                std::priority_queue<Number> q;

                q.push(data[0]);
                for (Number i = 1; i < size; ++i)
                {
                    const auto& item = data[i]
                        //This is to make it STRICTLY increasing.
                        //- i
                        ;
                    q.push(item);
                    if (item < q.top())
                    {
                        result += q.top() - item;
                        q.pop();
                        q.push(item);
                    }
                }

                return result;
            }
        }
    }
}