#pragma once
#include"binary_tree_utilities.h"

namespace Standard::Algorithms::Trees
{
    // Set value at a point,
    // get "sum/max" over an interval [leftInclusive, rightExclusive).
    //
    // Twice the linear space is required.
    // The actual data are stored at the higher indexes.
    // And the "sums" are stored at the lower indexes.
    // 0 based.
    template<class int_t, class operation_t>
    struct segment_tree final
    {
        constexpr explicit segment_tree(const std::size_t size, operation_t operation = {},
            // 0 for addition, 1 for multiplication.
            int_t zero = {})
            : Data(binary_tree_utilities<>::calc_tree_size(size), zero)
            , Operation(operation)
            , Zero(zero)
            , Max_index_exclusive(size)
        {
            assert(0U < Max_index_exclusive);
        }

        constexpr explicit segment_tree(const std::vector<int_t> &data, operation_t operation = {}, int_t zero = {})
            : segment_tree(data.size(), operation, zero)
        {
            std::copy(data.cbegin(), data.cend(), Data.begin() + data.size());

            init();
        }

        constexpr void init()
        {
            assert(0U < Max_index_exclusive);

            auto parent = Max_index_exclusive;

            while (0U < --parent)
            {
                auto &left = Data[binary_tree_utilities<>::left_child1(parent)];
                auto &right = Data[binary_tree_utilities<>::right_child1(parent)];

                Data[parent] = Operation(left, right);
            }
        }

        [[nodiscard]] constexpr auto max_index_exclusive() const noexcept -> std::size_t
        {
            assert(0U < Max_index_exclusive);

            return Max_index_exclusive;
        }

        [[nodiscard]] constexpr auto data() const &noexcept -> const std::vector<int_t> &
        {
            return Data;
        }

        [[nodiscard]] constexpr auto data() &noexcept -> std::vector<int_t> &
        {
            return Data;
        }

        // MaxIndexExclusive = 5
        // [0, 1, 2, 3, 4] are stored at indexes [5, 6, 7, 8, 9].
        //          1
        //       /     \;
        //      2       3
        //    /  \      /\;
        //   4    5    6  7
        //  /\;
        // 8  9
        //
        // In the constructor(), "parent" goes from 4 down to 1:
        // a[4] = a[8]+a[9]; then [3] = [6]+[7]; [2] = [4]+[5]; [1] = [2]+[3];
        //
        // Assume, get(0, 5) is being called.
        // After adding MaxIndexExclusive: left=5, right=10.
        //  result_sum += [5]; left=(5+1)/2=3; right=10/2=5;
        // left=3; right=5;
        //  result_sum += [3] + [5-1] == [3] + [4];
        // Stop as left == right == 2.
        // That is, result_sum = [5]+ ([3] + 4).

        // Return the aggregate on the interval [leftInclusive, rightExclusive).
        // The "index" starts from 0.
        [[nodiscard]] constexpr auto get(std::size_t left_inclusive, std::size_t right_exclusive) const -> int_t
        {
            binary_tree_utilities<>::check_indexes(
                left_inclusive, "segment_tree::get", right_exclusive, Max_index_exclusive);

            left_inclusive += Max_index_exclusive;
            right_exclusive += Max_index_exclusive;

            auto result = Zero;

            while (left_inclusive < right_exclusive)
            {
                if ((left_inclusive & 1U) !=
                    0U) // "leftInclusive" is right child - its left sibling must not be included.
                {
                    result = Operation(result, Data[left_inclusive]);
                    ++left_inclusive; // Go to the right sibling parent.
                }
                // else both are included - go to the parent.

                left_inclusive >>= 1U;

                if ((right_exclusive & 1U) != 0U) // "rightExclusive" is right child - include its left sibling.
                {
                    result = Operation(Data[right_exclusive ^ 1U], result);
                }

                // Go to the parent as "rightExclusive" means not inclusive.
                right_exclusive >>= 1U;
            }

            return result;
        }

        // The "index" starts from 0.
        constexpr void set(std::size_t index, const int_t &value)
        {
            binary_tree_utilities<>::check_index(index, Max_index_exclusive, "segment_tree::set");

            index += Max_index_exclusive;
            Data[index] = value;

            while (1U < index)
            {
                index = binary_tree_utilities<>::parent1(index);

                auto &left = Data[binary_tree_utilities<>::left_child1(index)];
                auto &right = Data[binary_tree_utilities<>::right_child1(index)];

                Data[index] = Operation(left, right);
            }
        }

private:
        std::vector<int_t> Data;
        operation_t Operation;
        const int_t Zero;

        const std::size_t Max_index_exclusive
            // False warning: constructor does not initialize these fields.
            {};
    };
} // namespace Standard::Algorithms::Trees
