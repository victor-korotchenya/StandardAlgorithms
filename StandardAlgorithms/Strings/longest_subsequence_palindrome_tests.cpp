#include"longest_subsequence_palindrome_tests.h"
#include"../Utilities/random.h"
#include"../Utilities/test_utilities.h"
#include"longest_subsequence_palindrome.h"
#include<unordered_set>

namespace
{
    struct test_case final : Standard::Algorithms::Tests::base_test_case
    {
        constexpr test_case(std::string &&name, std::size_t expected) noexcept
            : base_test_case(std::move(name))
            , Expected(expected)
        {
        }

        [[nodiscard]] constexpr auto expected() const &noexcept -> const std::size_t &
        {
            return Expected;
        }

        [[nodiscard]] constexpr auto is_computed() const noexcept -> bool
        {
            return 0U < Expected;
        }

        constexpr void validate() const override
        {
            base_test_case::validate();

            const auto size = name().size();
            Standard::Algorithms::require_less_equal(Expected, size, "expected");
        }

        void print(std::ostream &str) const override
        {
            ::Standard::Algorithms::print_value("expected", str, Expected);
        }

private:
        std::size_t Expected;
    };

    void generate_test_cases(std::vector<test_case> &test_cases)
    {
        test_cases.emplace_back("abcbb", 3);
        test_cases.emplace_back("abcb", 3);

        // NOLINTNEXTLINE
        test_cases.emplace_back("abcbbc", 4);
        test_cases.emplace_back("abab", 3);
        test_cases.emplace_back("baba", 3);

        // NOLINTNEXTLINE
        test_cases.emplace_back("ababa", 5);

        // NOLINTNEXTLINE
        test_cases.emplace_back("ad oba da", 7);

        // NOLINTNEXTLINE
        test_cases.emplace_back("ad ob da", 7);

        // NOLINTNEXTLINE
        test_cases.emplace_back("ad  da", 6);

        // NOLINTNEXTLINE
        test_cases.emplace_back("ad da", 5);

        // NOLINTNEXTLINE
        test_cases.emplace_back("adda", 4);
        test_cases.emplace_back("base1", 1);
        test_cases.emplace_back("base11", 2);
        test_cases.emplace_back("1bas1e", 3);

        // NOLINTNEXTLINE
        test_cases.emplace_back("abba", 4);
        test_cases.emplace_back("abbc", 2);

        // NOLINTNEXTLINE
        test_cases.emplace_back("abob", 3);

        std::unordered_set<std::string> uniq{ "" };

        for (const auto &test : test_cases)
        {
            uniq.insert(test.name());
        }

        Standard::Algorithms::Utilities::random_t<std::int32_t> rnd{};

        constexpr auto max_attempts = 1;

        for (std::int32_t att{}; att < max_attempts; ++att)
        {
            constexpr auto min_size = 1;
            constexpr auto max_size = 30;

            auto str = Standard::Algorithms::Utilities::random_string_unique(rnd, uniq, min_size, max_size);

            if (str.empty())
            {
                continue;
            }

            test_cases.emplace_back(std::move(str), 0U);
        }
    }

    constexpr void run_test_case(const test_case &test)
    {
        const auto &str = test.name();

        const auto fast = Standard::Algorithms::Strings::longest_subsequence_palindrome_other(str);

        if (test.is_computed())
        {
            ::Standard::Algorithms::ert::are_equal(test.expected(), fast, "longest_subsequence_palindrome_other");
        }

        const auto almost = Standard::Algorithms::Strings::longest_subsequence_palindrome_almost(str);

        ::Standard::Algorithms::ert::are_equal(fast, almost, "longest_subsequence_palindrome_almost");

        if (constexpr auto large_size = 10U; large_size < str.size())
        {
            return;
        }

        const auto slow = Standard::Algorithms::Strings::longest_subsequence_palindrome_slow(str);

        ::Standard::Algorithms::ert::are_equal(fast, slow, "longest_subsequence_palindrome_slow");
    }
} // namespace

void Standard::Algorithms::Strings::Tests::longest_subsequence_palindrome_tests()
{
    Standard::Algorithms::Tests::test_utilities<test_case>::run_tests(run_test_case, generate_test_cases);
}
