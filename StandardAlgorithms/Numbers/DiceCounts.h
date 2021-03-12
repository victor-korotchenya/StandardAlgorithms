#pragma once
#include <vector>
#include "Arithmetic.h"

namespace Privet
{
    namespace Algorithms
    {
        namespace Numbers
        {
            //Given a number n>0, a dice is rolled n times.
            //What are counts of all possible sums?
            //E.g. after n=1 throw, all sums 1..6 have count=1.
            template <typename Number = size_t>
            class DiceCounts final
            {
                DiceCounts() = delete;

            public:

                //In the returned vector: the index is the sum, and value is the count.
                static std::vector<Number> CalcCountPerEachSum(const Number& n);

                static constexpr int Sides = 6;

            private:

                inline static constexpr int OtherIndex(int currentIndex)
                    noexcept
                {
                    return 1 - currentIndex;
                }
            };

            template <typename Number>
            std::vector<Number> DiceCounts<Number>::CalcCountPerEachSum(const Number& n)
            {
                if (n < 1)
                {
                    throw std::runtime_error("The 'n' argument must be at least 1.");
                }

                const size_t maxSize = AddUnsigned(
                    MultiplyUnsigned(size_t(Sides), n),
                    size_t(1));

                std::vector<std::vector<Number>> buffer(2);

                int currentIndex = 0;
                buffer[currentIndex].resize(maxSize);
                buffer[OtherIndex(currentIndex)].resize(maxSize);

                for (int i = 1; i <= Sides; ++i)
                {
                    buffer[currentIndex][i] = Number(1);
                }

                for (Number rollIndex = 2; rollIndex <= n; ++rollIndex)
                {
                    const Number sumIndexMax = rollIndex * Number(Sides);
                    for (Number sumIndex = 0; sumIndex <= sumIndexMax; ++sumIndex)
                    {
                        buffer[OtherIndex(currentIndex)][sumIndex] = 0;
                    }

                    for (Number sumIndex = rollIndex; sumIndex <= sumIndexMax; ++sumIndex)
                    {
                        const auto upperIndex = min<Number>(Sides, sumIndex);

                        for (int sideIndex = 1; sideIndex <= upperIndex; ++sideIndex)
                        {
                            buffer[OtherIndex(currentIndex)][sumIndex] +=
                                buffer[currentIndex][sumIndex - sideIndex];
                        }
                    }

                    currentIndex = OtherIndex(currentIndex);
                }

                return buffer[currentIndex];
            }
        }
    }
}