#include "../Utilities/Random.h"
#include "min_time_array_abs_dif_sum_tests.h"
#include "min_time_array_abs_dif_sum.h"
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
    constexpr long_int_t not_computed = -1;

    struct test_case final : base_test_case
    {
        const vector<int_t> ar;
        const long_int_t expected;

        test_case(string&& name,
            vector<int_t>&& ar,
            long_int_t expected)
            : base_test_case(forward<string>(name)),
            ar(forward<vector<int_t>>(ar)),
            expected(expected)
        {
        }

        void Validate() const override
        {
            base_test_case::Validate();

            RequirePositive(ar.size(), "ar.size");
            if (expected != not_computed)
                RequireNonNegative(expected, "expected");
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
        test_cases.push_back({ "Base", { 2, 3, 3, 5, 5, 6, 6 }, 4 });
        //i ar
        //    0 1 2 3 4 5 6 7 8 9 10  -  time
        //0 2   1 0 1 2 3 (last + 1), min is 0
        //1 3     2 0 1 2 3 ...
        //2 3       2 1 2 3
        //3 5         3 1 2 3
        //4 5           3 2 3 4
        //5 6             3 3 4 5
        //6 6               4 5 6 7 - min is 4
        //
        // Note. For ar[i], there are (i+1) unused rows - skip them.

        test_cases.push_back({ "1", { 1 }, 0 });
        test_cases.push_back({ "1114", { 1,1,1,4 }, 3 });
        // They are buggy. test_cases.push_back({ "35", { 5,3 }, 0 });
        test_cases.push_back({ "many 1; 3", { 1,1,1,1,1,3 }, 13 });
        test_cases.push_back({ "many 1; 5", { 1,1,1,1,1,5 }, 11 });
        test_cases.push_back({ "many 1", { 1,1,1,1,1 }, 10 });
        test_cases.push_back({ "many 2", { 2,2,2,2,2,2 }, 11 });
        test_cases.push_back({ "1 then 3", { 1,1,1,3,3,3,3,3 }, 18 });
        test_cases.push_back({ "1, 1 and 3", { 1,1,1,1,3,3,3,3,3 }, 26 });
        // They are buggy. test_cases.push_back({ "20", { 20 }, 0 });

        // no a <=0 for now.
        //test_cases.push_back({ "-1", { -1 }, 2 });
        //test_cases.push_back({ "-1, -2", { -1, -2 }, 6 });

        test_cases.push_back({ "base0", { 3,2,1,7 }, 0 });
        test_cases.push_back({ "base1", { 1,2,5,7,7 }, 1 });
        test_cases.push_back({ "base2", { 1,2,5,7,7,7 }, 2 });
        test_cases.push_back({ "base3", { 1,2,6,7,7,7 }, 3 });
        test_cases.push_back({ "base20", { 1,2,3,9,7 }, 0 });
        test_cases.push_back({ "base30", { 2,2,4,4,4,5 }, 4 });

        IntRandom r;

        for (auto att = 0, att_max = 1; att < att_max; ++att)
        {
            const auto n = r(1, 10);
            vector<int_t> ar(n);

            for (auto i = 0; i < n; ++i)
                ar[i] = r(1, 15);

            sort(ar.begin(), ar.end());

            test_cases.emplace_back("random" + to_string(att), move(ar), not_computed);
        }
    }

    void run_test_case(const test_case& test_case)
    {
        const auto actual = min_time_array_abs_dif_sum<long_int_t>(test_case.ar);
        if (test_case.expected != not_computed)
            Assert::AreEqual(test_case.expected, actual, "min_time_array_abs_dif_sum");

        const auto they = min_time_array_abs_dif_sum_they<long_int_t>(test_case.ar);
        Assert::AreEqual(actual, they, "min_time_array_abs_dif_sum_they");
    }
}

void Privet::Algorithms::Numbers::Tests::min_time_array_abs_dif_sum_tests()
{
    test_utilities<test_case>::run_tests(run_test_case, generate_test_cases);
}