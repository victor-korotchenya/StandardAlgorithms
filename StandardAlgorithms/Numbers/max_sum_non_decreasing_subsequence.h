#pragma once
#include"../Utilities/is_debug.h"
#include"../Utilities/remove_duplicates.h"
#include"../Utilities/same_sign_leq_size.h"
#include"binary_search.h"
#include<tuple>

namespace Standard::Algorithms::Numbers
{
    // Given an array "values" of n>0 positive integers,
    // return an array "result" of n integers,
    // such that the total sum of the numbers in the "result" is maximized,
    // and for each index "i" in the range [0, n-1]
    // the following constraints hold:
    // 1) 0 <= result[i] <= values[i];
    // 2) all positive numbers in the "result" form a non-decreasing subsequence.
    //
    // For example, given { 3, 5 }, the result will be { 3, 5 }
    // as the result is non-decreasing, and cannot be beaten when everything is included.
    // { 5, 3 } return { 3, 3 } because (3+3) is greater than (5+0).
    // Note that { 5, 3 } is not a valid result because (5 <= 3) does not hold.
    // Also, { 5, 5 } is invalid as the second 5 must not exceed 3.
    // { 5, 2 } return { 5, 0 } because (2+2) is smaller than (5+0).
    // { 5, 2, 7 } return { 5, 0, 7 } because (5+0+7) is greater than (2+2+7).
    //
    // Here is a longer example: { 40, 20, 30, 10, 20, 50, 30, 100, 20 },
    // and the result: { 20, 20, 20, 0, 20, 30, 30, 100, 0 }.
    template<std::integral int_t, std::integral long_int_t>
    requires(same_sign_leq_size<int_t, long_int_t>)
    struct max_sum_non_decreasing_subsequence final
    {
        max_sum_non_decreasing_subsequence() = delete;

        // Time complexity is O(n*n*n).
        [[nodiscard]] static constexpr auto compute(const std::vector<int_t> &values) -> std::vector<int_t>
        {
            throw_if_empty("values", values);
            require_all_positive<int_t>(values, "values");

            const auto size = values.size();

            if (1U == size)
            {
                return values;
            }

            auto unique_values = values;
            Standard::Algorithms::sort_remove_duplicates(unique_values);

            const auto computed = compute_matrix(values, unique_values);

            assert(size == std::get<0>(computed).size());

            auto result = restore_result(unique_values, computed);
            return result;
        }

private:
        // Max sum, previous row, previous column.
        using sum_row_col_t = std::tuple<long_int_t, std::size_t, std::size_t>;
        using matrix_t = std::vector<std::vector<sum_row_col_t>>;

        // matrix, best sum, best row(index), best column(height).
        using computed_tuple = std::tuple<matrix_t, long_int_t, std::size_t, std::size_t>;

        static constexpr auto value_id = 0;
        static constexpr auto row_id = 1;
        static constexpr auto column_id = 2;

        // Time complexity O(log(n)).
        [[nodiscard]] static constexpr auto get_index(const std::vector<int_t> &sorted_values, const int_t &value)
            -> std::size_t
        {
            const auto size = sorted_values.size();
            const auto data = sorted_values.data();
            const auto spa = std::span(data, size);
            const auto *const found = binary_search(spa, value);

            const auto index = static_cast<std::size_t>(found - data);

            if (found != nullptr && index < size) [[likely]]
            {
                return index;
            }

            auto str = ::Standard::Algorithms::Utilities::w_stream();
            str << "Error in get_index: size " << size << " <= index " << index << ", value " << value << ".";

            throw_exception(str);
        }

        // It does not make sense to try to enumerate all possible heights.
        // Instead, list only the existing, not greater ones.
        static constexpr void try_smaller_heights(const long_int_t &current_value, const std::size_t index,
            const std::vector<sum_row_col_t> &previous_row, std::size_t height, sum_row_col_t &row)
        {
            const auto &cur = std::get<value_id>(row);

            for (;;)
            {
                assert(0U < height);
                --height;

                const auto &prev = std::get<value_id>(previous_row[height]);

                if (const auto &cand = static_cast<long_int_t>(prev + current_value); // todo(p3): overflow check.
                    cur < cand)
                {
                    row = { cand, index - 1U, height };
                }

                if (0U == height)
                {
                    return;
                }
            }
        }

        [[nodiscard]] static constexpr auto compute_row(const std::size_t index,
            const std::vector<int_t> &unique_values, const std::size_t value_index, matrix_t &matrix) -> sum_row_col_t
        {
            const auto unique_values_size = unique_values.size();

            auto &row = matrix[index];
            assert(unique_values_size == row.size());

            long_int_t best_sum{};
            std::size_t best_index{};
            std::size_t best_height{};

            for (std::size_t height{}; height < unique_values_size; ++height)
            {
                const auto is_within = height <= value_index;
                const auto &current_value = unique_values[is_within ? height : value_index];

                auto &cur = row[height];

                if (0U == index)
                {// No previous value.
                    cur = { current_value, 0, 0 };
                }
                else
                {
                    const auto &previous_row = matrix[index - 1U];
                    const auto &prev_cell = previous_row[height];

                    const auto &previous_value = std::get<value_id>(prev_cell);

                    cur = is_within ? sum_row_col_t{ static_cast<long_int_t>(
                                                         previous_value + current_value), // todo(p3): overflow check.
                        index - 1U, height }
                                    : prev_cell;

                    if (is_within && 0U < height)
                    {
                        try_smaller_heights(current_value, index, previous_row, height, cur);
                    }
                }

                if (const auto &sum = std::get<value_id>(cur); best_sum < sum)
                {
                    best_sum = sum, best_index = index, best_height = height;
                }
            }

            return { best_sum, best_index, best_height };
        }

        // A cell in the "matrix" will contain the maximum total sum value for the given value and height,
        // and the pointer to one of the best previous cells
        // which can be used to backtrack the resulting array.
        [[nodiscard]] static constexpr auto compute_matrix(
            const std::vector<int_t> &values, const std::vector<int_t> &unique_values) -> computed_tuple
        {
            const auto size = values.size();

            matrix_t matrix(size, std::vector<sum_row_col_t>(unique_values.size()));

            long_int_t best_sum{};
            std::size_t best_index{};
            std::size_t best_height{};

            for (std::size_t index{}; index < size; ++index)
            {
                const auto value_index = get_index(unique_values, values[index]);
                const auto row_result = compute_row(index, unique_values, value_index, matrix);

                if (const auto &sum = std::get<value_id>(row_result); best_sum < sum)
                {
                    best_sum = sum;
                    best_index = std::get<row_id>(row_result);
                    best_height = std::get<column_id>(row_result);
                }
            }

            return { std::move(matrix), best_sum, best_index, best_height };
        }

        static constexpr void move_to_previous_row_column(
            std::size_t &index, const sum_row_col_t &record, std::size_t &height)
        {
            if (const auto new_index = std::get<row_id>(record); new_index < index)
            {
                index = new_index;
            }
            else
            {
                auto str = ::Standard::Algorithms::Utilities::w_stream();
                str << "Error in MoveToPreviousRowColumn: index must decrease " << index << ", but <= newIndex "
                    << new_index << ".";

                throw_exception(str);
            }

            if (const auto new_height = std::get<column_id>(record); new_height <= height)
            {
                height = new_height;
            }
            else
            {
                auto str = ::Standard::Algorithms::Utilities::w_stream();
                str << "Error in MoveToPreviousRowColumn: height cannot grow " << height << ", but < newHeight "
                    << new_height << ".";

                throw_exception(str);
            }
        }

        // Find the reminder cell having the lowest row/column.
        static constexpr void move_to_edge_value(
            std::size_t &index, const matrix_t &matrix, const long_int_t &reminder, std::size_t &height)
        {
            for (;;)
            {
                auto shall_break = true;

                while (0U < index && std::get<value_id>(matrix[index - 1U][height]) == reminder)
                {
                    --index;
                    shall_break = false;
                }

                while (0U < height && std::get<value_id>(matrix[index][height - 1U]) == reminder)
                {
                    --height;
                    shall_break = false;
                }

                if (shall_break)
                {
                    break;
                }
            }
        }

        [[nodiscard]] static constexpr auto restore_result(
            const std::vector<int_t> &unique_values, const computed_tuple &computed) -> std::vector<int_t>
        {
            const auto &matrix = std::get<0>(computed);
            auto reminder = std::get<1>(computed);
            auto index = std::get<2>(computed);
            auto height = std::get<3>(computed);

            const auto size = matrix.size();
            std::vector<int_t> result(size);

            for (;;)
            {
                assert(index < size && long_int_t{} < reminder);

                const auto &val = unique_values[height];
                result[index] = val;

                if constexpr (::Standard::Algorithms::is_debug)
                {
                    if (reminder < val)
                    {
                        auto str = ::Standard::Algorithms::Utilities::w_stream();
                        str << "Error in RestoreResult: reminder " << reminder << " < " << val << " uniqueValues["
                            << height << "].";

                        throw_exception(str);
                    }
                }

                reminder -= val;

                if (long_int_t{} == reminder)
                {
                    break;
                }

                move_to_previous_row_column(index, matrix[index][height], height);

                move_to_edge_value(index, matrix, reminder, height);
            }

            return result;
        }
    };
} // namespace Standard::Algorithms::Numbers
