#pragma once
// "span.h"
#include"../Utilities/require_utilities.h"
#include<span>
#include<stack>
#include<vector>

namespace Standard::Algorithms::Numbers
{
    // Application domain: financial analysis.
    // Input: A is an array of numbers.
    // Output: Span (array).
    // Given the index x of A, the span of A[x] is maximum number of consecutive preceding elements
    //   A[x - 1], A[x - 2], .., such that A[y] <= A[x], where 0 <= y < x.
    //  Let A = {9, 5, 3, 5, 8, 1}.
    //  Span =  {0, 0, 0, 2, 3, 0}.
    struct span_calculator final
    {
        span_calculator() = delete;

        template<class int_t, std::size_t extent = std::dynamic_extent>
        static constexpr void calc_span_fast(const std::span<int_t, extent> numbers, std::vector<std::size_t> &result)
        {
            throw_if_null("data", numbers.data());

            const auto size = require_positive(numbers.size(), "size");
            result.resize(size);

            std::stack<std::size_t> indexes;

            for (std::size_t index{}; index < size; ++index)
            {
                while (!indexes.empty() && !(numbers[index] < numbers[indexes.top()]))
                {
                    indexes.pop();
                }

                const auto span1 = indexes.empty() ? index : index - 1U - indexes.top();

                result.at(index) = span1;
                indexes.push(index);
            }
        }

        template<class int_t, std::size_t extent = std::dynamic_extent>
        static constexpr void calc_span_simple(
            const std::span<int_t, extent> numbers, std::vector<std::size_t> &result)
        {
            throw_if_null("data", numbers.data());

            const auto size = require_positive(numbers.size(), "size");
            result.resize(size);
            result[0] = {};

            for (std::size_t index = 1; index < size; ++index)
            {
                auto &span1 = result[index];
                span1 = {};

                for (auto ind_2 = index - 1U;;)
                {
                    if (numbers[index] < numbers[ind_2] || 0U == ind_2--)
                    {
                        break;
                    }

                    ++span1;
                }
            }
        }
    };
} // namespace Standard::Algorithms::Numbers
