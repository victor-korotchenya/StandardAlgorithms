#include"array_min_moves_make_non_decreasing_tests.h"
#include"../Utilities/simple_test_case.h"
#include"../Utilities/test_utilities.h"
#include"array_min_moves_make_non_decreasing.h"

namespace
{
    using int_t = std::int64_t;
    using vec_int_t = std::vector<int_t>;

    struct unique_tag final
    {
    };

    using test_case = Standard::Algorithms::Tests::simple_test_case<unique_tag, vec_int_t, int_t>;

    constexpr void simple_validate(const test_case &test)
    {
        Standard::Algorithms::require_positive(test.input().size(), "input size");
    }

    constexpr void generate_test_cases(std::vector<test_case> &test_cases)
    {
        test_cases.emplace_back("One element",
            // NOLINTNEXTLINE
            vec_int_t{ 10 }, 0);

        test_cases.emplace_back("Two increasing elements",
            // NOLINTNEXTLINE
            vec_int_t{ 5, 10 }, 0);

        test_cases.emplace_back("Two decreasing elements",
            // NOLINTNEXTLINE
            vec_int_t{ 10, 5 }, 5);

        test_cases.emplace_back("Simple",
            // NOLINTNEXTLINE
            vec_int_t{ 5, 9, 1, 4, 10, 8 }, 11);
    }

    constexpr void run_test_case(const test_case &test)
    {
        const auto actual = Standard::Algorithms::Numbers::array_min_moves_make_non_decreasing(test.input());

        ::Standard::Algorithms::ert::are_equal(test.output(), actual, "array_min_moves_make_non_decreasing");
    }
} // namespace

void Standard::Algorithms::Numbers::Tests::array_min_moves_make_non_decreasing_tests()
{
    Standard::Algorithms::Tests::test_utilities<test_case>::run_tests(run_test_case, generate_test_cases);
}
