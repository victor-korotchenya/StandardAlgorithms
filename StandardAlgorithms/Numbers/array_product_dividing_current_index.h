#pragma once
#include"../Utilities/require_utilities.h"
#include<vector>

namespace Standard::Algorithms::Numbers
{
    // Given an array A[0..(n-1)], calculate another array B[0..(n-1)],
    // where  B[i] = PROD(A[j] for all j) / A[i].
    // A[i] can be zero.
    // Overflow check is not performed.
    template<std::integral int_t>
    [[nodiscard]] constexpr auto array_product_dividing_current_index(const std::vector<int_t> &data)
        -> std::vector<int_t>
    {
        const auto size = require_greater(data.size(), 1U, "data size");

        std::vector<int_t> result(size);
        result[0] = 1;

        for (std::size_t index = 1; index < size; ++index)
        {
            result[index] = result[index - 1U] * data[index - 1U];
        }

        // 0 1 2 3 4
        // A B C D E
        // 1 A AB ABC ABCD
        //            ABCD - it is already done
        //        ABC      - to be multiplied by E.

        int_t temp = 1;
        auto ind_2 = size - 2U;

        do
        {
            temp *= data[ind_2 + 1U];
            result[ind_2] *= temp;
        } while (0U < ind_2--);

        return result;
    }

    template<class int_t>
    [[nodiscard]] constexpr auto array_product_dividing_current_index_slow(const std::vector<int_t> &data)
        -> std::vector<int_t>
    {
        const auto size = require_greater(data.size(), 1U, "data size");

        std::vector<int_t> result(size);

        for (std::size_t index{}; index < size; ++index)
        {
            int_t prod = 1;

            for (std::size_t ind_2{}; ind_2 < size; ++ind_2)
            {
                if (index != ind_2)
                {
                    prod *= data[ind_2];
                }
            }

            result[index] = prod;
        }

        return result;
    }
} // namespace Standard::Algorithms::Numbers
