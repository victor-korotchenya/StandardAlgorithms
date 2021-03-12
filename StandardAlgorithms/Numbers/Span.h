#pragma once
#include <stdexcept>
#include <vector>
#include <stack>
#include "../Utilities/ExceptionUtilities.h"

namespace Privet
{
    namespace Algorithms
    {
        namespace Numbers
        {
            //Application domain: financial analysis.
            //Input: A is an array of numbers.
            //Output: Span (array).
            //Given the index x of A, the span of A[x] is maximum number of consecutive preceding elements
            //  A[x - 1], A[x - 2], ..., such that A[y] <= A[x], where 0 <= y < x.
            // Let A = {9, 5, 3, 5, 8, 1}.
            // Span =  {0, 0, 0, 2, 3, 0}.
            class SpanCalculator final
            {
                SpanCalculator() = delete;

            public:

                template <typename Number >
                static void CalcSpanSimple(
                    const Number* numbers, const size_t size, std::vector<size_t>& result);

                template <typename Number >
                static void CalcSpanFast(
                    const Number* numbers, const size_t size, std::vector<size_t>& result);
            };

            template <typename Number >
            void SpanCalculator::CalcSpanSimple(
                const Number* numbers, const size_t size, std::vector<size_t>& result)
            {
                RequirePositive(size, "size");

                result.resize(size);
                result[0] = 0;

                for (size_t i = 1; i < size; ++i)
                {
                    size_t span = 0;

                    size_t j = i - 1;
                    for (;;)
                    {
                        if (numbers[i] < numbers[j]
                            || 0 == j--)
                        {
                            break;
                        }

                        ++span;
                    }

                    result[i] = span;
                }
            }

            template <typename Number >
            void SpanCalculator::CalcSpanFast(
                const Number* numbers, const size_t size, std::vector< size_t >& result)
            {
                if (0 == size)
                {
                    throw std::runtime_error("The 'size' must be positive.");
                }

                result.resize(size);

                std::stack<size_t> indexes;

                for (size_t i = 0; i < size; ++i)
                {
                    while (!indexes.empty() && numbers[indexes.top()] <= numbers[i])
                    {
                        indexes.pop();
                    }

                    const auto span = indexes.empty()
                        ? i
                        : i - 1 - indexes.top();

                    result[i] = span;
                    indexes.push(i);
                }
            }
        }
    }
}