#include <unordered_set>
#include "../Utilities/Random.h"
#include "string_cut_into_min_palindromes_tests.h"
#include "string_cut_into_min_palindromes.h"
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

            if (expected != not_computed)
                RequireNonNegative(expected, "expected");
        }

        void Print(ostream& str) const override
        {
            base_test_case::Print(str);
            PrintValue(str, "expected", expected);
        }
    };

    void generate_test_cases(vector<test_case>& test_cases)
    {
        test_cases.emplace_back("abc", 2);
        test_cases.emplace_back("aaaaac", 1);
        test_cases.emplace_back("abbaca", 2);
        test_cases.emplace_back("abbc", 2);
        test_cases.emplace_back("ababbbabbababa", 3);
        test_cases.emplace_back("a", 0);
        test_cases.emplace_back("aa", 0);
        test_cases.emplace_back("aaa", 0);
        test_cases.emplace_back("aba", 0);
        test_cases.emplace_back("ab", 1);
        test_cases.emplace_back("abb", 1);

        IntRandom r;
        unordered_set<string> uniq;
        string s;

        for (auto att = 0, att_max = 1; att < att_max; ++att)
        {
            constexpr auto size_max = 10;
            const auto n = r(1, size_max);
            s.resize(n);

            for (auto i = 0; i < n; ++i) s[i] = static_cast<char>(r('0', '9'));

            if (uniq.insert(s).second)
                test_cases.emplace_back(move(s), not_computed);
        }
    }

    void run_test_case(const test_case& test_case)
    {
        const auto& s = test_case.get_Name();
        const auto actual = string_cut_into_min_palindromes(s);
        if (test_case.expected != not_computed)
            Assert::AreEqual(test_case.expected, actual, "string_cut_into_min_palindromes");

        const auto slow = string_cut_into_min_palindromes_slow(s);
        Assert::AreEqual(actual, slow, "string_cut_into_min_palindromes_slow");
    }
}

void Privet::Algorithms::Numbers::Tests::string_cut_into_min_palindromes_tests()
{
    test_utilities<test_case>::run_tests(run_test_case, generate_test_cases);
}