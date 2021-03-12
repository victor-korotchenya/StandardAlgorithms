#pragma once
#include <numeric>
#include <unordered_map>
#include <vector>
#include "../Utilities/ExceptionUtilities.h"

namespace
{
    template<class t, class find_t>
    void smawk_find_row_mins_rec(const std::vector<size_t>& rows,
        const std::vector<size_t>& cols,
        const find_t& lookup,
        std::unordered_map<size_t, size_t>& col_i,
        std::vector<size_t>& result)
    {
        assert(rows.size());

        std::vector<size_t> left_columns;
        for (const auto& col : cols)
        {
            while (left_columns.size())
            {
                const auto& row = rows[left_columns.size() - 1u];
                const auto& col2 = left_columns.back();
                if (lookup(row, col2) <= lookup(row, col))
                    break;

                left_columns.pop_back();
            }

            if (left_columns.size() < rows.size())
                left_columns.push_back(col);
        }

        std::vector<size_t> odd_rows;
        for (size_t i = 1; i < rows.size(); i += 2)
            odd_rows.push_back(rows[i]);

        if (odd_rows.size())
            smawk_find_row_mins_rec<t>(odd_rows, left_columns, lookup, col_i, result);

        col_i.clear();
        for (size_t i = 0; i < left_columns.size(); ++i)
            col_i[left_columns[i]] = i;

        for (size_t r_even = 0, start = 0; r_even < rows.size(); r_even += 2)
        {
            const auto& row = rows[r_even];
            const auto stop = r_even + 1u < rows.size()
                ? col_i[result[rows[r_even + 1u]]]
                : left_columns.size() - 1u;

            auto col_min = left_columns[start];
            auto val_min = lookup(row, col_min);
            for (auto c = start + 1u; c <= stop; ++c)
            {
                const auto value = lookup(row, left_columns[c]);
                if (c == start || value < val_min)
                    col_min = left_columns[c], val_min = value;
            }

            result[row] = col_min, start = stop;
        }
    }
}

namespace Privet::Algorithms::Numbers
{
    // Given a totally monotone matrix, SMAWK algorithm finds each row min.
    //
    // In a monotone matrix, row min is in a column >= previous row min column.
    //
    // A totally monotone matrix has the above for every submatrix,
    // or a 2-by-2 submatrix with row min in top-right and bottom-left does not exist.
    //
    // A submatrix is a matrix with optionally deleted rows and/or columns.
    //
    // Note. Monge array is totally monotone: in a 2-by-2 submatrix where
    // sum along the main diagonal is < sum of 2 anti-diagonal elements e.g. {{1,7}, {7,2}}.
    //
    // Time O(cols + cols*log(rows/cols))).
    template<class t>
    std::vector<size_t> smawk_find_row_mins_in_tot_monotone_matrix(const std::vector<t>& ar)
    {
        const auto size = ar.size();
        RequirePositive(size, "ar.size");

        const auto row0_size = ar[0].size();
        RequirePositive(row0_size, "ar[0].size");

        for (size_t r = 0; r < size; ++r)
        {
            const auto& line = ar[r];
            if (row0_size != line.size())
                throw std::runtime_error("Row " + std::to_string(r) +
                    " size (" + std::to_string(line.size()) +
                    ") must be the same " + std::to_string(row0_size));
        }

        uint64_t calls{};

        const auto lookup = [&ar, &calls](const size_t r, const size_t c) {
            ++calls;
            return ar[r][c];
        };

        std::vector<size_t> result_cols(size);

        std::vector<size_t> rows(size);
        std::iota(rows.begin(), rows.end(), size_t());

        std::vector<size_t> cols(row0_size);
        std::iota(cols.begin(), cols.end(), size_t());

        std::unordered_map<size_t, size_t> col_i;
        smawk_find_row_mins_rec<t>(rows, cols, lookup, col_i, result_cols);

        return result_cols;
    }

    // Slow time O(rows*cols).
    template<class t>
    std::vector<size_t> find_row_mins_in_2d_matrix_slow(const std::vector<std::vector<t>>& ar)
    {
        const auto rows = ar.size();
        RequirePositive(rows, "ar.size");

        const auto cols = ar[0].size();
        RequirePositive(cols, "ar[0].size");

        std::vector<size_t> result(rows);
        for (size_t r = 0; r < rows; ++r)
        {
            const auto& line = ar[r];
            if (cols != line.size())
                throw std::runtime_error("Row " + std::to_string(r) +
                    " size (" + std::to_string(line.size()) +
                    ") must be the same " + std::to_string(cols));

            result[r] = std::min_element(line.begin(), line.end()) - line.begin();
        }

        return result;
    }
}