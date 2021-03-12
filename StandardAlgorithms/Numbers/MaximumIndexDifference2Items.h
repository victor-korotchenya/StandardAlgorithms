#pragma once
#include <cassert>
#include <utility> // pair
#include <stack>
#include "../Utilities/StreamUtilities.h"
#include "../Utilities/QueueUtilities.h"

namespace Privet
{
    namespace Algorithms
    {
        namespace Numbers
        {
            //Given an array A, return max(indexMax - indexMin),
            // such that A[indexMin] < A[indexMax].
            //Return 0 when A is sorted in non-increasing order.
            template <typename T,
                //To decrease memory usage,
                //"unsigned int" might suit better when dealing with sizes smaller than (INT_MAX).
                typename TSize = size_t,
                typename TPair = std::pair<TSize, TSize>,
                typename TLess = std::less<T>,
                typename TEqual = std::equal_to<T>>
                class MaximumIndexDifference2Items final
            {
                TLess Less;
                TEqual Equal;

            public:
                explicit MaximumIndexDifference2Items(
                    const TLess& lessArg = TLess(),
                    const TEqual& equalArg = TEqual())
                    : Less(lessArg), Equal(equalArg)
                {
                }

                TPair Slow(const T* const data, const TSize size) const
                {
                    CheckInput(data, size);

                    auto result = TPair(TSize(0), TSize(0));
                    TSize minPosition{};
                    do
                    {
                        TSize maxPosition = minPosition + TSize(1);
                        do
                        {
                            if (Less(data[minPosition], data[maxPosition])
                                && result.second - result.first < maxPosition - minPosition)
                                result = TPair(minPosition, maxPosition);
                        } while (++maxPosition < size);
                    } while (++minPosition < size - TSize(1));

                    return result;
                }

                TPair Fast(const T* const data, const TSize size) const
                {
                    CheckInput(data, size);
                    if (Less(data[0], data[size - TSize(1)])) //Shortcut
                        return TPair(TSize(0), size - TSize(1));

                    auto result = TPair(TSize(0), TSize(0));
                    auto maximums = maximums_from_right(data, size);
                    TSize left{};
                    for (;;)
                    {
                        if (Equal(data[left], data[maximums.top()]))
                        {
                            do
                            {
                                if (maximums.top() <= ++left)
                                {//Maintain the invariant that left < right. right is maximums.top().
                                    maximums.pop();
                                    if (maximums.empty()) return result;
                                }
                            } while (left < size &&
                                Equal(data[left], data[maximums.top()]));
                        }

                        const auto& right = maximums.top();
                        assert(left < right && !Equal(data[left], data[right]));
                        if (Less(data[left], data[right]))
                        {
                            if (result.second - result.first < right - left)
                                result = TPair(left, right);

                            maximums.pop(); // Go to the right by a potentially big step.
                            if (maximums.empty()) return result;
                        }
                        else if (right <= ++left)
                        {
                            maximums.pop();
                            if (maximums.empty()) return result;
                        }
                    }
                }

            private:
                static void CheckInput(const T* const data, const TSize size)
                {
                    if (!data)
                        throw std::runtime_error("The 'data' must be not null.");

                    const auto minSize = TSize(2);
                    if (size < minSize)
                    {
                        std::ostringstream ss;
                        ss << "The size (" << size
                            << ") must be at least " << minSize << ".";
                        StreamUtilities::throw_exception(ss);
                    }
                }

                std::stack<TSize> maximums_from_right(
                    const T* const data,
                    const TSize size) const
                {
                    TSize imax = size - TSize(1), i = size - TSize(2);
                    std::stack<TSize> maximums;
                    maximums.push(imax);
                    do
                    {
                        if (Less(data[imax], data[i]))
                            maximums.push(imax = i);
                    } while (--i);
                    return maximums;
                }
            };
        }
    }
}