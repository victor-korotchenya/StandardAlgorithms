#include <vector>
#include "longest_common_substring.h"
#include "longest_common_substring_tests.h"
#include "ukkonen_suffix_tree_utilities.h"
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
    using p_t = pair<int, int>;
    using pp_t = pair<p_t, p_t>;
    constexpr auto not_computed = -1;

    struct test_case final : base_test_case
    {
        const string a, b;
        const pp_t expected;

        test_case(string&& name,
            string&& a,
            string&& b,
            int start0 = not_computed, int stop0 = not_computed,
            int start1 = not_computed, int stop1 = not_computed)
            : base_test_case(forward<string>(name)),
            a(forward<string>(a)),
            b(forward<string>(b)),
            expected({ {start0, stop0}, {start1, stop1} })
        {
        }

        void Validate() const override
        {
            base_test_case::Validate();

            const auto& p = expected.first, & p2 = expected.second;
            if (p.first == not_computed)
                return;

            test_check_pair("a", p, a.size());
            test_check_pair("b", p2, b.size());

            Assert::AreEqual(p2.second - p2.first, p.second - p.first, "lengths");
        }

        void Print(ostream& str) const override
        {
            base_test_case::Print(str);
            ::PrintValue(str, "a", a);
            ::PrintValue(str, "b", b);
            ::PrintValue(str, "expected", expected);
        }
    };

    void generate_test_cases(vector<test_case>& tests)
    {
        tests.push_back({ "ab ca", "ab", "ca", 0,1,1,2 });
        tests.push_back({ "base1", "basep1ba", "epd9bsep1qd", 2,6,5,9 });
        tests.push_back({ "empty", "", "", 0,0,0,0 });
        tests.push_back({ "a e", "a", "", 0,0,0,0 });
        tests.push_back({ "e a", "", "a", 0,0,0,0 });
        tests.push_back({ "a", "a", "a", 0,1,0,1 });

        IntRandom rnd;
        for (auto att = 0, att_max = 1; att < att_max; ++att)
        {
            constexpr auto length_max = 10;

            tests.emplace_back("random" + to_string(att), random_string(rnd, 1, length_max), random_string(rnd, 1, length_max));
        }
    }

    int length(const p_t& p)
    {
        return  p.second - p.first;
    }

    template<class f_t>
    void run_sub_test(const test_case& test, const pp_t& expected, const string& name, f_t func)
    {
        pp_t actual;
        func(test.a, test.b, actual);
        if (expected == actual)
            return;

        const auto len = length(expected.first);
        auto a_len = length(actual.first),
            b_len = length(actual.second);

        auto start1 = actual.first.first,
            start2 = actual.second.first;
        if (!a_len && b_len)
        {// todo: p3. It can be implemented: find positions in both strings.
            assert(!start1);
            a_len = b_len;

            const auto sub = test.b.substr(start2, b_len);
            start1 = static_cast<int>(test.a.find(sub));
            assert(start1 >= 0);
        }
        else if (a_len && !b_len)
        {
            assert(!start2);
            b_len = a_len;

            const auto sub = test.a.substr(start1, a_len);
            start2 = static_cast<int>(test.b.find(sub));
            assert(start2 >= 0);
        }

        Assert::AreEqual(len, a_len, name + " f length");
        Assert::AreEqual(len, b_len, name + " s length");

        auto a1 = test.a.substr(start1, a_len),
            b1 = test.b.substr(start2, b_len);
        Assert::AreEqual(a1, b1, name + " substr");
    }

    void run_test_case(const test_case& test)
    {
        pp_t expected;
        longest_common_substring_slow_still2(test.a, test.b, expected);
        if (test.expected.first.first != not_computed)
            Assert::AreEqual(test.expected, expected, "longest_common_substring_slow_still2");

        run_sub_test(test, expected, "longest_common_substring_slow_still", longest_common_substring_slow_still<string, string, pp_t>);

        run_sub_test(test, expected, "longest_common_substring", longest_common_substring<int, string, pp_t>);

        run_sub_test(test, expected, "longest_common_substring_slow", longest_common_substring_slow<string, string, pp_t>);
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
                void longest_common_substring_tests()
                {
                    test_utilities<test_case>::run_tests(run_test_case, generate_test_cases);
                }
            }
        }
    }
}