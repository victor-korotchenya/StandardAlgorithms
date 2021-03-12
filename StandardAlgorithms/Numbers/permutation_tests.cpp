#include"permutation_tests.h"
#include"../Utilities/simple_test_case.h"
#include"../Utilities/test_utilities.h"
#include"permutation.h"

namespace
{
    using int_t = std::uint16_t;
    using perm_t = std::vector<int_t>;

    struct unique_tag final
    {
    };

    using test_case = Standard::Algorithms::Tests::simple_test_case<unique_tag, perm_t, std::int32_t>;

    constexpr void simple_validate(const test_case &test)
    {
        constexpr auto is_zero_start = true;

        const auto &input = test.input();
        const auto is_perm = Standard::Algorithms::Numbers::is_permutation_2(input, is_zero_start);

        ::Standard::Algorithms::ert::are_equal(true, is_perm, "is permutation");
    }

    constexpr void generate_test_cases(std::vector<test_case> &test_cases)
    {
        test_cases.emplace_back("()", perm_t{}, 1);
        test_cases.emplace_back("(0)", perm_t{ 0 }, 1);

        test_cases.emplace_back("(0)(1)", perm_t{ 0, 1 }, 1);
        test_cases.emplace_back("(10)", perm_t{ 1, 0 }, -1);

        test_cases.emplace_back("(0)(1)(2)", perm_t{ 0, 1, 2 }, 1);
        test_cases.emplace_back("(0)(21)", perm_t{ 0, 2, 1 }, -1);
        test_cases.emplace_back("(10)(2)", perm_t{ 1, 0, 2 }, -1);
        test_cases.emplace_back("(120)", perm_t{ 1, 2, 0 }, 1);
        test_cases.emplace_back("(210)", perm_t{ 2, 0, 1 }, 1);
        test_cases.emplace_back("(20)(1)", perm_t{ 2, 1, 0 }, -1);

        test_cases.emplace_back("(0)(1)(2)(3)", perm_t{ 0, 1, 2, 3 }, 1);
        test_cases.emplace_back("(0)(1)(32)", perm_t{ 0, 1, 3, 2 }, -1);
    }

    constexpr void run_test_case(const test_case &test)
    {
        const auto &input = test.input();
        const auto actual = Standard::Algorithms::Numbers::permutation_sign(input);

        ::Standard::Algorithms::ert::are_equal(test.output(), actual, "permutation sign (permutation parity)");
    }
} // namespace

void Standard::Algorithms::Numbers::Tests::permutation_tests()
{
    Standard::Algorithms::Tests::test_utilities<test_case>::run_tests(run_test_case, generate_test_cases);
}
