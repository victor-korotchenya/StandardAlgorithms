#include "../Utilities/Random.h"
#include "count_add_sub_put_parentheses_distinct_tests.h"
#include "count_add_sub_put_parentheses_distinct.h"
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
    constexpr auto not_computed = -1;

    struct test_case final : base_test_case
    {
        vector<int_t> ar;
        vector<bool> negatives;
        int expected;

        test_case(string&& name,
            vector<int_t>&& ar,
            vector<bool>&& negatives,
            int expected = not_computed)
            : base_test_case(forward<string>(name)),
            ar(forward<vector<int_t>>(ar)),
            negatives(forward<vector<bool>>(negatives)),
            expected(expected)
        {
        }

        void Validate() const override
        {
            base_test_case::Validate();

            const auto size = ar.size();
            RequirePositive(size, "ar.size");

            Assert::AreEqual(negatives.size(), size, "negatives.size");
            Assert::AreEqual(false, negatives[0], "negatives[0]");
            RequireAllNonNegative(ar, "ar");

            if (expected == not_computed)
                return;

            RequirePositive(expected, "expected");
        }

        void Print(ostream& str) const override
        {
            base_test_case::Print(str);
            ::Print("ar", ar, str);
            ::Print("negatives", negatives, str);
            PrintValue(str, "expected", expected);
        }
    };

    void generate_test_cases(vector<test_case>& test_cases)
    {
        constexpr auto t = true, f = false;
        test_cases.push_back({ "b1", { 10 }, { f, }, 1 });
        test_cases.push_back({ "b2", { 10,20 }, { f,t}, 1 });
        test_cases.push_back({ "b3", { 1,2,4 }, { f,t,t}, 2 });//1-2-4=-5, 1-(2-4)=3.
        test_cases.push_back({ "b4", { 1,2,4,8 }, { f,t,t,t}, 4 });
        test_cases.push_back({ "b41", { 0,1,2,4 }, { f,t,t,t}, 4 });
        test_cases.push_back({ "b5", { 1,2,4,8,16 }, { f,t,t,t,t}, 8 });
        test_cases.push_back({ "b51", { 1,2,4,2,16 }, { f,t,t,t,t}, 8 });

        IntRandom r;
        vector<int_t> ar;
        vector<bool> signes;

        for (auto att = 0, att_max = 1; att < att_max; ++att)
        {
            constexpr auto size_max = 10, value_min = 0, value_max = 15;

            const auto n = r(1, size_max);
            ar.resize(n);
            signes.resize(n);

            for (auto i = 0; i < n; ++i)
            {
                ar[i] = r(value_min, value_max);
                signes[i] = i && static_cast<bool>(r(0, 1));
            }

            test_cases.emplace_back("random" + to_string(att), move(ar), move(signes));
        }
    }

    void run_test_case(const test_case& test)
    {
        const auto actual = count_add_sub_put_parentheses_distinct<int_t>(test.ar, test.negatives);
        if (test.expected != not_computed)
            Assert::AreEqual(test.expected, actual, "count_add_sub_put_parentheses_distinct");

        //const auto slow = count_add_sub_put_parentheses_distinct_slow<int_t>(test.ar, test.negatives);
        //Assert::AreEqual(actual, slow, "count_add_sub_put_parentheses_distinct_slow");
    }
}

void Privet::Algorithms::Numbers::Tests::count_add_sub_put_parentheses_distinct_tests()
{
    test_utilities<test_case>::run_tests(run_test_case, generate_test_cases);
}