#pragma once
#include"../Utilities/clang_constexpr.h"
#include"../Utilities/require_utilities.h"
#include<cmath>
#include<iostream>
#include<numeric> // iota
#include<span>
#include<vector>

namespace Standard::Algorithms::Numbers::Inner
{
    constexpr auto shall_print_gauss_elim = false;

    // todo(p4): find max pivot in the whole submatrix (swap columns as well).
    // todo(p3): parallelize Gauss Eli.
    // Time O(n).
    template<class matrix_t>
    [[nodiscard]] constexpr auto find_max_to_reduce_error(
        const std::int32_t size, const matrix_t &matr, const std::int32_t start_row) -> std::int32_t
    {
        assert(0 <= start_row && start_row < size);

        auto max_value = ::Standard::Algorithms::fabs(matr[start_row][start_row]);
        auto max_row = start_row;

        for (auto row_2 = start_row + 1; row_2 < size; ++row_2)
        {
            const auto cand = ::Standard::Algorithms::fabs(matr[row_2][start_row]);

            if (max_value < cand)
            {
                max_value = cand;
                max_row = row_2;
            }
        }

        return max_row;
    }
} // namespace Standard::Algorithms::Numbers::Inner

namespace Standard::Algorithms::Numbers
{
    enum class [[nodiscard]] gaussian_elimination_result : std::uint8_t
    {
        singular,
        one_solution,
        inf_many_solutions,
    };

    auto operator<< (std::ostream &str, const gaussian_elimination_result &result) -> std::ostream &;

    // todo(p3): move out identity matrix.
    template<class int_t, class matrix_t>
    requires(std::is_arithmetic_v<int_t>)
    constexpr void matrix_identity(const std::size_t size, matrix_t &matr)
    {
        constexpr int_t zero{};
        constexpr int_t one{ 1 };

        matr.resize(size);

        for (std::size_t row{}; row < size; ++row)
        {
            auto &linie = matr[row];
            linie.assign(size, zero);
            linie[row] = one;
        }
    }

    // todo(p3): move out identity matrix.
    template<class int_t, class x_t = std::vector<int_t>, class matrix_t = std::vector<x_t>>
    requires(std::is_arithmetic_v<int_t>)
    [[nodiscard]] constexpr auto matrix_identity(const std::size_t size) -> matrix_t
    {
        constexpr int_t zero{};

        matrix_t matr(size, x_t(size, zero));
        matrix_identity<int_t, matrix_t>(size, matr);

        return matr;
    }

    // todo(p3): move out Matrix multiply.
    // Time O(n**3).
    template<class floating_t, class matrix_t, class x_t = std::vector<floating_t>>
    [[nodiscard]] constexpr auto matrix_product_slow(const std::size_t rows, //
        const matrix_t &matrix_1, const std::size_t columns_1, //
        const matrix_t &matrix_2, const std::size_t columns_2, //
        const bool is_matrix_1_augmented = false) -> matrix_t
    {
        require_positive(rows, "rows");
        require_positive(columns_1, "columns 1 and rows 2");
        require_positive(columns_2, "columns 2");

        const auto aug = is_matrix_1_augmented ? 1ZU : 0ZU;

        require_equal(rows, "matrix 1 rows", matrix_1.size());
        require_equal(columns_1 + aug, "matrix 1 columns", matrix_1.at(0).size());
        require_equal(columns_1, "matrix 2 rows", matrix_2.size());

        for (std::size_t row{}; row < columns_1; ++row)
        {
            const auto &linie = matrix_2[row];
            require_equal(columns_2, "matrix 2 columns at row", linie.size());
        }

        matrix_t result(rows, x_t(columns_2, floating_t{}));

        for (std::size_t row{}; row < rows; ++row)
        {
            const auto &linie_1 = matrix_1[row];
            require_equal(columns_1 + aug, "matrix 1 columns at row", linie_1.size());

            auto &dest = result[row];

            for (std::size_t mid{}; mid < columns_1; ++mid)
            {
                const auto &coef = linie_1[mid];
                const auto &linie_2 = matrix_2[mid];

                for (std::size_t col{}; col < columns_2; ++col)
                {
                    // CPU L3 cache misses are lower this way.
                    dest[col] += coef * linie_2[col];
                }
            }
        }

        return result;
    }

    // Gaussian elimination is used to solve a system of linear equations and
    // for calculating: determinant, invertible square matrix inverse, matrix rank.
    // It is good for one-time computation.
    // Time O(n**3).
    template<std::floating_point floating_t, class matrix_t>
    [[nodiscard]] constexpr auto gaussian_elimination(const std::int32_t size,
        // The matrix is edited.
        matrix_t &matr) -> std::pair<gaussian_elimination_result, std::vector<floating_t>>
    {
        require_positive(size, "size");
        require_greater(matr.size(), static_cast<std::size_t>(size - 1), "matrix size");
        require_greater(matr[0].size(), static_cast<std::size_t>(size), "matrix columns");

        constexpr floating_t zero{};

        for (std::int32_t row{}; row < size; ++row)
        {
            const auto max_row = Inner::find_max_to_reduce_error<matrix_t>(size, matr, row);

            auto &lines = matr[row];

            if (zero == lines[max_row])
            {
                const auto has = zero == lines[size];

                // todo(p3): fix 8 solutions - could be none - check the remaining matrix parts.

                return { has ? gaussian_elimination_result::inf_many_solutions : gaussian_elimination_result::singular,
                    {} };
            }

            if (max_row != row)
            {
                auto &lines_2 = matr[max_row];

                for (auto col = row; col <= size; ++col)
                {
                    std::swap(lines[col], lines_2[col]);
                }
            }

            const auto &maxi = static_cast<floating_t>(lines[row]);
            assert(zero != maxi);

            for (auto row_2 = row + 1; row_2 < size; ++row_2)
            {
                auto &line_3 = matr[row_2];
                const auto coef = line_3[row] / maxi;

                line_3[row] = {};

                for (auto col_2 = row + 1; col_2 <= size; ++col_2)
                {
                    line_3[col_2] -= lines[col_2] * coef;
                }
            }
        }

        std::vector<floating_t> solut(size);

        for (auto row = size - 1; 0 <= row; --row)
        {
            const auto &lines = matr[row];

            auto rhs = static_cast<floating_t>(lines[size]);

            for (auto col = row + 1; col < size; ++col)
            {
                rhs -= lines[col] * solut[col];
            }

            assert(lines[row] != zero); // todo(p3): div by 0?

            solut[row] = rhs / lines[row];
        }

        return { gaussian_elimination_result::one_solution, std::move(solut) };
    }

    // LUP part 1. The LUP decomposition output can be reused many times.
    // Both Lower-left and Upper-right matrices are stored in the editable source matrix,
    // where its main diagonal will keep the upper-right diagonal items,
    // and the lower-left matrix actually does not store the diagonal ones.
    // See also solve_via_lup.
    // Time O(n**3).
    template<std::floating_point floating_t, class matrix_t>
    [[nodiscard]] constexpr auto bottom_left_top_right_permut_decomposition(const std::int32_t size,
        matrix_t &matrix_decompot, std::vector<std::int32_t> &permutation) -> gaussian_elimination_result
    {
        require_positive(size, "size");
        require_equal(static_cast<std::size_t>(size), "matrix rows", matrix_decompot.size());
        require_less_equal(static_cast<std::size_t>(size), matrix_decompot[0].size(), "matrix columns");

        permutation.resize(size);
        std::iota(permutation.begin(), permutation.end(), std::int32_t{});

        {// Example. Source matrix {{2, 4}, {3, 12}},
          // imaginary permutation matrix {{0, 1}, {1, 0}}, actual permutation {1, 0};
          // imaginary LL matrix {{1, 0}, {2/3.0, 1}}, imaginary RU matrix {{3, 12}, {0, -4}};
          // actual LU matrix {{3, 12}, {2/3.0, -4}}.
          // Check: Perm * A = L * U.
          // Perm * A = {{0, 1},   {{2, 4},
          //                    {1, 0}} *   {3, 12}} =
          // = {{3, 12},
          //     {2, 4}}.
          // L * U = {{1, 0},         {{3, 12},
          //              {2/3.0, 1}} *   {0, -4}} =
          // = {{3, 12},
          //      {2, 4}}. The check has passed, great Ivan.
        }

        constexpr floating_t zero{};

        for (std::int32_t row{}; row < size; ++row)
        {
            const auto max_row = Inner::find_max_to_reduce_error<matrix_t>(size, matrix_decompot, row);

            auto &lines = matrix_decompot[row];

            if (zero == lines[max_row])
            {
                return gaussian_elimination_result::singular;
            }

            if (max_row != row)
            {
                std::swap(permutation[max_row], permutation[row]);

                auto &lines_2 = matrix_decompot[max_row];

                for (std::int32_t col{}; col < size; ++col)
                {
                    std::swap(lines[col], lines_2[col]);
                }
            }

            const auto &maxi = static_cast<floating_t>(lines[row]);
            assert(zero != maxi);

            for (auto row_2 = row + 1; row_2 < size; ++row_2)
            {
                auto &line_3 = matrix_decompot[row_2];
                auto &coef = line_3[row];
                coef /= maxi;

                for (auto col_2 = row + 1; col_2 < size; ++col_2)
                {
                    line_3[col_2] -= coef * lines[col_2];
                }
            }
        }

        return gaussian_elimination_result::one_solution;
    }

    // LUP part 2. See also bottom_left_top_right_permut_decomposition.
    // Time O(n**2).
    template<std::floating_point floating_t, class matrix_t, class x_t>
    constexpr void solve_via_lup(const x_t &b_coefficients, //
        const auto &permutation, const matrix_t &lower_upper_compot, x_t &sol)
    {
        const auto size = require_positive(b_coefficients.size(), "Right hand side B coefficients size.");

        // todo(p4): validate the input thoroughly.
        assert(size == b_coefficients.size() && size == permutation.size());
        assert(0U < size && size == lower_upper_compot.size() && size <= lower_upper_compot[0].size());

        if constexpr (Inner::shall_print_gauss_elim)
        {
            ::Standard::Algorithms::print("\n\nSolve via LUP b_coefficients", b_coefficients, std::cout);
            ::Standard::Algorithms::print("permutation", permutation, std::cout);
            ::Standard::Algorithms::print("lower_upper_compot", lower_upper_compot, std::cout);
        }

        // Perm * A = L * U; by LUP decomposition.
        // A * x = B; to be solved.
        // Perm * A * x = Perm * B.
        // L * U * x = Perm * B.
        // U * x = w; introduce a new variable w.
        constexpr floating_t zero{};
        {
            constexpr auto reuse_sol_memory_for_www = 2LLU;

            sol.assign(size * reuse_sol_memory_for_www, zero);
        }

        // L * w = Perm * B; let's solve for w.
        std::span www = sol;
        www = www.subspan(size, size);

        // L sample:
        // {{1, 0},
        //  {2/3.0, 1}}
        // Note. The lower_upper_compot does not actually store 1 in the diagonal!

        // {1, 0}                  = a = -15; RHS is already permuted.
        // {2/3.0, 1, 0} * w = b = -4
        // First, w[0] = a = -15. Let's subtract beta times the 0-th row from the next rows.
        // beta1 = 2/3.0; {2/3.0 - 2/3.0, 1} * w[1] = b - 2/3.0*a
        // {0, 1} * w[1] = b - 2/3.0*w[0] = -4 + 2/3.0*15 = -4 + 10 = 6
        // Second, w[1] = 6. Stop.

        for (std::size_t row{}; row < size; ++row)
        {
            const auto &col = permutation[row];
            assert(0 <= col && static_cast<std::size_t>(col) < size);

            auto &mul = www[row];
            mul += b_coefficients.at(col);

            if (zero == mul)
            {
                continue;
            }

            for (auto row_2 = row; ++row_2 < size;)
            {// Subtract from the next rows.
                const auto &beta = lower_upper_compot[row_2].at(row);
                const auto prod = static_cast<floating_t>(beta * mul);
                auto &val = www[row_2];
                val -= prod;
            }
        }

        if constexpr (Inner::shall_print_gauss_elim)
        {
            ::Standard::Algorithms::print("sol + First www", sol, std::cout);
        }

        // U * x = w; let's solve for x.
        // U sample, all diagonal items are non-zero as U is not singular:
        // {{3, 12},
        //  {0, -4}}
        //
        // {3, 12} * x = w[0] = -15
        // {0, -4}       = w[1] = 6
        // Divide the last row by -4, assigning w[1] = w[1]/-4 = 6/-4 = -1.5:
        // {0, 1} * x[1] = w[1]
        // Last, x[1] = w[1] = -1.5. Then, subtract beta times from the previous rows.
        // beta0=12, {3, 12 - 12} * x[0] = w[0] - 12*x[1] = -15 - 12*-1.5 = 3
        // {3, 0} * x[0] = 3
        // First, divide by 3, x[0] = 3/3 = 1. Stop.
        for (auto row = size; 0U < row--;)
        {
            auto &mul = sol[row];

            {
                const auto &div = lower_upper_compot[row].at(row);
                if (zero == div) [[unlikely]]
                {
                    throw std::runtime_error("Division by 0 in LUP solve - must have got a non-singular U matrix.");
                }

                mul = www[row] / div;
            }

            if (zero == mul)
            {
                continue;
            }

            for (std::size_t row_2{}; row_2 < row; ++row_2)
            {// Subtract from the previous rows.
                const auto &beta = lower_upper_compot[row_2].at(row);
                const auto prod = static_cast<floating_t>(beta * mul);
                auto &val = www[row_2];
                val -= prod;
            }
        }

        if constexpr (Inner::shall_print_gauss_elim)
        {
            ::Standard::Algorithms::print("sol + Last www", sol, std::cout);
        }

        sol.resize(size);
    }
} // namespace Standard::Algorithms::Numbers
