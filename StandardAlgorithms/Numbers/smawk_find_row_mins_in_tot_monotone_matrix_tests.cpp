#include"smawk_find_row_mins_in_tot_monotone_matrix_tests.h"
#include"../Utilities/simple_test_case.h"
#include"../Utilities/test_utilities.h"
#include"smawk_find_row_mins_in_tot_monotone_matrix.h"
#include<array>

namespace
{
    using int_t = std::int16_t;
    using input_t = std::vector<std::vector<int_t>>;
    using output_t = std::vector<std::size_t>;

    struct unique_tag final
    {
    };

    using test_case = Standard::Algorithms::Tests::simple_test_case<unique_tag, input_t, output_t>;

    constexpr void simple_validate(const test_case &test)
    {
        const auto &input = test.input();
        const auto size = Standard::Algorithms::require_positive(input.size(), "input size");

        const auto columns = Standard::Algorithms::require_positive(input.at(0).size(), "input[0] columns");

        for (const auto &inp : input)
        {
            ::Standard::Algorithms::ert::are_equal(columns, inp.size(), "is a valid matrix with the same # of columns");
        }

        {
            const auto ism = Standard::Algorithms::Numbers::is_totally_monotonic_slow(input);
            ::Standard::Algorithms::ert::are_equal(true, ism, "is_totally_monotonic_slow");
        }

        const auto &output = test.output();

        if (output.empty())
        {
            return;
        }

        ::Standard::Algorithms::ert::are_equal(size, output.size(), "output size");
        {
            const auto actual = std::is_sorted(output.begin(), output.end());
            ::Standard::Algorithms::ert::are_equal(true, actual, "output is sorted");
        }
        {
            const auto &max_column = output.back();
            Standard::Algorithms::require_greater(columns, max_column, "output max column");
        }
    }

    constexpr void test_is_totally_monotonic()
    {
        {
            const std::vector<std::vector<int_t>> source{
                // NOLINTNEXTLINE
                { 20, 40, 10 }, // NOLINTNEXTLINE
                { 20, 40, 30 }
                // Consider this sub-rectangle:
                //   { 20, 10 }, the min is in column 1.
                //   { 20, 30 }, the min is in column 0.
                // Since 1 decreased to 0, the source matrix is not totally monotonic.
            };
            const auto fast = Standard::Algorithms::Numbers::is_totally_monotonic_slow(source);

            ::Standard::Algorithms::ert::are_equal(false, fast, "is_totally_monotonic_slow sample1");
        }
        {
            const std::vector<std::vector<int_t>> source{// NOLINTNEXTLINE
                { 20, 40, 30 }, // NOLINTNEXTLINE
                { 20, 40, 10 }
            };
            const auto fast = Standard::Algorithms::Numbers::is_totally_monotonic_slow(source);

            ::Standard::Algorithms::ert::are_equal(true, fast, "is_totally_monotonic_slow sample2");
        }

        // todo(p3): more tests.
    }

    constexpr void generate_test_cases(std::vector<test_case> &test_cases)
    {
        test_cases.emplace_back("larmore",
            input_t{
                // NOLINTNEXTLINE
                { 25, 21, 13, 10, 20, 13, 19, 35, 37, 41, 58, 66, 82, 99, 124, 133, 156, 178 },
                //           10
                //           20 // NOLINTNEXTLINE
                { 42, 35, 26, 20, 29, 21, 25, 37, 36, 39, 56, 64, 76, 91, 116, 125, 146, 164 }, // NOLINTNEXTLINE
                { 57, 48, 35, 28, 33, 24, 28, 40, 37, 37, 54, 61, 72, 83, 107, 113, 131, 146 },
                //                 24
                //                 35 // NOLINTNEXTLINE
                { 78, 65, 51, 42, 44, 35, 38, 48, 42, 42, 55, 61, 70, 80, 100, 106, 120, 135 }, // NOLINTNEXTLINE
                { 90, 76, 58, 48, 49, 39, 42, 48, 39, 35, 47, 51, 56, 63, 80, 86, 97, 110 },
                //                             35
                //                             33 // NOLINTNEXTLINE
                { 103, 85, 67, 56, 55, 44, 44, 49, 39, 33, 41, 44, 49, 56, 71, 75, 84, 96 }, // NOLINTNEXTLINE
                { 123, 105, 86, 75, 73, 59, 57, 62, 51, 44, 50, 52, 55, 59, 72, 74, 80, 92 },
                //                             44
                //                             43 // NOLINTNEXTLINE
                { 142, 123, 100, 86, 82, 65, 61, 62, 50, 43, 47, 45, 46, 46, 58, 59, 65, 73 },
                //                                          20 // NOLINTNEXTLINE
                { 151, 130, 104, 88, 80, 59, 52, 49, 37, 29, 29, 24, 23, 20, 28, 25, 31, 39 },
                //},{ 10, 20, 24, 35, 35, 33, 44, 43, 20 } });
            }, // NOLINTNEXTLINE
            output_t{ 3, 3, 5, 5, 9, 9, 9, 9, 13 });

        test_cases.push_back({ "base3", // NOLINTNEXTLINE
            input_t{ { 2, 4, 3 }, { 2, 4, 1 } }, output_t{ 0, 2 } });

        test_cases.push_back({ "same 3", // NOLINTNEXTLINE
            input_t{ { 2, 2, 4, 4, 4, 3, 3 }, { 2, 2, 4, 4, 4, 1, 1 } }, output_t{ 0, 5 } });

        test_cases.push_back({ "trivial", // NOLINTNEXTLINE
            input_t{ { 200 } }, output_t{ 0 } });

        test_cases.push_back({ "base2", // NOLINTNEXTLINE
            input_t{ { 200, 100 }, { 160, 101 } }, output_t{ 1, 1 } });

        test_cases.push_back({ "base22", // NOLINTNEXTLINE
            input_t{ { 20, 100 }, { 160, 101 } }, output_t{ 0, 1 } });

        test_cases.push_back({ "base24", // NOLINTNEXTLINE
            input_t{ { 200, 100, 99, 100 }, { 160, 130, 111, 102 } }, output_t{ 2, 3 } });

        test_cases.push_back({ "base42", // NOLINTNEXTLINE
            input_t{ { 99, 100 }, { 120, 130 }, { 111, 102 }, { -1, -2 } }, output_t{ 0, 0, 1, 1 } });
        // todo(p3): do Standard::Algorithms::Utilities::random_t<std::int32_t> rnd{};
    }

    constexpr void run_test_case(const test_case &test)
    {
        const auto fast = Standard::Algorithms::Numbers::smawk_find_row_mins_in_tot_monotone_matrix(test.input());

        if (!test.output().empty())
        {
            ::Standard::Algorithms::ert::are_equal(test.output(), fast, "smawk_find_row_mins_in_tot_monotone_matrix");
        }

        const auto slow = Standard::Algorithms::Numbers::find_row_mins_in_2d_matrix_slow(test.input());

        ::Standard::Algorithms::ert::are_equal(fast, slow, "find_row_mins_in_2d_matrix_slow");
    }
} // namespace

void Standard::Algorithms::Numbers::Tests::smawk_find_row_mins_in_tot_monotone_matrix_tests()
{
    test_is_totally_monotonic();

    ::Standard::Algorithms::Tests::test_utilities<test_case>::run_tests(run_test_case, generate_test_cases);
}
