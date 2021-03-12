#pragma once
#include"../Utilities/require_matrix.h"
#include"arithmetic.h" // pop_count
#include<set>

namespace Standard::Algorithms::Numbers::Inner
{
    // Time O(rows * columns).
    template<std::signed_integral int_t, std::unsigned_integral u_int_t>
    requires(sizeof(std::int32_t) <= sizeof(int_t) && sizeof(int_t) == sizeof(u_int_t))
    [[nodiscard]] constexpr auto min_merges_all_binary_1_proc_mask_slow(
        const int_t &prev_result, const std::vector<std::vector<bool>> &arr, const u_int_t &row_mask) -> int_t
    {
        const auto bit_count = static_cast<int_t>(Standard::Algorithms::Numbers::pop_count(row_mask));

        assert(0U < row_mask && int_t{} < prev_result && int_t{} < bit_count);

        if (!(bit_count < prev_result))
        {// todo(p4): do not try out all masks - only those with fewer set bits.
            return int_t{};
        }

        const auto rows = static_cast<int_t>(arr.size());
        const auto columns = static_cast<int_t>(arr.at(0).size());

        constexpr u_int_t uone = 1;

        auto column_mask = static_cast<u_int_t>((uone << columns) - uone);
        assert(0U < column_mask);

        for (int_t row{}; row < rows; ++row)
        {
            if (const auto row_chosen = (row_mask >> row) & 1U; 0U == row_chosen)
            {
                continue;
            }

            const auto &linie = arr[row];
            assert(static_cast<std::size_t>(columns) == linie.size());

            for (int_t col{}; col < columns; ++col)
            {// todo(p4): can be done in 1 step when columns <= 64. Note that 2**64 is too large - replace
              // std::vector<bool> with std::uint64_t.
                if (linie[col])
                {
                    column_mask &= ~(uone << col);
                }
            }

            if (0U == column_mask)
            {
                return bit_count;
            }
        }

        return {};
    }

    // Return true if a row has all true values.
    // All false rows are deleted.
    template<class int_t>
    [[nodiscard]] constexpr auto delete_constant_rows(
        std::vector<std::vector<bool>> &arr, std::vector<std::pair<int_t, int_t>> &column_count_1st_row) -> bool
    {
        const auto columns = static_cast<int_t>(arr.at(0).size());
        column_count_1st_row.assign(columns, {});

        for (int_t row{}; row < static_cast<int_t>(arr.size());)
        {
            const auto &linie = arr[row];
            assert(static_cast<std::size_t>(columns) == linie.size());

            auto has_all = true;
            auto ignored = true;

            for (int_t col{}; col < columns; ++col)
            {
                if (!linie[col])
                {
                    has_all = false;
                    continue;
                }

                ignored = false;

                if (auto &par = column_count_1st_row[col]; int_t{} == par.first++)
                {
                    par.second = row;
                }
            }

            if (has_all)
            {
                arr.clear();
                return true;
            }

            if (ignored)
            {// Delete the row.
                std::swap(arr[row], arr.back());
                arr.pop_back();
                continue;
            }

            ++row;
        }

        return false;
    }

    // An unsatisfiable matrix has a column with false in all rows.
    template<class int_t>
    [[nodiscard]] constexpr auto check_false_columns(std::set<int_t> &del_rows,
        const std::vector<std::vector<bool>> &arr, const std::vector<std::pair<int_t, int_t>> &column_count_1st_row,
        std::set<int_t> &delete_columns, int_t &count) -> bool
    {
        assert(!column_count_1st_row.empty());

        if (arr.empty())
        {
            if constexpr (::Standard::Algorithms::is_debug)
            {
                constexpr int_t zero{};

                const auto ite = std::find_if(column_count_1st_row.cbegin(), column_count_1st_row.cend(),
                    [&zero](const auto &par)
                    {
                        auto vaz = zero == par.first;
                        return vaz;
                    });

                [[maybe_unused]] const auto has_0 = ite != column_count_1st_row.cend();

                assert(has_0);
            }

            return true;
        }

        delete_columns.clear();
        del_rows.clear();

        const auto columns = static_cast<int_t>(arr[0].size());

        for (int_t col{}; col < columns; ++col)
        {
            const auto &cnt_row = column_count_1st_row[col];

            if (int_t{} == cnt_row.first)
            {
                return true;
            }

            if (cnt_row.first == static_cast<int_t>(arr.size()))
            {// If a column has true in all rows, ignore such columns.
                delete_columns.insert(col);
                continue;
            }

            if (cnt_row.first != 1 || !del_rows.insert(cnt_row.second).second)
            {
                continue;
            }

            // If a column has true in the only row: count such row, and remove all true columns in this row.
            ++count;

            const auto &linie = arr[cnt_row.second];

            for (int_t col_2{}; col_2 < columns; ++col_2)
            {
                if (linie[col_2])
                {
                    delete_columns.insert(col_2);
                }
            }
        }

        return false;
    }

    template<class int_t>
    constexpr void delete_row_column(const std::set<int_t> &del_rows, std::vector<std::vector<bool>> &arr,
        const std::set<int_t> &delete_columns, std::vector<bool> &remain_values)
    {
        for (auto iter = del_rows.crbegin(); iter != del_rows.crend(); ++iter)
        {// Delete rows from the end.
            const auto &row = *iter;
            std::swap(arr.at(row), arr.back());
            arr.pop_back();
        }

        const auto del_col_size = static_cast<int_t>(delete_columns.size());

        if (arr.empty() || int_t{} == del_col_size)
        {
            return;
        }

        const auto columns = static_cast<int_t>(arr[0].size());

        if (columns == del_col_size)
        {// Delete all columns.
            arr.clear();
            return;
        }

        assert(del_col_size < columns);

        for (auto &linie : arr)
        {
            assert(linie.size() == static_cast<std::size_t>(columns));

            remain_values.clear();

            for (int_t col{}; col < columns; ++col)
            {
                // todo(p3): find can be O(1), not O(log(n)).
                const auto iter = delete_columns.find(col);

                if (iter == delete_columns.end())
                {
                    remain_values.push_back(linie[col]);
                }
            }

            assert(!remain_values.empty() && remain_values.size() + delete_columns.size() == linie.size());

            linie.resize(remain_values.size());

            std::copy(remain_values.cbegin(), remain_values.cend(), linie.begin());
        }
    }

    template<class int_t>
    [[nodiscard]] constexpr auto min_merges_all_binary_1_clear(std::vector<std::vector<bool>> &arr) -> int_t
    {
        // The only case to return -1 is when a column has false in all rows.
        // If a row has all true values, return 1.
        // If a row has all false, ignore it.
        // If a column has true in all rows, ignore it too.
        // If a column has true in the only row: count such row, and remove all true columns in this row.
        // If 'arr' has become empty, return the included row count.

        std::vector<std::pair<int_t, int_t>> column_count_1st_row;
        std::set<int_t> delete_columns;
        std::set<int_t> del_rows;
        std::vector<bool> remain_values;
        int_t count{};

        for (;;)
        {
            assert(!arr.empty());

            if (delete_constant_rows<int_t>(arr, column_count_1st_row))
            {
                return static_cast<int_t>(count + 1);
            }

            if (check_false_columns<int_t>(del_rows, arr, column_count_1st_row, delete_columns, count))
            {
                return -static_cast<int_t>(1);
            }

            delete_row_column<int_t>(del_rows, arr, delete_columns, remain_values);

            if (arr.empty() || (delete_columns.empty() && del_rows.empty()))
            {
                assert(int_t{} < count || !arr.empty());

                return count;
            }
        }
    }

    template<class int_t>
    [[nodiscard]] constexpr auto to_masks(const std::vector<std::vector<bool>> &arr) -> std::vector<int_t>
    {
        const auto rows = static_cast<int_t>(arr.size());
        const auto columns = static_cast<int_t>(arr.at(0).size());

        assert(int_t{} < rows && int_t{} < columns);

        std::vector<int_t> masks(rows);

        for (int_t row{}; row < rows; ++row)
        {
            const auto &linie = arr[row];
            assert(static_cast<std::size_t>(columns) == linie.size());

            auto &mask = masks[row];

            for (int_t col{}; col < columns; ++col)
            {
                if (!linie[col])
                {
                    continue;
                }

                constexpr int_t one = 1;

                mask |= static_cast<int_t>(one << col);
            }

            assert(int_t{} != mask);
        }

        return masks;
    }

    // Given the masks, find min rows to cover all columns.
    // Due to the preliminary cleaning, each column is covered by at least 2 rows.
    // Thus, the returned value cannot include all rows.
    // Time O(2**columns * rows).
    template<std::signed_integral int_t>
    [[nodiscard]] constexpr auto min_merges_all_binary_1_by_masks(const std::vector<int_t> &masks, const int_t &columns)
        -> int_t
    {
        require_positive(columns, "columns");

        const auto rows = require_positive(static_cast<int_t>(masks.size()), "rows");

        constexpr int_t one = 1;

        const auto edge_mask = static_cast<int_t>(one << columns);
        const auto max_mask = static_cast<int_t>(edge_mask - one);

        assert(int_t{} < max_mask && max_mask < edge_mask);

        std::vector<int_t> min_merges(edge_mask, -one);

        for (int_t row{}; row < rows; ++row)
        {
            const auto &item_mask = masks[row];
            assert(int_t{} != item_mask);

            for (auto mask = max_mask; int_t{} < --mask;)
            {
                const auto &prev = min_merges[mask];

                if (prev < int_t{})
                {
                    continue;
                }

                const auto mask_2 = static_cast<int_t>(mask | item_mask);
                auto &cur = min_merges[mask_2];

                if (auto cand = static_cast<int_t>(prev + one); cur < int_t{} || cand < cur)
                {
                    cur = cand;
                }

                assert(int_t{} < cur);
            }

            min_merges[item_mask] = one;
        }

        const auto &result = min_merges[max_mask];

        assert(int_t{} < result && result < rows);

        return result;
    }
} // namespace Standard::Algorithms::Numbers::Inner

namespace Standard::Algorithms::Numbers
{
    constexpr auto mm_ab1_max_size = 25U;

    template<class item_t>
    [[nodiscard]] constexpr auto check_mm_matrix_size(const std::vector<std::vector<item_t>> &matrix) noexcept(false)
        -> std::size_t
    {
        {
            constexpr auto square = false;
            constexpr auto some_rows = true;
            constexpr auto some_columns = true;
            constexpr check_matrix_options options{ square, some_rows, some_columns };

            require_matrix(matrix, options);
        }

        assert(!matrix.empty() && !matrix.at(0).empty());

        const auto rows = require_positive(matrix.size(), "matrix rows");

        require_less_equal(rows, mm_ab1_max_size, "matrix rows");

        {
            constexpr const auto *const name = "matrix columns";

            const auto columns = require_positive(matrix.at(0).size(), name);

            require_less_equal(columns, mm_ab1_max_size, name);
        }

        return rows;
    }

    // Min rows to get ORed 'true' in all columns, or -1 if impossible.
    // Example. Given 4 by 6 array:
    // {{t,t,t,f,f,f},
    //  {t,f,f,f,f,t},
    //  {f,t,f,f,t,f},
    //  {f,f,t,t,f,f}},
    //  the last 3 rows are merged to get all 6 true values.
    // All rows have the same # of columns which should be small.
    // Good for small columns.
    // Time O(2**columns * rows).
    template<std::signed_integral int_t = std::int32_t>
    requires(sizeof(std::int32_t) <= sizeof(int_t))
    [[nodiscard]] constexpr auto min_merges_get_all_binary_1_favor_columns(
        // Can be edited.
        std::vector<std::vector<bool>> &editable_arr) -> int_t
    {
        const auto initial_rows = static_cast<int_t>(check_mm_matrix_size(editable_arr));

        const auto count = Inner::min_merges_all_binary_1_clear<int_t>(editable_arr);

        if (count < int_t{} || editable_arr.empty())
        {
            assert(count != int_t{});

            return count;
        }

        const auto columns = static_cast<int_t>(editable_arr.at(0).size());
        const auto masks = Inner::to_masks<int_t>(editable_arr);

        auto result = Inner::min_merges_all_binary_1_by_masks<int_t>(masks, columns);

        assert(int_t{} < result && int_t{} < result + count &&
            static_cast<std::size_t>(result) <= editable_arr.size() && result + count <= initial_rows);

        result += count;

        return result;
    }

    // Good for small rows.
    // Time O(2**rows * rows * columns).
    template<std::signed_integral int_t = std::int32_t, std::unsigned_integral u_int_t = std::uint32_t>
    requires(sizeof(std::int32_t) <= sizeof(int_t) && sizeof(int_t) == sizeof(u_int_t))
    [[nodiscard]] constexpr auto min_merges_get_all_binary_1_favor_rows(const std::vector<std::vector<bool>> &arr)
        -> int_t
    {
        const auto rows = static_cast<int_t>(check_mm_matrix_size(arr));
        // todo(p5): Do the cleaning.

        constexpr u_int_t uone = 1;

        auto row_mask = static_cast<u_int_t>((uone << rows) - uone);
        assert(0U < row_mask);

        auto result =
            Inner::min_merges_all_binary_1_proc_mask_slow<int_t, u_int_t>(static_cast<int_t>(rows + 1), arr, row_mask);

        if (const auto all_not_covered = int_t{} == result; all_not_covered)
        {
            return -static_cast<int_t>(1);
        }

        constexpr int_t best_possible_result = 1;

        if (result == best_possible_result)
        {
            return result;
        }

        while (0U < --row_mask)
        {
            const auto cand = Inner::min_merges_all_binary_1_proc_mask_slow<int_t, u_int_t>(result, arr, row_mask);

            if (int_t{} < cand && cand < result)
            {
                if (cand == best_possible_result)
                {// todo(p4): del this check. Whether there is a row with all true values can be done before.
                    return cand;
                }

                result = cand;
            }
        }

        assert(int_t{} < result && result <= rows);

        return result;
    }
} // namespace Standard::Algorithms::Numbers
