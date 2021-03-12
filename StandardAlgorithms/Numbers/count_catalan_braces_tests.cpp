#include "count_catalan_braces.h"
#include "count_catalan_braces_tests.h"
#include "../Utilities/PrintUtilities.h"
#include "../test_utilities.h"
#include "../Utilities/RandomGenerator.h"

using namespace std;
using namespace Privet::Algorithms::Numbers::Tests;
using namespace Privet::Algorithms::Numbers;
using namespace Privet::Algorithms::Tests;
using namespace Privet::Algorithms;

namespace
{
    struct test_case final : base_test_case
    {
        const int expected;

        test_case(string&& name,
            const int expected)
            : base_test_case(forward<string>(name)),
            expected(expected)
        {
            Assert::Greater(this->get_Name().size(), 0u, "source.size");
            Assert::GreaterOrEqual(expected, 0, "expected");
        }

        void Print(ostream& str) const override
        {
            base_test_case::Print(str);
            ::PrintValue(str, "expected", expected);
        }
    };

    void gen_random(vector<test_case>& tests, IntRandom& rnd, const int size)
    {
        assert(size > 0 && !(size & 1));
        string s(size, 0);

        for (auto i = 0; i < size; ++i)
        {
            const auto r = rnd.operator()();
            if (r & 1)
            {
                s[i] = '?';
                continue;
            }

            s[i] = r & 2 ? '[' : ']';
        }

        const auto actual = count_catalan_braces(s);
        tests.emplace_back(move(s), actual);
    }

    void generate_test_cases(vector<test_case>& tests)
    {
        {// size 10, 8
            IntRandom rnd;
            gen_random(tests, rnd, 10);
            gen_random(tests, rnd, 8);
        }

        // size 6
        tests.emplace_back("??????", 5);

        tests.emplace_back("[?[??]", 3);
        //[][??]
        //[][[]] +1
        //[][][] +1

        //[[[??]
        //[[[]]] + 1

        tests.emplace_back("?[[]]?", 1);

        // size 4
        tests.emplace_back("????", 2);
        tests.emplace_back("[??]", 2);
        tests.emplace_back("???]", 2);
        tests.emplace_back("[]]]", 0);
        tests.emplace_back("[[[]", 0);

        // 3**2 strings of size=2.
        tests.emplace_back("??", 1);
        tests.emplace_back("?]", 1);
        tests.emplace_back("?[", 0);

        tests.emplace_back("[?", 1);
        tests.emplace_back("[[", 0);
        tests.emplace_back("[]", 1);

        tests.emplace_back("]?", 0);
        tests.emplace_back("]]", 0);
        tests.emplace_back("][", 0);
    }

    void run_test_case(const test_case& test)
    {
        const auto actual = count_catalan_braces(test.get_Name());
        Assert::AreEqual(test.expected, actual, "count_catalan_braces");

        const auto actual_slow = count_catalan_braces_slow(test.get_Name());
        Assert::AreEqual(test.expected, actual_slow, "count_catalan_braces_slow");
    }
}

namespace Privet
{
    namespace Algorithms
    {
        namespace Numbers
        {
            namespace Tests
            {
                void count_catalan_braces_tests()
                {
                    test_utilities<test_case>::run_tests(run_test_case, generate_test_cases);
                }
            }
        }
    }
}