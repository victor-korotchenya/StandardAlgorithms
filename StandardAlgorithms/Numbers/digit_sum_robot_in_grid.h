#pragma once
#include"../Utilities/require_utilities.h"
#include"digit_sum.h"
#include<array>
#include<stack>

namespace Standard::Algorithms::Numbers
{
    // A robot starts at (0,0) in a rows by columns grid.
    // It can move to a valid neighboring {left, right, up, down} cell C
    // when the digits sum of the C.row plus that of the C.column
    // is less than or equal to digitSumLimitInclusive.
    //
    // Input: positive integers: rows, columns, digitSumLimitInclusive.
    // Output: number of cells reachable from (0,0).
    template<std::integral int_t1>
    struct digit_sum_robot_in_grid final
    {
        using int_t = int_t1;
        using dim_t = std::pair<int_t, int_t>;

        digit_sum_robot_in_grid() = delete;

        // Time(rows + columns).
        [[nodiscard]] static constexpr auto fast(const int_t &digit_sum_limit_inclusive, const dim_t &rows_columns)
            -> int_t
        {
            require_positive(digit_sum_limit_inclusive, "digitSumLimitInclusive");

            const auto &rows = require_positive(rows_columns.first, "rows");

            const auto &columns = require_positive(rows_columns.second, "columns");

            const auto rows_max_sum = max_digit_sum(rows);

            if (rows_max_sum < digit_sum_limit_inclusive)
            {
                const auto columns_max_sum = rows == columns ? rows_max_sum : max_digit_sum(columns);

                if (const auto match_everything = !(digit_sum_limit_inclusive < rows_max_sum + columns_max_sum);
                    match_everything)
                {
                    auto result = static_cast<int_t>(rows * columns);
                    return result;
                }
            }

            constexpr int_t zero{};

            auto result = zero;

            // todo(p3): finish fast.. by following the border.
            ++result;

            check_result(rows_columns, result);

            return result;
        }

        [[nodiscard]] static constexpr auto decimal_digits_count(const int_t &value) -> int_t
        {
            assert(int_t{} <= value);

            auto cnt = static_cast<int_t>(1 + std::get<0>(log10_slow<int_t>(value)));

            assert(int_t{} < cnt);

            return cnt;
        }

        // Maximum possible "digit sum".
        // E.g. for 10 it is 9.
        // 20 - 10, 28 - 10, 29 - 11.
        [[nodiscard]] static constexpr auto max_digit_sum(const int_t &value) -> int_t
        {
            assert(int_t{} <= value);

            constexpr int_t ten = 10;

            if (value < ten)
            {
                return value;
            }

            const auto digit_count = decimal_digits_count(value);

            constexpr int_t one = 1;

            const auto one_and_zeros = exp10_slow(digit_count - one);
            const auto lead_digit = static_cast<int_t>(value / one_and_zeros);
            assert(int_t{} < lead_digit);

            const auto lead_digit_and_nines = static_cast<int_t>((lead_digit + one) * one_and_zeros - one);

            const auto are_all_lower_nines = value == lead_digit_and_nines;

            auto result =
                static_cast<int_t>(digit_max * (digit_count - one) + lead_digit - (are_all_lower_nines ? 0 : 1));

            assert(int_t{} < result);

            return result;
        }

        // Time(rows * columns).
        [[nodiscard]] static constexpr auto slow(const int_t &digit_sum_limit_inclusive, const dim_t &rows_columns)
            -> int_t
        {
            require_positive(digit_sum_limit_inclusive, "digitSumLimitInclusive");

            const auto &rows = require_positive(rows_columns.first, "rows");

            const auto &columns = require_positive(rows_columns.second, "columns");

            constexpr int_t zero{};
            constexpr int_t one = 1;

            constexpr auto avoid_boundary_tests = 2LL;

            std::vector<std::vector<bool>> data(
                rows + avoid_boundary_tests, std::vector<bool>(columns + avoid_boundary_tests, false));

            auto row = zero;

            do
            {
                const auto row_sum = single_number_digit_sum_slow(row);

                if (digit_sum_limit_inclusive < row_sum)
                {
                    continue;
                }

                auto &data_row = data[row + one];
                auto column = zero;

                do
                {
                    const auto column_sum = single_number_digit_sum_slow(column);

                    if (!(digit_sum_limit_inclusive < row_sum + column_sum))
                    {
                        data_row[column + one] = true;
                    }
                } while (++column < columns);
            } while (++row < rows);

            constexpr auto candinates_size = 4;
            std::array<dim_t, candinates_size> candinates;

            std::stack<dim_t> sta;
            sta.emplace(one, one);

            data[one][one] = false;

            auto result = one; // (0, 0) is always reachable.

            do
            {
                const auto &top = sta.top();

                candinates[0].first = top.first - one;
                candinates[0].second = top.second;

                candinates[1].first = top.first + one;
                candinates[1].second = top.second;

                candinates[2].first = top.first;
                candinates[2].second = top.second - one;

                candinates[3].first = top.first;
                candinates[3].second = top.second + one;

                sta.pop();

                for (const auto &candinate : candinates)
                {
                    if (!data[candinate.first][candinate.second])
                    {
                        continue;
                    }

                    data[candinate.first][candinate.second] = false;

                    sta.push(candinate);
                    ++result;
                }
            } while (!sta.empty());

            check_result(rows_columns, result);

            return result;
        }

private:
        // Given n >= 0, return power(10, n).
        // E.g. for 1 return 10, 2 - 100, 6 - 1000000.
        [[nodiscard]] static constexpr auto exp10_slow(int_t pow1) -> int_t
        {
            assert(int_t{} <= pow1);

            // todo(p3): use a faster power(**) method.
            int_t result = 1;

            constexpr int_t zero{};

            while (zero < pow1--)
            {
                constexpr int_t ten = 10;

                result *= ten;

                assert(int_t{} < result);
            }

            return result;
        }

        static constexpr void check_result(const dim_t &rows_columns, const int_t &result)
        {
            require_positive(result, "result");

            const auto achievable_result = static_cast<int_t>(rows_columns.first * rows_columns.second);

            require_positive(achievable_result, "achievable result");

            require_less_equal(result, achievable_result, "possible overflow result");
        }
    };
} // namespace Standard::Algorithms::Numbers
