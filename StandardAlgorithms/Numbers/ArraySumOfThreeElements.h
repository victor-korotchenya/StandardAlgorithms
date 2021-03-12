#pragma once
#include <vector>
#include <tuple>
#include "SortedArraySumOfTwoElements.h"

#ifdef _DEBUG
#include "../Utilities/VectorUtilities.h"
#endif

namespace Privet
{
    namespace Algorithms
    {
        namespace Numbers
        {
            //Given an array and a "sum",
            // finds three distinct indexes of elements,
            // which sum to "sum", some indexes.
            //If it is impossible, a 3-tuple of (-1) is returned.
            class ArraySumOfThreeElements final
            {
                ArraySumOfThreeElements() = delete;

            public:
                using Tuple = std::tuple<size_t, size_t, size_t>;

                //Use "Fast" which is faster.
                //O(n*n*n) time.
                template <typename Number>
                static Tuple Slow(
                    const std::vector<Number>& data, const Number& sum);

                //O(n*n) time.
                //TODO: p2. Can the 3-sum problem can be solved even faster?
                //E.g. when positive numbers are below certain limit, indexes must go up...
                template <typename Number>
                static Tuple Fast(
                    const std::vector<Number>& data, const Number& sum);

                static Tuple NotFoundTuple();
            };

            inline ArraySumOfThreeElements::Tuple ArraySumOfThreeElements::NotFoundTuple()
            {
                const size_t minusOne = size_t(0) - size_t(1);

                const Tuple result = Tuple(minusOne, minusOne, minusOne);
                return result;
            }

            template <typename Number>
            ArraySumOfThreeElements::Tuple ArraySumOfThreeElements::Slow(
                const std::vector<Number>& data,
                const Number& sum)
            {
                const size_t size = data.size();
                if (2 < size)
                {
                    for (size_t index1 = 0; index1 < size - 2; ++index1)
                    {
                        for (size_t index2 = index1 + 1; index2 < size - 1; ++index2)
                        {
                            for (size_t index3 = index2 + 1; index3 < size; ++index3)
                            {
                                const Number actual = data[index1] + data[index2] + data[index3];
                                if (sum == actual)
                                {
                                    return Tuple(index1, index2, index3);
                                }
                            }
                        }
                    }
                }

                return NotFoundTuple();
            }

            template <typename Number>
            ArraySumOfThreeElements::Tuple ArraySumOfThreeElements::Fast(
                const std::vector<Number>& data,
                const Number& sum)
            {
#ifdef _DEBUG
                Privet::Algorithms::VectorUtilities::RequireArrayIsSorted(data, "data");
#endif
                const size_t size = data.size();
                if (2 < size)
                {
                    const auto not_found_pair = SortedArraySumOfTwoElements::NotFoundPair();

                    for (size_t offset = 0; offset < size - 2; offset++)
                    {
                        const Number delta = sum - data[offset];
                        const auto p2 = SortedArraySumOfTwoElements::Fast(data, delta, offset + 1);
                        if (not_found_pair != p2)
                        {
                            return Tuple(offset, p2.first, p2.second);
                        }
                    }
                }

                return NotFoundTuple();
            }
        }
    }
}