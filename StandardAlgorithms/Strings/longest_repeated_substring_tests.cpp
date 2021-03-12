#include <vector>
#include "ukkonen_suffix_tree_utilities.h"
#include "longest_repeated_substring_tests.h"
#include "../test_utilities.h"
#include "../Utilities/Random.h"
#include "../Utilities/PrintUtilities.h"

using namespace std;
using namespace Privet::Algorithms::Tests;
using namespace Privet::Algorithms::Strings::Tests;
using namespace Privet::Algorithms::Strings;
using namespace Privet::Algorithms;

namespace
{
    struct test_case final : base_test_case
    {
        const pair<int, int> expected;

        test_case(string&& name,
            int start, int stop)
            : base_test_case(forward<string>(name)),
            expected({ start, stop })
        {
        }

        void Validate() const override
        {
            base_test_case::Validate();
            const auto& name = get_Name();
            test_check_pair(name, expected, name.size());
        }

        void Print(ostream& str) const override
        {
            base_test_case::Print(str);
            PrintValue(str, "expected", expected);
        }
    };

    void generate_test_cases(vector<test_case>& tests)
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

        IntRandom rnd;
        unordered_set<string> uniq;

        for (auto att = 0, att_max = 1; att < att_max; ++att)
        {
            constexpr auto length_max = 10;
            constexpr auto unique_last = '#';
            const auto a = random_string(rnd, 1, length_max);
            if (uniq.insert(a).second)
                tests.emplace_back(a + unique_last, 0, 0);
        }
    }

    pair<int, int> longest_repeated_substring_slow(const string& text)
    {
        const auto size = static_cast<int>(text.size());
        assert(size > 0);

        for (auto len = size - 1; len > 0; --len)
        {
            for (auto start = 0; start < size - len; ++start)
            {
                const auto subs = text.substr(start, len);
                const auto it = text.find(subs, start + 1);
                if (it != string::npos)
                    return { start, start + len };
            }
        }

        return { 0, 0 };
    }

    template<class string_t, class string_t2, class pair_t, class pair_t2>
    void RequireSubstringsEqual(const string_t& name, const string_t2& text, const pair_t& p, const pair_t2& p2)
    {
        if (p == p2)
            return;

        if (p.second - p.first == p2.second - p2.first)
        {// "aabbc" can return "aa" and "bb".
            const auto sub1 = text.substr(p.first, p.second - p.first);
            const auto count1 = find_count_max2(text, sub1);

            const auto sub2 = text.substr(p2.first, p2.second - p2.first);
            const auto count2 = find_count_max2(text, sub2);
            if ((2 == count1) == (2 == count2))
                return;
        }

        Assert::AreEqual(p, p2, name);
    }

    void run_test_case(const test_case& test)
    {
        const auto& name = test.get_Name();
        const auto expected = longest_repeated_substring(name);
        if (test.expected.second)
            RequireSubstringsEqual("longest_repeated_substring", name, test.expected, expected);

        const auto slow = longest_repeated_substring_slow(name);
        RequireSubstringsEqual("longest_repeated_substring_slow", name, expected, slow);
    }
}

namespace Privet
{
    namespace Algorithms
    {
        namespace Strings
        {
            namespace Tests
            {
                void longest_repeated_substring_tests()
                {
                    test_utilities<test_case>::run_tests(run_test_case, generate_test_cases);
                }
            }
        }
    }
}