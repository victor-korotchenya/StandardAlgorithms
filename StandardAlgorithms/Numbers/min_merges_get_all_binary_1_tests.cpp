#include "../Utilities/Random.h"
#include "min_merges_get_all_binary_1_tests.h"
#include "min_merges_get_all_binary_1.h"
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
    constexpr auto not_computed = 0,
        impossible = -1;

    struct test_case final : base_test_case
    {
        const vector<vector<bool>> ar;
        const int_t expected;

        test_case(string&& name,
            vector<vector<bool>>&& ar,
            int_t expected)
            : base_test_case(forward<string>(name)),
            ar(forward<vector<vector<bool>>>(ar)),
            expected(expected)
        {
        }

        void Validate() const override
        {
            base_test_case::Validate();

            RequirePositive(ar.size(), "ar.size");
            RequirePositive(ar[0].size(), "ar[0].size");
            if (expected != not_computed && expected != impossible)
                RequirePositive(expected, "expected");
        }

        void Print(ostream& str) const override
        {
            base_test_case::Print(str);
            ::Print("ar", ar, str);
            PrintValue(str, "expected", expected);
        }
    };

    void generate_test_cases(vector<test_case>& test_cases)
    {
        constexpr auto t = true, f = false;

        test_cases.push_back({ "2 cleans",{
            { t,t,f,t,f,f },
            { f,f,t,t,f,f },
            { t,t,f,f,f,f },
            { t,f,f,t,t,f },
            { f,f,f,f,t,t },
        }, 3 });
        test_cases.push_back({ "largest excluded", {
            {t,t,t,f,f,f}, // largest 3
            {t,f,f,f,f,t},
            {f,t,f,f,t,f},
            {f,f,t,t,f,f},
        }, 3 });
        test_cases.push_back({ "0s row", {
            {t,f,f},
            {f,f,f}, //0
            {f,t,t},
        }, 2 });
        test_cases.push_back({ "0s row 2", {
            {f,f,f,f,f,f}, //0
            {t,f,f,f,f,t},
            {f,t,f,f,t,f},
            {f,f,t,t,f,f},
        }, 3 });
        test_cases.push_back({ "all 1", {
            {t,f,f,f,f,t},
            {f,t,f,f,t,f},
            {f,f,t,t,f,f},
            {t,t,t,t,t,t}, //1
        }, 1 });
        test_cases.push_back({ "two", {
            { t,t,f,t,f,t },
            { f,f,t,f,t,f },
            { f,t,f,t,t,f },
            { t,f,f,t,t,t },
            { t,f,t,t,t,t },//
        }, 2 });
        test_cases.push_back({ "1", { {t} }, 1 });
        test_cases.push_back({ "-1", { {f} }, -1 });

        IntRandom r;
        for (auto att = 0, att_max = 1; att < att_max; ++att)
        {
            constexpr auto size_max = 6;
            const auto n = r(1, size_max), m = r(1, size_max);

            vector<vector<bool>> ar(n, vector<bool>(m));
            for (auto i = 0; i < n; ++i)
            {
                for (auto j = 0; j < m;)
                {
                    const auto u = r();
                    auto k = 0;
                    do
                    {
                        ar[i][j] = (u >> k) & 1;
                    } while (++j < m && ++k < 32);
                }
            }

            test_cases.emplace_back("random" + to_string(att), move(ar), not_computed);
        }
    }

    void run_test_case(const test_case& test_case)
    {
        const auto actual = min_merges_get_all_binary_1<int_t>(test_case.ar);
        if (test_case.expected != not_computed)
            Assert::AreEqual(test_case.expected, actual, "min_merges_get_all_binary_1");

        const auto slow = min_merges_get_all_binary_1_rows<int_t>(test_case.ar);
        Assert::AreEqual(actual, slow, "min_merges_get_all_binary_1_slow");
    }
}

void Privet::Algorithms::Numbers::Tests::min_merges_get_all_binary_1_tests()
{
    test_utilities<test_case>::run_tests(run_test_case, generate_test_cases);
}