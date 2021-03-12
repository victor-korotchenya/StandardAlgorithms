#include "adjacent_divide_permutation.h"
#include "adjacent_divide_permutation_tests.h"
#include "../Utilities/RandomGenerator.h"
#include "../Utilities/PrintUtilities.h"
#include "../test_utilities.h"

using namespace std;
using namespace Privet::Algorithms::Numbers::Tests;
using namespace Privet::Algorithms::Numbers;
using namespace Privet::Algorithms::Tests;
using namespace Privet::Algorithms;

namespace
{
    constexpr auto n_max = 7, mod = 101;

    struct test_case final : base_test_case
    {
        vector<char> allowed;
        int n;
        vector<int> expected;

        test_case(string&& name,
            vector<char>&& allowed, int n, vector<int>&& expected = {})
            : base_test_case(forward<string>(name)),
            allowed(forward<vector<char>>(allowed)),
            n(n),
            expected(forward<vector<int>>(expected))
        {
            Assert::GreaterOrEqual(n, 2, "n");
            Assert::Greater(static_cast<int>(this->allowed.size()), n, "allowed.size");

            if (this->expected.empty())
                return;

            Assert::AreEqual(static_cast<int>(this->expected.size()), n, "expected.size");
        }

        void Print(ostream& str) const override
        {
            base_test_case::Print(str);
            ::Print(", allowed=", allowed, str);
            str << "n=" << n;
            ::Print(", expected=", expected, str);
        }
    };

    void generate_test_cases(vector<test_case>& tests)
    {
        IntRandom r;
        for (auto att = 0, att_max = 1; att < att_max; ++att)
        {
            const auto n = r(2, n_max);

            vector<char> allowed(n + 1ll);
            for (auto i = 2; i <= n; ++i)
                allowed[i] = char(r(0, 1));

            tests.emplace_back("random" + to_string(att), move(allowed), n);
        }
    }

    void run_test_case(const test_case& test)
    {
        const auto actual = adjacent_divide_permutation(test.allowed.data(), test.n, mod);
        if (test.expected.size())
            Assert::AreEqual(test.expected, actual, "adjacent_divide_permutation");

        const auto slow = adjacent_divide_permutation_slow(test.allowed.data(), test.n, mod);
        Assert::AreEqual(actual, slow, "adjacent_divide_permutation_slow");
    }
}

void Privet::Algorithms::Numbers::Tests::adjacent_divide_permutation_tests()
{
    test_utilities<test_case>::run_tests(run_test_case, generate_test_cases);
}