#include"simplex_method_tests.h"
#include"../Utilities/random.h"
#include"../Utilities/test_utilities.h"
#include"simplex_method.h"

namespace
{
    // todo(p4): using floating_t = Standard::Algorithms::floating_point_type;
    using floating_t = double;
    using v_t = std::vector<floating_t>;
    using v_t_2d = std::vector<v_t>;
    using simplex_result = Standard::Algorithms::Geometry::simplex_result;
    using expected_t = std::pair<floating_t, simplex_result>;

    // Dot product(objective_func_coefficients, optimal_variables) ~== expected_max
    constexpr void check_dot_product(std::size_t columns, const v_t &objective_func_coefficients,
        const floating_t &expected_max, const v_t &optimal_variables)
    {
        columns = std::min(columns, optimal_variables.size());
        Standard::Algorithms::require_positive(columns, "columns in check_dot_product");

        const auto *const name = "expected max";

        Standard::Algorithms::require_less_equal(floating_t{}, expected_max, name);

        floating_t actual{};

        for (std::size_t col{}; col < columns; ++col)
        {
            const auto &coef = objective_func_coefficients.at(col);
            const auto &vari = optimal_variables.at(col);
            const auto prod = static_cast<floating_t>(coef * vari);
            actual += prod;
        }

        ::Standard::Algorithms::ert::are_equal_with_epsilon(expected_max, actual, name);
    }

    // coef_matrix * optimal_variables <= right_sides
    constexpr void check_lp_constraints(const v_t &optimal_variables, const v_t_2d &coef_matrix, const v_t &right_sides)
    {
        const auto rows = Standard::Algorithms::require_positive(coef_matrix.size(), "rows");

        const auto columns = std::min(coef_matrix.at(0).size(), optimal_variables.size());
        Standard::Algorithms::require_positive(columns, "columns in check_lp_constraints");

        std::string name;

        for (std::size_t row{}; row < rows; ++row)
        {
            const auto &coefs = coef_matrix.at(row);
            floating_t actual{};

            for (std::size_t col{}; col < columns; ++col)
            {
                const auto &coef = coefs.at(col);
                const auto &vari = optimal_variables.at(col);
                const auto prod = static_cast<floating_t>(coef * vari);
                actual += prod;
            }

            const auto &right = right_sides.at(row);
            if (right < actual)
            {
                name = "Constraint check at row " + std::to_string(row);

                ::Standard::Algorithms::ert::are_equal_with_epsilon(right, actual, name);
            }
        }
    }

    constexpr void require_feasible_lp_input(const v_t &objective_func_coefficients, const v_t_2d &coef_matrix,
        const v_t &right_sides, const floating_t &expected_max, const v_t &optimal_variables)
    {
        Standard::Algorithms::require_positive(coef_matrix.size(), "rows");

        Standard::Algorithms::require_positive(optimal_variables.size(), "optimal variables size");
        Standard::Algorithms::require_all_non_negative(optimal_variables, "optimal variables");

        const auto columns = Standard::Algorithms::require_positive(coef_matrix.at(0).size(), "columns");

        check_lp_constraints(optimal_variables, coef_matrix, right_sides);

        check_dot_product(columns, objective_func_coefficients, expected_max, optimal_variables);
    }

    struct test_case final : Standard::Algorithms::Tests::base_test_case
    {
        constexpr test_case(std::string &&name, v_t &&objective_func_coefficients, v_t_2d &&coef_matrix,
            v_t &&right_sides, expected_t &&expected, v_t &&optimal_variables = {}) noexcept
            : base_test_case(std::move(name))
            , Objective_func_coefficients(std::move(objective_func_coefficients))
            , Coef_matrix(std::move(coef_matrix))
            , Right_sides(std::move(right_sides))
            , Expected(std::move(expected))
            , Optimal_variables(std::move(optimal_variables))
        {
            if (Optimal_variables.empty())
            {
                Optimal_variables.resize(Objective_func_coefficients.size());
            }
        }

        [[nodiscard]] constexpr auto objective_func_coefficients() const &noexcept -> const v_t &
        {
            return Objective_func_coefficients;
        } // columns.

        [[nodiscard]] constexpr auto coef_matrix() const &noexcept -> const v_t_2d &
        {
            return Coef_matrix;
        } // rows by columns.

        [[nodiscard]] constexpr auto right_sides() const &noexcept -> const v_t &
        {
            return Right_sides;
        } // rows.

        [[nodiscard]] constexpr auto expected() const &noexcept -> const expected_t &
        {
            return Expected;
        }

        [[nodiscard]] constexpr auto optimal_variables() const &noexcept -> const v_t &
        {
            return Optimal_variables;
        } // columns.

        constexpr void validate() const override
        {
            const auto rows = Standard::Algorithms::require_positive(Coef_matrix.size(), "rows");
            ::Standard::Algorithms::ert::are_equal(rows, Right_sides.size(), "Right side size");

            const auto columns = Standard::Algorithms::require_positive(Coef_matrix.at(0).size(), "columns");
            ::Standard::Algorithms::ert::are_equal(
                columns, Objective_func_coefficients.size(), "Objective func coefficients size");

            {
                const auto *const name = "Optimal variables size";
                const auto ops = Optimal_variables.size();
                Standard::Algorithms::require_positive(ops, name);
                Standard::Algorithms::require_less_equal(ops, columns, name);
            }

            if (const auto &res = Expected.second;
                res != simplex_result::one_solution && res != simplex_result::many_solutions)
            {
                return;
            }

            require_feasible_lp_input(
                Objective_func_coefficients, Coef_matrix, Right_sides, Expected.first, Optimal_variables);
        }

        void print(std::ostream &str) const override
        {
            ::Standard::Algorithms::print("Objective func coefficients", Objective_func_coefficients, str);
            ::Standard::Algorithms::print("Coef matrix", Coef_matrix, str);
            ::Standard::Algorithms::print("Right sides", Right_sides, str);
            ::Standard::Algorithms::print("Expected", Expected, str);
            ::Standard::Algorithms::print("Optimal variables", Optimal_variables, str);
        }

private:
        v_t Objective_func_coefficients; // columns.
        v_t_2d Coef_matrix; // rows by columns.
        v_t Right_sides; // rows.

        expected_t Expected;
        v_t Optimal_variables; // rows.
    };

    constexpr void add_infeasible_cases(std::vector<test_case> &tests)
    {
        constexpr auto infeasible = simplex_result::infeasible;

        tests.emplace_back("no solution several",
            // f = 58*x + 6*y
            // x <= 5
            // y <= 4
            // 50*x + 172*y <= -2023
            // NOLINTNEXTLINE
            v_t{ 58, 6, 0, 0, 0 }, // obj func // NOLINTNEXTLINE
            v_t_2d{ { 1, 0, 1, 0, 0 }, { 0, 1, 0, 1, 0 }, { 50, 172, 0, 0, 1 } }, // NOLINTNEXTLINE
            v_t{ 5, 4, -2023 }, // right sides // NOLINTNEXTLINE
            expected_t{ 0, infeasible });

        tests.emplace_back("no cross solution",
            // f = x
            // x <= 5
            // x >= 1900 ; or -x <= -1900
            // NOLINTNEXTLINE
            v_t{ 1, 0, 0 }, // obj func // NOLINTNEXTLINE
            v_t_2d{ { 1, 1, 0 }, { -1, 0, 1 } }, // NOLINTNEXTLINE
            v_t{ 5, -1900 }, // right sides // NOLINTNEXTLINE
            expected_t{ 0, infeasible });

        tests.emplace_back("no solution pine",
            // f = 59*x
            // 7*x <= -2022
            // NOLINTNEXTLINE
            v_t{ 59, 0 }, // obj func // NOLINTNEXTLINE
            v_t_2d{ { 7, 1 } }, // NOLINTNEXTLINE
            v_t{ -2022 }, // right sides // NOLINTNEXTLINE
            expected_t{ 0, infeasible });

        tests.emplace_back("no solution pine amore",
            // f = 102*x
            // 5*x <= 2222
            // 7*x <= -2022
            // NOLINTNEXTLINE
            v_t{ 102, 0, 0 }, // obj func // NOLINTNEXTLINE
            v_t_2d{ { 5, 1, 0 }, { 7, 0, 1 } }, // NOLINTNEXTLINE
            v_t{ 2222, -2022 }, // right sides // NOLINTNEXTLINE
            expected_t{ 0, infeasible });

        tests.emplace_back("alt ansatz",
            // f = 102*x
            // 0*x <= -3323
            // NOLINTNEXTLINE
            v_t{ 102, 0 }, // obj func // NOLINTNEXTLINE
            v_t_2d{ { 0, 1 } }, // NOLINTNEXTLINE
            v_t{ -3323 }, // right sides // NOLINTNEXTLINE
            expected_t{ 0, infeasible });
    }

    constexpr void add_unbounded_cases(std::vector<test_case> &tests)
    {
        constexpr auto unbounded = simplex_result::unbounded;

        tests.emplace_back("unbounded one",
            // max f = 8*x = inf
            // -2*x <= 70 ; or -70 <= 2*x ; or -35 <= x.
            // NOLINTNEXTLINE
            v_t{ 8, 0 }, // obj func // NOLINTNEXTLINE
            v_t_2d{ { -2, 1 } }, // NOLINTNEXTLINE
            v_t{ 70 }, // right sides
            expected_t{ 0, unbounded });

        tests.emplace_back("unbounded moron",
            // f = 8*x = inf
            // -2*x <= 70
            // -x <= 3456
            // -x <= 123
            // NOLINTNEXTLINE
            v_t{ 8, 0, 0, 0 }, // obj func // NOLINTNEXTLINE
            v_t_2d{ { -2, 1, 0, 0 }, { -1, 0, 1, 0 }, { -1, 0, 0, 1 } }, // NOLINTNEXTLINE
            v_t{ 70, 3456, 123 }, // right sides
            expected_t{ 0, unbounded });

        tests.emplace_back("unbounded two",
            // f = 10*x + 5*y
            // -x <= 5
            // -y <= 4
            // NOLINTNEXTLINE
            v_t{ 10, 5, 0, 0 }, // obj func // NOLINTNEXTLINE
            v_t_2d{ { -1, 0, 1, 0 }, { 0, -1, 0, 1 } }, // NOLINTNEXTLINE
            v_t{ 5, 4 }, // right sides
            expected_t{ 0, unbounded });

        tests.emplace_back("unbounded tre",
            // f = 10*x - 5*y
            // -x <= 5
            // -y <= 4
            // NOLINTNEXTLINE
            v_t{ 10, -5, 0, 0 }, // obj func // NOLINTNEXTLINE
            v_t_2d{ { -1, 0, 1, 0 }, { 0, -1, 0, 1 } }, // NOLINTNEXTLINE
            v_t{ 5, 4 }, // right sides
            expected_t{ 0, unbounded });
    }

    constexpr void generate_test_cases(std::vector<test_case> &tests)
    {
        constexpr auto one_solution = simplex_result::one_solution;
        constexpr auto many_solutions = simplex_result::one_solution; // todo(p3): many_solutions
        {
            constexpr floating_t max_diff{ 116 };

            tests.emplace_back("floating_point_type failing sorento",
                // f = 2*x - y ; max f(501, 5) = 1002 - 886 = 116
                // 2*x - y <= 116; thus, f = 2*x - y <= 116.
                // x >= 501 ; or -x <= -501
                // y >= 5 ; or -y <= -5
                v_t{ 2, -1, 0, 0, 0 }, // obj func // NOLINTNEXTLINE
                v_t_2d{ { 2, -1, 1, 0, 0 }, { -1, 0, 0, 1, 0 }, { 0, -1, 0, 0, 1 } }, // NOLINTNEXTLINE
                v_t{ max_diff, -501, -5 }, // right sides // NOLINTNEXTLINE
                expected_t{ max_diff, many_solutions }, v_t{ 501, 886 });

            tests.emplace_back("diff test with unbounded region",
                // f = x - y. max f(116, 0) = 116 - 0 = 116
                // x - y <= 116; thus, f = x - y <= 116.
                v_t{ 1, -1, 0 }, // obj func // NOLINTNEXTLINE
                v_t_2d{ { 1, -1, 1 } }, // NOLINTNEXTLINE
                v_t{ max_diff }, // right sides
                expected_t{ max_diff, many_solutions }, v_t{ max_diff, 0 });

            tests.emplace_back("diff test with unbounded region + an xtra var",
                // f = x - y + 0*z ; max f(116, 0, 0) = 116 - 0 + 0= 116
                // x - y <= 116; thus, f = x - y <= 116.
                v_t{ 1, -1, 0, 0 }, // obj func // NOLINTNEXTLINE
                v_t_2d{ { 1, -1, 0, 1 } }, // NOLINTNEXTLINE
                v_t{ max_diff }, // right sides
                expected_t{ max_diff, many_solutions }, v_t{ max_diff, 0, 0 });

            tests.emplace_back("diff test with unbounded region + lower bounds",
                // f = 2*x - y ; max f(60.5, 5) = 121 - 5 = 116
                // 2*x - y <= 116; thus, f = 2*x - y <= 116.
                // x >= 41 ; or -x <= -41
                // y >= 5 ; or -y <= -5
                v_t{ 2, -1, 0, 0, 0 }, // obj func // NOLINTNEXTLINE
                v_t_2d{ { 2, -1, 1, 0, 0 }, { -1, 0, 0, 1, 0 }, { 0, -1, 0, 0, 1 } }, // NOLINTNEXTLINE
                v_t{ max_diff, -41, -5 }, // right sides // NOLINTNEXTLINE
                expected_t{ max_diff, many_solutions }, v_t{ 60.5, 5 });

            tests.emplace_back("basic mess sumka",
                // f = 2*x + y ; max f(111/2.0, 5) = 111 + 5 = 116
                // 2*x + y <= 116; thus, f = 2*x + y <= 116.
                // y >= 5 ; or -y <= -5
                v_t{ 2, 1, 0, 0 }, // obj func // NOLINTNEXTLINE
                v_t_2d{ { 2, 1, 1, 0 }, { 0, -1, 0, 1 } }, // NOLINTNEXTLINE
                v_t{ max_diff, -5 }, // right sides // NOLINTNEXTLINE
                expected_t{ max_diff, many_solutions }, v_t{ 111 / 2.0, 5, 0, 0 });
        }

        tests.emplace_back("3 edges min costa",
            // An undirected graph on 3 nodes x -> y -> z and x -> z,
            // 4 undirected (or 6 directed) edges with capacities of 53, 110 and 13 kops, 1 kop costs 3 pennies.
            // f = 3*z. max f(13) = 3*13 = 39
            // x - y <= 53
            // -x + y <= 53
            // y - z <= 110
            // -y + z <= 110
            // x - z <= 13
            // -x + z <= 13
            // x == 0
            // NOLINTNEXTLINE
            v_t{ 0, 0, 3, 0, 0, 0, 0, 0, 0 }, // obj func // NOLINTNEXTLINE
            v_t_2d{// NOLINTNEXTLINE
                { 1, -1, 0, 1, 0, 0, 0, 0, 0 }, { -1, 1, 0, 0, 1, 0, 0, 0, 0 },
                // NOLINTNEXTLINE
                { 0, 1, -1, 0, 0, 1, 0, 0, 0 }, { 0, -1, 1, 0, 0, 0, 1, 0, 0 },
                // NOLINTNEXTLINE
                { 1, 0, -1, 0, 0, 0, 0, 1, 0 }, { -1, 0, 1, 0, 0, 0, 0, 0, 1 },
                // NOLINTNEXTLINE
                { 1, 0, 0, 0, 0, 0, 0, 0, 0 } }, // x == 0; NOLINTNEXTLINE
            v_t{ 53, 53, 110, 110, 13, 13, 0 }, // right sides NOLINTNEXTLINE
            expected_t{ 39, one_solution }, v_t{ 0, 0, 13 });

        tests.emplace_back("2 edges min costa",
            // An undirected graph (tree) on 3 nodes x -> y -> z,
            // 2 undirected (or 4 directed) edges with capacities of 5 and 11 kops, 1 kop costs 3 pennies.
            // f = 3*z. max f(5+11) = 3*16 = 48
            // x - y <= 5
            // -x + y <= 5
            // y - z <= 11
            // -y + z <= 11
            // x == 0
            // NOLINTNEXTLINE
            v_t{ 0, 0, 3, 0, 0, 0, 0 }, // obj func // NOLINTNEXTLINE
            v_t_2d{ { 1, -1, 0, 1, 0, 0, 0 }, { -1, 1, 0, 0, 1, 0, 0 }, // NOLINTNEXTLINE
                { 0, 1, -1, 0, 0, 1, 0 }, { 0, -1, 1, 0, 0, 0, 1 }, // NOLINTNEXTLINE
                { 1, 0, 0, 0, 0, 0, 0 } }, // x == 0; NOLINTNEXTLINE
            v_t{ 5, 5, 11, 11, 0 }, // right sides NOLINTNEXTLINE
            expected_t{ 48, one_solution }, v_t{ 0, 5, 16 });

        tests.emplace_back("1 edge min costa",
            // An undirected graph (tree) on 2 nodes x -> y,
            // 1 edge with capacity of 5 kops, 1 kop costs 3 pennies.
            // f = 3*y. max f(5) = 3*5 = 15
            // x - y <= 5 ; or y >= -5 ; it can be skipped as y is non-negative.
            // -x + y <= 5 ; or y <= 5 + x <= 5, because x == 0.
            // x == 0
            // NOLINTNEXTLINE
            v_t{ 0, 3, 0, 0 }, // obj func // NOLINTNEXTLINE
            v_t_2d{ { 1, -1, 1, 0 }, { -1, 1, 0, 1 }, { 1, 0, 0, 0 } }, // NOLINTNEXTLINE
            v_t{ 5, 5, 0 }, // right sides // NOLINTNEXTLINE
            expected_t{ 15, one_solution }, v_t{ 0, 5, 10, 0 });

        tests.emplace_back("1 edge min cost prepare",
            // An undirected graph (tree) on 2 nodes,
            // 1 edge with capacity of 5 kops, 1 kop costs 3 pennies.
            // f = 3*y. max f(6) = 3*6 = 18
            // x - y <= 5 ; or x - 5 <= y ; or y >= -4; it can be removed as y >= 0.
            // -x + y <= 5 ; or y <= 5 + x; x must be maximum possible.
            // x <= 1
            // NOLINTNEXTLINE
            v_t{ 0, 3, 0, 0, 0 }, // obj func // NOLINTNEXTLINE
            v_t_2d{ { 1, -1, 1, 0, 0 }, { -1, 1, 0, 1, 0 }, { 1, 0, 0, 0, 1 } }, // NOLINTNEXTLINE
            v_t{ 5, 5, 1 }, // right sides // NOLINTNEXTLINE
            expected_t{ 18, one_solution }, v_t{ 1, 6, 10, 0, 0 });

        tests.emplace_back("in between +-5",
            // f = 3*x. max f(5) = 3*5 = 15
            // x <= 5
            // x >= -5 ; or -x <= 5
            // NOLINTNEXTLINE
            v_t{ 3, 0, 0 }, // obj func // NOLINTNEXTLINE
            v_t_2d{ { 1, 1, 0 }, { -1, 0, 1 } }, // NOLINTNEXTLINE
            v_t{ 5, 5 }, // right sides // NOLINTNEXTLINE
            expected_t{ 15, one_solution }, v_t{ 5, 0, 10 });

        tests.emplace_back("in between +-5 but (less equal 2)",
            // f = 3*x. max f(2) = 3*2 = 6
            // x <= 5
            // x <= 2
            // x >= -5 ; or -x <= 5
            // NOLINTNEXTLINE
            v_t{ 3, 0, 0, 0 }, // obj func // NOLINTNEXTLINE
            v_t_2d{ { 1, 1, 0, 0 }, { 1, 0, 1, 0 }, { -1, 0, 0, 1 } }, // NOLINTNEXTLINE
            v_t{ 5, 2, 5 }, // right sides // NOLINTNEXTLINE
            expected_t{ 6, one_solution }, v_t{ 2, 3, 0, 7 });

        tests.emplace_back("test1",
            // f = 2*x + 3*y. max f(2, 4) = 4 + 12 = 16
            // x <= 5
            // y <= 4
            // x + y <= 6
            // NOLINTNEXTLINE
            v_t{ 2, 3, 0, 0, 0 }, // obj func // NOLINTNEXTLINE
            v_t_2d{ { 1, 0, 1, 0, 0 }, { 0, 1, 0, 1, 0 }, { 1, 1, 0, 0, 1 } }, // NOLINTNEXTLINE
            v_t{ 5, 4, 6 }, // right sides // NOLINTNEXTLINE
            expected_t{ 16, one_solution }, v_t{ 2, 4 });

        tests.emplace_back("test2",
            // f = 2*x + 3*y. max f(5, 4) = 10 + 12 = 22
            // x <= 5
            // y <= 4
            // x + y <= 100
            // NOLINTNEXTLINE
            v_t{ 2, 3, 0, 0, 0 }, // obj func // NOLINTNEXTLINE
            v_t_2d{ { 1, 0, 1, 0, 0 }, { 0, 1, 0, 1, 0 }, { 1, 1, 0, 0, 1 } }, // NOLINTNEXTLINE
            v_t{ 5, 4, 100 }, // right sides // NOLINTNEXTLINE
            expected_t{ 22, one_solution }, v_t{ 5, 4 });

        tests.emplace_back("test3",
            // f = 3*x + 2*y + 0*z + 0*w. max f(3, 0, 0, 0) = 9
            // x <= 5
            // y <= 4
            // x + y <= 3
            // 6*x + 8*y <= 67890
            // NOLINTNEXTLINE
            v_t{ 3, 2, 0, 0, 0, 0, 0, 0 }, // obj func // NOLINTNEXTLINE
            v_t_2d{ { 1, 0, 0, 0, 1, 0, 0, 0 }, { 0, 1, 0, 0, 0, 1, 0, 0 }, // NOLINTNEXTLINE
                { 1, 1, 0, 0, 0, 0, 1, 0 }, { 6, 8, 0, 0, 0, 0, 0, 1 } }, // NOLINTNEXTLINE
            v_t{ 5, 4, 3, 67890 }, // right sides // NOLINTNEXTLINE
            expected_t{ 9, one_solution }, v_t{ 3, 0, 0, 0 });

        tests.emplace_back("cut corner",
            // y
            // ^
            // |
            // 3*m
            // 2***
            // 1****
            // 0****
            // 00123 ->x
            //
            // f = 19*x + 31*y. max f(1, 3) = 19 + 93 = 112
            // x <= 3
            // y <= 3
            // x + y <= 4
            // NOLINTNEXTLINE
            v_t{ 19, 31, 0, 0, 0 }, // obj func // NOLINTNEXTLINE
            v_t_2d{ { 1, 0, 1, 0, 0 }, { 0, 1, 0, 1, 0 }, { 1, 1, 0, 0, 1 } }, // NOLINTNEXTLINE
            v_t{ 3, 3, 4 }, // right sides // NOLINTNEXTLINE
            expected_t{ 112, one_solution }, v_t{ 1, 3 });

        tests.emplace_back("cut corner symmetry", // NOLINTNEXTLINE
            v_t{ 31, 19, 0, 0, 0 }, // obj func // NOLINTNEXTLINE
            v_t_2d{ { 1, 0, 1, 0, 0 }, { 0, 1, 0, 1, 0 }, { 1, 1, 0, 0, 1 } }, // NOLINTNEXTLINE
            v_t{ 3, 3, 4 }, // right sides // NOLINTNEXTLINE
            expected_t{ 112, one_solution }, v_t{ 3, 1 });

        tests.emplace_back("semi-rhombus",
            // 3*m
            // 2***
            //              ; a point (2.5, 1.5) is valid; max(x) = 2.5.
            // 1***     ; But x cannot be 3, as y must be both <= 1 and >= 2, impossible.
            // 0**
            // 0012 ->x
            //
            // f = 19*x + 31*y. max f(1, 3) = 19 + 93 = 112
            // x <= 3
            // y <= 3
            // x + y <= 4
            // x - y <= 1
            // NOLINTNEXTLINE
            v_t{ 19, 31, 0, 0, 0, 0 }, // obj func // NOLINTNEXTLINE
            v_t_2d{ { 1, 0, 1, 0, 0, 0 }, { 0, 1, 0, 1, 0, 0 }, // NOLINTNEXTLINE
                { 1, 1, 0, 0, 1, 0 }, { 1, -1, 0, 0, 0, 1 } }, // NOLINTNEXTLINE
            v_t{ 3, 3, 4, 1 }, // right sides// NOLINTNEXTLINE
            expected_t{ 112, one_solution }, v_t{ 1, 3 });

        tests.emplace_back("semi-rhombus again",
            // 3**
            // 2***
            //              ; a point (2.5, 1.5) is valid; max(x) = 2.5.
            // 1***
            // 0**
            // 0012 ->x
            //
            // f = 31*x + 19*y. max f(2.5, 1.5) = 31*2.5 + 19*1.5 = 77.5 + 28.5 = 106
            // x <= 3
            // y <= 3
            // x + y <= 4
            // x - y <= 1
            // NOLINTNEXTLINE
            v_t{ 31, 19, 0, 0, 0, 0 }, // obj func // NOLINTNEXTLINE
            v_t_2d{ { 1, 0, 1, 0, 0, 0 }, { 0, 1, 0, 1, 0, 0 }, // NOLINTNEXTLINE
                { 1, 1, 0, 0, 1, 0 }, { 1, -1, 0, 0, 0, 1 } }, // NOLINTNEXTLINE
            v_t{ 3, 3, 4, 1 }, // right sides// NOLINTNEXTLINE
            expected_t{ 106, one_solution }, v_t{ 2.5, 1.5 });

        tests.emplace_back("trivial equal; no slack variable",
            // f = 2*x. max f(3) = 6
            // 7*x == 21 ; or x == 3
            // NOLINTNEXTLINE
            v_t{ 2 }, // obj func // NOLINTNEXTLINE
            v_t_2d{ { 7 } }, // NOLINTNEXTLINE
            v_t{ 21 }, // right sides // NOLINTNEXTLINE
            expected_t{ 6, one_solution }, v_t{ 3 });

        tests.emplace_back("trivial less equal; 1 slack variable",
            // f = 2*x. max f(3) = 6
            // 7*x <= 21
            // NOLINTNEXTLINE
            v_t{ 2, 0 }, // obj func // NOLINTNEXTLINE
            v_t_2d{ { 7, 1 } }, // NOLINTNEXTLINE
            v_t{ 21 }, // right sides // NOLINTNEXTLINE
            expected_t{ 6, one_solution }, v_t{ 3, 0 });

        tests.emplace_back("min of all",
            // f = 2*x. max f(3) = 6
            // 2*x <= 8
            // 5*x <= 26
            // 7*x <= 21
            // NOLINTNEXTLINE
            v_t{ 2, 0, 0, 0 }, // obj func // NOLINTNEXTLINE
            v_t_2d{ { 2, 1, 0, 0 }, { 5, 0, 1, 0 }, { 7, 0, 0, 1 } }, // NOLINTNEXTLINE
            v_t{ 8, 26, 21 }, // right sides // NOLINTNEXTLINE
            expected_t{ 6, one_solution }, v_t{ 3 });

        add_unbounded_cases(tests);
        add_infeasible_cases(tests);
    }

    constexpr void run_test_case(const test_case &test)
    {
        auto objs = test.objective_func_coefficients();
        auto matr = test.coef_matrix();
        auto rights = test.right_sides();
        v_t optimal_variables(test.optimal_variables().size());

        const auto actual =
            Standard::Algorithms::Geometry::simplex_method<floating_t>(objs, matr, rights, &optimal_variables);

        {
            const auto &expected = test.expected();

            ::Standard::Algorithms::ert::are_equal(expected.second, actual.second, "expected simplex result");

            ::Standard::Algorithms::ert::are_equal_with_epsilon(expected.first, actual.first, "expected value");
        }

        ::Standard::Algorithms::ert::are_equal_with_epsilon(
            test.optimal_variables(), optimal_variables, "Optimal variables");
    }
} // namespace

void Standard::Algorithms::Geometry::Tests::simplex_method_tests()
{
    Standard::Algorithms::Tests::test_utilities<test_case>::run_tests(run_test_case, generate_test_cases);
}
