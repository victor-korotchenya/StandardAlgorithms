#pragma once
#include"binary_tree_utilities.h"
#include<vector>

namespace Standard::Algorithms::Trees
{
    // Set value on an interval [leftInclusive, rightExclusive);
    // get "sum" at a point.
    template<class int_t, class operation_t>
    struct segment_tree_reverted final
    {
        constexpr explicit segment_tree_reverted(std::size_t initial_size, operation_t operation = {},
            // 0 for addition, 1 for multiplication.
            int_t zero = {})
            : Data(binary_tree_utilities<>::calc_tree_size(initial_size))
            , Operation(operation)
            , Zero(zero)
            , Max_index_exclusive(initial_size)
        {
            assert(0U < Max_index_exclusive);
        }

        constexpr explicit segment_tree_reverted(
            const std::vector<int_t> &data, operation_t operation = {}, int_t zero = {})
            : segment_tree_reverted(data.size(), operation, zero)
        {
            std::copy(data.cbegin(), data.cend(), Data.begin() + data.size());

            // All data are ready already.
        }

        [[nodiscard]] constexpr auto max_index_exclusive() const noexcept -> std::size_t
        {
            assert(0U < Max_index_exclusive);

            return Max_index_exclusive;
        }

        [[nodiscard]] constexpr auto data() const &noexcept -> const std::vector<int_t> &
        {
            assert(0U < Max_index_exclusive);

            return Data;
        }

        // Return the aggregate at a point index.
        [[nodiscard]] constexpr auto get(std::size_t index) const -> int_t
        {
            assert(0U < Max_index_exclusive);

            binary_tree_utilities<>::check_index(index, Max_index_exclusive, "segment_tree_reverted::get");

            index += Max_index_exclusive;

            auto result = Zero;

            do
            {
                result = Operation(result, Data[index]);
                index >>= 1U;
            } while (0U < index);

            return result;
        }

        //          1
        //       /     \;
        //      2       3
        //    /  \      /\;
        //   4    5    6  7
        //  /\;
        // 8  9
        constexpr void set(std::size_t left_inclusive, std::size_t right_exclusive, const int_t &value)
        {
            assert(0U < Max_index_exclusive);

            binary_tree_utilities<>::check_indexes(
                left_inclusive, "segment_tree_reverted::set", right_exclusive, Max_index_exclusive);

            left_inclusive += Max_index_exclusive;
            right_exclusive += Max_index_exclusive;

            while (left_inclusive < right_exclusive)
            {
                if ((left_inclusive & 1U) != 0U) // "leftInclusive" is right child - its left sibling must be excluded.
                {
                    auto &datum = Data[left_inclusive];
                    datum = Operation(datum, value);
                    ++left_inclusive; // Go to the right sibling parent.
                }

                // else both are included - go to the parent.
                left_inclusive >>= 1U;

                if ((right_exclusive & 1U) != 0U) // "rightExclusive" is right child - include its left sibling.
                {
                    auto &datum = Data[right_exclusive ^ 1U];
                    datum = Operation(value, datum);
                }

                // Just go to the parent as "rightExclusive" means not inclusive.
                right_exclusive >>= 1U;
            }
        }

        // To inspect all the items in O(n), push all the changes to the leaves
        // and then get the items starting with the index Max_index_exclusive().
        //
        // It works only! if
        //  the order of element modifications doesn't affect the result.
        // Assignment, for example, isn't good.
        // Use the lazy propagation.
        constexpr void push_changes_to_leaves()
        {
            assert(0U < Max_index_exclusive);

            std::size_t parent{};

            while (++parent < Max_index_exclusive)
            {
                auto &par_datum = Data[parent];

                {
                    auto &left = Data[binary_tree_utilities<>::left_child1(parent)];
                    left = Operation(left, par_datum);
                }
                {
                    auto &right = Data[binary_tree_utilities<>::right_child1(parent)];
                    right = Operation(par_datum, right);
                }

                par_datum = Zero;
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
