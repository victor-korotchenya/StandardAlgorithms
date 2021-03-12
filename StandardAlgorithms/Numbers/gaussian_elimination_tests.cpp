#include"gaussian_elimination_tests.h"
#include"../Utilities/random_matrix.h"
#include"../Utilities/require_matrix.h"
#include"../Utilities/test_utilities.h"
#include"gaussian_elimination.h"
#include"matrix_transpose.h"
#include"permutation.h"
#include<optional>

auto Standard::Algorithms::Numbers::operator<< (
    std::ostream &str, const Standard::Algorithms::Numbers::gaussian_elimination_result &result) -> std::ostream &
{
    switch (result)
    {
        using enum gaussian_elimination_result;

    case singular:
        str << "singular";
        break;
    case one_solution:
        str << "one solution";
        break;
    case inf_many_solutions:
        str << "infinitely many solutions";
        break;
    default:
        if constexpr (::Standard::Algorithms::is_debug)
        {
            auto str2 = ::Standard::Algorithms::Utilities::w_stream();
            str2 << "Unknown value of gaussian_elimination_result " << std::to_underlying(result);

            throw std::out_of_range(str2.str());
        }
        else
        {
            assert(0);
            std::unreachable();
        }
    }

    return str;
}

namespace
{
    using floating_t = Standard::Algorithms::floating_point_type;
    using x_t = std::vector<floating_t>;
    using matrix_t = std::vector<x_t>;

    using sol_t = std::pair<Standard::Algorithms::Numbers::gaussian_elimination_result, x_t>;

    using permutation_t = std::vector<std::int32_t>;
    using lup_t = std::pair<permutation_t, matrix_t>;

    constexpr auto shall_print_gauss_elim = Standard::Algorithms::Numbers::Inner::shall_print_gauss_elim;
    constexpr auto transposed_sol_columns = 1ZU;

    constexpr floating_t epsilon = 1.0e-3;
    static_assert(floating_t{} < epsilon);

    // todo(p4): del it?
    template<std::integral int_t, class destiny_t = int_t>
    requires(std::convertible_to<int_t, destiny_t>)
    [[nodiscard]] constexpr auto to_full_permutation(const std::vector<int_t> &permutation)
        -> std::vector<std::vector<destiny_t>>
    {
        constexpr destiny_t zero{};
        constexpr destiny_t one{ 1 };

        const auto size = permutation.size();

        std::vector<std::vector<destiny_t>> full_perm(size, std::vector<destiny_t>(size, zero));

        for (std::size_t row{}; row < size; ++row)
        {
            const auto &col = permutation[row];
            auto &linie = full_perm[row];
            linie.at(col) = one;
        }

        return full_perm;
    }

    constexpr void verify_one_solution(const matrix_t &source_matrix, const x_t &sol)
    {
        const auto size = Standard::Algorithms::require_positive(source_matrix.size(), "source matrix size.");
        ::Standard::Algorithms::ert::are_equal(size, sol.size(), "solution size.");

        constexpr auto is_augmented = true;

        const auto transposed_sol = Standard::Algorithms::Numbers::matrix_transpose(sol);

        const auto source_times_sol = Standard::Algorithms::Numbers::matrix_product_slow<floating_t>(
            size, source_matrix, size, transposed_sol, transposed_sol_columns, is_augmented);

        ::Standard::Algorithms::ert::are_equal(size, source_times_sol.size(), "source * sol size");

        for (std::size_t index{}; index < size; ++index)
        {
            const auto &prod_line = source_times_sol.at(index);
            ::Standard::Algorithms::ert::are_equal(1ZU, prod_line.size(), "source_times_sol[index] size");

            const auto &expected_prod = source_matrix[index].at(size);
            const auto &actual_prod = prod_line[0];
            const auto name = "actual product A*x at " + std::to_string(index);

            ::Standard::Algorithms::ert::are_equal_with_epsilon(expected_prod, actual_prod, name, epsilon);
        }
    }

    constexpr void verify_lup_decomp(
        const permutation_t &permutation, const matrix_t &source_matrix, const matrix_t &decompot)
    {
        const auto size = Standard::Algorithms::require_positive(source_matrix.size(), "source matrix rows.");
        ::Standard::Algorithms::ert::are_equal(size, permutation.size(), "permutation size.");
        ::Standard::Algorithms::ert::are_equal(size, decompot.size(), "de compot rows.");

        constexpr floating_t zero{};
        constexpr floating_t one = 1;

        auto lower_lefts = decompot;
        auto up_rights = decompot;
        auto square_matrix = source_matrix;

        for (std::size_t row{}; row < size; ++row)
        {
            {
                auto &lefts = lower_lefts[row];
                lefts.resize(size);
                lefts[row] = one;

                for (auto col = row + 1ZU; col < size; ++col)
                {
                    lefts[col] = zero;
                }
            }
            {
                auto &rights = up_rights[row];
                rights.resize(size);

                for (std::size_t col{}; col < row; ++col)
                {
                    rights[col] = zero;
                }
            }
            {
                auto &square = square_matrix[row];
                square.resize(size);
            }
        }

        const auto ones_matrix = to_full_permutation<std::int32_t, floating_t>(permutation);

        const auto ones_src_prod =
            Standard::Algorithms::Numbers::matrix_product_slow<floating_t>(size, ones_matrix, size, square_matrix, size);
        ::Standard::Algorithms::ert::are_equal(size, ones_src_prod.size(), "perm_ones*source_square_matrix size");

        const auto left_right_prod =
            Standard::Algorithms::Numbers::matrix_product_slow<floating_t>(size, lower_lefts, size, up_rights, size);
        ::Standard::Algorithms::ert::are_equal(size, left_right_prod.size(), "L*U size");

        if constexpr (shall_print_gauss_elim)
        {
            ::Standard::Algorithms::print("\n\n Verify LUP decomp source_matrix", source_matrix, std::cout);
            ::Standard::Algorithms::print("lower_lefts", lower_lefts, std::cout);
            ::Standard::Algorithms::print("up_rights", up_rights, std::cout);
            ::Standard::Algorithms::print("ones_matrix", ones_matrix, std::cout);
            ::Standard::Algorithms::print("square_matrix", square_matrix, std::cout);
            ::Standard::Algorithms::print("left_right_prod", left_right_prod, std::cout);
            ::Standard::Algorithms::print("ones_src_prod", ones_src_prod, std::cout);
        }

        for (std::size_t row{}; row < size; ++row)
        {
            const auto &odna = ones_src_prod.at(row);
            const auto &deva = left_right_prod.at(row);
            const auto name = "Perm*A == L*U at row " + std::to_string(row);

            ::Standard::Algorithms::ert::are_equal_with_epsilon(odna, deva, name, epsilon);
        }
    }

    // Time O(n).
    [[nodiscard]] constexpr auto exract_coefficients(const matrix_t &source_matrix) -> x_t
    {
        const auto size = source_matrix.size();
        x_t coefficients(size);

        for (std::size_t row{}; row < size; ++row)
        {
            coefficients[row] = source_matrix[row].at(size);
        }

        return coefficients;
    }

    // Time O(n**2).
    constexpr void verify_one_solution_lup(const permutation_t &permutation, const matrix_t &source_matrix,
        const x_t &expected_sol, const matrix_t &matrix_decompot)
    {
        const auto b_coefficients = exract_coefficients(source_matrix);

        if constexpr (shall_print_gauss_elim)
        {
            ::Standard::Algorithms::print("b_coefficients", b_coefficients, std::cout);
            ::Standard::Algorithms::print("Expected solution", expected_sol, std::cout);
        }

        x_t sol;
        Standard::Algorithms::Numbers::solve_via_lup<floating_t>(b_coefficients, permutation, matrix_decompot, sol);

        if constexpr (shall_print_gauss_elim)
        {
            ::Standard::Algorithms::print("Actual solution", sol, std::cout);
        }

        ::Standard::Algorithms::ert::are_equal_with_epsilon(expected_sol, sol, "LUP solution", epsilon);
    }

    struct test_case final : Standard::Algorithms::Tests::base_test_case
    {
        constexpr test_case(
            std::string &&name, matrix_t &&matrix, std::optional<sol_t> &&sol = {}, std::optional<lup_t> &&lup = {}) noexcept
            : base_test_case(std::move(name))
            , Matrix(std::move(matrix))
            , Sol(std::move(sol))
            , Lup(std::move(lup))
        {
        }

        [[nodiscard]] constexpr auto matrix() const &noexcept -> const matrix_t &
        {
            return Matrix;
        }

        [[nodiscard]] constexpr auto solution() const &noexcept -> const std::optional<sol_t> &
        {
            return Sol;
        }

        [[nodiscard]] constexpr auto lup() const &noexcept -> const std::optional<lup_t> &
        {
            return Lup;
        }

        constexpr void validate() const override
        {
            base_test_case::validate();

            const auto size = Standard::Algorithms::require_positive(Matrix.size(), "matrix size.");

            ::Standard::Algorithms::ert::are_equal(size + 1ZU, Matrix[0].size(), "matrix columns.");

            constexpr Standard::Algorithms::check_matrix_options options{
                .some_rows = true,
                .some_columns = true,
            };
            static_assert(options.throw_on_error);

            Standard::Algorithms::require_matrix(Matrix, options);

            if (Sol.has_value())
            {
                const auto &[res, sol] = Sol.value();

                if (res == Standard::Algorithms::Numbers::gaussian_elimination_result ::one_solution)
                {
                    ::Standard::Algorithms::ert::are_equal(size, sol.size(), "solution size");
                }
            }

            if (Lup.has_value())
            {
                const auto &[permutation, decompot] = Lup.value();
                ::Standard::Algorithms::ert::are_equal(size, permutation.size(), "permutation size");

                ::Standard::Algorithms::ert::are_equal(size, decompot.size(), "decompot rows.");
                ::Standard::Algorithms::ert::are_equal(size, decompot[0].size(), "decompot columns.");

                Standard::Algorithms::require_matrix(decompot, options);
            }
        }

        void print(std::ostream &str) const override
        {
            ::Standard::Algorithms::print("input matrix", Matrix, str);

            if (Sol.has_value())
            {
                const auto &[res, sol] = Sol.value();
                str << ", solution kind " << res;
                ::Standard::Algorithms::print(", solution vector", sol, str);
            }
            else
            {
                str << ", unknown solution";
            }

            if (Lup.has_value())
            {
                ::Standard::Algorithms::print(", LUP permutation", Lup.value().first, str);
                ::Standard::Algorithms::print(", LUP matrix", Lup.value().second, str);
            }
            else
            {
                str << ", unknown LUP";
            }
        }

private:
        matrix_t Matrix;
        std::optional<sol_t> Sol;
        std::optional<lup_t> Lup;
    };

    void generate_test_cases(std::vector<test_case> &test_cases)
    {
        // {{2, 4, -4},
        //  {3, 12, -15}}. Source.
        // {{3, 12, -15},
        //  {2, 4, -4}}. Swap rows 0, 1.
        // {{3, 12, -15},
        //  {2 - 3*2/3.0, 4 - 12*2/3.0, -4 + 15*2/3.0}}. Subtract 2/3.0 of the row 0.
        // {{3, 12, -15},
        //  {0, -4, 6}}. Simplify.
        test_cases.emplace_back("base2",
            matrix_t{// NOLINTNEXTLINE
                { 2, 4, -4 }, { 3, 12, -15 } },
            sol_t{ Standard::Algorithms::Numbers::gaussian_elimination_result::one_solution, // NOLINTNEXTLINE
                { 1, -1.5 } },
            lup_t{ permutation_t{ 1, 0 },
                matrix_t{// NOLINTNEXTLINE
                    { 3, 12 }, { 2 / 3.0, -4 } } });

        test_cases.emplace_back("base3",
            matrix_t{// NOLINTNEXTLINE
                // { 1, 0, 0 } * { 2, 3, 4 } = { 2, 3, 4 }
                // { 6, 1, 0 } * { 0, 7, 8 } = { 12, 25, 32 }
                // { 10, 9, 1} * { 0, 0, 11} = { 20, 93, 123 } NOLINTNEXTLINE
                { 2, 3, 4, 2 * 1 + 3 * 2 + 4 * 3 }, // NOLINTNEXTLINE
                { 12, 25, 32, 12 + 50 + 96 }, // NOLINTNEXTLINE
                { 20, 93, 123, 20 + 186 + 369 } },
            sol_t{ Standard::Algorithms::Numbers::gaussian_elimination_result::one_solution, // NOLINTNEXTLINE
                { 1, 2, 3 } },
            lup_t{ permutation_t{// NOLINTNEXTLINE
                       2, 1, 0 },
                matrix_t{// NOLINTNEXTLINE
                    { 20, 93, 123 }, // NOLINTNEXTLINE
                    { 0.6, -30.8, -41.8 }, // NOLINTNEXTLINE
                    { 0.1, 225 / 1100.0, 0.25 } } });

        test_cases.emplace_back("base 33",
            matrix_t{// NOLINTNEXTLINE
                { 3, 2, -4, 3 }, { 2, 3, 3, 15 }, { 5, -3, 1, 14 } },
            sol_t{ Standard::Algorithms::Numbers::gaussian_elimination_result::one_solution, // NOLINTNEXTLINE
                { 3, 1, 2 } },
            lup_t{ permutation_t{// NOLINTNEXTLINE
                       2, 1, 0 },
                matrix_t{// NOLINTNEXTLINE
                    { 5, -3, 1 }, // NOLINTNEXTLINE
                    { 0.4, 4.2, 2.6 }, // NOLINTNEXTLINE
                    { 0.6, 0.9047619047619046561692, -6.952380952380951661951 } } });

        test_cases.emplace_back("base1",
            matrix_t{// NOLINTNEXTLINE
                { 5, 10 } },
            sol_t{ Standard::Algorithms::Numbers::gaussian_elimination_result::one_solution, { 2 } },
            lup_t{ permutation_t{// NOLINTNEXTLINE
                       0 },
                matrix_t{// NOLINTNEXTLINE
                    { 5 } } });

        test_cases.emplace_back("infinitely many solutions",
            matrix_t{// NOLINTNEXTLINE
                { 3, 7, 10 }, { 3, 7, 10 } },
            sol_t{ Standard::Algorithms::Numbers::gaussian_elimination_result::inf_many_solutions, {} });

        test_cases.emplace_back("infinitely many solutions -1",
            matrix_t{// NOLINTNEXTLINE
                { 3, 7, 10 }, { -3, -7, -10 } },
            sol_t{ Standard::Algorithms::Numbers::gaussian_elimination_result::inf_many_solutions, {} });

        test_cases.emplace_back("infinitely many solutions 2",
            matrix_t{// NOLINTNEXTLINE
                { 3, 7, 10 }, { 3 * 2, 7 * 2, 10 * 2 } },
            sol_t{ Standard::Algorithms::Numbers::gaussian_elimination_result::inf_many_solutions, {} });

        test_cases.emplace_back("singular 1",
            matrix_t{// NOLINTNEXTLINE
                { 3, 7, 10 }, { 3, 7, 10 + 1 } },
            sol_t{ Standard::Algorithms::Numbers::gaussian_elimination_result::singular, {} });

        constexpr auto max_attempts = 1;

        for (std::int32_t att{}; att < max_attempts; ++att)
        {
            constexpr auto add_extra_column = true;

            auto matrix = Standard::Algorithms::Utilities ::random_matrix<floating_t>(add_extra_column);

            test_cases.emplace_back("Random" + std::to_string(att), std::move(matrix));
        }
    }

    constexpr void run_test_case(const test_case &test)
    {
        const auto &source_matrix = test.matrix();
        const auto size = static_cast<std::int32_t>(source_matrix.size());

        auto matr = source_matrix;
        const auto [actual_res, actual_sol] = Standard::Algorithms::Numbers::gaussian_elimination<floating_t>(size, matr);

        const auto has_1_sol = actual_res == Standard::Algorithms::Numbers::gaussian_elimination_result::one_solution;

        if (has_1_sol)
        {
            verify_one_solution(source_matrix, actual_sol);
        }

        if (const auto &expected = test.solution(); expected.has_value())
        {
            const auto &[test_res, test_sol] = expected.value();

            ::Standard::Algorithms::ert::are_equal(test_res, actual_res, "gaussian_elimination precomputed kind");

            if (has_1_sol)
            {
                ::Standard::Algorithms::ert::are_equal_with_epsilon(
                    test_sol, actual_sol, "Gaussian elimination precomputed solution", epsilon);
            }
        }

        auto matrix_decompot = source_matrix;

        for (auto &linie : matrix_decompot)
        {// Remove the right side coefficients.
            linie.resize(size);
        }

        permutation_t permutation;

        const auto res_dec =
            Standard::Algorithms::Numbers::bottom_left_top_right_permut_decomposition<floating_t, matrix_t>(
                size, matrix_decompot, permutation);

        ::Standard::Algorithms::ert::are_equal(static_cast<std::size_t>(size), permutation.size(), "permutation size");

        {
            constexpr auto is_zero_start = true;

            const auto is_perm = ::Standard::Algorithms::Numbers::is_permutation_2(permutation, is_zero_start);

            ::Standard::Algorithms::ert::are_equal(true, is_perm, "is permutation");
        }

        const auto &expected_lup = test.lup();

        if (res_dec == Standard::Algorithms::Numbers::gaussian_elimination_result::singular)
        {
            ::Standard::Algorithms::ert::not_equal(Standard::Algorithms::Numbers::gaussian_elimination_result::one_solution,
                actual_res, "bottom_left_top_right_permut_decomposition kind");

            ::Standard::Algorithms::ert::are_equal(false, expected_lup.has_value(), "LUP cannot exist");

            return;
        }

        verify_lup_decomp(permutation, source_matrix, matrix_decompot);

        if (expected_lup.has_value())
        {
            ::Standard::Algorithms::ert::are_equal(
                expected_lup.value().first, permutation, "bottom_left_top_right_permut_decomposition permutation");

            ::Standard::Algorithms::ert::are_equal_with_epsilon(expected_lup.value().second, matrix_decompot,
                "bottom_left_top_right_permut_decomposition LUP", epsilon);
        }

        if (has_1_sol)
        {
            verify_one_solution_lup(permutation, source_matrix, actual_sol, matrix_decompot);
        }
    }
} // namespace

void Standard::Algorithms::Numbers::Tests::gaussian_elimination_tests()
{
    // todo(p3): test many solutions, but fix the code first.
    Standard::Algorithms::Tests::test_utilities<test_case>::run_tests(run_test_case, generate_test_cases);
}
