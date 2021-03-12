#pragma once
// "simplex_method.h"
#include"../Utilities/require_utilities.h"
#include<cmath>
#include<iomanip>
#include<iostream>
#include<vector>

namespace Standard::Algorithms::Geometry
{
    enum class [[nodiscard]] simplex_result : std::uint8_t
    {
        infeasible,
        one_solution,
        many_solutions,
        unbounded,
    };

    inline auto operator<< (std::ostream &str, const simplex_result &res) -> std::ostream &
    {
        switch (res)
        {
            using enum simplex_result;

        case infeasible:
            str << "Infeasible";
            break;
        case one_solution:
            str << "One solution";
            break;
        case many_solutions:
            str << "Many solutions";
            break;
        case unbounded:
            str << "Unbounded";
            break;
        default:
            if constexpr (::Standard::Algorithms::is_debug)
            {
                auto str2 = ::Standard::Algorithms::Utilities::w_stream();
                str2 << "Unknown value of simplex_result " << std::to_underlying(res);

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
} // namespace Standard::Algorithms::Geometry

namespace Standard::Algorithms::Geometry::Inner
{
    constexpr auto shall_print_lin_prog = false;

    template<std::floating_point floating_t>
    struct simplex_method_context final
    {
        constexpr simplex_method_context(std::vector<floating_t> &objective_func_coefficients,
            std::vector<std::vector<floating_t>> &coef_matrix, std::vector<floating_t> &right_sides)
            : Rows(require_positive(coef_matrix.size(), "rows"))
            , Columns(require_positive(coef_matrix.at(0).size(), "columns"))
            , Objective_func_coefficients(objective_func_coefficients)
            , Coef_matrix(coef_matrix)
            , Right_sides(right_sides)
        {
            require_equal(Rows, "Right sides size", Right_sides.size());

            require_equal(Columns, "Objective function coefficients size", Objective_func_coefficients.size());

            for (std::size_t row{}; const auto &coefs : Coef_matrix)
            {
                require_equal(Columns, "columns vs coefficient matrix[" + std::to_string(row) + "] size", coefs.size());
                ++row;
            }
        }

        void print(auto &str) const
        {
            ::Standard::Algorithms::print("Objective_func_coefficients", Objective_func_coefficients, str);
            ::Standard::Algorithms::print("Coef_matrix", Coef_matrix, str);
            ::Standard::Algorithms::print("Right_sides", Right_sides, str);
            str << '\n';

            std::cout << str.str();
        }

        [[nodiscard]] constexpr auto solve_lp() -> std::pair<floating_t, simplex_result>
        {
            if constexpr (shall_print_lin_prog)
            {
                const auto rep = "\n\n\n   Start computing Simplex on " + std::to_string(Rows) + " by " +
                    std::to_string(Columns) + " coefficients matrix..\n";

                std::cout << rep;
            }

            constexpr floating_t zero{};

            assert(!Right_sides.empty());

            if (const auto mini = std::min_element(Right_sides.cbegin(), Right_sides.cend()); *mini < zero)
            {// Zero point might be infeasible e.g. x >= 1.
                const auto orig_objs = Objective_func_coefficients;

                {
                    const auto pivot_row = static_cast<std::size_t>(mini - Right_sides.cbegin());
                    assert(pivot_row < Right_sides.size());

                    prepare_aux(pivot_row);
                }
                {
                    constexpr auto is_aux = true;
                    solve_tail(is_aux);
                }

                const auto [aux_val, isbasic_row] = optimal_is_basic_row(Columns - 1ZU);

                if (aux_val != zero) // todo(p3): epsilon?
                {
                    if constexpr (shall_print_lin_prog)
                    {
                        auto str = ::Standard::Algorithms::Utilities::w_stream();
                        str << "\nInfeasible solution, temp maximum " << Maximum << ".\n";

                        this->print(str);
                    }

                    return { zero, simplex_result::infeasible };
                }

                enure_nonbasic(isbasic_row.first, isbasic_row.second);
                run_aux(orig_objs);
            }

            solve_tail();
            return { Maximum, Sim_res };
        }

        constexpr void check_optimal_variables_size(const std::size_t size) const
        {
            const auto *const name = "Optimal variables size";
            require_positive(size, name);
            require_less_equal(size, Columns, name);
        }

        constexpr void optimal_variables(std::vector<floating_t> &vars) const noexcept
        {
            assert(!vars.empty() && vars.size() <= Columns);

            for (std::size_t col{}; col < vars.size(); ++col)
            {
                auto &opti = vars[col];
                opti = optimal_is_basic_row(col).first;
            }
        }

private:
        [[nodiscard]] constexpr auto optimal_is_basic_row(const std::size_t column) const noexcept
            -> std::pair<floating_t, std::pair<bool, std::size_t>>
        {// todo(p3): use an array to store which variables are slack/basic not to make an accidental mistake.
            assert(column < Columns);

            constexpr floating_t zero{};
            constexpr floating_t one{ 1 };
            static_assert(zero < one);

            auto row1 = Rows;
            std::size_t zero_count{};

            for (std::size_t row{}; row < Rows; ++row)
            {
                const auto &val = Coef_matrix[row][column];

                if (val == zero) // todo(p3): epsilon?
                {
                    ++zero_count;
                }
                else if (val == one)
                {
                    row1 = row;
                }
            }

            const auto is_basic = zero_count == Rows - 1ZU && row1 < Rows;
            auto var = is_basic ? Right_sides[row1] : zero;
            return std::make_pair(var, std::make_pair(is_basic, row1));
        }

        constexpr void prepare_aux(const std::size_t pivot_row)
        {
            assert(pivot_row < Rows);

            constexpr floating_t zero{};
            constexpr floating_t one{ 1 };
            constexpr floating_t minus_one = -one;
            static_assert(minus_one < zero && minus_one + one == zero);

            assert(0U < Columns && Columns == Objective_func_coefficients.size());

            if constexpr (shall_print_lin_prog)
            {
                auto str = ::Standard::Algorithms::Utilities::w_stream();
                str << " Prepare aux at pivot row " << pivot_row << '\n';

                this->print(str);
            }

            for (std::size_t row{}; row < Rows; ++row)
            {
                Coef_matrix[row].push_back(minus_one);
            }

            ++Columns;
            Objective_func_coefficients.assign(Columns, zero);
            Objective_func_coefficients.back() = minus_one;

            if constexpr (shall_print_lin_prog)
            {
                auto str = ::Standard::Algorithms::Utilities::w_stream();
                str << "\n  Before infeasibility pivot.\n";

                this->print(str);
            }

            const auto pivot_column = Columns - 1ZU;
            assert(0U < pivot_column);

            update_max_and_objs(pivot_column, pivot_row);
            update_coefs(pivot_column, pivot_row);

            if constexpr (shall_print_lin_prog)
            {
                auto str = ::Standard::Algorithms::Utilities::w_stream();
                str << "\n  Afer infeasibility test pivot at column and row " << pivot_column << ", " << pivot_row
                    << ". Zero point might be infeasible, temp maximum " << Maximum << ".\n";

                this->print(str);
            }
        }

        constexpr void enure_nonbasic(const bool is_basic, const std::size_t pivot_row)
        {
            if (!is_basic)
            {
                if constexpr (shall_print_lin_prog)
                {
                    std::cout << "\nAux is not basic.\n";
                }

                return;
            }

            if constexpr (shall_print_lin_prog)
            {
                const auto rep = "\nAux is basic at row " + std::to_string(pivot_row) + ".\n";

                std::cout << rep;
            }

            assert(1ZU < Columns && pivot_row < Rows);

            const auto pivot_column = Columns - 1ZU;
            update_max_and_objs(pivot_column, pivot_row);
            update_coefs(pivot_column, pivot_row);
        }

        constexpr void run_aux(const std::vector<floating_t> &orig_objs)
        {
            for (std::size_t row{}; row < Rows; ++row)
            {
                Coef_matrix[row].pop_back();
            }

            constexpr floating_t zero{};

            --Columns;
            Maximum = zero;
            Objective_func_coefficients.assign(Columns, zero);

            for (std::size_t col{}; col < Columns; ++col)
            {
                const auto &orig_obj = orig_objs[col];
                if (zero == orig_obj) // todo(p3): epsilon?
                {
                    continue;
                }

                const auto [is_base, row] = optimal_is_basic_row(col).second;
                if (!is_base)
                {
                    auto &future_obj = Objective_func_coefficients[col];
                    future_obj += orig_obj;
                    continue;
                }

                assert(row < Rows);

                const auto &coefs = Coef_matrix.at(row);

                const auto &temp_coef = coefs.at(col);
                if (zero == temp_coef) [[unlikely]]
                {
                    auto str = ::Standard::Algorithms::Utilities::w_stream();
                    str << "Error: cannot divide by the temporary coefficient [" << row << ", " << col
                        << "] which is zero.";

                    throw std::runtime_error(str.str());
                }

                const auto mul_coef = static_cast<floating_t>(orig_obj / temp_coef);
                {
                    const auto &coef = Right_sides.at(row);
                    const auto prod = static_cast<floating_t>(mul_coef * coef);
                    Maximum += prod;
                }

                for (std::size_t col_2{}; col_2 < Columns; ++col_2)
                {
                    if (col == col_2)
                    {
                        continue;
                    }

                    const auto &coef = coefs.at(col_2);
                    const auto prod = static_cast<floating_t>(mul_coef * coef);
                    auto &future_obj = Objective_func_coefficients[col_2];
                    future_obj -= prod;
                }
            }
        }

        constexpr void solve_tail(const bool is_aux = false)
        {
            constexpr std::size_t max_steps = 1U << 10U;
            static_assert(0U < max_steps);

            for (std::size_t step{};;)
            {
                if (!compute_more(step))
                {
                    return;
                }

                if (!(++step < max_steps)) [[unlikely]]
                {
                    auto err = "Too many LP steps " + std::to_string(max_steps) + ", is auxilary " +
                        std::to_string(is_aux ? 1 : 0) + ", error.";

                    throw std::runtime_error(err);
                }
            }
        }

        [[nodiscard]] constexpr auto compute_more(const std::size_t step) -> bool
        {
            if constexpr (shall_print_lin_prog)
            {
                auto str = ::Standard::Algorithms::Utilities::w_stream();
                str << "   Start step " << step << '\n';

                this->print(str);
            }

            // Heuristic is to find the highest positive coefficient in order
            // to achieve the greatest function increment.

            // todo(p4): it can be calculated initially, and directly in update_max_and_objs; however it is not the
            // hottest place.
            const auto pivot_column = static_cast<std::size_t>(
                std::max_element(Objective_func_coefficients.cbegin(), Objective_func_coefficients.cend()) -
                Objective_func_coefficients.cbegin());

            constexpr floating_t zero{};

            {
                const auto &val = Objective_func_coefficients.at(pivot_column);
                if (!(zero < val)) // todo(p3): epsilon?
                {
                    if constexpr (shall_print_lin_prog)
                    {
                        auto str = ::Standard::Algorithms::Utilities::w_stream();
                        str << " LP success, maximum " << Maximum << ".\n\n";

                        std::cout << str.str();
                    }

                    Sim_res = simplex_result::one_solution;
                    return false;
                }
            }

            const auto row_unbounded = find_pivot_row(pivot_column);
            if (row_unbounded.second)
            {
                Sim_res = simplex_result::unbounded;

                if constexpr (shall_print_lin_prog)
                {
                    auto str = ::Standard::Algorithms::Utilities::w_stream();
                    str << "  LP unbounded at pivot column " << pivot_column << ", max " << Maximum << "\n\n";

                    std::cout << str.str();
                }

                Maximum = {};
                return false;
            }

            const auto &pivot_row = row_unbounded.first;
            update_max_and_objs(pivot_column, pivot_row);
            update_coefs(pivot_column, pivot_row);

            if constexpr (shall_print_lin_prog)
            {
                auto str = ::Standard::Algorithms::Utilities::w_stream();
                str << "\n  LP found pivot column and row: " << pivot_column << ", " << pivot_row
                    << ", temporal maximum " << Maximum << ".\n";

                std::cout << str.str();
            }

            return true;
        }

        [[nodiscard]] constexpr auto find_pivot_row(const std::size_t pivot_column) const noexcept
            -> std::pair<std::size_t, bool>
        {
            assert(pivot_column < Columns && 0U < Rows);

            constexpr floating_t zero{};

            auto best_row = Rows;
            auto maximum_incr = zero;
            auto zer_max_coef = zero;

            for (std::size_t row{}; row < Rows; ++row)
            {
                const auto &right = Right_sides[row];
                if (right < zero)
                {// todo(p2): To increase the Maximum, must both the right and the coef be positive?
                    continue;
                }

                const auto &coef = Coef_matrix[row][pivot_column];
                if (zero == right)
                {// Between (a*x <= b) and (c*x == 0), the last is stronger.
                    if (!(zer_max_coef < coef))
                    {
                        continue;
                    }

                    best_row = row;
                    zer_max_coef = coef;
                    continue;
                }

                if (zero < zer_max_coef)
                {
                    continue;
                }

                // The maximum of (coef / right) will guarantee the strictest bound.
                // Given x <= 5, 2*x <= 7, it follows that x <= 7/2.
                // Using the max instead of the min leads to the simpler code.
                const auto increment = static_cast<floating_t>(coef / right);
                if (maximum_incr < increment)
                {
                    best_row = row;
                    maximum_incr = increment;
                }
            }

            const auto row_unbounded = Rows == best_row;
            return { best_row, row_unbounded };
        }

        constexpr void update_max_and_objs(const std::size_t pivot_column, const std::size_t pivot_row) noexcept
        {
            constexpr floating_t zero{};

            assert(pivot_row < Rows && pivot_column < Columns);

            const auto pivot_coef = Coef_matrix[pivot_row][pivot_column];
            assert(zero != pivot_coef);

            const auto obj = Objective_func_coefficients[pivot_column];
            assert(zero != obj);

            {
                auto &rhs = Right_sides[pivot_row];
                rhs /= pivot_coef;

                const auto incr = static_cast<floating_t>(obj * rhs);
                const auto new_max = static_cast<floating_t>(Maximum + incr);

                Maximum = new_max;
            }

            const auto obj_2 = obj / pivot_coef;

            for (std::size_t col{}; col < Columns; ++col)
            {
                auto &mat = Coef_matrix[pivot_row][col];
                {
                    const auto prod = static_cast<floating_t>(mat * obj_2);
                    auto &ofc = Objective_func_coefficients[col];
                    ofc -= prod;
                }
                mat /= pivot_coef;
            }

            {// Avoid tiny remnants.
                auto &ofc = Objective_func_coefficients[pivot_column];
                ofc = zero;
            }

            [[maybe_unused]] constexpr floating_t one{ 1 };

            assert(Coef_matrix[pivot_row][pivot_column] == one);
        }

        constexpr void update_coefs(const std::size_t pivot_column, const std::size_t pivot_row) noexcept
        {
            constexpr floating_t zero{};
            assert(pivot_row < Rows && pivot_column < Columns);

            const auto right = Right_sides[pivot_row];

            for (std::size_t row{}; row < Rows; ++row)
            {
                const auto old_coef = Coef_matrix[row][pivot_column];
                if (row == pivot_row || old_coef == zero) // todo(p3): epsilon?
                {
                    continue;
                }

                {
                    const auto prod = static_cast<floating_t>(old_coef * right);
                    auto &rig = Right_sides[row];
                    rig -= prod;
                }

                const auto &piv_coefs = Coef_matrix[pivot_row];
                auto &coefs = Coef_matrix[row];

                for (std::size_t col{}; col < Columns; ++col)
                {// It must be the hottest spot to optimize.
                    const auto prod = static_cast<floating_t>(old_coef * piv_coefs[col]);
                    auto &coe = coefs[col];
                    coe -= prod;
                }
            }
        }

        const std::size_t Rows;
        std::size_t Columns;

        std::vector<floating_t> &Objective_func_coefficients; // columns.
        std::vector<std::vector<floating_t>> &Coef_matrix; // Rows by Columns.
        std::vector<floating_t> &Right_sides; // Rows.

        floating_t Maximum{};
        simplex_result Sim_res{};
        // todo(p3): impl the interior point method.
    };
} // namespace Standard::Algorithms::Geometry::Inner

namespace Standard::Algorithms::Geometry
{
    // Kantorovich linear programming.
    // All optimal variables must be non-negative provided a solution exists.
    // The objective function is being maximized.
    // The slack (artificial, surplus) variables if any must be provided in the input.
    // E.g. given 1 variable x, f = max(x), and 1 constraint (x <= 7), the input must:
    // - include 1 extra slack variable y;
    // - objective_func_coefficients {1, 0}; // f = x + 0*y
    // - coef_matrix {{1, 1}}; // x + y == 7
    // - right_sides {7};
    // - optimals variables: 0) can be omitted; 1) have size 1; 2) or have size 2.
    // There is no sense to add a condition (x >=0) as it is assumed to hold for every variable.
    template<std::floating_point floating_t>
    [[nodiscard]] constexpr auto simplex_method(std::vector<floating_t> &objective_func_coefficients,
        std::vector<std::vector<floating_t>> &coef_matrix, std::vector<floating_t> &right_sides,
        // size must be in [1 .. rows].
        std::vector<floating_t> *const optimals = nullptr) -> std::pair<floating_t, simplex_result>
    {
        assert(&objective_func_coefficients != &right_sides);
        assert(&objective_func_coefficients != optimals);
        assert(&right_sides != optimals);

        Inner::simplex_method_context<floating_t> context(objective_func_coefficients, coef_matrix, right_sides);

        const auto has_opt_vars = nullptr != optimals;
        if (has_opt_vars)
        {
            context.check_optimal_variables_size(optimals->size());
        }

        auto result = context.solve_lp();

        if (has_opt_vars)
        {
            if (simplex_result::one_solution == result.second || simplex_result::many_solutions == result.second)
            {
                context.optimal_variables(*optimals);
            }
            else
            {
                optimals->assign(optimals->size(), floating_t{});
            }
        }

        return result;
    }
} // namespace Standard::Algorithms::Geometry
