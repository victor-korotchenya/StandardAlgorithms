#pragma once
#include"../Utilities/require_utilities.h"

namespace Standard::Algorithms::Numbers
{
    // Input: A is an unsorted array of numbers from [1..n],
    // having two repeating distinct items.
    //
    // E.g. A = {1, 2,2, 3, 4,4, 5}, size=7, n=size - 2=5.
    // Output: 2 and 4.
    //
    // Example 2: A = {1, 2,2, 3,3 }, size=5, n=size - 2=3.
    // Output: 2, 3.
    //
    // Another example: A = {1, 2, 3,3, 4, 5, 6, 7, 8,8 }, size=10, n=size - 2=8.
    // Output: 3, 8.
    //
    // Yet another example: A = {1,1, 2,2 }, size=4, n=size - 2=2.
    // Output: 1, 2.
    template<std::unsigned_integral int_t>
    constexpr void find_two_repeating_distinct_items_slow(
        const int_t *const numbers, const int_t &size, int_t &number1, int_t &number2)
    {
        throw_if_null("numbers", numbers);

        {
            constexpr auto min_size = 4U;

            require_less_equal(min_size, size, "size");
        }

        const auto max_number = static_cast<int_t>(size - 2U);

        // To compensate when index == size - 1.
        const auto initial_xor_data = static_cast<int_t>(size - 1U);

        auto xor_data = initial_xor_data;

        for (int_t index{}; index < size; ++index)
        {
            const auto &numb = require_positive(numbers[index], "numb");

            require_less_equal(numb, max_number, "numb");

            xor_data ^= static_cast<int_t>(numb ^ index);
        }

        require_positive(xor_data, "xor_data");

        const auto lowest_bit = static_cast<int_t>(xor_data & (int_t{} - xor_data));

        number1 = int_t{} < (lowest_bit & initial_xor_data) ? initial_xor_data : int_t{};

        for (int_t index{}; index < size; ++index)
        {
            const auto &numb = numbers[index];

            // Divide into two sets by "lowest_bit" bit number: 0 or 1?
            if (int_t{} < (lowest_bit & numb))
            {
                number1 ^= numb;
            }

            if (int_t{} < (lowest_bit & index))
            {
                number1 ^= index;
            }
        }

        number2 = static_cast<int_t>(number1 ^ xor_data);
    }
} // namespace Standard::Algorithms::Numbers
