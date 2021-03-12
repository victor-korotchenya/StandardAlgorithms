#include"longest_increasing_subsequence_tests.h"
#include"../Utilities/simple_test_case.h"
#include"../Utilities/test_utilities.h"
#include"longest_increasing_subsequence.h"
#include<array>

namespace
{
    using item_t = std::int32_t;
    using input_t = std::vector<item_t>;
    using output_t = std::vector<std::size_t>;

    struct unique_tag final
    {
    };

    using test_case = Standard::Algorithms::Tests::simple_test_case<unique_tag, input_t, output_t>;

    constexpr void generate_test_cases(std::vector<test_case> &test_cases)
    {
        test_cases.emplace_back("Increasing", // NOLINTNEXTLINE
            input_t{ 5, 10, 20, 500 }, // NOLINTNEXTLINE
            output_t{ 0, 1, 2, 3 });

        test_cases.emplace_back("NonIncreasing", // NOLINTNEXTLINE
            input_t{ 50, 10 }, output_t{ 1 });

        test_cases.emplace_back("Trivial", // NOLINTNEXTLINE
            input_t{ -97'864 }, output_t{ 0 });

        test_cases.emplace_back("Many", // NOLINTNEXTLINE
            input_t{ 200, -50, 10, 90, 25, -3, 1'000, 50, 500 }, // NOLINTNEXTLINE
            output_t{ 1, 2, 4, 7, 8 });
    }

    constexpr void run_test_case(const test_case &test)
    {
        using alg_t = std::vector<std::size_t> (*)(const input_t &source, std::less<item_t>);
        using name_alg_t = std::pair<std::string, alg_t>;

        const std::array sub_tests{ name_alg_t("Slow",
                                        Standard::Algorithms::Numbers::longest_increasing_subsequence<item_t>::slow),
            name_alg_t("Fast", Standard::Algorithms::Numbers::longest_increasing_subsequence<item_t>::fast) };

        for (const auto &sub_test : sub_tests)
        {
            const auto actual = sub_test.second(test.input(), std::less<item_t>{});

            ::Standard::Algorithms::ert::are_equal(test.output(), actual, sub_test.first);
        }
    }
} // namespace

void Standard::Algorithms::Numbers::Tests::longest_increasing_subsequence_tests()
{
    Standard::Algorithms::Tests::test_utilities<test_case>::run_tests(run_test_case, generate_test_cases);
}
