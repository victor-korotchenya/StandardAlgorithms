#pragma once
#include "../Utilities/StreamUtilities.h"

namespace Privet
{
    namespace Algorithms
    {
        namespace Numbers
        {
            //Input: A is an unsorted array of numbers from [1..n],
            // having two repeating distinct items.
            //
            //E.g. A = {1, 2,2, 3, 4,4, 5}, size=7, n=size - 2 = 5.
            //Output: 2 and 4.
            //
            //Another example: A = {1, 2, 3,3, 4, 5, 6, 7, 8,8 }, size=10, n=size - 2 = 8.
            //Output: 3 and 8.
            class Find2RepeatingDistinctItems final
            {
                Find2RepeatingDistinctItems() = delete;

            public:

                template <typename Number >
                static void Slow(
                    const Number* const numbers, const size_t size,
                    Number& number1,
                    Number& number2);
            };

            template <typename Number >
            void Find2RepeatingDistinctItems::Slow(
                const Number* const numbers,
                const size_t size,
                Number& number1,
                Number& number2)
            {
                if (nullptr == numbers)
                {
                    throw std::runtime_error("nullptr == numbers");
                }

                const size_t minSize = 4;
                if (size < minSize)
                {
                    std::ostringstream ss;
                    ss << "The size (" << size
                        << ") must be at least " << minSize << ".";

                    StreamUtilities::throw_exception(ss);
                }

                Number xorData =
                    //To compensate when i == size - 1.
                    Number(size - 1);

                for (size_t i = 0; i < size; ++i)
                {
                    const Number& number = numbers[i];
#ifdef _DEBUG
                    const Number maxNumber = Number(size - 2);
                    if (number < 1 || maxNumber < number)
                    {
                        std::ostringstream ss;
                        ss << "The number (" << number
                            << ") must be in the range [1, " << maxNumber << "].";
                        StreamUtilities::throw_exception(ss);
                    }
#endif
                    xorData ^= (number ^ i);
                }

#ifdef _DEBUG
                if (0 == xorData)
                {
                    throw std::runtime_error("0 == xorData");
                }
#endif
                const Number rightMostBit = xorData & (Number(0) - xorData);

                number1 = (rightMostBit & (size - 1))
                    //To compensate when i == size - 1.
                    ? Number(size - 1)
                    : Number(0);

                for (size_t i = 0; i < size; ++i)
                {
                    const Number& number = numbers[i];

                    //Divide into two sets by "rightMostBit" bit number: 0 or 1?
                    if (rightMostBit & number)
                    {
                        number1 ^= number;
                    }

                    if (rightMostBit & i)
                    {
                        number1 ^= i;
                    }
                }

                number2 = number1 ^ xorData;
            }
        }
    }
}