#pragma once
#include"gaussian_elimination.h"

namespace Standard::Algorithms::Numbers::Inner
{
    constexpr void swap_entire_rows(auto &lines, const std::size_t size, auto &lines_2) noexcept
    {
        assert(&lines != &lines_2 && size <= lines.size() && size <= lines_2.size());

        for (std::size_t col{}; col < size; ++col)
        {
            std::swap(lines[col], lines_2[col]);
        }
    }

    template<class floating_t, class matrix_t, class x_t>
    constexpr void divide_row_by_maxi(
        const std::size_t row, x_t &lines, const std::size_t size, matrix_t &inverse, auto &maxi)
    {
        constexpr floating_t one{ 1 };

        assert(row < size);
        assert(floating_t{} != maxi && one != maxi);

        {
            auto &inv_lines = inverse[row];

            for (std::size_t col{}; col < size; ++col)
            {
                auto &cell = inv_lines[col];
                cell /= maxi;
            }
        }

        for (auto col = row + 1ZU; col < size; ++col)
        {
            auto &cell = lines[col];
            cell /= maxi;
        }

        maxi = one;
    }

    template<class floating_t, class matrix_t>
    constexpr void zero_out_coefficient(const std::size_t size, matrix_t &source_matrix, const std::size_t row,
        matrix_t &inverse, const std::size_t row_2, auto &coef)
    {
        constexpr floating_t zero{};

        assert(row < size && row_2 < size && row != row_2);
        assert(zero != coef);

        {
            const auto &lines = inverse[row];
            auto &lines_2 = inverse[row_2];

            for (std::size_t col{}; col < size; ++col)
            {
                const auto &cell = lines[col];
                const auto prod = static_cast<floating_t>(coef * cell);
                auto &cell_2 = lines_2[col];
                cell_2 -= prod;
            }
        }
        {
            // 1 0 coef   cell_2 ; ???? ; row_2 = 0; to have (0 == coef), must -= coef*(2nd row).
            // 0 1 coef1 ?         ; ????
            // 0 0  1       cell     ; ???? ; row = 2
            // 0 0 coef2 ?         ; ????
            const auto &lines = source_matrix[row];
            auto &lines_2 = source_matrix[row_2];

            // To the left of source_matrix[row] are zeros - can skip the columns (col < row).
            for (auto col = row + 1ZU; col < size; ++col)
            {
                const auto &cell = lines[col];
                const auto prod = static_cast<floating_t>(coef * cell);
                auto &cell_2 = lines_2[col];
                cell_2 -= prod;
            }
        }

        assert(zero != coef);
        coef = zero;
    }
} // namespace Standard::Algorithms::Numbers::Inner

namespace Standard::Algorithms::Numbers
{
    // Time O(n**3).
    template<class floating_t, class matrix_t>
    [[nodiscard]] constexpr auto matrix_inverse_gauss(matrix_t &source_matrix, matrix_t &inverse) -> bool
    {
        assert(&source_matrix != &inverse);

        const auto size = source_matrix.size();
        inverse.resize(size);

        if (0U == size)
        {
            return true;
        }

        for (std::size_t row{}; row < size; ++row)
        {
            require_less_equal(size, source_matrix[row].size(), "source matrix columns at row");
        }

        {// The original source and its future inverse (the identity matrix at the moment):
          //  {2, -1/2.0} ; {1, 0}
          //  {-4, 5} ; {0, 1}
          //
          // Swap the first two rows (better stability):
          //  {-4, 5} ; {0, 1}
          //  {2, -1/2.0} ; {1, 0}
          // Divide the first row by -4:
          //  {1, -5/4.0} ; {0, -1/4.0}
          //  {2, -1/2.0} ; {1, 0}
          // Add -2*(the first row) to (the second row):
          //  {1, -5/4.0} ; {0, -1/4.0}
          //  {2 - 2*1, -1/2.0 + 2*5/4.0} ; {1 + 2*0, 0 + 2*1/4.0}
          // Semper fi:
          //  {1, -5/4.0} ; {0, -1/4.0}
          //  {0, 2} ; {1, 1/2.0}
          //
          // Divide the second row by 2:
          //  {1, -5/4.0} ; {0, -1/4.0}
          //  {0, 1} ; {1/2.0, 1/4.0}
          // Add 5/4.0*(the second row) to (the first row):
          //  {1 + 5/4.0*0, -5/4.0 + 5/4.0*1} ; {0 + 5/4.0*1/2.0, -1/4.0 + 5/4.0*1/4.0}
          //  {0, 1} ; {1/2.0, 1/4.0}
          // Semper fi:
          //  {1, 0} ; {5/8.0 = 0.625, 1/16.0 = 0.0625}
          //  {0, 1} ; {1/2.0, 1/4.0}
          //
          // Check that source * inverse = I2:
          //  {2, -1/2.0} * {5/8.0, 1/16.0}
          //  {-4, 5}       {1/2.0, 1/4.0}
          // Multiply:
          //  {2*5/8.0 + -1/2.0*1/2.0, 2*1/16.0 + -1/2.0*1/4.0}
          //  {-4*5/8.0 + 5*1/2.0, -4*1/16.0 + 5*1/4.0}
          // Semper fi:
          //  {10/8.0 -2/8.0, 1/8.0 - 1/8.0}
          //  {-5/2.0 + 5/2.0, -1/4.0 + 5/4.0}
          // Semper fi; have the identity matrix indeed:
          //  {1, 0}
          //  {0, 1}.
        }

        matrix_identity<floating_t, matrix_t>(size, inverse);

        constexpr floating_t zero{};
        constexpr floating_t one{ 1 };

        for (std::size_t row{}; row < size; ++row)
        {
            const std::size_t max_row = Inner::find_max_to_reduce_error<matrix_t>(size, source_matrix, row);

            assert(max_row < size);

            auto &lines = source_matrix[row];

            if (const auto &cell = lines[max_row]; zero == cell)
            {
                return false;
            }

            if (max_row != row)
            {
                {
                    auto &lines_2 = source_matrix[max_row];
                    Inner::swap_entire_rows(lines, size, lines_2);
                }
                {
                    auto &inv_lines = inverse[row];
                    auto &inv_lines_2 = inverse[max_row];
                    Inner::swap_entire_rows(inv_lines, size, inv_lines_2);
                }
            }

            {
                auto &maxi = lines[row];
                assert(zero != maxi);

                if (one != maxi)
                {
                    Inner::divide_row_by_maxi<floating_t, matrix_t>(row, lines, size, inverse, maxi);
                }
            }

            for (std::size_t row_2{}; row_2 < size; ++row_2)
            {
                auto &coef = source_matrix[row_2][row];
                if (row == row_2 || zero == coef)
                {
                    continue;
                }

                Inner::zero_out_coefficient<floating_t, matrix_t>(size, source_matrix, row, inverse, row_2, coef);
            }
        }

        return true;
    }

    // Note. Gauss matrix inverse could be faster.
    // Time O(n**3).
    template<class floating_t, class matrix_t, class x_t = std::vector<floating_t>>
    [[nodiscard]] constexpr auto matrix_inverse_via_lup(matrix_t &source_matrix, matrix_t &inverse) -> bool
    {
        assert(&source_matrix != &inverse);

        const auto size = source_matrix.size();
        inverse.resize(size);

        if (0U == size)
        {
            return true;
        }

        std::vector<std::int32_t> permutation(size);

        if (const auto res =
                bottom_left_top_right_permut_decomposition<floating_t, matrix_t>(size, source_matrix, permutation);
            gaussian_elimination_result::one_solution != res)
        {
            return false;
        }

        constexpr floating_t zero{};
        constexpr floating_t one{ 1 };

        const auto &lower_upper_compot = source_matrix;
        x_t b_coefficients(size);
        x_t sol(size);

        for (std::size_t row{}; row < size; ++row)
        {
            assert(size <= lower_upper_compot.at(row).size());

            auto &obhcc = inverse[row];
            obhcc.resize(size);
        }

        for (std::size_t row{}; row < size; ++row)
        {
            auto &the_one_and_only = b_coefficients[row];
            the_one_and_only = one;

            {
                solve_via_lup<floating_t, matrix_t, x_t>(b_coefficients, permutation, lower_upper_compot, sol);

                for (std::size_t col{}; col < size; ++col)
                {
                    inverse[col][row] = sol[col]; // todo(p3): del Transpose?
                }
            }

            the_one_and_only = zero;
        }

        return true;
    }
} // namespace Standard::Algorithms::Numbers
