#pragma once
#include <cassert>
#include <type_traits>
#include <set>
#include <vector>
#include "Arithmetic.h"
#include "../Utilities/ExceptionUtilities.h"

namespace
{
    // Time O(rows * columns).
    template <class int_t, class u_int_t>
    int_t min_merges_get_all_binary_1_proc_mask_slow(
        const std::vector<std::vector<bool>>& ar,
        const int_t prev_result, const int_t mask)
    {
        const auto bit_count = static_cast<int_t>(PopCount(static_cast<u_int_t>(mask)));
        assert(prev_result > 0 && bit_count > 0);
        if (bit_count >= prev_result)
            return 0;

        constexpr int_t one = 1;
        const auto size = static_cast<int_t>(ar.size());
        const auto row_size = static_cast<int_t>(ar[0].size());

        int_t left = (one << row_size) - one;
        assert(left > 0);

        for (int_t i = 0; i < size; ++i)
        {
            const auto row_chosen = (mask >> i) & 1;
            if (!row_chosen)
                continue;

            const auto& a = ar[i];
            assert(row_size == static_cast<int_t>(a.size()));

            for (int_t j = 0; j < row_size; ++j)
                // todo: p3. can be done in 1 step when row_size <= 30.
                if (a[j])
                    left &= ~(one << j);

            if (!left)
                return bit_count;
        }

        return 0;
    }

    // Return true if a row has all true values.
    // All false rows are deleted.
    template <class int_t>
    bool delete_constant_rows(std::vector<std::vector<bool>>& ar, std::vector<std::pair<int_t, int_t>>& column_count_1st_row)
    {
        const auto row_size = static_cast<int_t>(ar[0].size());
        column_count_1st_row.assign(row_size, {});

        for (int_t row = 0; row < static_cast<int_t>(ar.size());)
        {
            const auto& a = ar[row];
            assert(row_size == static_cast<int_t>(a.size()));

            auto has_all = true, ignored = true;
            for (int_t col = 0; col < row_size; ++col)
            {
                if (a[col])
                {
                    ignored = false;
                    if (!column_count_1st_row[col].first++)
                        column_count_1st_row[col].second = row;
                }
                else
                    has_all = false;
            }

            if (has_all)
            {
                ar.clear();
                return true;
            }

            if (ignored)
            {// Delete row.
                std::swap(ar[row], ar.back());
                ar.pop_back();
                continue;
            }

            ++row;
        }

        return false;
    }

    template <class int_t>
    bool check_empty_columns(const std::vector<std::vector<bool>>& ar,
        const std::vector<std::pair<int_t, int_t>>& column_count_1st_row,
        std::set<int_t>& delete_columns,
        std::set<int_t>& del_rows,
        int_t& count)
    {
        assert(column_count_1st_row.size());
        if (ar.empty())
        {
#if _DEBUG
            auto has_0 = false;
            for (const auto& p : column_count_1st_row)
            {
                if (!p.first)
                {
                    has_0 = true;
                    break;
                }
            }
            assert(has_0);
#endif
            return true;
        }

        delete_columns.clear();
        del_rows.clear();

        const auto row_size = static_cast<int_t>(ar[0].size());
        for (int_t col = 0; col < row_size; ++col)
        {
            const auto& cnt_row = column_count_1st_row[col];
            if (!cnt_row.first)
                return true;

            if (cnt_row.first == static_cast<int_t>(ar.size()))
            {// If a column has true in all rows, ignore such columns.
                delete_columns.insert(col);
                continue;
            }

            if (cnt_row.first != 1 ||
                !del_rows.insert(cnt_row.second).second)
                continue;

            // If a column has true in the only row: count such rows, then ignore all true columns in them, ignore them.
            ++count;

            const auto& a = ar[cnt_row.second];
            for (int_t c2 = 0; c2 < row_size; ++c2)
                if (a[c2])
                    delete_columns.insert(c2);
        }

        return false;
    }

    template <class int_t>
    void delete_row_column(std::vector<std::vector<bool>>& ar,
        const std::set<int_t>& delete_columns,
        const std::set<int_t>& del_rows,
        std::vector<bool>& temp)
    {
        for (auto it = del_rows.rbegin(); it != del_rows.rend(); ++it)
        {// Delete rows from the end.
            const auto& row = *it;
            std::swap(ar[row], ar.back());
            ar.pop_back();
        }

        const auto del_col_size = static_cast<int_t>(delete_columns.size());
        if (ar.empty() || !del_col_size)
            return;

        const auto row_size = static_cast<int_t>(ar[0].size());
        if (row_size == del_col_size)
            // Delete all columns.
            ar.clear();
        else
            assert(del_col_size < row_size);

        for (auto& a : ar)
        {
            temp.clear();

            for (int_t col = 0; col < row_size; ++col)
            {
                // todo: p3. find can be O(1), not O(log(n)).
                const auto it = delete_columns.find(col);
                if (it == delete_columns.end())
                    temp.push_back(a[col]);
            }

            assert(temp.size() && temp.size() + delete_columns.size() == a.size());
            a = std::move(temp);
        }
    }

    template <class int_t>
    int_t min_merges_get_all_binary_1_clear(std::vector<std::vector<bool>>& ar)
    {
        // The only case to return -1 is when a column has false in all rows.
        // Return 1 only when a row has all true.
        //
        // If a row has all false, ignore it.
        // If a column has true in all rows, ignore such columns.
        // If a column has true in the only row: include such rows, then ignore them and all true columns in them.
        // If 'ar' became empty, return the included row count.

        int_t count = 0;
        std::vector<std::pair<int_t, int_t>> column_count_1st_row;
        std::set<int_t> delete_columns, del_rows;
        std::vector<bool> temp;
        for (;;)
        {
            assert(ar.size());
            if (delete_constant_rows<int_t>(ar, column_count_1st_row))
                return count + 1;

            if (check_empty_columns<int_t>(ar, column_count_1st_row, delete_columns, del_rows, count))
                // The only case to return -1 is when a column has false in all rows.
                return -1;

            delete_row_column<int_t>(ar, delete_columns, del_rows, temp);

            if (ar.empty() || delete_columns.empty() && del_rows.empty())
            {
                assert(count > 0 || ar.size());
                return count;
            }
        }
    }

    template <class int_t>
    std::vector<int_t> to_masks(const std::vector<std::vector<bool>>& ar)
    {
        const auto size = static_cast<int_t>(ar.size());
        const auto row_size = ar[0].size();
        assert(size > 0 && row_size > 0 && row_size <= 30);

        std::vector<int_t> masks(size);
        for (int_t row = 0; row < size; ++row)
        {
            const auto& a = ar[row];
            auto& b = masks[row];
            for (int_t col = 0; col < row_size; ++col)
                if (a[col])
                    b |= int_t(1) << col;

            assert(b);
        }

        return masks;
    }

    // Given masks, find min rows to cover all 'row_size' columns.
    // Return -1 when not all columns covered.
    // Time O(2**row_size * rows).
    template<class int_t>
    int_t min_merges_get_all_binary_1_by_masks(
        const std::vector<int_t>& masks, const int_t row_size)
    {
        static_assert(std::is_signed_v<int_t>);

        const auto size = static_cast<int_t>(masks.size());
        RequirePositive(size, "size");
        RequirePositive(row_size, "row_size");
        assert(row_size <= 30);

        const int_t edge_mask = int_t(1) << row_size,
            max_mask = edge_mask - int_t(1);
        assert(edge_mask && max_mask);

        std::vector<int_t> min_merges(edge_mask, -1);
        for (int_t i = 0; i < size; ++i)
        {
            const auto& item_mask = masks[i];
            if (!item_mask)
                continue;

            for (int_t mask = 1; mask <= max_mask; ++mask)
            {
                const auto& prev = min_merges[mask];
                if (prev < 0)
                    continue;

                const auto mask2 = mask | item_mask;
                auto& cur = min_merges[mask2];
                if (cur < 0 || cur > prev + 1)
                    cur = prev + 1;

                assert(cur > 0);
            }

            min_merges[item_mask] = 1;
        }

        const auto& result = min_merges[max_mask];
        assert(result == -1 || result > 0 && result <= size);
        return result;
    }
}

namespace Privet
{
    namespace Algorithms
    {
        namespace Numbers
        {
            // For small columns.
            // Min rows to get ORed 'true' in all columns, or -1 if impossible.
            // Example. Given 4 by 6 array:
            // {{t,t,t,f,f,f},
            //  {t,f,f,f,f,t},
            //  {f,t,f,f,t,f},
            //  {f,f,t,t,f,f}},
            //  the last 3 rows are merged to get all 6 true values.
            // All rows have the same size.
            // Time O(2**columns * rows).
            template<class int_t = int>
            int_t min_merges_get_all_binary_1(std::vector<std::vector<bool>> ar)
            {
                static_assert(std::is_signed_v<int_t> && sizeof(int_t) >= 4);

                const auto initial_size = static_cast<int_t>(ar.size());
                RequirePositive(initial_size, "ar.size");
                RequirePositive(ar[0].size(), "ar[0].size");

                // todo: p3. not sure - delete cleaning.
                const auto count = min_merges_get_all_binary_1_clear<int_t>(ar);
                if (count < 0 || ar.empty())
                {
                    assert(count);
                    return count;
                }

                const auto row_size = static_cast<int_t>(ar[0].size());
                assert(row_size <= 25);

                const auto masks = to_masks<int_t>(ar);
                auto result = min_merges_get_all_binary_1_by_masks<int_t>(masks, row_size);

                assert(result > 0 && result + count > 0 && result <= static_cast<int_t>(ar.size()) && result + count <= initial_size);
                result += count;
                return result;
            }

            // For small rows.
            // Time O(2**rows * rows * columns).
            template<class int_t = int, class u_int_t = unsigned>
            int_t min_merges_get_all_binary_1_rows(const std::vector<std::vector<bool>>& ar)
            {
                static_assert(sizeof(int_t) <= sizeof(u_int_t) && std::is_signed_v<int_t> && std::is_unsigned_v<u_int_t>);

                const auto size = static_cast<int_t>(ar.size());
                RequirePositive(size, "ar.size");

                {
                    const auto row_size = static_cast<int_t>(ar[0].size());
                    RequirePositive(row_size, "ar[0].size");
                    assert(size <= 20 && row_size <= 20);
                }

                constexpr int_t one = 1;
                const auto max_mask_row = (one << size) - one;
                assert(max_mask_row);

                auto result = min_merges_get_all_binary_1_proc_mask_slow<int_t, u_int_t>(ar, size + 1, max_mask_row);
                if (!result)
                    return -1;

                for (int_t mask = 1; mask < max_mask_row; ++mask)
                {
                    const auto cand = min_merges_get_all_binary_1_proc_mask_slow<int_t, u_int_t>(ar, result, mask);
                    if (cand > 0 && result > cand)
                    {
                        result = cand;
                        if (result == 1)
                            return 1;
                    }
                }

                assert(result > 0 && result <= size);
                return result;
            }
        }
    }
}