#pragma once

#ifdef _DEBUG
#include "Arithmetic.h"
#endif

#include "../Utilities/StreamUtilities.h"

namespace Privet
{
    namespace Algorithms
    {
        namespace Numbers
        {
            //Given an array A, where elements are from 1 to A.size,
            // print the frequencies (occurrences) of elements using O(1) memory.
            //Assume the Number is signed.
            //The array can is to be modified in order to store frequencies.
            class PrintFrequenciesInEditableArray final
            {
                PrintFrequenciesInEditableArray() = delete;

            public:

                template <typename Number >
                static void PrintFrequencies(
                    Number* numbers,
                    const size_t size);

            private:

                template <class Number >
                static void CheckRange(Number* numbers, const Number size);

                template <class Number >
                static void Compute(Number* numbers, const Number size);

                template <class Number >
                static void TakeAbsolute(Number* numbers, const Number size);
            };

            template <typename Number >
            void PrintFrequenciesInEditableArray::PrintFrequencies(
                Number* numbers,
                const size_t size)
            {
#ifdef _DEBUG
                AssertSigned< Number >("PrintFrequencies");
#endif
                if (0 == size)
                {
                    throw std::runtime_error("0 == size");
                }

                if (nullptr == numbers)
                {
                    throw std::runtime_error("nullptr == numbers");
                }

                const Number sizeAsNumber = Number(size);
                if (size != size_t(sizeAsNumber))
                {
                    std::ostringstream ss;
                    ss << "The Number type is too small(" << sizeAsNumber <<
                        ") to hold the size(" << size << ").";
                    StreamUtilities::throw_exception(ss);
                }

                CheckRange< Number >(numbers, sizeAsNumber);

                Compute< Number >(numbers, sizeAsNumber);

                TakeAbsolute< Number >(numbers, sizeAsNumber);
            }

            template <typename Number >
            void PrintFrequenciesInEditableArray::CheckRange(
                Number* numbers, const Number size)
            {
                for (Number i = 0; i < size; ++i)
                {
                    const bool isValid = Number(0) < numbers[i] && numbers[i] <= size;
                    if (!isValid)
                    {
                        std::ostringstream ss;
                        ss << "The number " << numbers[i]
                            << " at index=" << i
                            << " must be in the range [1, " << size << "].";
                        StreamUtilities::throw_exception(ss);
                    }
                }
            }

            template <typename Number >
            void PrintFrequenciesInEditableArray::TakeAbsolute(
                Number* numbers, const Number size)
            {
                for (Number k = 0; k < size; ++k)
                {
                    numbers[k] = Number(abs(numbers[k]));
                }
            }

            template <typename Number>
            void PrintFrequenciesInEditableArray::Compute(Number* numbers, const Number size)
            {
                Number i = 0;
                do
                {
                    if (Number(0) < numbers[i])
                    {
                        const Number expectedIndex = numbers[i] - Number(1);

                        if (Number(0) < numbers[expectedIndex])
                        {
                            std::swap(numbers[i], numbers[expectedIndex]);
                            numbers[expectedIndex] = Number(-1);
                        }
                        else
                        {
                            --numbers[expectedIndex];
                            numbers[i++] = Number(0);
                        }
                    }
                    else
                    {
                        ++i;
                    }
                } while (i < size);
            }
        }
    }
}