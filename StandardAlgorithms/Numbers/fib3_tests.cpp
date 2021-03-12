#include "../Utilities/Random.h"
#include "fib3.h"
#include "fib3_tests.h"
#include "../test_utilities.h"

using namespace std;
using namespace Privet::Algorithms::Numbers::Tests;
using namespace Privet::Algorithms::Numbers;
using namespace Privet::Algorithms::Tests;
using namespace Privet::Algorithms;

namespace
{
    using int_t = int64_t;
    constexpr int mod = 1e9 + 7, not_computed = -1;

    struct test_case final : base_test_case
    {
        int_t n, expected;

        test_case(
            int_t n,
            int_t expected = not_computed) : base_test_case(string()),
            n(n),
            expected(expected)
        {
        }

        void Validate() const override
        {
            base_test_case::Validate();

            RequireNonNegative(n, "n");
            Assert::GreaterOrEqual(expected, not_computed, "expected");
            Assert::Greater(mod, expected, "expected");
        }

        void Print(ostream& str) const override
        {
            base_test_case::Print(str);

            PrintValue(str, "expected", expected);
        }
    };

    void fix_names(vector<test_case>& tests)
    {
        for (auto& test : tests)
            if (test.get_Name().empty())
                test.set_Name(to_string(test.n));
    }

    void generate_test_cases(vector<test_case>& tests)
    {
        tests.emplace_back(1000000000000000000ll, 538436942);
        tests.emplace_back(0, 1);
        tests.emplace_back(1, 1);
        tests.emplace_back(2, 2);
        tests.emplace_back(3, 4);

        IntRandom r;
        for (auto att = 0, att_max = 1; att < att_max; ++att)
        {
            const auto n = r(4, 100);
            tests.emplace_back(n);
        }

        //It must be the last line.
        fix_names(tests);
    }

    void run_test_case(const test_case& test)
    {
        const auto actual = fib3<int_t, mod>(test.n);
        if (test.expected >= 0)
            Assert::AreEqual(test.expected, actual, "fib3");

        constexpr int_t large = 1e6;
        if (test.n > large)
            return;

        const auto slow = fib3_slow<int_t, mod>(test.n);
        Assert::AreEqual(actual, slow, "fib3_slow");
    }
}

void Privet::Algorithms::Numbers::Tests::fib3_tests()
{
    test_utilities<test_case>::run_tests(run_test_case, generate_test_cases);
}