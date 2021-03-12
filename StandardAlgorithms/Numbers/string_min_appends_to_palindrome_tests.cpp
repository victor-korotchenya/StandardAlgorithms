#include"string_min_appends_to_palindrome_tests.h"
#include"../Utilities/random.h"
#include"../Utilities/test_utilities.h"
#include"string_min_appends_to_palindrome.h"
#include<unordered_set>

namespace
{
    using int_t = std::int32_t;

    constexpr auto not_computed = -1;

    struct test_case final : Standard::Algorithms::Tests::base_test_case
    {
        constexpr test_case(std::string &&name, int_t expected) noexcept
            : base_test_case(std::move(name))
            , Expected(expected)
        {
        }

        [[nodiscard]] constexpr auto expected() const noexcept -> int_t
        {
            return Expected;
        }

        constexpr void validate() const override
        {
            base_test_case::validate();

            if (Expected == not_computed)
            {
                return;
            }

            const auto maxim = static_cast<int_t>(this->name().size() - 1U);
            Standard::Algorithms::require_between(int_t{}, Expected, maxim, "expected");
        }

        void print(std::ostream &str) const override
        {
            ::Standard::Algorithms::print_value("expected", str, Expected);
        }

private:
        int_t Expected;
    };

    void generate_test_cases(std::vector<test_case> &test_cases)
    {
        test_cases.emplace_back("abbaca", 3); // NOLINTNEXTLINE
        test_cases.emplace_back("a", 0); // NOLINTNEXTLINE
        test_cases.emplace_back("aa", 0); // NOLINTNEXTLINE
        test_cases.emplace_back("aaa", 0); // NOLINTNEXTLINE
        test_cases.emplace_back("aaaaac", 5); // NOLINTNEXTLINE
        test_cases.emplace_back("aba", 0); // NOLINTNEXTLINE
        test_cases.emplace_back("ab", 1); // NOLINTNEXTLINE
        test_cases.emplace_back("abb", 1); // NOLINTNEXTLINE
        test_cases.emplace_back("abbc", 3); // NOLINTNEXTLINE
        test_cases.emplace_back("abc", 2); // NOLINTNEXTLINE
        test_cases.emplace_back("abcd", 3); // NOLINTNEXTLINE
        test_cases.emplace_back("abcdabd", 6); // NOLINTNEXTLINE
        test_cases.emplace_back("abcdefgh", 7); // NOLINTNEXTLINE
        test_cases.emplace_back("abcebbe", 3); // NOLINTNEXTLINE
        test_cases.emplace_back("qwertyuiop", 9); // NOLINTNEXTLINE

        Standard::Algorithms::Utilities::random_t<std::int32_t> rnd{};
        std::unordered_set<std::string> uniq{};

        constexpr auto max_attempts = 1;

        for (std::int32_t att{}; att < max_attempts; ++att)
        {
            constexpr auto min_size = 1;
            constexpr auto max_size = 10;

            constexpr auto min_char = '0';
            constexpr auto max_char = '9';

            auto str = Standard::Algorithms::Utilities::random_string<decltype(rnd), min_char, max_char>(
                rnd, min_size, max_size);

            if (uniq.insert(str).second)
            {
                test_cases.emplace_back(std::move(str), not_computed);
            }
        }
    }

    constexpr void run_test_case(const test_case &test)
    {
        const auto &str = test.name();
        const auto fast = Standard::Algorithms::Numbers::string_min_appends_to_palindrome<int_t>(str);

        if (test.expected() != not_computed)
        {
            ::Standard::Algorithms::ert::are_equal(test.expected(), fast, "string_min_appends_to_palindrome");
        }

        const auto slow = Standard::Algorithms::Numbers::string_min_appends_to_palindrome_slow<int_t>(str);

        ::Standard::Algorithms::ert::are_equal(fast, slow, "string_min_appends_to_palindrome_slow");
    }
} // namespace

void Standard::Algorithms::Numbers::Tests::string_min_appends_to_palindrome_tests()
{
    Standard::Algorithms::Tests::test_utilities<test_case>::run_tests(run_test_case, generate_test_cases);
}
