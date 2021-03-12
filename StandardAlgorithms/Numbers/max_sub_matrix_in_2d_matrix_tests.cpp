#include"max_sub_matrix_in_2d_matrix_tests.h"
#include"../Geometry/point.h"
#include"../Geometry/rectangle2d.h"
#include"../Utilities/random.h"
#include"../Utilities/test_utilities.h"
#include"matrix_transpose.h"
#include"max_sub_matrix_in_2d_matrix.h"

namespace
{
    using distance_t = std::size_t;

    using point_t = Standard::Algorithms::Geometry::point2d<distance_t>;

    // The second point in a rectangle is exclusive.
    using rectangle_t = Standard::Algorithms::Geometry::rectangle2d<distance_t, distance_t, point_t>;

    using algorithm_t = Standard::Algorithms::Numbers::max_sub_matrix_in_2d_matrix<point_t, rectangle_t>;

    using area_function_t = rectangle_t (*)(const std::vector<std::vector<bool>> &data1);

    using int_t = std::int32_t;
    using long_int_t = std::int64_t;
    using pair_t = std::pair<std::size_t, std::size_t>;

    template<class size_t1>
    [[maybe_unused]] constexpr auto check_area_has_all_true_values(const std::pair<size_t1, size_t1> &low,
        const std::vector<std::vector<bool>> &data, const std::pair<size_t1, size_t1> &high_exclusive,
        const std::string &error_message, const bool throw_on_exception = true) -> bool
    {
        const auto size = data.size();
        Standard::Algorithms::require_positive(size, "data size");

        const auto row_size = data[0].size();
        Standard::Algorithms::require_positive(row_size, "row size");

        const auto &x1_inclusive = low.first;
        const auto &x2_exclusive = high_exclusive.first;
        Standard::Algorithms::require_greater(x2_exclusive, x1_inclusive, "x-coordinate");
        Standard::Algorithms::require_less_equal(x2_exclusive, size, "x-coordinate size");

        const auto &y1_inclusive = low.second;
        const auto &y2_exclusive = high_exclusive.second;
        Standard::Algorithms::require_greater(y2_exclusive, y1_inclusive, "y-coordinate");

        for (auto row = x1_inclusive; row < x2_exclusive; ++row)
        {
            const auto row_size1 = data[row].size();
            Standard::Algorithms::require_less_equal(y2_exclusive, row_size1, "rowSize");

            for (auto column = y1_inclusive; column < y2_exclusive; ++column)
            {
                if (data[row][column])
                {
                    continue;
                }

                if (!throw_on_exception)
                {
                    return false;
                }

                auto str = ::Standard::Algorithms::Utilities::w_stream();
                str << "There is false value in data[" << row << "][" << column << "].";

                if (!error_message.empty())
                {
                    str << " " << error_message;
                }

                Standard::Algorithms::throw_exception(str);
            }
        }

        return true;
    }

    constexpr void check_rectangle(
        const std::string &name, const rectangle_t &rectangle, const pair_t &rows_columns, const bool is_square = false)
    {
        assert(!name.empty());

        rectangle_t::validate(rectangle, name);

        {
            const auto &right_bottom = rectangle.right_bottom();
            const auto &[xxx, yyy] = right_bottom;

            const auto &[rows, columns] = rows_columns;

            Standard::Algorithms::require_less_equal(xxx, rows, name + " rows");

            Standard::Algorithms::require_less_equal(yyy, columns, name + " columns");
        }

        if (!is_square)
        {
            return;
        }

        const auto height = rectangle.height();
        const auto width = rectangle.width();

        ::Standard::Algorithms::ert::are_equal(height, width, name + " height and width");
    }

    struct boolean_test_case final : Standard::Algorithms::Tests::base_test_case
    {
        constexpr boolean_test_case(std::string &&name, std::vector<std::vector<bool>> &&data,
            // todo(p3): fix "2 similar adjacent parameters"
            // NOLINTNEXTLINE
            const rectangle_t &rectangle_ = {}, const rectangle_t &square_ = {}) noexcept
            : base_test_case(std::move(name))
            , Data(std::move(data))
            , Rectangle(rectangle_)
            , Square(square_)
        {
        }

        [[nodiscard]] constexpr auto data() const &noexcept -> const std::vector<std::vector<bool>> &
        {
            return Data;
        }

        [[nodiscard]] constexpr auto rectangle() const &noexcept -> const rectangle_t &
        {
            return Rectangle;
        }

        [[nodiscard]] constexpr auto square() const &noexcept -> const rectangle_t &
        {
            return Square;
        }

        constexpr void validate() const override
        {
            base_test_case::validate();

            const auto rows = Standard::Algorithms::require_positive(Data.size(), "Data size");

            const auto columns = Standard::Algorithms::require_positive(Data[0].size(), "Data[0] size");

            const pair_t rows_columns{ rows, columns };

            check_rectangle("rectangle", Rectangle, rows_columns);
            check_rectangle("square", Square, rows_columns, true);
        }

        void print(std::ostream &str) const override
        {
            ::Standard::Algorithms::print("Data", Data, str);
            str << " Expected rectangle " << Rectangle << ", Expected square " << Square;
        }

private:
        std::vector<std::vector<bool>> Data;
        rectangle_t Rectangle;
        rectangle_t Square;
    };

    struct test_case_sum final : Standard::Algorithms::Tests::base_test_case
    {
        constexpr test_case_sum(std::string &&name, std::vector<std::vector<int_t>> &&data, long_int_t sum = {},
            const rectangle_t &rectangle = {}) noexcept
            : base_test_case(std::move(name))
            , Data(std::move(data))
            , Sum(sum)
            , Rectangle(rectangle)
        {
        }

        [[nodiscard]] constexpr auto data() const &noexcept -> const std::vector<std::vector<int_t>> &
        {
            return Data;
        }

        [[nodiscard]] constexpr auto sum() const &noexcept -> const long_int_t &
        {
            return Sum;
        }

        [[nodiscard]] constexpr auto rectangle() const &noexcept -> const rectangle_t &
        {
            return Rectangle;
        }

        constexpr void validate() const override
        {
            base_test_case::validate();

            const auto rows = Standard::Algorithms::require_positive(Data.size(), "Data size");

            const auto columns = Standard::Algorithms::require_positive(Data[0].size(), "Data[0] size");

            const pair_t rows_columns{ rows, columns };

            check_rectangle("rectangle", Rectangle, rows_columns);
        }

        void print(std::ostream &str) const override
        {
            ::Standard::Algorithms::print("Data", Data, str);
            str << " Expected sum " << Sum << ", Expected rectangle " << Rectangle;
        }

private:
        std::vector<std::vector<int_t>> Data;
        long_int_t Sum;
        rectangle_t Rectangle;
    };

    constexpr void run_boolean_sub_test(const bool is_squared, const std::string &name_suffix,
        const boolean_test_case &test, area_function_t area_function)
    {
        Standard::Algorithms::require_positive(name_suffix.size(), "name suffix");

        Standard::Algorithms::throw_if_null("area function", area_function);

        const auto name = test.name() + " " + name_suffix;
        const auto &data = test.data();

        const auto actual = area_function(data);
        {
            const auto rows = Standard::Algorithms::require_positive(data.size(), name + " Data size");

            const auto columns = Standard::Algorithms::require_positive(data[0].size(), name + " Data[0] size");

            const pair_t rows_columns{ rows, columns };

            check_rectangle(name + " rectangle", actual, rows_columns);
        }

        const auto actual_area = actual.area();

        if (long_int_t{} < actual_area)
        {
            const auto &[left_top, right_bottom] = actual;

            const auto &[xx1, yy1] = left_top;
            const auto &[xx2, yy2] = right_bottom;

            check_area_has_all_true_values(std::make_pair(xx1, yy1), test.data(), std::make_pair(xx2, yy2), name);
        }

        const auto &expected_rectangle = is_squared ? test.square() : test.rectangle();

        if (const auto are_equal = expected_rectangle == actual; !are_equal)
        {
            // The area must be exactly the same,
            // but the coordinates might differ.
            const auto expected_area = expected_rectangle.area();

            ::Standard::Algorithms::ert::are_equal(expected_area, actual_area, name + " area");
        }

        if (!is_squared)
        {
            return;
        }

        ::Standard::Algorithms::ert::are_equal(test.square().width(), test.square().height(), name + " Expected sides");

        ::Standard::Algorithms::ert::are_equal(test.square().width(), test.square().width(), name + " width");

        ::Standard::Algorithms::ert::are_equal(test.square().height(), actual.height(), name + " height");
    }

    [[nodiscard]] constexpr auto transpose_rectangle(const rectangle_t &rectangle) -> rectangle_t
    {
        const auto &top = rectangle.left_top();
        const auto height = rectangle.height();
        const auto width = rectangle.width();

        rectangle_t result{ point_t{ top.y, top.x }, point_t{ top.y + width, top.x + height } };

        return result;
    }

    constexpr void add_transposed_test_cases(std::vector<boolean_test_case> &test_cases)
    {
        const auto size = Standard::Algorithms::require_positive(test_cases.size(), "test cases before transposing");

        test_cases.reserve(size << 1U);

        const std::string transposed = " transposed";

        for (std::size_t index{}; index < size; ++index)
        {
            const auto &old_test_case = test_cases[index];
            const auto &old_name = old_test_case.name();

            auto data = old_test_case.data();

            const auto height = data.size();
            ::Standard::Algorithms::ert::greater(height, 0U, old_name + " data size");

            const auto width = data.at(0).size();
            ::Standard::Algorithms::ert::greater(width, 0U, old_name + " data[0] size");

            // New one.
            auto name = old_name + transposed;

            Standard::Algorithms::Numbers::matrix_transpose(old_test_case.data(), data);

            auto rectangle = transpose_rectangle(old_test_case.rectangle());
            auto square = transpose_rectangle(old_test_case.square());

            test_cases.emplace_back(std::move(name), std::move(data), rectangle, square);
        }
    }

    void generate_test_cases(std::vector<boolean_test_case> &test_cases)
    {
        // Less typing.
        // NOLINTNEXTLINE
        constexpr auto f = false;

        // NOLINTNEXTLINE
        constexpr auto t = true;

        test_cases.emplace_back("Ladder 3", std::vector<std::vector<bool>>{ { f, f, t }, { f, t, t }, { t, t, t } },
            rectangle_t{ point_t{ 1, 1 }, point_t{ 3, 3 } }, rectangle_t{ point_t{ 1, 1 }, point_t{ 3, 3 } });

        test_cases.emplace_back("Ladder 4",
            std::vector<std::vector<bool>>{ { f, f, f, t }, { f, f, t, t }, { f, t, t, t }, { t, t, t, t } },
            rectangle_t{ point_t{ 2, 1 }, point_t{ 4, 4 } }, rectangle_t{ point_t{ 1, 2 }, point_t{ 3, 4 } });

        test_cases.emplace_back("Simple 1",
            std::vector<std::vector<bool>>{ { f, t, t, f, f, f, f }, { f, t, t, t, t, t, f } }, // NOLINTNEXTLINE
            rectangle_t{ point_t{ 1, 1 }, point_t{ 2, 6 } }, // NOLINTNEXTLINE
            rectangle_t{ point_t{ 0, 1 }, point_t{ 2, 3 } });

        test_cases.emplace_back("Simple 1 rotated",
            std::vector<std::vector<bool>>{
                { f, f }, { t, t }, { t, t }, { t, f }, { t, f }, { t, f }, { f, f } }, // NOLINTNEXTLINE
            rectangle_t{ point_t{ 1, 0 }, point_t{ 6, 1 } }, // NOLINTNEXTLINE
            rectangle_t{ point_t{ 1, 0 }, point_t{ 3, 2 } });

        test_cases.emplace_back("Cross",
            std::vector<std::vector<bool>>{
                { f, t, t, f, f, f, f }, { t, t, t, t, t, f, t }, { f, t, f, t, t, f, t } }, // NOLINTNEXTLINE
            rectangle_t{ point_t{ 1, 0 }, point_t{ 2, 5 } }, // NOLINTNEXTLINE
            rectangle_t{ point_t{ 0, 1 }, point_t{ 2, 3 } });

        test_cases.emplace_back("All 1s",
            std::vector<std::vector<bool>>{ { t, t, t, t, t }, { t, t, t, t, t }, { t, t, t, t, t } }, // NOLINTNEXTLINE
            rectangle_t{ point_t{ 0, 0 }, point_t{ 3, 5 } }, // NOLINTNEXTLINE
            rectangle_t{ point_t{ 0, 0 }, point_t{ 3, 3 } });

        test_cases.emplace_back("One 1",
            std::vector<std::vector<bool>>{ { f, f, f, f, f }, { f, f, f, t, f }, { f, f, f, f, f } },
            rectangle_t{ point_t{ 1, 3 }, point_t{ 2, 4 } }, rectangle_t{ point_t{ 1, 3 }, point_t{ 2, 4 } });

        test_cases.emplace_back("Two 1s",
            std::vector<std::vector<bool>>{ { f, f, f, f, f }, { f, f, t, t, f }, { f, f, f, f, f } },
            rectangle_t{ point_t{ 1, 2 }, point_t{ 2, 4 } }, rectangle_t{ point_t{ 1, 2 }, point_t{ 2, 3 } });

        test_cases.emplace_back("All 0s", std::vector<std::vector<bool>>{ { f, f, f, f, f }, { f, f, f, f, f } });

        test_cases.emplace_back("Simple 2",
            std::vector<std::vector<bool>>{ { f, f, t, f, f, t, f, t, f, f }, { f, f, f, f, f, f, f, f, f, f },
                { f, t, f, t, t, t, f, t, t, f }, { t, f, f, t, t, t, f, t, t, f }, { t, t, f, t, t, t, f, t, t, f },
                { t, t, f, t, t, t, f, t, t, f }, { f, f, f, f, f, f, f, f, f, f } }, // NOLINTNEXTLINE
            rectangle_t{ point_t{ 2, 3 }, point_t{ 6, 6 } }, // NOLINTNEXTLINE
            rectangle_t{ point_t{ 2, 3 }, point_t{ 5, 6 } });

        {
            constexpr auto min_value = ::Standard::Algorithms::is_debug ? 3U : 5U;
            constexpr auto max_value = ::Standard::Algorithms::is_debug ? 6U : 50U;

            Standard::Algorithms::Utilities::random_t<std::size_t> size_random(min_value, max_value);
            const auto random1 = size_random();
            const auto random2 = size_random();

            std::vector<pair_t> pairs{ pair_t(random1, random2) };

            if constexpr (!::Standard::Algorithms::is_debug)
            {
                pairs.insert(pairs.end(),
                    // NOLINTNEXTLINE
                    { pair_t(5, 3), pair_t(11, 18), pair_t(35, 46) });
            }

            for (std::size_t pair_index{}; pair_index < pairs.size(); ++pair_index)
            {
                const auto &pai = pairs[pair_index];
                const auto &[size, rowSize] = pai;

                std::vector<std::vector<bool>> data(size);

                for (std::size_t index{}; index < size; ++index)
                {
                    std::vector<bool> linie;
                    Standard::Algorithms::Utilities::fill_random(linie, rowSize);

                    data[index] = std::move(linie);
                }

                auto rectangle = algorithm_t::max_area(data);

                // todo(p3): Implement another faster method to test.
                auto square = algorithm_t::max_square(data);

                test_cases.emplace_back(
                    "Random test " + std::to_string(pair_index), std::move(data), rectangle, square);
            }
        }

        // It must be the last line.
        add_transposed_test_cases(test_cases);
    }

    void generate_test_cases_sum(std::vector<test_case_sum> &test_cases)
    {
        {// Long-running test cases go first.
            constexpr auto min_value = ::Standard::Algorithms::is_debug ? 3U : 5U;
            constexpr auto max_value = ::Standard::Algorithms::is_debug ? 6U : 20U;

            Standard::Algorithms::Utilities::random_t<std::size_t> size_random(min_value, max_value);
            const auto random1 = size_random();
            const auto random2 = size_random();

            std::vector<pair_t> pairs{ pair_t(random1, random2) };

            if constexpr (!::Standard::Algorithms::is_debug)
            {
                pairs.insert(pairs.end(), // NOLINTNEXTLINE
                    { pair_t(3, 5), pair_t(15, 13) });
            }

            for (std::size_t pair_index{}; pair_index < pairs.size(); ++pair_index)
            {
                const auto &pai = pairs[pair_index];
                const auto &[size, rowSize] = pai;

                std::vector<std::vector<int_t>> data(size);

                for (std::size_t index{}; index < size; ++index)
                {
                    std::vector<int_t> rows(rowSize);
                    Standard::Algorithms::Utilities::fill_random(rows, rowSize);

                    data[index] = rows;
                }

                rectangle_t rectangle{};

                auto sum = algorithm_t::max_sum<int_t, long_int_t>(data, rectangle);

                test_cases.emplace_back("Random test " + std::to_string(pair_index), std::move(data), sum, rectangle);
            }
        }

        test_cases.emplace_back("Dol By Nl", // NOLINTNEXTLINE
            std::vector<std::vector<int_t>>{ { -500, 10, 20 }, // NOLINTNEXTLINE
                { 2, -8, 0 }, { 1, -1, 11 } }, // NOLINTNEXTLINE
            32, rectangle_t{ point_t{ 0, 1 }, point_t{ 3, 3 } });

        test_cases.emplace_back("Only one positive", // NOLINTNEXTLINE
            std::vector<std::vector<int_t>>{ { -500, -10, -20 }, // NOLINTNEXTLINE
                { -500, -10, -20 }, { -500, -10, -20 }, { -2, -1, 789 }, // NOLINTNEXTLINE
                { -2, -9, 0 }, { -2, -9, 0 }, { -500, -10, -20 } }, // NOLINTNEXTLINE
            789, rectangle_t{ point_t{ 3, 2 }, point_t{ 4, 3 } });

        test_cases.emplace_back("All negative", // NOLINTNEXTLINE
            std::vector<std::vector<int_t>>{ { -50, -10, -20 }, // NOLINTNEXTLINE
                { -900, -10, -20 }, { -5, -10, -20 }, { -7000, -10, -20 } });

        test_cases.emplace_back("All positive", // NOLINTNEXTLINE
            std::vector<std::vector<int_t>>{ { 1, 3, 8, 20 }, // NOLINTNEXTLINE
                { 1, 3, 8, 20 }, { 1, 3, 8, 20 } }, // NOLINTNEXTLINE
            3 * (1 + 3 + 8 + 20), rectangle_t{ point_t{ 0, 0 }, point_t{ 3, 4 } });

        test_cases.emplace_back("Left top positive, right bottom zeros", // NOLINTNEXTLINE
            std::vector<std::vector<int_t>>{ { 1, 3, 8, 0 }, // NOLINTNEXTLINE
                { 1, 3, 8, 0 }, { 0, 0, 0, 0 } }, // NOLINTNEXTLINE
            2 * (1 + 3 + 8), rectangle_t{ point_t{ 0, 0 }, point_t{ 2, 3 } });
    }

    constexpr void run_test_case(const boolean_test_case &test)
    {
        run_boolean_sub_test(false, "max_area", test, &algorithm_t::max_area);

        // todo(p3): Exclude the buggy code for now.
        // RunBooleanSubTest(false, "MaxAreaMethod2", test, &algorithm_t::MaxAreaMethod2);

        run_boolean_sub_test(true, "max_square", test, &algorithm_t::max_square);
    }

    constexpr void run_test_case_sum(const test_case_sum &test)
    {
        const auto name = "Sum test " + test.name();

        {
            rectangle_t actual_rectangle{};

            const auto actual_sum = algorithm_t::max_sum_slow<int_t, long_int_t>(test.data(), actual_rectangle);

            ::Standard::Algorithms::ert::are_equal(test.sum(), actual_sum, name + " sum slow");

            ::Standard::Algorithms::ert::are_equal(test.rectangle(), actual_rectangle, name + " rectangle slow");
        }
        {
            rectangle_t actual_rectangle{};

            const auto actual_sum = algorithm_t::max_sum<int_t, long_int_t>(test.data(), actual_rectangle);

            ::Standard::Algorithms::ert::are_equal(test.sum(), actual_sum, name + " sum");

            ::Standard::Algorithms::ert::are_equal(test.rectangle(), actual_rectangle, name + " rectangle");
        }
    }

    constexpr void bool_tests()
    {
        Standard::Algorithms::Tests::test_utilities<boolean_test_case>::run_tests(run_test_case, generate_test_cases);
    }

    constexpr void sum_tests()
    {
        Standard::Algorithms::Tests::test_utilities<test_case_sum>::run_tests(run_test_case_sum, generate_test_cases_sum);
    }
} // namespace

void Standard::Algorithms::Numbers::Tests::max_sub_matrix_in_2d_matrix_tests()
{
    bool_tests();
    sum_tests();
}
