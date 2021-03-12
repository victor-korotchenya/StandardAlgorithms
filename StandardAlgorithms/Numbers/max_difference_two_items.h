#pragma once
#include"../Utilities/require_utilities.h"
#include<vector>

namespace Standard::Algorithms::Numbers
{
    // Given an array A, return Max({ A[max_index] - A[min_index] }),
    // where min_index < max_index.
    // Time O(n).
    template<std::signed_integral int_t,
        // To check for an overflow.
        // using SubtractFunction = int_t(*)(const int_t&one, const int_t& two);
        class subtract_t>
    [[nodiscard]] constexpr auto max_difference_two_items(subtract_t subtract, const std::vector<int_t> &numbers)
        -> std::pair<int_t, std::pair<std::size_t, std::size_t>>
    {
        throw_if_null_skip_non_pointer("subtract function", subtract);

        const auto size = require_greater(numbers.size(), 1U, "size");

        auto result = subtract(numbers[1], numbers[0]);
        std::size_t min_index{};
        std::size_t max_index = 1;

        std::size_t min_pos{};
        std::size_t position = 1;

        while (position < size && !(numbers[position - 1U] < numbers[position]))
        {// Going down.
            const auto &cur = numbers[position];

            if (const auto &mini = numbers[min_pos]; cur < mini)
            {
                min_pos = position;
            }

            const auto &prev = numbers[position - 1U];

            if (auto temp = subtract(cur, prev); result < temp)
            {
                result = std::move(temp);
                min_index = position - 1U;
                max_index = position;
            }

            ++position;
        }

        while (position < size)
        {// There must have been at least 1 pair of the consecutive increasing numbers.
            const auto &mini = numbers[min_pos];
            const auto &cur = numbers[position];

            if (cur < mini)
            {
                min_pos = position;
            }
            else if (auto temp = subtract(cur, mini); result < temp)
            {
                result = std::move(temp);
                min_index = min_pos;
                max_index = position;
            }

            assert(int_t{} < result);

            ++position;
        }

        return { result, { min_index, max_index } };
    }

    // Slow time O(n*n).
    template<std::signed_integral int_t, class subtract_t>
    [[nodiscard]] constexpr auto max_difference_two_items_slow(subtract_t subtract, const std::vector<int_t> &numbers)
        -> std::pair<int_t, std::pair<std::size_t, std::size_t>>
    {
        throw_if_null_skip_non_pointer("subtract function", subtract);

        const auto size = require_greater(numbers.size(), 1U, "size");

        auto result = subtract(numbers[1], numbers[0]);
        std::size_t min_index{};
        std::size_t max_index = 1;

        std::size_t index{};

        do
        {
            auto ind_2 = index + 1U;

            assert(std::max(index, ind_2) < size);

            do
            {
                auto temp = subtract(numbers[ind_2], numbers[index]);

                if (result < temp)
                {
                    result = std::move(temp);
                    min_index = index;
                    max_index = ind_2;
                }
            } while (++ind_2 < size);
        } while (++index < size - 1U);

        return { result, { min_index, max_index } };
    }
} // namespace Standard::Algorithms::Numbers
