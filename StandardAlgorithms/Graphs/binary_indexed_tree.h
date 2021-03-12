#pragma once
#include"../Utilities/is_debug.h"
#include"../Utilities/throw_exception.h"
#include<vector>

namespace Standard::Algorithms::Trees
{
    // Fenwick tree running time is O(log(N)), where N is the input size.
    // It is ideal when 1) adding a number to a slot,
    // and then 2) calculating the sum between consecutive slots i to j, where i <= j.
    // It cannot be directly used to compute GCD, LCM, MIN, MAX.
    // A linear space is required.
    //
    // A node stores (the value of itself) + (its left subtree).
    //
    // The indexes start from 1.
    // Get up, while(0 != index), index decreases.
    // Set, while (index < size), - down (add the right-most 1-bit).
    //               8
    //           /       \;
    //       4              12
    //     /   \          /   \;
    //    2     6       10     14
    //   / \   / \     / \    / \;
    //  1  3   5  7   9  11  13  15
    //
    //      7 values:
    // [5]  [1]  [15]  [11]  [52]  [28] [0]
    //      Sums:
    // [5]  [6]  [21]  [32]  [84]  [112] [112]
    // [+5] [+1] [+15] [+11] [+52] [+28] [+0]
    //   1   2     3     4     5     6     7
    //
    // After:
    //       4 [+32]
    //       /      \;
    //   2 [+6]     6 [+80]
    //    /   \       /     \;
    // 1[+5] 3[+15]  5[+52] 7[+0]
    template<class int_t, class operation_t, class reverse_operation_t = operation_t>
    class binary_indexed_tree final
    {
        std::vector<int_t> Data;
        operation_t Oper;
        reverse_operation_t Rev_ope;
        const int_t Zero;

public:
        static constexpr std::size_t initial_index = 1;

        constexpr explicit binary_indexed_tree(std::size_t initial_size = {}, operation_t operation = {},
            reverse_operation_t reverse_operation = {},
            // E.g. 0 for addition, 1 for multiplication.
            int_t zero = {})
            : Data(0U == initial_size ? 0U // For empty collections.
                                      : initial_size + initial_index,
                  zero)
            , Oper(operation)
            , Rev_ope(reverse_operation)
            , Zero(zero)
        {
        }

        // Return the sum from 1 to the "index".
        [[nodiscard]] constexpr auto get(std::size_t index) const -> int_t
        {
            check_index(index);

            auto result = Zero;
            do
            {
                result = Oper(result, Data[index]);
                index &= index - 1U; // Remove the right-most 1-bit.
            } while (0U < index);

            return result;
        }

        constexpr void set(std::size_t index, const int_t &value = 1)
        {
            check_index(index);

            const auto size = Data.size();

            do
            {
                Data[index] = Oper(Data[index], value);
                index += index & (0U - index); // Add the right-most 1-bit.
            } while (index < size);
        }

        // Return the maximum supported index.
        [[nodiscard]] constexpr auto max_index() const noexcept -> std::size_t
        {
            return Data.size() - initial_index;
        }

        // When "leftInclusive" is ether 0 or 1,
        // the sum is taken from the beginning to the "rightInclusive".
        // Otherwise, the returned sum is taken between indexes inclusively.
        [[nodiscard]] constexpr auto get(std::pair<std::size_t, std::size_t> from_to_inclusive) const -> int_t
        {
            auto left_inclusive = from_to_inclusive.first;
            auto right_inclusive = from_to_inclusive.second;

            if constexpr (::Standard::Algorithms::is_debug)
            {
                if (right_inclusive < left_inclusive) [[unlikely]]
                {
                    auto str = ::Standard::Algorithms::Utilities::w_stream();
                    str << "The rightInclusive " << right_inclusive << " cannot be smaller than leftInclusive "
                        << left_inclusive << ", size " << Data.size() << ".";

                    throw_exception<std::out_of_range>(str);
                }
            }

            auto val = get(right_inclusive);
            auto result = left_inclusive <= initial_index ? val : Rev_ope(val, get(left_inclusive - initial_index));

            return result;
        }

        // Return the scalar value at "index",
        //  which is semantically equivalent to get(index, index),
        //  but can run faster.
        [[nodiscard]] constexpr auto value_at(std::size_t index) const -> int_t
        {
            check_index(index);

            auto result = Data[index];

            const auto stop_index = index & (index - 1U); // Remove the right-most 1-bit.
            --index;

            while (stop_index != index)
            {
                result = Rev_ope(result, Data[index]);
                index &= index - 1U;
            }

            return result;
        }

private:
        constexpr void check_index(const std::size_t index) const
        {
            if (index == 0U || Data.size() <= index) [[unlikely]]
            {
                auto str = ::Standard::Algorithms::Utilities::w_stream();
                str << "The index (" << index << ") must be between " << initial_index << " and "
                    << (Data.size() - initial_index) << ".";

                throw_exception<std::out_of_range>(str);
            }
        }
    };
} // namespace Standard::Algorithms::Trees
