#pragma once
#include"../Utilities/require_utilities.h"
#include"binary_tree_utilities.h"
#include<vector>

namespace Standard::Algorithms::Trees::Inner
{
    template<class int_t>
    constexpr auto set_lazy(const int_t &first,
        const int_t &second) -> int_t // todo(p4): -> const int_t&
    {
        // NOLINTNEXTLINE NOLINT
        if (first == int_t{})
        {
        }

        return second;
    }
} // namespace Standard::Algorithms::Trees::Inner

namespace Standard::Algorithms::Trees
{
    // For tests only - use segment_tree_add_sum or similar.
    template<class int_t, class operation_t, class add_operation_t = int_t (*)(const int_t &, const int_t &)>
    struct segment_tree_lazy final
    {
        constexpr explicit segment_tree_lazy(std::size_t initial_size, operation_t operation = {},
            // 0 for addition, 1 for multiplication.
            int_t zero = {}, add_operation_t add_operation = Inner::set_lazy<int_t>)
            : Data(binary_tree_utilities<>::calc_tree_size(initial_size), zero)
            , Lazy(initial_size, zero)
            , Operation(operation)
            , Add_operation(add_operation)
            , Zero(zero)
            , Max_index_exclusive(initial_size)
            , Height(require_positive(
                  binary_tree_utilities<>::calc_tree_height(initial_size), "Height in segment_tree_lazy()."))
        {
            assert(is_valid());
        }

        constexpr explicit segment_tree_lazy(const std::vector<int_t> &data, operation_t operation = {},
            int_t zero = {}, add_operation_t add_operation = Inner::set_lazy<int_t>)
            : segment_tree_lazy(data.size(), operation, zero, add_operation)
        {
            assert(is_valid());

            std::copy(data.cbegin(), data.cend(), Data.begin() + data.size());

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
            assert(is_valid());

            return Max_index_exclusive;
        }

        constexpr void set(std::size_t index, const int_t &val)
        {
            assert(is_valid());

            set(index, index + 1U, val);
        }

        [[nodiscard]] constexpr auto get(std::size_t index) -> int_t
        {
            assert(is_valid());

            auto result = get(index, index + 1U);
            return result;
        }

        // constexpr void push_changes_to_leaves()
        //{ assert(is_valid());
        //    for (auto parent = 1; parent < MaxIndexExclusive; ++parent)
        //    {
        //        const auto left = binary_tree_utilities<>::left_child1(parent);
        //        const auto right = binary_tree_utilities<>::right_child1(parent);
        //        if (Zero != Lazy[parent])
        //        {
        //            value(left, Lazy[parent]);
        //            value(right, Lazy[parent]);
        //            Lazy[parent] = Zero;
        //        }

        //        Data[left] = Operation(Data[left], Data[parent]);
        //        Data[right] = Operation(Data[right], Data[parent]);
        //        Data[parent] = Zero;
        //    }
        //}

        // The "index" starts from 0.
        constexpr void set(std::size_t left_inclusive, std::size_t right_exclusive, const int_t &val)
        {
            assert(is_valid());

            binary_tree_utilities<>::check_indexes(
                left_inclusive, "segment_tree_lazy::set", right_exclusive, Max_index_exclusive);

            left_inclusive += Max_index_exclusive;
            right_exclusive += Max_index_exclusive;

            const auto left_old = left_inclusive;
            const auto right_inc = right_exclusive - 1U;

            push_lazy_down(left_inclusive);
            push_lazy_down(right_inc);

            while (left_inclusive < right_exclusive)
            {
                if ((left_inclusive & 1U) != 0U)
                {
                    value(left_inclusive++, val);
                }

                if ((right_exclusive & 1U) != 0U)
                {
                    value(right_exclusive - 1U, val);
                }

                left_inclusive >>= 1U;
                right_exclusive >>= 1U;
            }

            restore_up(left_old);
            restore_up(right_inc);
        }

        // Return the aggregate on the interval [leftInclusive, rightExclusive).
        [[nodiscard]] constexpr auto get(std::size_t left_inclusive, std::size_t right_exclusive) -> int_t
        {
            assert(is_valid());

            binary_tree_utilities<>::check_indexes(
                left_inclusive, "segment_tree_lazy::get", right_exclusive, Max_index_exclusive);

            left_inclusive += Max_index_exclusive;
            right_exclusive += Max_index_exclusive;

            push_lazy_down(left_inclusive);
            push_lazy_down(right_exclusive - 1U);

            auto result = Zero;

            while (left_inclusive < right_exclusive)
            {
                if ((left_inclusive & 1U) != 0U)
                {
                    result = Operation(result, Data[left_inclusive++]);
                }

                if ((right_exclusive & 1U) != 0U)
                {
                    result = Operation(Data[right_exclusive - 1U], result);
                }

                left_inclusive >>= 1U;
                right_exclusive >>= 1U;
            }

            return result;
        }

private:
        [[nodiscard]] constexpr auto is_valid() const noexcept
        {
            return 0U < Max_index_exclusive && 0U < Height;
        }

        constexpr void restore_up(std::size_t parent)
        {
            while (1U < parent)
            {
                parent >>= 1U;

                auto &laz = Lazy[parent];
                const auto is_empty = Zero == laz;

                auto &left = Data[parent << 1U];
                auto &right = Data[(parent << 1U) | 1U];

                Data[parent] =
                    Operation(is_empty ? left : Add_operation(left, laz), is_empty ? right : Add_operation(right, laz));
            }
        }

        constexpr void push_lazy_down(const std::size_t index)
        {
            auto hei = Height;

            do
            {
                assert(0U < hei);

                const auto parent = index >> hei;
                auto &laz = Lazy[parent];

                if (Zero == laz)
                {
                    continue;
                }

                value(parent << 1U, laz);
                value((parent << 1U) | 1U, laz);

                laz = Zero;
            } while (0U < --hei);
        }

        constexpr void value(const std::size_t index, const int_t &val)
        {
            auto &datum = Data[index];

            datum = Add_operation(datum, val);

            if (index < Max_index_exclusive)
            {
                auto &laz = Lazy[index];
                laz = Add_operation(laz, val);
            }
        }

        std::vector<int_t> Data;
        std::vector<int_t> Lazy;
        operation_t Operation;
        add_operation_t Add_operation;
        const int_t Zero;
        const std::size_t Max_index_exclusive
            // False warning: constructor does not initialize these fields.
            {};
        const std::size_t Height
            // False warning: constructor does not initialize these fields.
            {};
    };
} // namespace Standard::Algorithms::Trees
