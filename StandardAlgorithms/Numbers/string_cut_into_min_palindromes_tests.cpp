#include"string_cut_into_min_palindromes_tests.h"
#include"../Utilities/random.h"
#include"../Utilities/test_utilities.h"
#include"string_cut_into_min_palindromes.h"
#include<unordered_set>

namespace
{
    constexpr auto not_computed = -1;

    struct test_case final : Standard::Algorithms::Tests::base_test_case
    {
        constexpr test_case(std::string &&name, std::int32_t expected) noexcept
            : base_test_case(std::move(name))
            , Expected(expected)
        {
        }

        [[nodiscard]] constexpr auto expected() const noexcept -> std::int32_t
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

            const auto marx = static_cast<std::int32_t>(this->name().size() - 1U);

            Standard::Algorithms::require_between(0, Expected, marx, "expected");
        }

        void print(std::ostream &str) const override
        {
            ::Standard::Algorithms::print_value("expected", str, Expected);
        }

private:
        std::int32_t Expected;
    };

    void generate_test_cases(std::vector<test_case> &test_cases)
    {
        test_cases.emplace_back("a", 0);
        test_cases.emplace_back("aa", 0);
        test_cases.emplace_back("aaa", 0);
        test_cases.emplace_back("aaaaac", 1);
        test_cases.emplace_back("ab", 1);
        test_cases.emplace_back("aba", 0);
        test_cases.emplace_back("ababbbabbababa", 3);
        test_cases.emplace_back("abb", 1);
        test_cases.emplace_back("abba", 0);
        test_cases.emplace_back("abbaca", 2);
        test_cases.emplace_back("abbc", 2);
        test_cases.emplace_back("abc", 2);
        test_cases.emplace_back("baba", 1);
        test_cases.emplace_back("borodaq", 4);

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
        const auto fast = Standard::Algorithms::Numbers::string_cut_into_min_palindromes(str);

        if (test.expected() != not_computed)
        {
            ::Standard::Algorithms::ert::are_equal(test.expected(), fast, "string_cut_into_min_palindromes");
        }

        const auto slow = Standard::Algorithms::Numbers::string_cut_into_min_palindromes_slow(str);

        ::Standard::Algorithms::ert::are_equal(fast, slow, "string_cut_into_min_palindromes_slow");
    }
} // namespace

void Standard::Algorithms::Numbers::Tests::string_cut_into_min_palindromes_tests()
{
    Standard::Algorithms::Tests::test_utilities<test_case>::run_tests(run_test_case, generate_test_cases);
}
