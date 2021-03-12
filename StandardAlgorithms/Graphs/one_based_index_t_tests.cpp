#include"one_based_index_t_tests.h"
#include"../Utilities/test_utilities.h"
#include"one_based_index_t.h"
#include<initializer_list>

namespace
{
    struct test_case final : Standard::Algorithms::Tests::base_test_case
    {
        constexpr test_case(std::size_t input, std::size_t expected)
            : base_test_case(::Standard::Algorithms::Utilities::zu_string(input) + " -> " +
                  ::Standard::Algorithms::Utilities::zu_string(expected))
            , Input(input)
            , Expected(expected)
        {
        }

        [[nodiscard]] constexpr auto input() const noexcept -> std::size_t
        {
            return Input;
        }

        [[nodiscard]] constexpr auto expected() const noexcept -> std::size_t
        {
            return Expected;
        }

        constexpr void validate() const override
        {
            base_test_case::validate();

            constexpr auto edge = 1U;
            Standard::Algorithms::require_greater(Input, edge, "input");
            Standard::Algorithms::require_greater(Expected, edge, "expected");

            ::Standard::Algorithms::ert::not_equal(Input, Expected, "input");
        }

        void print(std::ostream &str) const override
        {
            str << ", input " << Input << ", expected " << Expected;
        }

private:
        std::size_t Input;
        std::size_t Expected;
    };

    constexpr void generate_test_cases(std::vector<test_case> &tests)
    {
        using input_expected_t = std::pair<std::size_t, std::size_t>;

        const std::initializer_list<input_expected_t> one_based_data{
            // NOLINTNEXTLINE
            { 2, 3 },
            // NOLINTNEXTLINE
            { 4, 6 },
            { 5, 7 },
            // NOLINTNEXTLINE
            { 8, 12 },
            { 9, 13 },
            { 10, 14 },
            { 11, 15 },
            // NOLINTNEXTLINE
            { 16, 24 },
            { 17, 25 },
            { 18, 26 },
            { 19, 27 },
            { 20, 28 },
            { 21, 29 },
            { 22, 30 },
            { 23, 31 },
            // NOLINTNEXTLINE
            { 32, 48 },
            { 33, 49 },
            { 34, 50 },
        };

        for (const auto &input_expected : one_based_data)
        {
            assert(input_expected.first < input_expected.second);

            tests.emplace_back(input_expected.first, input_expected.second);

            // The function is symmetric.
            tests.emplace_back(input_expected.second, input_expected.first);
        }
    }

    constexpr void run_test_case(const test_case &test)
    {
        const auto zero_index = test.input() - 1U;

        const Standard::Algorithms::Heaps::one_based_index_t index{ zero_index };

        const auto actual = Standard::Algorithms::Heaps::symmetric_index(index);

        ::Standard::Algorithms::ert::are_equal(test.expected(), actual.one_based(), "symmetric_index");
    }
} // namespace

void Standard::Algorithms::Heaps::Tests::one_based_index_t_tests()
{
    ::Standard::Algorithms::Tests::test_utilities<test_case>::run_tests(run_test_case, generate_test_cases);
}
