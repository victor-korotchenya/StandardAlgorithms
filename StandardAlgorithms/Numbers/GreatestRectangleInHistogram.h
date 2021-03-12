#pragma once
#include <exception>
#include <stack>
#include <stdexcept>
#include <vector>

namespace Privet
{
    namespace Algorithms
    {
        namespace Numbers
        {
            //see also MaxSubMatrixIn2DMatrix.h

            //Histogram consists of rectangles
            //  each having unit width and non-negative height.
            //The question is to find the rectangle with greatest area.
            template <typename Number = unsigned int,
                //To avoid an overflow.
                typename NumberSquared = unsigned long long int >
                class GreatestRectangleInHistogram final
            {
                GreatestRectangleInHistogram() = delete;

                static_assert(sizeof(Number) <= sizeof(NumberSquared),
                    "It is expected that sizeof(Number) <= sizeof(NumberSquared)");

            public:

                //The startHeight is heigths[startIndex].
                static NumberSquared Compute(
                    const std::vector< Number >& heigths,
                    size_t& startIndex,
                    Number& startHeigth,
                    size_t& startWidth,
                    //When true, then startIndex will
                    // be calculated, and it might take O(N) time.
                    const bool shallComputeStartIndex = true);

                static NumberSquared ComputeFast(
                    const std::vector< Number >& heigths,
                    size_t& startIndex,
                    Number& startHeigth,
                    size_t& startWidth,
                    const bool shallComputeStartIndex = true);

            private:

                //Sometimes the "startIndex" must be moved left.
                inline static void FixStartIndex(
                    const std::vector< Number >& heigths,
                    const Number& startHeigth,
                    size_t& startIndex);
            };

            template <typename Number, typename NumberSquared >
            NumberSquared GreatestRectangleInHistogram< Number, NumberSquared >::Compute(
                const std::vector< Number >& heigths,
                size_t& startIndex,
                Number& startHeigth,
                size_t& startWidth,
                const bool shallComputeStartIndex)
            {
                const size_t size = heigths.size();
                if (0 == size)
                {
                    throw std::runtime_error("The 'heigths' must have at least one element.");
                }

                startIndex = 0;
                startHeigth = 0;
                startWidth = 0;

                NumberSquared result = NumberSquared(0);

                for (size_t i = 0; i < size; ++i)
                {
                    const Number current = heigths[i];
                    NumberSquared width = 1;

                    if (i) //Try to go left.
                    {
                        size_t goLeft = i - 1;
                        while (current <= heigths[goLeft])
                        {
                            ++width;
                            if (0 == goLeft--)
                            {
                                break;
                            }
                        }
                    }

                    size_t goRight = i + 1;
                    while (goRight < size && current <= heigths[goRight])
                    {
                        ++width;
                        ++goRight;
                    }

                    const NumberSquared temp = NumberSquared(current) * width;
                    if (result < temp)
                    {
                        startIndex = i;
                        startHeigth = heigths[startIndex];

                        startWidth = size_t(width);
                        result = temp;
                    }
                }

                if (shallComputeStartIndex && 0 < result)
                {
                    FixStartIndex(heigths, startHeigth, startIndex);
                }

                return result;
            }

            template <typename Number, typename NumberSquared >
            NumberSquared GreatestRectangleInHistogram< Number, NumberSquared>
                ::ComputeFast(
                    const std::vector< Number >& heigths,
                    size_t& startIndex,
                    Number& startHeigth,
                    size_t& startWidth,
                    const bool shallComputeStartIndex)
            {
                const size_t size = heigths.size();
                if (0 == size)
                {
                    throw std::runtime_error("The 'heigths' must have at least one element.");
                }

                startIndex = 0;
                startHeigth = 0;
                startWidth = 0;

                NumberSquared result = NumberSquared(0);
                std::stack < size_t > s;

                for (size_t i = 0; i <= size; ++i)
                {
                    while (!s.empty()
                        && (//Empty the stack after processing all the items.
                            size == i
                            //The height is smaller than previous.
                            || heigths[i] < heigths[s.top()]))
                    {
                        const auto top = s.top();
                        s.pop();

                        const size_t width = s.empty()
                            ? i
                            : i - size_t(1) - s.top();

                        const NumberSquared temp = NumberSquared(heigths[top])
                            * static_cast<NumberSquared>(width);
                        if (result < temp)
                        {
                            startIndex = top;
                            startHeigth = heigths[top];

                            startWidth = width;
                            result = temp;
                        }
                    }

                    if (size != i)
                    {
                        s.push(i);
                    }
                }

                if (shallComputeStartIndex && 0 < result)
                {
                    FixStartIndex(heigths, startHeigth, startIndex);
                }

                return result;
            }

            template <typename Number, typename NumberSquared >
            void GreatestRectangleInHistogram< Number, NumberSquared >::FixStartIndex(
                const std::vector< Number >& heigths,
                const Number& startHeigth,
                size_t& startIndex)
            {
#ifdef _DEBUG
                if (0 == startHeigth)
                {
                    throw std::runtime_error("0 == startHeigth in FixStartIndex");
                }
#endif
                while (0 < startIndex && startHeigth <= heigths[startIndex - 1])
                {//Move to the left.
                    --startIndex;
                }
            }
        }
    }
}