#pragma once
#include"../Utilities/is_debug.h"
#include"sparse_table_log_and_size_validate.h"
#include<algorithm>
#include<array>
#include<cassert>
#include<stdexcept>

namespace Standard::Algorithms::Numbers
{
    // Given a matrix n*m,
    // calculate the min (or max) value in a rectangle [x1, y1] [x2, y2],
    // where (0 <= x1 <= x2 < row_max) and (0 <= y1 <= y2 < col_max).
    // O(1) get time because min[a..b..c..d] == min(min[a..c], min[b..d]).
    template<class value_t, std::size_t log_row_max, std::size_t row_max, std::size_t log_col_max, std::size_t col_max,
        class operation_t>
    requires(sparse_table_log_and_size_validate(log_row_max, row_max) &&
        sparse_table_log_and_size_validate(log_col_max, col_max))
    struct sparse_table_range_min_query_2d final
    {
        static constexpr std::size_t one = 1;

        constexpr explicit sparse_table_range_min_query_2d(const auto &source, operation_t operation = {})
            : Operation(operation)
        {
            assert(!(source.size() < row_max));

            for (std::size_t row{}; row < row_max; ++row)
            {
                const auto &source_line = source[row];
                auto &target = Data[0][0][row];

                std::copy(source_line.cbegin(), source_line.cbegin() + col_max, target.begin());
            }

            for (std::size_t log_col{}; log_col < log_col_max; ++log_col)
            {
                const auto shift = one << log_col;

                const auto &froms = Data[0][log_col];
                auto &tos = Data[0][log_col + one];

                for (std::size_t row{}; row < row_max; ++row)
                {
                    for (std::size_t col{}; col + shift < col_max; ++col)
                    {
                        const auto &left = froms[row][col];
                        const auto &right = froms[row][col + shift];

                        tos[row][col] = Operation(left, right);
                    }
                }
            }

            for (std::size_t log_row{}; log_row < log_row_max; ++log_row)
            {
                const auto shift = one << log_row;

                for (std::size_t log_col{}; log_col <= log_col_max; ++log_col)
                {
                    const auto &froms = Data[log_row][log_col];
                    auto &tos = Data[log_row + one][log_col];

                    for (std::size_t row{}; row + shift < row_max; ++row)
                    {
                        for (std::size_t col{}; col < col_max; ++col)
                        {
                            const auto &left = froms[row][col];
                            const auto &right = froms[row + shift][col];

                            tos[row][col] = Operation(left, right);
                        }
                    }
                }
            }
        }

        // Select in a region [x1, y1] and [x2, y2] inclusively.
        [[nodiscard]] constexpr auto get(const std::size_t xx1, const std::size_t yy1, const std::size_t x2_incl,
            const std::size_t y2_incl) const -> value_t
        {
            if constexpr (::Standard::Algorithms::is_debug)
            {
                if (const auto oki = xx1 <= x2_incl && x2_incl < row_max && yy1 <= y2_incl && y2_incl < col_max; !oki)
                    [[unlikely]]
                {
                    throw std::runtime_error("Bad rectangle coordinates in sparse_table_range_min_query_2d.");
                }
            }

            const auto length_x = x2_incl - xx1 + one;
            const auto length_y = y2_incl - yy1 + one;

            const auto log_x = static_cast<std::size_t>(log_base_2_down(length_x));
            const auto log_y = static_cast<std::size_t>(log_base_2_down(length_y));
            assert(log_x <= log_row_max && log_y <= log_col_max);

            const auto xx3 = x2_incl + one - (one << log_x);
            const auto yy3 = y2_incl + one - (one << log_y);
            assert(xx3 < row_max && yy3 < col_max);

            const auto &dialog = Data[log_x][log_y];

            auto result_1 = Operation(dialog[xx1][yy1], dialog[xx1][yy3]);
            auto result_2 = Operation(dialog[xx3][yy1], dialog[xx3][yy3]);

            auto result = Operation(result_1, result_2);
            return result;
        }

private:
        operation_t Operation;

        std::array<std::array<std::array<std::array<value_t, col_max>, row_max>, log_col_max + 1U>, log_row_max + 1U>
            Data{};
    };
} // namespace Standard::Algorithms::Numbers
