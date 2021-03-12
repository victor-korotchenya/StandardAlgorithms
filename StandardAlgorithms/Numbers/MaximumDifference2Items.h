#pragma once
#include "../Utilities/StreamUtilities.h"

namespace Privet
{
    namespace Algorithms
    {
        namespace Numbers
        {
            //Given an array A, returns Max( A[indexMax] - A[indexMin] ),
            // where indexMin = [0..(n-2)], and indexMax=[1..(n-1)].
            //It is assumed that indexMin < indexMax.
            class MaximumDifference2Items final
            {
                MaximumDifference2Items() = delete;

            public:

                //To check for overflow.
                template <typename T >
                using SubtractFunction = T(*)(T const a, T const b);

                template <typename Number >
                static Number Slow(
                    SubtractFunction < Number > subtractFunction,
                    const Number* numbers,
                    const size_t size,
                    size_t& indexMin,
                    size_t& indexMax);

                template <typename Number >
                static Number Fast(
                    SubtractFunction < Number > subtractFunction,
                    const Number* numbers,
                    const size_t size,
                    size_t& indexMin,
                    size_t& indexMax);
            };

            template <typename Number >
            Number MaximumDifference2Items::Slow(
                SubtractFunction < Number > subtractFunction,
                const Number* numbers,
                const size_t size,
                size_t& indexMin,
                size_t& indexMax)
            {
                if (nullptr == subtractFunction)
                {
                    throw std::runtime_error("nullptr == subtractFunction");
                }

                const size_t minSize = 2;
                if (size < minSize)
                {
                    std::ostringstream ss;
                    ss << "The size (" << size
                        << ") must be at least " << minSize << ".";
                    StreamUtilities::throw_exception(ss);
                }

                indexMin = 0;
                indexMax = 1;

                Number result = subtractFunction(numbers[indexMax], numbers[indexMin]);

                size_t minPosition = 0;
                do
                {
                    size_t maxPosition = minPosition + 1;
                    do
                    {
                        const Number temp = subtractFunction(numbers[maxPosition], numbers[minPosition]);
                        if (result < temp)
                        {
                            result = temp;
                            indexMin = minPosition;
                            indexMax = maxPosition;
                        }
                    } while (++maxPosition < size);
                } while (++minPosition < size - 1);

                return result;
            }

            template <typename Number >
            Number MaximumDifference2Items::Fast(
                SubtractFunction < Number > subtractFunction,
                const Number* numbers,
                const size_t size,
                size_t& indexMin,
                size_t& indexMax)
            {
                if (nullptr == subtractFunction)
                {
                    throw std::runtime_error("nullptr == subtractFunction");
                }

                const size_t minSize = 2;
                if (size < minSize)
                {
                    std::ostringstream ss;
                    ss << "The size (" << size
                        << ") must be at least " << minSize << ".";

                    StreamUtilities::throw_exception(ss);
                }

                indexMin = 0;
                indexMax = 1;

                //The first instance index of the current minimum number.
                size_t indexMinFirst = indexMin;

                Number result = subtractFunction(numbers[1], numbers[0]);

                //Find the first number, that is greater than the previous one.
                size_t i = 1;
                while (i < size && numbers[i] <= numbers[i - 1])
                {
                    if (numbers[i] < numbers[indexMinFirst])
                    {
                        indexMinFirst = i;
                    }

                    const Number temp = subtractFunction(numbers[i], numbers[i - 1]);
                    if (result < temp)
                    {
                        indexMin = i - 1;
                        indexMax = i;
                        result = temp;
                    }

                    ++i;
                }

                while (i < size)
                {
                    if (numbers[i] < numbers[indexMinFirst])
                    {
                        indexMinFirst = i;
                    }
                    else
                    {
                        const Number temp = subtractFunction(numbers[i], numbers[indexMinFirst]);
                        if (result < temp)
                        {
                            indexMin = indexMinFirst;
                            indexMax = i;
                            result = temp;
                        }
                    }

#ifdef _DEBUG
                    if (result < 0)
                    {
                        throw std::runtime_error("The result must have become non-negative.");
                    }
#endif

                    ++i;
                }

                return result;
            }
        }
    }
}