#include"longest_palindromic_factor_manacher_tests.h"
#include"../Utilities/random.h"
#include"../Utilities/test_utilities.h"
#include"longest_palindromic_factor_manacher.h"
#include"ukkonen_suffix_tree_utilities.h"
#include<unordered_set>

namespace
{
    using pair_t = std::pair<std::size_t, std::size_t>;

    struct test_case final : Standard::Algorithms::Tests::base_test_case
    {
        constexpr test_case(std::string &&name, pair_t &&expected) noexcept
            : base_test_case(std::move(name))
            , Expected(std::move(expected))
        {
        }

        [[nodiscard]] constexpr auto expected() const &noexcept -> const pair_t &
        {
            return Expected;
        }

        [[nodiscard]] constexpr auto is_computed() const noexcept -> bool
        {
            return 0U < Expected.second;
        }

        constexpr void validate() const override
        {
            base_test_case::validate();

            if (!is_computed())
            {
                return;
            }

            Standard::Algorithms::require_greater(Expected.second, Expected.first, "stop");

            const auto size = name().size();
            Standard::Algorithms::require_less_equal(Expected.second, size, "stop");
        }

        void print(std::ostream &str) const override
        {
            ::Standard::Algorithms::print_value("expected", str, Expected);
        }

private:
        pair_t Expected;
    };

    void generate_test_cases(std::vector<test_case> &test_cases)
    {
        test_cases.emplace_back("abcbb", // NOLINTNEXTLINE
            pair_t{ 1, 4 });

        test_cases.emplace_back("abcb", // NOLINTNEXTLINE
            pair_t{ 1, 4 });

        test_cases.emplace_back("abcbbce", // NOLINTNEXTLINE
            pair_t{ 2, 6 });

        test_cases.emplace_back("ababa", // NOLINTNEXTLINE
            pair_t{ 0, 5 });

        test_cases.emplace_back("a", pair_t{ 0, 1 });
        test_cases.emplace_back("aa", pair_t{ 0, 2 });
        test_cases.emplace_back("ab", pair_t{ 0, 1 });
        test_cases.emplace_back("aba", // NOLINTNEXTLINE
            pair_t{ 0, 3 });

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
            constexpr auto max_size = 40;

            auto str = Standard::Algorithms::Utilities::random_string_unique(rnd, uniq, min_size, max_size);

            if (str.empty())
            {
                continue;
            }

            test_cases.emplace_back(std::move(str), pair_t{});
        }
    }

    constexpr void compare_palindromic_substrings(
        const std::string &name, const pair_t &expected, const std::string &str, const pair_t &actual)
    {
        if (expected == actual)
        {
            return;
        }

        auto good = expected.first < expected.second && expected.second <= str.size() && actual.first < actual.second &&
            actual.second <= str.size();
        if (good)
        {
            good = expected.second - expected.first == actual.second - actual.first;
        }

        if (good)
        {
            good = Standard::Algorithms::Strings::is_palindrome(&str[expected.first], expected.second - expected.first);
        }

        if (good)
        {
            good = Standard::Algorithms::Strings::is_palindrome(&str[actual.first], actual.second - actual.first);
        }

        if (good)
        {
            return;
        }

        ::Standard::Algorithms::ert::are_equal(expected, actual, name);
    }

    constexpr void run_test_case(const test_case &test)
    {
        const auto &str = test.name();
        const auto fast = Standard::Algorithms::Strings::longest_palindromic_factor_manacher(str);

        if (test.is_computed())
        {
            compare_palindromic_substrings("longest_palindromic_factor_manacher", test.expected(), str, fast);
        }

        const auto slow = Standard::Algorithms::Strings::longest_palindromic_factor_slow(str);

        compare_palindromic_substrings("longest_palindromic_factor_slow", fast, str, slow);

        // constexpr auto last_unique_char = '~';
        //
        //  todo(p2): finish // const auto actual = longest_palindromic_factor<std::int32_t>(str + last_unique_char);
        // compare_palindromic_substrings("longest_palindromic_factor", fast, str, actual);
    }
} // namespace

void Standard::Algorithms::Strings::Tests::longest_palindromic_factor_manacher_tests()
{
    Standard::Algorithms::Tests::test_utilities<test_case>::run_tests(run_test_case, generate_test_cases);
}
