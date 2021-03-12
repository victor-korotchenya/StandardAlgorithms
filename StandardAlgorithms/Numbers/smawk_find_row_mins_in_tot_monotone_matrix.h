#pragma once
// This is not an ObjC file!
// Error processing file: Clang format file errors:
// Configuration file(s) do(es) not support Objective-C: .clang-format

#include"../Utilities/iota_vector.h"
#include"../Utilities/require_utilities.h"
#include<numeric>
#include<unordered_map>

namespace Standard::Algorithms::Numbers::Inner
{
    template<class lookup_t>
    [[nodiscard]] constexpr auto leave_min_columns(const std::vector<std::size_t> &rows, const lookup_t &lookup,
        const std::vector<std::size_t> &cols) -> std::vector<std::size_t>
    {
        assert(!rows.empty() && !cols.empty());

        // todo(p3): use 'cols', remove 'min_columns'.
        std::vector<std::size_t> min_columns;

        for (const auto &col : cols)
        {
            while (!min_columns.empty())
            {
                const auto ind = min_columns.size() - 1U;
                const auto &col2 = min_columns[ind];
                const auto &row = rows[ind];

                if (!(lookup(row, col) < lookup(row, col2)))
                {
                    break;
                }

                min_columns.pop_back();
            }

            if (min_columns.size() < rows.size())
            {
                min_columns.push_back(col);
            }
        }

        return min_columns;
    }

    template<class lookup_t>
    constexpr void smawk_find_row_mins_rec(const std::vector<std::size_t> &rows, const lookup_t &lookup,
        const std::vector<std::size_t> &cols, std::unordered_map<std::size_t, std::size_t> &col_index,
        std::vector<std::size_t> &result)
    {
        assert(!rows.empty() && !cols.empty());

        const auto left_antidiagonal_columns = leave_min_columns<lookup_t>(rows, lookup, cols);

        if (1U < rows.size())
        {
            std::vector<std::size_t> odd_rows(rows.size() >> 1U);

            for (std::size_t index = 1; index < rows.size(); index += 2U)
            {
                odd_rows[index >> 1U] = rows[index];
            }

            smawk_find_row_mins_rec<lookup_t>(odd_rows, lookup, left_antidiagonal_columns, col_index, result);
        }

        col_index.clear();

        for (std::size_t index{}; index < left_antidiagonal_columns.size(); ++index)
        {
            const auto &col = left_antidiagonal_columns[index];
            col_index[col] = index;
        }

        for (std::size_t r_even{}, start{}; r_even < rows.size(); r_even += 2U)
        {
            const auto &row = rows[r_even];
            // std::cout << " row " << row << "\n";
            //
            // 9 rows will be processed in the following order:
            // 7, 3,
            // then
            // 1, 5
            // and lastly
            // 0, 2, 4, 6, 8.
            //
            // So, the odd position rows in [0..9) are {1, 3, 5, 7}
            // of which the odd position rows are {1, 3, 5, 7}
            // of which the odd position rows are {3, 7}
            // of which the odd position rows are {7}.
            //
            // Having finished with {1, 3, 5, 7},
            // for example, to process row 2 will require enumerating the columns
            // that are in between those of row 1 and row 3.

            auto &col_min = result[row];
            col_min = left_antidiagonal_columns[start];

            auto val_min = lookup(row, col_min);

            const auto stop = r_even + 1U < rows.size() ? col_index[result[rows[r_even + 1U]]]
                                                        : left_antidiagonal_columns.size() - 1U;

            for (auto col = start + 1U; col <= stop; ++col)
            {
                const auto &laco = left_antidiagonal_columns[col];
                const auto &value = lookup(row, laco);

                if (value < val_min)
                {
                    val_min = value;
                    col_min = laco;
                }
            }

            start = stop;
        }
    }
} // namespace Standard::Algorithms::Numbers::Inner

namespace Standard::Algorithms::Numbers
{
    // Given a totally monotone matrix, SMAWK algorithm finds each row min.
    //
    // In a monotone matrix, row min is in a column >= previous row min column.
    //
    // A submatrix is a matrix with optionally deleted rows and/or columns.
    //
    // A totally monotone matrix has the above for every submatrix, or a 2-by-2 submatrix,
    // and it is impossible to have row min in top-right and bottom-left positions.
    //
    // Monge array arr satisfies:
    // (arr[row, col] + arr[row2, col2] <= arr[row2, col] + arr[row, col2])
    // when (row < row2 && col < col2).
    //
    // Monge matrix is totally monotone: in a 2-by-2 submatrix where the sum along the main diagonal
    // is < the sum of 2 anti-diagonal elements.
    // E.g. {{4,3}, {8,6}}, and {{4,8}, {3,6}}, also {{6,3}, {8,4}}, and {{6,8}, {3,4}}.
    //
    // Time O(cols + rows + cols*log(rows/cols))).
    template<class t>
    [[nodiscard]] constexpr auto smawk_find_row_mins_in_tot_monotone_matrix(const std::vector<t> &arr)
        -> std::vector<std::size_t>
    {
        const auto size = require_positive(arr.size(), "arr size");
        const auto row0_size = require_positive(arr.at(0).size(), "arr[0].size");

        for (const auto &line : arr)
        {
            require_equal(row0_size, "columns in a row", line.size());
        }

        [[maybe_unused]] std::uint64_t calls{};

        // This is not an ObjC file, yet Clang format fails on the following line:
        // const auto lookup = [&arr, &calls](const std::size_t row1, const std::size_t col2) -> const auto&
        // Solution: const auto lookup = [&] ..
        const auto lookup = [&](const std::size_t row1, const std::size_t col2) -> const auto &
        {
            ++calls;
            return arr[row1][col2];
        };
        using lookup_t = decltype(lookup);

        std::vector<std::size_t> result_cols(size);
        std::unordered_map<std::size_t, std::size_t> col_index;

        auto rows = ::Standard::Algorithms::Utilities::iota_vector<std::size_t>(size);
        auto cols = ::Standard::Algorithms::Utilities::iota_vector<std::size_t>(row0_size);

        Inner::smawk_find_row_mins_rec<lookup_t>(rows, lookup, cols, col_index, result_cols);

        return result_cols;
    }

    // Slow time O(rows*cols).
    template<class t>
    [[nodiscard]] constexpr auto find_row_mins_in_2d_matrix_slow(const std::vector<std::vector<t>> &arr)
        -> std::vector<std::size_t>
    {
        const auto rows = require_positive(arr.size(), "arr size");
        const auto cols = require_positive(arr.at(0).size(), "arr[0] size");

        std::vector<std::size_t> result(rows);

        for (std::size_t row{}; row < rows; ++row)
        {
            const auto &line = arr[row];
            require_equal(cols, "columns in a row", line.size());

            result[row] = static_cast<std::size_t>(std::min_element(line.cbegin(), line.cend()) - line.cbegin());
        }

        return result;
    }

    // The input must be a matrix, where each row has the same number of columns.
    // Try all non-zero-area rectangles: min value column cannot move left on the next row.
    // Slow time O(m*m*n*n).
    template<class t>
    [[nodiscard]] constexpr auto is_totally_monotonic_slow(const std::vector<std::vector<t>> &arr) -> bool
    {
        const auto size = arr.size();
        if (size <= 1U)
        {
            return true;
        }

        const auto row0_size = require_positive(arr[0].size(), "0-th row size in is_totally_monotonic_slow");

        if (row0_size == 1U)
        {
            return true;
        }

        for (std::size_t row{}; row < size - 1U; ++row)
        {
            assert(row0_size == arr[row].size());

            for (auto row_next = row + 1U; row_next < size; ++row_next)
            {
                for (std::size_t col{}; col < row0_size - 1U; ++col)
                {
                    const auto &v00 = arr[row][col];
                    const auto &v10 = arr[row_next][col];

                    for (auto col_next = col + 1U; col_next < row0_size; ++col_next)
                    {
                        if (const auto &v11 = arr[row_next][col_next]; v11 < v10)
                        {// The min could only go right.
                            continue;
                        }

                        if (const auto &v01 = arr[row][col_next]; v01 < v00)
                        {// The min has moved left - a violation.
                            return false;
                        }
                    }
                }
            }
        }

        return true;
    }
} // namespace Standard::Algorithms::Numbers
