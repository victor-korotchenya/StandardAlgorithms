#include"longest_repeated_substring_tests.h"
#include"../Utilities/random.h"
#include"../Utilities/test_utilities.h"
#include"longest_repeated_substring.h"
#include"ukkonen_suffix_tree_utilities.h"
#include<vector>

namespace
{
    struct test_case final : Standard::Algorithms::Tests::base_test_case
    {
        constexpr test_case(std::string &&name, std::int32_t start, std::int32_t stop) noexcept
            : base_test_case(std::move(name))
            , Expected({ start, stop })
        {
        }

        [[nodiscard]] constexpr auto expected() const noexcept -> std::pair<std::int32_t, std::int32_t>
        {
            return Expected;
        }

        constexpr void validate() const override
        {
            base_test_case::validate();

            const auto &nam = name();
            Standard::Algorithms::Strings::test_check_pair(nam, Expected, nam.size());
        }

        void print(std::ostream &str) const override
        {
            ::Standard::Algorithms::print_value("Expected", str, Expected);
        }

private:
        std::pair<std::int32_t, std::int32_t> Expected;
    };

    void generate_test_cases(std::vector<test_case> &tests)
    {
        tests.emplace_back("abcabx", 0, 2);
        tests.emplace_back(" base1bas _", 1, 4);
        tests.emplace_back(" aaa bbd", 1, 3);
        tests.emplace_back("aaa ", 0, 2);
        tests.emplace_back("aab", 0, 1);
        tests.emplace_back("eabcdaw", 1, 2);
        tests.emplace_back("abcd", 0, 0);
        tests.emplace_back("abc", 0, 0);
        tests.emplace_back("ab", 0, 0);
        tests.emplace_back("a", 0, 0);
        tests.emplace_back("base1", 0, 0);

        Standard::Algorithms::Utilities::random_t<std::int32_t> rnd{};
        std::unordered_set<std::string> uniq{ "" };

        constexpr auto max_attempts = 1;

        for (std::int32_t att{}; att < max_attempts; ++att)
        {
            constexpr auto min_size = 1;
            constexpr auto max_size = 10;

            auto str = Standard::Algorithms::Utilities::random_string_unique(rnd, uniq, min_size, max_size);

            if (str.empty())
            {
                continue;
            }

            constexpr auto last_unique_char = '#';

            str += last_unique_char;

            tests.emplace_back(std::move(str), 0, 0);
        }
    }

    constexpr void require_substrings_equal(
        const std::string &name, const std::string_view text, const auto &par, const auto &par2)
    {
        assert(!name.empty());

        if (par == par2)
        {
            return;
        }

        if (par.second - par.first == par2.second - par2.first)
        {// "aabbc" can return "aa" and "bb".
            const auto sub1 = text.substr(par.first, par.second - par.first);
            const auto count1 = Standard::Algorithms::Strings::find_count_max2(sub1, text);

            const auto sub2 = text.substr(par2.first, par2.second - par2.first);
            const auto count2 = Standard::Algorithms::Strings::find_count_max2(sub2, text);

            if ((2 == count1) == (2 == count2))
            {
                return;
            }
        }

        ::Standard::Algorithms::ert::are_equal(par, par2, name);
    }

    void run_test_case(const test_case &test)
    {
        const auto &name = test.name();
        const auto fast = Standard::Algorithms::Strings::suffix_tree_longest_repeated_substring(name);

        if (const auto &expected = test.expected(); expected.second != 0)
        {
            require_substrings_equal("suffix_tree_longest_repeated_substring", name, expected, fast);
        }

        {
            const auto actual = Standard::Algorithms::Strings::lcp_longest_repeated_substring(name);

            require_substrings_equal("lcp_longest_repeated_substring", name, fast, actual);
        }
        {
            const auto actual = Standard::Algorithms::Strings::longest_repeated_substring_slow(name);

            require_substrings_equal("longest_repeated_substring_slow", name, fast, actual);
        }
    }
} // namespace

void Standard::Algorithms::Strings::Tests::longest_repeated_substring_tests()
{
    Standard::Algorithms::Tests::test_utilities<test_case>::run_tests(run_test_case, generate_test_cases);
}
