#include "../Utilities/Random.h"
#include "coin_change_count_tests.h"
#include "coin_change_count.h"
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
    constexpr auto not_computed = -1, mod = 1000 * 1000 * 1000 + 7;

    struct test_case final : base_test_case
    {
        vector<int_t> coins;
        int_t sum, expected;

        test_case(string&& name,
            vector<int_t>&& coins,
            int_t sum,
            int_t expected = not_computed)
            : base_test_case(forward<string>(name)),
            coins(forward<vector<int_t>>(coins)),
            sum(sum),
            expected(expected)
        {
        }

        void Validate() const override
        {
            base_test_case::Validate();

            const auto size = static_cast<int_t>(coins.size());
            RequirePositive(size, "coins.size");

            for (int_t i = 0; i < size; ++i)
            {
                const auto& c = coins[i];
                RequirePositive(c, "coins");
                if (i)
                    Assert::Greater(c, coins[i - 1], "Coin value");
            }

            if (expected == not_computed)
                return;

            RequireNonNegative(expected, "expected");
            RequireGreater(mod, expected, "expected");
        }

        void Print(ostream& str) const override
        {
            base_test_case::Print(str);
            ::Print("coins", coins, str);
            PrintValue(str, "sum", sum);
            PrintValue(str, "expected", expected);
        }
    };

    void generate_test_cases(vector<test_case>& test_cases)
    {
        test_cases.push_back({ "too large coins", { 5,6,7 }, 4, 0 });
        test_cases.push_back({ "base1", { 1,2,3 }, 4, 4 });//1111, 112, 22, 13.
        test_cases.push_back({ "base2", { 2,3,5,6 }, 10, 5 });//22222, 2233, 226, 235, 55.

        IntRandom r;
        set<int_t> uniq;
        vector<int_t> coins;

        for (auto att = 0, att_max = 1; att < att_max; ++att)
        {
            constexpr auto size_max = 6, value_max = 10, sum_max = 12;
            const auto n = r(1, size_max), sum = r(1, sum_max);

            uniq.clear();
            for (;;)
            {
                const auto c = r(1, value_max);
                if (!uniq.insert(c).second)
                    continue;

                if (n == static_cast<int_t>(uniq.size()))
                    break;
            }

            coins.resize(uniq.size());
            copy(uniq.begin(), uniq.end(), coins.begin());
            // set sorts.
            test_cases.emplace_back("random" + to_string(att), move(coins), sum);
        }
    }

    void run_test_case(const test_case& test)
    {
        const auto actual = coin_change_count<int_t, mod>(test.coins, test.sum);
        if (test.expected != not_computed)
            Assert::AreEqual(test.expected, actual, "coin_change_count");

        const auto slow_still = coin_change_count_slow_still<int_t, mod>(test.coins, test.sum, static_cast<int_t>(test.coins.size()) - 1);
        Assert::AreEqual(actual, slow_still, "coin_change_count_slow_still");

        const auto slow = coin_change_count_slow<long_int_t, int_t, mod>(test.coins, test.sum);
        Assert::AreEqual(actual, slow, "coin_change_count_slow");
    }
}

void Privet::Algorithms::Numbers::Tests::coin_change_count_tests()
{
    test_utilities<test_case>::run_tests(run_test_case, generate_test_cases);
}