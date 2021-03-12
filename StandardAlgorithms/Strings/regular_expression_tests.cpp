#include <vector>
#include "../Utilities/IteratorUtilities.h"
#include "../Utilities/Random.h"
#include "../test_utilities.h"
#include "regular_expression.h"
#include "regular_expression_tests.h"

using namespace std;
using namespace Privet::Algorithms::Strings::Tests;
using namespace Privet::Algorithms::Strings;
using namespace Privet::Algorithms::Tests;
using namespace Privet::Algorithms;

namespace
{
    void set_wildcards(string& s)
    {
        for (auto& c : s)
            if (c <= '2')
                c = '*';
            else if (c <= '5')
                c = '?';
    }

    void set_wildcards_tests()
    {
        string s = "Z0123456789a";
        set_wildcards(s);
        Assert::AreEqual("Z***???6789a"s, s, "set_wildcards");
    }

    constexpr auto not_computed = -1;

    struct test_case final : base_test_case
    {
        string pattern, text;
        signed char expected;

        test_case(string&& pattern = {},
            string&& text = {},
            int expected = not_computed)
            : base_test_case(""),
            pattern(forward<string>(pattern)),
            text(forward<string>(text)),
            expected(static_cast<signed char>(expected))
        {
        }

        void Print(ostream& str) const override
        {
            base_test_case::Print(str);

            str << " pattern='" << pattern
                << "', text='" << text
                << "', expected=" << static_cast<int>(expected);
        }
    };

    void generate_test_cases_plain(vector<test_case>& tests)
    {
        tests.emplace_back("*?6*", "86?587", true);

        //Too long pattern/text
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

        tests.emplace_back("same", "same", true);
        tests.emplace_back("same", "same not", false);
        tests.emplace_back("not same", "same", false);

        // empty
        tests.emplace_back("", "", true);
        tests.emplace_back("", "Snowflake", true);
        tests.emplace_back("", "snow? ball", true);

        tests.emplace_back("*", "", true);
        tests.emplace_back("*", "some thing", true);
        tests.emplace_back("****", "", true);
        tests.emplace_back("**a*", "", false);
        tests.emplace_back("***?*", "", false);

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
        tests.emplace_back("*a*?b*?c?*d*e?f*g?h*", "*a*?b*?c?*d*e?f*g?h*", true);
        tests.emplace_back("*a*?b*?c?*d*e?f*g?h*", "a?b?c?de?fg?h", true);
        tests.emplace_back("*a*?b*?c?*d*e?f*g?h*", "a?b?c?di?fg?h", false);

        for (size_t i = 0; i < tests.size(); ++i)
            tests[i].set_Name(to_string(i));
    }

    void generate_test_cases_random(vector<test_case>& tests)
    {
        IntRandom r;
        for (auto att = 0, att_max = 1; att < att_max; ++att)
        {
            constexpr auto min_size = 0, max_size = 20;

            auto pattern = random_string(r, min_size, max_size),
                text = random_string(r, min_size, max_size);

            set_wildcards(pattern);
            set_wildcards(text);

            tests.emplace_back(move(pattern), move(text));
            tests.back().set_Name("random" + to_string(att));
        }
    }

    void generate_test_cases(vector<test_case>& tests)
    {
        generate_test_cases_plain(tests);
        generate_test_cases_random(tests);
    }

    void run_test_case(const test_case& test)
    {
        const auto actual = is_wildcard_match_slow(test.pattern, test.text);
        if (test.expected >= 0)
            Assert::AreEqual(test.expected > 0, actual, "is_wildcard_match");

        const auto still = is_wildcard_match_slow_still(test.pattern, test.text);
        Assert::AreEqual(actual, still, "is_wildcard_match_slow_still");

        //const auto slow = is_wildcard_match_slow(test.pattern, test.text);
        //Assert::AreEqual(actual, slow, "is_wildcard_match_slow");
    }

    void wildcard_tests()
    {
        test_utilities<test_case>::run_tests(run_test_case, generate_test_cases);
    }
}

void Privet::Algorithms::Strings::Tests::regular_expression_tests()
{
    wildcard_tests();
    set_wildcards_tests();
}