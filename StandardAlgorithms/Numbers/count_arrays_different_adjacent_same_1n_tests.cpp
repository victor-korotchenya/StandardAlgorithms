#include<unordered_set>
#include "../Utilities/Random.h"
#include "count_arrays_different_adjacent_same_1n_tests.h"
#include "count_arrays_different_adjacent_same_1n.h"
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
    constexpr int_t mod = 1000 * 1000 * 1000 + 7;

    struct test_case final : base_test_case
    {
        const int_t n, m, expected;

        test_case(string&& name,
            int_t n,
            int_t m,
            int_t expected)
            : base_test_case(forward<string>(name)),
            n(n),
            m(m),
            expected(expected)
        {
        }

        void Validate() const override
        {
            base_test_case::Validate();

            RequirePositive(n, "n");
            RequirePositive(m, "m");
            RequireNonNegative(expected, "expected");
            RequireGreater(mod, expected, "expected");
        }

        void Print(ostream& str) const override
        {
            base_test_case::Print(str);
            PrintValue(str, "n", n);
            PrintValue(str, "m", m);
            PrintValue(str, "expected", expected);
        }
    };

    void generate_test_cases(vector<test_case>& test_cases)
    {
        // 121, 131, 212, 232, 313, 323.
        test_cases.emplace_back("base1", 1, 1, 1);
        test_cases.emplace_back("base2", 2, 1, 1);
        test_cases.emplace_back("base3", 3, 1, 0);
        test_cases.emplace_back("base4", 2, 2, 2);
        test_cases.emplace_back("base6", 3, 3, 6);
        test_cases.emplace_back("base7", 5, 4, 84);

        unordered_set<int> uniq;

        const auto lam = [](int_t a, int_t b)->int_t {
            return a << 16 | b;
        };
        for (const auto& test_case : test_cases)
        {
            auto p = uniq.insert(lam(test_case.n, test_case.m));
            assert(p.second);
        }

        IntRandom r;
        for (auto att = 0, att_max = 1; att < att_max; ++att)
        {
            const auto n = r(1, 6), m = r(1, 6);
            const auto key = lam(n, m);
            if (!uniq.insert(key).second)
                continue;

            test_cases.emplace_back("random" + to_string(att), n, m, 0);
        }
    }

    void run_test_case(const test_case& test_case)
    {
        const auto actual = count_arrays_different_adjacent_same_1n<int_t, mod>(test_case.n, test_case.m);
        if (test_case.expected)
            Assert::AreEqual(test_case.expected, actual, "count_arrays_different_adjacent_same_1n");

        const auto slow = count_arrays_different_adjacent_same_1n_slow<int_t, mod>(test_case.n, test_case.m);
        Assert::AreEqual(actual, slow, "count_arrays_different_adjacent_same_1n_slow");
    }
}

void Privet::Algorithms::Numbers::Tests::count_arrays_different_adjacent_same_1n_tests()
{
    test_utilities<test_case>::run_tests(run_test_case, generate_test_cases);
}