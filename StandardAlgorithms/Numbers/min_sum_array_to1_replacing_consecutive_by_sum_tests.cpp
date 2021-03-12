#include "../Utilities/Random.h"
#include "min_sum_array_to1_replacing_consecutive_by_sum_tests.h"
#include "min_sum_array_to1_replacing_consecutive_by_sum.h"
#include "../test_utilities.h"
#include "../Utilities/PrintUtilities.h"

using namespace std;
using namespace Privet::Algorithms::Numbers::Tests;
using namespace Privet::Algorithms::Numbers;
using namespace Privet::Algorithms::Tests;
using namespace Privet::Algorithms;

namespace
{
    using int_t = int;
    using long_int_t = int64_t;
    constexpr int_t not_computed = -2;

    struct test_case final : base_test_case
    {
        const vector<int_t> ar;
        const int_t m;
        const long_int_t expected;

        test_case(string&& name,
            vector<int_t>&& ar,
            int_t m,
            long_int_t expected)
            : base_test_case(forward<string>(name)),
            ar(forward<vector<int_t>>(ar)),
            m(m),
            expected(expected)
        {
        }

        void Validate() const override
        {
            base_test_case::Validate();

            RequirePositive(ar.size(), "ar.size");
            Assert::GreaterOrEqual(m, 2, "m");
        }

        void Print(ostream& str) const override
        {
            base_test_case::Print(str);
            ::Print("ar", ar, str);
            PrintValue(str, "m", m);
            PrintValue(str, "expected", expected);
        }
    };

    void add_random(vector<test_case>& test_cases)
    {
        IntRandom r;
        vector<int_t> ar;
        for (auto att = 0, att_max = 1; att < att_max; ++att)
        {
            const auto n = r(1, 10), m = (2, n + 1);
            ar.resize(n);

            for (auto i = 0; i < n; ++i)
            {
                const auto value_max = 100;
                ar[i] = r(0, value_max);
            }

            test_cases.push_back({ "random" + to_string(att), move(ar), m, not_computed });
        }
    }

    void generate_test_cases(vector<test_case>& test_cases)
    {
        test_cases.push_back({ "base7", { 5, 3, 1, 2, 6, 8, 4 }, 4, 40 });
        test_cases.push_back({ "base4", { 5, 3, 1, 2 }, 4, 11 });
        test_cases.push_back({ "base10", { 5, 3, 1, 2, 6, 8, 4, 2, 5, 1 }, 4, 60 });
        test_cases.push_back({ "base1", { 5 }, 4, 5 });

        test_cases.push_back({ "they1", { 3, 5, 1, 2, 6 }, 3, 25 });
        test_cases.push_back({ "all < 0", { -1, -12, -17 }, 2, -59 });
        test_cases.push_back({ "all > 0", { 1, 12, 17 }, 2, 43 });
        test_cases.push_back({ "Impossible", { 1, 2 }, 7, -1 });
        test_cases.push_back({ "0", { 0 }, 2, 0 });
        test_cases.push_back({ "1", { 1 }, 3, 1 });
        test_cases.push_back({ "-2", { -2 }, 4, -2 });
        add_random(test_cases);
    }

    void run_test_case(const test_case& test_case)
    {
        const auto actual = min_sum_array_to1_replacing_consecutive_by_sum_they<long_int_t, int_t>(test_case.ar, test_case.m);
        if (test_case.expected != not_computed)
            Assert::AreEqual(test_case.expected, actual, "min_sum_array_to1_replacing_consecutive_by_sum");

        //const auto they = min_sum_array_to1_replacing_consecutive_by_sum_they<int_t>(test_case.ar, test_case.m);
        //Assert::AreEqual(actual, they, "min_sum_array_to1_replacing_consecutive_by_sum_they");

        if (test_case.ar.size() > factorial_max_slow)
            return;

        const auto slow = min_sum_array_to1_replacing_consecutive_by_sum_slow<int_t>(test_case.ar, test_case.m);
        Assert::AreEqual(actual, slow, "min_sum_array_to1_replacing_consecutive_by_sum_slow");
    }
}

void Privet::Algorithms::Numbers::Tests::min_sum_array_to1_replacing_consecutive_by_sum_tests()
{
    test_utilities<test_case>::run_tests(run_test_case, generate_test_cases);
}