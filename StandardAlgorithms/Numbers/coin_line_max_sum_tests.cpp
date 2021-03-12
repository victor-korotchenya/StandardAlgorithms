#include"coin_line_max_sum_tests.h"
#include"../Utilities/random.h"
#include"../Utilities/test_utilities.h"
#include"coin_line_max_sum.h"

namespace
{
    using int_t = std::uint64_t;
    using int_vec = std::vector<int_t>;

    struct test_case final : Standard::Algorithms::Tests::base_test_case
    {
        constexpr test_case(std::string &&name, int_vec &&values, const int_t &expected_min_value = int_t{}) noexcept
            : base_test_case(std::move(name))
            , Values(std::move(values))
            , Expected_min_value(expected_min_value)
        {
        }

        [[nodiscard]] constexpr auto values() const &noexcept -> const int_vec &
        {
            return Values;
        }

        [[nodiscard]] constexpr auto expected_min_value() const &noexcept -> const int_t &
        {
            return Expected_min_value;
        }

        constexpr void validate() const override
        {
            base_test_case::validate();

            ::Standard::Algorithms::ert::greater(Expected_min_value, int_t{}, "Expected min value");
        }

        void print(std::ostream &str) const override
        {
            ::Standard::Algorithms::print(", Values", Values, str);
            str << ", Expected min value" << Expected_min_value;
        }

private:
        int_vec Values;
        int_t Expected_min_value;
    };

    void add_tests_random(std::vector<test_case> &test_cases)
    {
        constexpr std::size_t size_min = 1;
        constexpr std::size_t size_max = 50;

        const auto even_size = Standard::Algorithms::Utilities::random_t<std::size_t>(size_min, size_max)() << 1U;

        constexpr int_t cutter = 1U << 20U;

        int_vec values;
        Standard::Algorithms::Utilities::fill_random(values, even_size, cutter);

        auto expected = Standard::Algorithms::Numbers::coin_line_max_sum<int_t>::min_guaranteed_value_fast(values);

        test_cases.emplace_back("Random", std::move(values), expected);
    }

    // Reversing the values must give the same result;
    constexpr void add_tests_reversed(std::vector<test_case> &test_cases)
    {
        const auto size = test_cases.size();

        const std::string post_fix = "_rev";

        for (std::size_t index{}; index < size; ++index)
        {
            const auto &old_test_case = test_cases[index];
            auto name = old_test_case.name() + post_fix;

            auto values = old_test_case.values();
            std::reverse(values.begin(), values.end());

            test_cases.emplace_back(std::move(name), std::move(values), old_test_case.expected_min_value());
        }
    }

    constexpr void add_tests_same_size(std::vector<test_case> &test_cases)
    {
        const std::string prefix = "Same";

        constexpr int_t min_size = 6;
        constexpr int_t max_size = 10;
        constexpr int_t value = min_size + max_size + 5;

        for (auto index = min_size; index <= max_size; ++index)
        {
            auto name = prefix + std::to_string(index);

            auto expectedvalue = static_cast<int_t>(value * ((index + 1) >> 1U));

            test_cases.emplace_back(std::move(name), int_vec(index, value), expectedvalue);
        }
    }

    void generate_test_cases(std::vector<test_case> &test_cases)
    {
        constexpr int_t value = 20;

        test_cases.emplace_back("Size5",
            // NOLINTNEXTLINE
            int_vec({ 5, 8, 2, 1, 4 }), 11);

        test_cases.emplace_back("Trivial Size 2",
            // NOLINTNEXTLINE
            int_vec({ value, value << 1U }), value << 1U);

        // All unique permutations of {1,2,3,4}:
        test_cases.emplace_back("Permute4_1",
            // NOLINTNEXTLINE
            int_vec({ 1, 2, 3, 4 }), 6);

        test_cases.emplace_back("Permute4_2",
            // NOLINTNEXTLINE
            int_vec({ 1, 2, 4, 3 }), 5);

        test_cases.emplace_back("Permute4_3",
            // NOLINTNEXTLINE
            int_vec({ 1, 3, 2, 4 }), 7);

        test_cases.emplace_back("Permute4_4",
            // NOLINTNEXTLINE
            int_vec({ 1, 3, 4, 2 }), 5);

        test_cases.emplace_back("Permute4_5",
            // NOLINTNEXTLINE
            int_vec({ 1, 4, 2, 3 }), 7);

        test_cases.emplace_back("Permute4_6",
            // NOLINTNEXTLINE
            int_vec({ 1, 4, 3, 2 }), 6);

        test_cases.emplace_back("Permute4_7",
            // NOLINTNEXTLINE
            int_vec({ 2, 1, 3, 4 }), 6);

        test_cases.emplace_back("Permute4_8",
            // NOLINTNEXTLINE
            int_vec({ 2, 1, 4, 3 }), 6);

        test_cases.emplace_back("Permute4_9",
            // NOLINTNEXTLINE
            int_vec({ 2, 3, 1, 4 }), 7);

        test_cases.emplace_back("Permute4_10",
            // NOLINTNEXTLINE
            int_vec({ 2, 4, 1, 3 }), 7);

        test_cases.emplace_back("Permute4_11",
            // NOLINTNEXTLINE
            int_vec({ 3, 1, 2, 4 }), 6);

        test_cases.emplace_back("Permute4_12",
            // NOLINTNEXTLINE
            int_vec({ 3, 2, 1, 4 }), 6);

        test_cases.emplace_back("Size4",
            // NOLINTNEXTLINE
            int_vec({ 5, 8, 2, 1 }), 9);

        test_cases.emplace_back("Size3",
            // NOLINTNEXTLINE
            int_vec({ 5, 8, 2 }), 7);

        test_cases.emplace_back("Size3_2",
            // NOLINTNEXTLINE
            int_vec({ 8, 2, 1 }), 9);

        // These must be the last lines.
        add_tests_random(test_cases);
        add_tests_reversed(test_cases);
        add_tests_same_size(test_cases);
    }

    constexpr void run_test_case(const test_case &test)
    {
        using alg = int_t (*)(const int_vec &);

        const std::vector<std::pair<std::string, alg>> sub_tests = {
            { "Slow", Standard::Algorithms::Numbers::coin_line_max_sum<int_t>::min_guaranteed_value_slow },
            { "Fast", Standard::Algorithms::Numbers::coin_line_max_sum<int_t>::min_guaranteed_value_fast }
        };

        for (const auto &sub : sub_tests)
        {
            const auto actual = sub.second(test.values());

            ::Standard::Algorithms::ert::are_equal(test.expected_min_value(), actual, sub.first);
        }
    }
} // namespace

void Standard::Algorithms::Numbers::Tests::coin_line_max_sum_tests()
{
    Standard::Algorithms::Tests::test_utilities<test_case>::run_tests<all_cores>(run_test_case, generate_test_cases);
}
