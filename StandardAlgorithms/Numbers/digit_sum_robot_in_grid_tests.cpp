#include"digit_sum_robot_in_grid_tests.h"
#include"../Utilities/test_utilities.h"
#include"digit_sum_robot_in_grid.h"

namespace
{
    using int_t = std::int64_t;
    using dim_t = std::pair<int_t, int_t>;

    struct test_case final : Standard::Algorithms::Tests::base_test_case
    {
        constexpr test_case(
            std::string &&name, int_t digit_sum_limit, const dim_t &rows_columns, int_t expected) noexcept
            : base_test_case(std::move(name))
            , Rows(rows_columns.first)
            , Columns(rows_columns.second)
            , Digit_sum_limit(digit_sum_limit)
            , Expected(expected)
        {
        }

        [[nodiscard]] constexpr auto rows() const noexcept -> int_t
        {
            return Rows;
        }

        [[nodiscard]] constexpr auto columns() const noexcept -> int_t
        {
            return Columns;
        }

        [[nodiscard]] constexpr auto digit_sum_limit() const noexcept -> int_t
        {
            return Digit_sum_limit;
        }

        [[nodiscard]] constexpr auto expected() const noexcept -> int_t
        {
            return Expected;
        }

        constexpr void validate() const override
        {
            base_test_case::validate();

            Standard::Algorithms::require_positive(Rows, "Rows");
            Standard::Algorithms::require_positive(Columns, "Columns");
            Standard::Algorithms::require_positive(Digit_sum_limit, "Digit sum limit");
            Standard::Algorithms::require_positive(Expected, "Expected");
        }

        void print(std::ostream &str) const override
        {
            str << ", Rows " << Rows << ", Columns " << Columns << ", Digit sum limit " << Digit_sum_limit
                << ", Expected " << Expected;
        }

private:
        int_t Rows{};
        int_t Columns{};
        int_t Digit_sum_limit{};
        int_t Expected{};
    };

    constexpr void generate_test_cases(std::vector<test_case> &test_cases)
    {
        constexpr int_t side = 10;

        constexpr auto max_digit_sum_limit =
            2 * Standard::Algorithms::Numbers::digit_sum_robot_in_grid<int_t>::max_digit_sum(side);

        {
            constexpr int_t expect = 18;

            static_assert(max_digit_sum_limit == expect);
        }

        test_cases.emplace_back("All", max_digit_sum_limit, std::make_pair(side, side), side * side);

        test_cases.emplace_back("All with extra", max_digit_sum_limit * side, std::make_pair(side, side), side * side);

        for (auto digit_sum_limit = max_digit_sum_limit - 1; 0 < digit_sum_limit; --digit_sum_limit)
        {
            const auto excess = max_digit_sum_limit - digit_sum_limit;

            const auto expected = excess <= side ? side * side - Standard::Algorithms::Numbers::consecutive_sum(excess)
                                                 : Standard::Algorithms::Numbers::consecutive_sum(digit_sum_limit + 1);

            test_cases.emplace_back("Digit sum limit " + std::to_string(digit_sum_limit), digit_sum_limit,
                std::make_pair(side, side), expected);
        }

        if constexpr (!::Standard::Algorithms::is_debug)
        {
            constexpr auto digit_sum_limit = 16;
            constexpr auto rows = 54;
            constexpr auto columns = 29;
            constexpr auto expected = 1344;

            test_cases.emplace_back("middle 1", digit_sum_limit, std::make_pair(rows, columns), expected);

            constexpr auto expected_2 = 1251;

            test_cases.emplace_back("middle 2", digit_sum_limit - 1, std::make_pair(rows, columns), expected_2);
        }
    }

    constexpr void digit_count_test()
    {
        const std::vector<dim_t> tests{// NOLINTNEXTLINE
            { 1, 1 }, { 2, 1 }, { 9, 1 }, { 10, 2 }, { 11, 2 }, { 19, 2 },
            // NOLINTNEXTLINE
            { 20, 2 }, { 21, 2 }, { 98, 2 }, { 99, 2 }, { 100, 3 },
            // NOLINTNEXTLINE
            { 101, 3 }, { 1'000'000 - 1, 6 }, { 1'000'000, 7 }
        };

        for (const auto &test : tests)
        {
            const auto actual =
                Standard::Algorithms::Numbers::digit_sum_robot_in_grid<int_t>::decimal_digits_count(test.first);

            const auto name = "decimal_digits_count " + std::to_string(test.first);

            Standard::Algorithms::require_positive(actual, name);

            ::Standard::Algorithms::ert::are_equal(test.second, actual, name);
        }
    }

    constexpr void digit_sum_test()
    {
        const std::vector<dim_t> tests{ { 1, 1 }, { 2, 2 }, { 9, 9 }, { 10, 1 }, { 11, 2 }, { 19, 10 },
            // NOLINTNEXTLINE
            { 20, 2 }, { 21, 3 }, { 98, 17 }, { 99, 18 }, { 100, 1 }, { 101, 2 },
            // NOLINTNEXTLINE
            { 999'999, 9 * 6 }, { 1'000'000, 1 }, { 1'000'025, 8 } };

        for (const auto &test : tests)
        {
            const auto actual = Standard::Algorithms::Numbers::single_number_digit_sum_slow(test.first);

            ::Standard::Algorithms::ert::are_equal(
                test.second, actual, "single_number_digit_sum_slow " + std::to_string(test.first));
        }
    }

    constexpr void max_digit_sum_test()
    {
        std::vector<dim_t> tests;

        constexpr int_t val1 = 1;
        constexpr int_t val9 = 9;
        constexpr int_t val10 = 10;
        constexpr int_t val17 = 17;
        constexpr int_t val18 = 18;
        constexpr int_t val19 = 19;
        constexpr int_t val29 = 29;
        constexpr int_t val89 = 89;
        constexpr int_t val99 = 99;
        constexpr int_t val199 = 199;
        constexpr int_t val299 = 299;

        for (auto index = val1; index < val10; ++index)
        {
            tests.emplace_back(index, index);
        }

        for (auto index = val10; index < val19; ++index)
        {
            tests.emplace_back(index, val9);
        }

        for (auto index = val19; index < val29; ++index)
        {
            tests.emplace_back(index, val10);
        }

        for (int_t index = val29, max_value{}; index < val89; ++index)
        {
            const auto temp = Standard::Algorithms::Numbers::single_number_digit_sum_slow(index);

            max_value = std::max(max_value, temp);

            tests.emplace_back(index, max_value);
        }

        for (auto index = val89; index < val99; ++index)
        {
            tests.emplace_back(index, val17);
        }

        for (auto index = val99; index < val199; ++index)
        {
            tests.emplace_back(index, val18);
        }

        for (auto index = val199; index < val299; ++index)
        {
            tests.emplace_back(index, val19);
        }

        for (const auto &test : tests)
        {
            const auto &input_data = test.first;

            const auto name = "max_digit_sum " + std::to_string(input_data);

            const auto actual = Standard::Algorithms::Numbers::digit_sum_robot_in_grid<int_t>::max_digit_sum(input_data);

            ::Standard::Algorithms::ert::are_equal(test.second, actual, name);
        }
    }

    constexpr void run_test_case(const test_case &test)
    {
        {
            const auto actual = Standard::Algorithms::Numbers::digit_sum_robot_in_grid<int_t>::slow(
                test.digit_sum_limit(), std::make_pair(test.rows(), test.columns()));

            ::Standard::Algorithms::ert::are_equal(test.expected(), actual, "Slow");
        }
        {// todo(p3):  finish fast test.
          //            const auto actual =
          //            Standard::Algorithms::Numbers::digit_sum_robot_in_grid<int_t>::Fast(test.digitSumLimit(),
          //                std::make_pair(test.rows(), test.columns()));
          //
          //            ::Standard::Algorithms::ert::are_equal(test.expected(), actual, "Fast");
        }
    }

    constexpr void main_test()
    {
        Standard::Algorithms::Tests::test_utilities<test_case>::run_tests(run_test_case, generate_test_cases);
    }
} // namespace

void Standard::Algorithms::Numbers::Tests::digit_sum_robot_in_grid_tests()
{
    digit_count_test();
    digit_sum_test();
    max_digit_sum_test();
    main_test();
}
