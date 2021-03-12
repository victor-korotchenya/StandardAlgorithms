#include"regular_expression_tests.h"
#include"../Utilities/random.h"
#include"../Utilities/test_utilities.h"
#include"regular_expression.h"
#include<vector>

namespace // fix_wildcards
{
    inline constexpr auto question = Standard::Algorithms::Strings::question;

    inline constexpr auto star = Standard::Algorithms::Strings::star;

    constexpr void fix_wildcards(std::string &str) noexcept
    {
        for (auto &cha : str)
        {
            if (cha <= '2')
            {
                cha = star;
            }
            else if (cha <= '5')
            {
                cha = question;
            }
        }
    }

    [[nodiscard]] constexpr auto set_wildcards_tests() -> bool
    {
        const auto *const source = "Z012456789a3";

        std::string str{ source };
        fix_wildcards(str);

        {
            const auto *const expected = "Z***??6789a?";

            ::Standard::Algorithms::ert::are_equal(expected, str, "fix_wildcards");
        }

        return true;
    }
} // namespace

namespace
{
    static_assert(std::is_same<Standard::Algorithms::Strings::pattern_text_view<std::string, std::int32_t>,
        decltype([]{
            // Cannot be const - must be movable.
            // NOLINTNEXTLINE
            Standard::Algorithms::Strings::pattern_text_view value { std::string{}, std::int32_t{} };
            return value;
            }())>(),
        "Deduction guide test.");

    constexpr auto not_computed = -1;

    struct test_case final : Standard::Algorithms::Tests::base_test_case
    {
        constexpr test_case(
            std::string &&pattern, std::string &&text, std::int32_t expected, std::string &&name) noexcept
            : base_test_case(std::move(name))
            , Pattern(std::move(pattern))
            , Text(std::move(text))
            , Expected(expected)
        {
        }

        constexpr test_case(std::string &&pattern, std::string &&text, bool expected, std::string &&name = {}) noexcept
            : test_case(std::move(pattern), std::move(text), expected ? 1 : 0, std::move(name))
        {
        }

        [[nodiscard]] constexpr auto pattern() const &noexcept -> const std::string &
        {
            return Pattern;
        }

        [[nodiscard]] constexpr auto text() const &noexcept -> const std::string &
        {
            return Text;
        }

        [[nodiscard]] constexpr auto expected() const noexcept -> std::int32_t
        {
            return Expected;
        }

        void print(std::ostream &str) const override
        {
            str << " pattern '" << Pattern << "', text '" << Text << "', expected " << Expected;
        }

private:
        std::string Pattern;
        std::string Text;
        std::int32_t Expected;
    };

    constexpr void generate_test_cases_plain(std::vector<test_case> &tests)
    {
        // todo(p3): review, then del duplicate tests.
        {
            const std::string mid = "12?*";
            auto words = std::string().append(mid).append(mid).append(mid).append(mid);

            tests.emplace_back("*3" + words, "4" + words, false, "missing 3");
        }

        tests.emplace_back("*7?6?*", "?17676??", false);

        tests.emplace_back("*??1?1*", "*?122*11*1", true);
        tests.emplace_back("??*1?1*", "*?122*11*1", true);
        tests.emplace_back("*1?1*", "122*11*1", true);
        tests.emplace_back("*1?1*", "22*11*1", true);
        tests.emplace_back("*1?1*", "22*111", true);
        tests.emplace_back("*1?1*", "22*1*1", true);

        {
            const std::string pat = "*b*a*d* *l*u*c*k*";
            const std::string tex = "bad luck";

            const std::string sep1 = "1";
            const std::string sep2 = "2";
            const std::string sep3 = "3";
            const std::string sep4 = "4";

            tests.emplace_back(std::move(std::string()
                                             .append(pat)
                                             .append(sep1)
                                             .append(pat)
                                             .append(sep2)
                                             .append(pat)
                                             .append(sep3)
                                             .append(pat)
                                             .append(sep4)
                                             .append(pat)),
                std::move(std::string()
                              .append(tex)
                              .append(sep1)
                              .append(tex)
                              .append(sep2)
                              .append(tex)
                              .append(sep3)
                              .append(tex)
                              .append(sep4)
                              .append(tex)),
                true);

            tests.emplace_back(
                std::move(
                    std::string().append(pat).append(sep1).append(pat).append(sep2).append(pat).append(sep3).append(
                        pat)),
                std::move(
                    std::string().append(tex).append(sep1).append(tex).append(sep2).append(tex).append(sep3).append(
                        tex)),
                true);

            tests.emplace_back(std::move(std::string().append(pat).append(sep1).append(pat).append(sep2).append(pat)),
                std::move(std::string().append(tex).append(sep1).append(tex).append(sep2).append(tex)), true);

            tests.emplace_back(std::move(std::string().append(pat).append(sep1).append(pat)),
                std::move(std::string().append(tex).append(sep1).append(tex)), true);

            tests.emplace_back(pat + "", tex + "", true);
        }

        tests.emplace_back("1?2", "12", false);
        tests.emplace_back("1?2", "1a2", true);
        tests.emplace_back("*1?2*", "1a2", true);

        tests.emplace_back("1?2?", "12", false);
        tests.emplace_back("1?2?", "111", false);
        tests.emplace_back("1?2?", "112", false);
        tests.emplace_back("1?2?", "121", false);
        tests.emplace_back("1?2?", "122", false);
        tests.emplace_back("1?2?", "1212", false);
        tests.emplace_back("1?2?", "1a2b", true);

        tests.emplace_back("1?2?", "1a2b", true);
        tests.emplace_back("*1?2?*", "1a2b", true);

        tests.emplace_back("a", "", false);
        tests.emplace_back("", "a", false);
        tests.emplace_back("a", "b", false);
        tests.emplace_back("?", "a", true);
        tests.emplace_back("a", "a", true);
        tests.emplace_back("a", "?", false);
        tests.emplace_back("a", "A", false);
        tests.emplace_back("A", "a", false);

        tests.emplace_back("b", "abc", false);
        tests.emplace_back("b*", "abc", false);
        tests.emplace_back("*b", "abc", false);
        tests.emplace_back("Abc", "abc", false, "Case-sensitive");
        tests.emplace_back("*b*", "abc", true);
        tests.emplace_back("ab*", "abc", true);
        tests.emplace_back("*bc", "abc", true);
        tests.emplace_back("abc", "abc", true);

        tests.emplace_back("", "", true);
        tests.emplace_back("*", "", true);
        tests.emplace_back("**", "", true);
        tests.emplace_back("***", "", true);
        tests.emplace_back("?", "", false);
        tests.emplace_back("*?", "", false);
        tests.emplace_back("?*", "", false);
        tests.emplace_back("*?*", "", false);
        tests.emplace_back("", "Snowflake", false);
        tests.emplace_back("", "snow? ball", false);

        tests.emplace_back("*?6*", "86?587", true);
        tests.emplace_back("*?6*", "6?587", false);

        tests.emplace_back("a", "aa", false);
        tests.emplace_back("a", "aaa", false);
        tests.emplace_back("aa", "aaa", false);

        tests.emplace_back("gazgaz", "gaz", false);
        tests.emplace_back("gaz*gaz", "gaz", false);
        tests.emplace_back("gaz*gaz", "gazga", false);
        tests.emplace_back("gaz*gaz", "gazaz", false);
        tests.emplace_back("gaz***gaz", "gazz", false);
        tests.emplace_back("gaz***gaz", "gazgaz", true);
        tests.emplace_back("gaz**gaz", "gazgaz", true);
        tests.emplace_back("gaz*gaz", "gazgaz", true);
        tests.emplace_back("gazgaz", "gazgaz", true);

        // Too long pattern/text.
        tests.emplace_back("?7123*", "*7", false);
        tests.emplace_back("a**??**mid*??*?*c", "ab??c", false);
        tests.emplace_back("*2?*?****a", "2 a", false);
        tests.emplace_back("*2?*?**??**a", "2 a", false);
        tests.emplace_back("1*?2?*?**a", "1 2 a", false);
        tests.emplace_back("??1***??*?**a", "no1 a", false);
        tests.emplace_back("n*?*1***?*?**a", "no1 a", false);
        tests.emplace_back("*?*?*?", "no", false);

        tests.emplace_back("a**??****c", "ab???***c", true);
        tests.emplace_back("a**??**mid**c", "ab???*mid*5c", true);

        tests.emplace_back("Sam", "Sam", true);
        tests.emplace_back("Sam", "Sam auz Uganda", false);
        tests.emplace_back("Sam auz Uganda", "Sama", false);

        tests.emplace_back("****", "", true);
        tests.emplace_back("**a*", "", false);
        tests.emplace_back("***?*", "", false);
        tests.emplace_back("*", "some thing", true);

        tests.emplace_back("*?*?*?", "net", true);
        tests.emplace_back("*?*?*?", "nett", true);
        tests.emplace_back("*?*?*?", "new a", true);

        tests.emplace_back("*?*a?*?f*", "new ab free Apache", true);
        tests.emplace_back("*?***a?***?f****", "new ab free Apache", true);
        tests.emplace_back("*?*a?*?f*", "anew A  fork ", false);
        tests.emplace_back("*?*a?*?f*", "new anew Apache   ", false);
        tests.emplace_back("*?*a?*?f*", "new ton a free", false);
        tests.emplace_back("**?*a?****?f***", "new ton a free", false);

        tests.emplace_back("*", "**", true);
        tests.emplace_back("?*", "**", true);
        tests.emplace_back("?*", "**?", true);
        tests.emplace_back("**", "*", true);
        tests.emplace_back("*?*", "*", true);
        tests.emplace_back("*??*", "*", false);
        tests.emplace_back("**", "***", true);
        tests.emplace_back("*?*", "***?", true);
        tests.emplace_back("*?*", "****?", true);
        tests.emplace_back("****", "***", true);
        tests.emplace_back("**?**", "***?", true);
        tests.emplace_back("**??**", "***?", true);
        tests.emplace_back("**??**", "???***", true);

        tests.emplace_back("a**??**ce", "ab???***cd", false);
        tests.emplace_back("a**??**qd", "ab???***cd", false);
        tests.emplace_back("a**??**cd", "ab???***cd", true);
        tests.emplace_back("a**??**cd*", "ab???***cd", true);
        tests.emplace_back("a**??**cd", "ab???***cd*", false);

        tests.emplace_back("*a*?b*?c?*d*e?f*g?h*", "*a*?b*?c?*d*e?f*g?h*", true);
        tests.emplace_back("*a*?b*?c?*d*e?f*g?h*", "a?b?c?de?fg?h", true);
        tests.emplace_back("*a*?b*?c?*d*e?f*g?h*", "a?b?c?di?fg?h", false);

        for (std::size_t index{}; index < tests.size(); ++index)
        {
            auto &test = tests[index];

            if (test.name().empty())
            {
                test.name("plain" + std::to_string(index));
            }
        }
    }

    void generate_test_cases_random(std::vector<test_case> &tests)
    {
        Standard::Algorithms::Utilities::random_t<std::int32_t> rnd{};

        constexpr auto max_attempts = 1;

        for (std::int32_t att{}; att < max_attempts; ++att)
        {
            constexpr auto min_size = 0;
            constexpr auto max_size = 10;

            auto pattern = Standard::Algorithms::Utilities::random_string(rnd, min_size, max_size);

            auto text = Standard::Algorithms::Utilities::random_string(rnd, min_size, max_size);

            fix_wildcards(pattern);
            fix_wildcards(text);

            tests.emplace_back(std::move(pattern), std::move(text), not_computed, "random" + std::to_string(att));
        }
    }

    void generate_test_cases(std::vector<test_case> &tests)
    {
        generate_test_cases_plain(tests);
        generate_test_cases_random(tests);
    }

    void run_test_case(const test_case &test)
    {
        const Standard::Algorithms::Strings::pattern_text_view<const std::string_view, const std::string_view> pat_text{
            test.pattern(), test.text()
        };

        const auto fast = Standard::Algorithms::Strings::is_wildcard_match_naive(pat_text);

        if (const auto &expected = test.expected(); 0 <= expected)
        {
            ::Standard::Algorithms::ert::are_equal(0 < expected, fast, "is_wildcard_match_naive");
        }

        {
            const auto still = Standard::Algorithms::Strings::is_wildcard_match_slow_still(pat_text);

            ::Standard::Algorithms::ert::are_equal(fast, still, "is_wildcard_match_slow_still");
        }
        {
            const auto slow = Standard::Algorithms::Strings::is_wildcard_match_slow(pat_text);

            ::Standard::Algorithms::ert::are_equal(fast, slow, "is_wildcard_match_slow");
        }
    }
} // namespace

void Standard::Algorithms::Strings::Tests::regular_expression_tests()
{
    static_assert(set_wildcards_tests());

    Standard::Algorithms::Tests::test_utilities<test_case>::run_tests(run_test_case, generate_test_cases);
}
