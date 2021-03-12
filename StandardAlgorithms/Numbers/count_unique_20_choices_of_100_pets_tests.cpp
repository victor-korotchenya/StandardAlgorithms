#include "../Utilities/Random.h"
#include "count_unique_20_choices_of_100_pets_tests.h"
#include "count_unique_20_choices_of_100_pets.h"
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
    constexpr int_t not_computed = -1, mod = 1000 * 1000 * 1000 + 7;

    struct test_case final : base_test_case
    {
        const vector<vector<int_t>> ar;
        const int_t expected;

        test_case(string&& name,
            vector<vector<int_t>>&& ar,
            int_t expected)
            : base_test_case(forward<string>(name)),
            ar(forward<vector<vector<int_t>>>(ar)),
            expected(expected)
        {
        }

        void Validate() const override
        {
            base_test_case::Validate();

            RequirePositive(ar.size(), "ar.size");
            RequirePositive(ar[0].size(), "ar[0].size");
            if (expected != not_computed)
            {
                RequireNonNegative(expected, "expected");
                RequireGreater(mod, expected, "expected");
            }
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
        test_cases.push_back({ "base1",{ { 1,2,3 },{ 0 },{ 3,2 } }, 4 });

        IntRandom r;
        unordered_set<int_t> uniq;

        for (auto att = 0, att_max = 1; att < att_max; ++att)
        {
            constexpr auto up_value = 6;
            const auto m = r(1, up_value), p = r((m + 1) >> 1, up_value);

            vector<vector<int_t>> ar(m);
            for (auto i = 0; i < m; ++i)
            {
                const auto mp = r(1, p);
                auto& a = ar[i];
                a.resize(mp);

                uniq.clear();
                for (auto j = 0; j < mp;)
                {
                    a[j] = r(0, p - 1);
                    j += uniq.insert(a[j]).second;
                }
            }

            test_cases.emplace_back("random" + to_string(att), move(ar), not_computed);
        }
    }

    void run_test_case(const test_case& test_case)
    {
        const auto actual = count_unique_20_choices_of_100_pets<int_t, mod>(test_case.ar);
        if (test_case.expected != not_computed)
            Assert::AreEqual(test_case.expected, actual, "count_unique_20_choices_of_100_pets");

        const auto slow = count_unique_20_choices_of_100_pets_slow<int_t, mod>(test_case.ar);
        Assert::AreEqual(actual, slow, "count_unique_20_choices_of_100_pets_slow");
    }
}

void Privet::Algorithms::Numbers::Tests::count_unique_20_choices_of_100_pets_tests()
{
    test_utilities<test_case>::run_tests(run_test_case, generate_test_cases);
}