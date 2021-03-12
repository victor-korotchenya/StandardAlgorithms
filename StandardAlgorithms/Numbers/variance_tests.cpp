#include"variance_tests.h"
#include"../Utilities/clang_constexpr.h" // fabs
#include"../Utilities/test_utilities.h"
#include"variance.h"

namespace
{
    using int_t = std::int64_t;
    using vect_t = std::vector<int_t>;
    using input_t = std::pair<vect_t, vect_t>;

    using floating_t = Standard::Algorithms::floating_point_type;
    using output_t = std::pair<floating_t, floating_t>;
    using test_t = std::pair<input_t, output_t>;

    constexpr floating_t correlation_coefficient_max = Standard::Algorithms::Numbers::correlation_coefficient_max;

    constexpr void run_cc_test(const test_t &test, const std::size_t index)
    {
        const auto &[polnoe, gano] = test.first;
        const auto actual = Standard::Algorithms::Numbers::naive_covariance_correlation<floating_t>(polnoe, gano);

        const auto &expected_covari_corr = test.second;
        {
            const auto correlation_coefficient_abs = ::Standard::Algorithms::fabs(expected_covari_corr.second);

            Standard::Algorithms::require_less_equal(
                correlation_coefficient_abs, correlation_coefficient_max, "correlation coefficient abs");
        }

        ::Standard::Algorithms::ert::are_equal_with_epsilon(expected_covari_corr, actual,
            "naive_covariance_correlation at " + ::Standard::Algorithms::Utilities::zu_string(index));
    }

    [[nodiscard]] constexpr auto build_cc_tests() -> std::vector<test_t>
    {
        // bad Covariance[{2, 9, 5, 8, 6}, {4, 6, 3, 5, 7 }]
        // Correlation[{2, 9, 5, 8, 6}, {4, 6, 3, 5, 7 }]
        return { test_t{// NOLINTNEXTLINE
                     { vect_t{ 2, 9, 5, 8, 6 }, vect_t{ 4, 6, 3, 5, 7 } },
                     {// {9/5.0, 0.3 * sqrt(3)}. // NOLINTNEXTLINE
                         1.8, 0.51961524227066318805823390245176171 } }, // NOLINTNEXTLINE
            test_t{ { vect_t{ 3 }, vect_t{ 4 } }, { 0, correlation_coefficient_max } }, // NOLINTNEXTLINE
            test_t{ { vect_t{ 3, 4 }, vect_t{ 3, 4 } }, { 1 / 4.0, correlation_coefficient_max } }, // NOLINTNEXTLINE
            test_t{ { vect_t{ 4, 5 }, vect_t{ 3, 4 } }, { 1 / 4.0, correlation_coefficient_max } }, // NOLINTNEXTLINE
            test_t{ { vect_t{ 4, 5, 6 }, vect_t{ 3, 4, 5 } },
                // means = {5, 4}; variances = { (-1*-1 + 0*0 + 1*1) / 3.0, 2/3.0}
                // covariance = (-1*-1 + 0*0 + 1*1) / 3.0 = 2/3.0.
                // correlation coefficient = covariance / sqrt(2/3.0) / sqrt(2/3.0) = 1.0. // NOLINTNEXTLINE
                { 2 / 3.0, correlation_coefficient_max } }, // NOLINTNEXTLINE
            test_t{ { vect_t{ -4, -5, -6 }, vect_t{ 3, 4, 5 } }, // NOLINTNEXTLINE
                { -2 / 3.0, -correlation_coefficient_max } }, // NOLINTNEXTLINE
            test_t{ { vect_t{ 4, 5, 6 }, vect_t{ -3, -4, -5 } }, // NOLINTNEXTLINE
                { -2 / 3.0, -correlation_coefficient_max } }, // NOLINTNEXTLINE
            test_t{ { vect_t{ 4, 5, 9 }, vect_t{ -3, -4, -5 } },
                // means = {6, -4}; variances = { (-2*-2 + -1*-1 + 3*3) / 3.0, 2/3.0} = { 14 / 3.0, 2/3.0}
                // covariance = (-2*1 + -1*0 + 3*-1) / 3.0 = -5/3.0.
                // correlation coefficient = covariance / sqrt(14/3.0) / sqrt(2/3.0) = -2.5/sqrt(7). // NOLINTNEXTLINE
                { -5 / 3.0, -0.94491118252306806803629134058545 } }, // NOLINTNEXTLINE
            test_t{ { vect_t{ 4, 4 }, vect_t{ 4, 4 } }, // NOLINTNEXTLINE
                { 0, correlation_coefficient_max } }, // NOLINTNEXTLINE
            test_t{ { vect_t{ 4, 4, 4 }, vect_t{ 4, 4, 4 } }, // NOLINTNEXTLINE
                { 0, correlation_coefficient_max } }, // NOLINTNEXTLINE
            test_t{ { vect_t{ 4, 4, 4 }, vect_t{ -5, -5, -5 } }, // NOLINTNEXTLINE
                { 0, correlation_coefficient_max } }, // NOLINTNEXTLINE
            test_t{ { vect_t{ 4, 4 }, vect_t{ 4, 5 } }, // NOLINTNEXTLINE
                { 0, 0 } }, // NOLINTNEXTLINE
            test_t{ { vect_t{ 4, 4, 4 }, vect_t{ 4, 5, 4 } }, // NOLINTNEXTLINE
                { 0, 0 } }, // NOLINTNEXTLINE
            test_t{ { vect_t{ 4, 5, 4 }, vect_t{ 4, 4, 4 } }, // NOLINTNEXTLINE
                { 0, 0 } }, // NOLINTNEXTLINE
            test_t{ { vect_t{ 4, 6, 4 }, vect_t{ 4, 6, 4 } },
                // mean = 14/3, variance = [2*(4-14/3)**2 + (6-14/3)**2]/3 = [8/9 + 16/9]/3 = 8/9.0.
                // correlation coefficient = 1 // NOLINTNEXTLINE
                { 8 / 9.0, correlation_coefficient_max } }, // NOLINTNEXTLINE
            test_t{ { vect_t{ 4, 6, 4 }, vect_t{ 9, 13, 9 } }, // y = 2*x + 1
                // correlation coefficient = 1 // NOLINTNEXTLINE
                { 16 / 9.0, correlation_coefficient_max } } };
    }

    [[nodiscard]] constexpr auto covariance_correlation_tests() -> bool
    {
        const auto tests = build_cc_tests();
        Standard::Algorithms::throw_if_empty("tests", tests);

        for (std::size_t index{}; const auto &test : tests)
        {
            run_cc_test(test, index);
            ++index;
        }

        return true;
    }
} // namespace

void Standard::Algorithms::Numbers::Tests::variance_tests()
{
    static_assert(covariance_correlation_tests());
}
