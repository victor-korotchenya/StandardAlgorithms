#include<unordered_set>
#include "../Utilities/Random.h"
#include "longest_subsequence_palindrome_tests.h"
#include "longest_subsequence_palindrome.h"
#include "../test_utilities.h"
#include "../Utilities/PrintUtilities.h"

using namespace std;
using namespace Privet::Algorithms::Numbers::Tests;
using namespace Privet::Algorithms::Numbers;
using namespace Privet::Algorithms::Tests;
using namespace Privet::Algorithms;

namespace
{
    constexpr auto not_computed = -1;

    struct test_case final : base_test_case
    {
        const int expected;

        test_case(string&& name,
            int expected)
            : base_test_case(forward<string>(name)),
            expected(expected)
        {
        }

        void Validate() const override
        {
            base_test_case::Validate();

            if (expected == not_computed)
                return;

            RequirePositive(expected, "expected");

            const auto& s = get_Name();
            const auto size = static_cast<int>(s.size());
            RequireNotGreater(expected, size, "expected");
        }

        void Print(ostream& str) const override
        {
            base_test_case::Print(str);
            PrintValue(str, "expected", expected);
        }
    };

    void generate_test_cases(vector<test_case>& test_cases)
    {
        test_cases.emplace_back("abcbb", 3);
        test_cases.emplace_back("abcb", 3);
        test_cases.emplace_back("abcbbc", 4);
        test_cases.emplace_back("abab", 3);
        test_cases.emplace_back("baba", 3);
        test_cases.emplace_back("ababa", 5);
        test_cases.emplace_back("ad oba da", 7);
        test_cases.emplace_back("ad ob da", 7);
        test_cases.emplace_back("ad  da", 6);
        test_cases.emplace_back("ad da", 5);
        test_cases.emplace_back("adda", 4);
        test_cases.emplace_back("base1", 1);
        test_cases.emplace_back("base11", 2);
        test_cases.emplace_back("1bas1e", 3);
        test_cases.emplace_back("abba", 4);
        test_cases.emplace_back("abbc", 2);
        test_cases.emplace_back("abob", 3);

        IntRandom r;
        string s;
        unordered_set<string> uniq;

        for (auto att = 0, att_max = 1; att < att_max; ++att)
        {
            constexpr auto size_max = 30;
            const auto n = r(1, size_max);
            s.resize(n);

            for (;;)
            {
                for (auto i = 0; i < n; ++i) s[i] = static_cast<char>(r('A', 'Z'));

                if (uniq.insert(s).second)
                    break;
            }

            test_cases.emplace_back(move(s), not_computed);
        }
    }

    void run_test_case(const test_case& test_case)
    {
        const auto& s = test_case.get_Name();
        const auto actual = longest_subsequence_palindrome_they(s);
        if (test_case.expected != not_computed)
            Assert::AreEqual(test_case.expected, actual, "longest_subsequence_palindrome");

        const auto almost = longest_subsequence_palindrome_almost(s);
        Assert::AreEqual(actual, almost, "longest_subsequence_palindrome_almost");

        if (s.size() > 10)
            return;

        const auto slow = longest_subsequence_palindrome_slow(s);
        Assert::AreEqual(actual, slow, "longest_subsequence_palindrome_slow");
    }
}

void Privet::Algorithms::Numbers::Tests::longest_subsequence_palindrome_tests()
{
    test_utilities<test_case>::run_tests(run_test_case, generate_test_cases);
}