#include<unordered_set>
#include "../Utilities/Random.h"
#include "longest_palindromic_substring_manacher_tests.h"
#include "longest_palindromic_substring_manacher.h"
#include "../Strings/ukkonen_suffix_tree_utilities.h"
#include "../test_utilities.h"
#include "../Utilities/PrintUtilities.h"

using namespace std;
using namespace Privet::Algorithms::Numbers::Tests;
using namespace Privet::Algorithms::Numbers;
using namespace Privet::Algorithms::Strings;
using namespace Privet::Algorithms::Tests;
using namespace Privet::Algorithms;

namespace
{
    using pair_t = pair<int, int>;

    struct test_case final : base_test_case
    {
        const pair_t expected;

        test_case(string&& name,
            int from, int to)
            : base_test_case(forward<string>(name)),
            expected(from, to)
        {
        }

        void Validate() const override
        {
            base_test_case::Validate();

            if (!expected.second)
                return;

            RequireNonNegative(expected.first, "expected");
            RequireGreater(expected.second, expected.first, "expected");

            const auto& s = get_Name();
            const auto size = static_cast<int>(s.size());
            RequireNotGreater(expected.second, size, "expected");
        }

        void Print(ostream& str) const override
        {
            base_test_case::Print(str);
            PrintValue(str, "expected", expected);
        }
    };

    void generate_test_cases(vector<test_case>& test_cases)
    {
        test_cases.emplace_back("abcbb", 1, 4);
        test_cases.emplace_back("abcb", 1, 4);
        test_cases.emplace_back("abcbbce", 2, 6);
        test_cases.emplace_back("ababa", 0, 5);
        test_cases.emplace_back("a", 0, 1);
        test_cases.emplace_back("aa", 0, 2);
        test_cases.emplace_back("ab", 0, 1);
        test_cases.emplace_back("aba", 0, 3);

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

            test_cases.emplace_back(move(s), 0, 0);
        }
    }

    template<class name_t, class string_t, class pair_t, class pair_t2>
    void compare_palindromic_substrings(const name_t& name, const string_t& s, const pair_t& p, const pair_t2& p2)
    {
        if (p == p2)
            return;

        const auto same_length = p.second - p.first == p2.second - p2.first;
        if (same_length)
        {
            const auto valid_positions = p.first >= 0 && p.first < p.second&&
                p.second <= static_cast<int>(s.size()) &&
                p2.first >= 0 && p2.first < p2.second&&
                p2.second <= static_cast<int>(s.size());

            if (valid_positions && is_palindrome(&s[p.first], p.second - p.first) &&
                is_palindrome(&s[p2.first], p2.second - p2.first))
                return;
        }

        Assert::AreEqual(p, p2, name);
    }

    void run_test_case(const test_case& test_case)
    {
        const auto& s = test_case.get_Name();
        const auto actual = longest_palindromic_substring_manacher<pair_t>(s);
        if (test_case.expected.second)
            compare_palindromic_substrings("longest_palindromic_substring_manacher",
                s, test_case.expected, actual);

        const auto slow = longest_palindromic_substring_slow<pair_t>(s);
        compare_palindromic_substrings("longest_palindromic_substring_slow", s, actual, slow);

        //constexpr auto uniq = '~';
        //const auto p = longest_palindromic_substring<int>(s + uniq);
        //compare_palindromic_substrings("longest_palindromic_substring", s, actual, p);
    }
}

void Privet::Algorithms::Numbers::Tests::longest_palindromic_substring_manacher_tests()
{
    test_utilities<test_case>::run_tests(run_test_case, generate_test_cases);
}