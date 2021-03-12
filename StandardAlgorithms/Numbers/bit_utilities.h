#pragma once
#include<cassert>
#include<type_traits>
#include<vector>
#include "Arithmetic.h"

namespace Privet
{
    namespace Algorithms
    {
        namespace Numbers
        {
            // Find the smallest greater number with the same number of binary 1-bits.
            // Zero is not valid input.
            // Watch out for an overflow!
            template<class number_t>
            number_t next_greater_same_pop_count(const number_t value)
            {
                static_assert(std::is_unsigned_v<number_t>);
                assert(value);

                const number_t low = lowest_bit<number_t>(value),
                    temp = value + low,
                    result = (((temp ^ value) >> 2) / low) | temp;
                assert(result && value != result);
#if _DEBUG
                const auto bits0 = PopCount(value);
                const auto bits1 = PopCount(result);
                assert(bits0 > 0 && bits0 == bits1);
#endif
                return result;
            }

            template<class number_t>
            number_t next_smaller_same_pop_count(const number_t value)
            {
                static_assert(std::is_unsigned_v<number_t>);
                assert(value);

                const auto result = ~next_greater_same_pop_count(~value);
                assert(result && value != result);
#if _DEBUG
                const auto bits0 = PopCount(value);
                const auto bits1 = PopCount(result);
                assert(bits0 > 0 && bits0 == bits1);

                const auto temp = next_greater_same_pop_count(result);
                assert(value == temp);
#endif
                return result;
            }

            // Given 6, return 2.
            // 7 -> 1
            // 12 -> 4
            // 0 -> 0
            template<class number_t>
            number_t lowest_bit(const number_t a)
            {
                const auto result = a & (number_t() - a);
                return result;
            }

            // Start from a, then in the decreasing order.
            // E.g. given 6, return {6,4,2}.
            template<class number_t>
            std::vector<number_t> enumerate_all_subsets_of_mask(const number_t a)
            {
                std::vector<number_t> result;
                if (a)
                    result.reserve(1llu << PopCount(a));

                for (auto i = a; i; i = (i - 1u) & a)
                    result.push_back(i);

                return result;
            }
        }
    }
}