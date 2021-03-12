#include <list>
#include <vector>
#include "longest_alternative_subsequence_tests.h"
#include "longest_alternative_subsequence.h"
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

    struct test_case final : base_test_case
    {
        vector<int_t> ar;
        size_t expected;

        test_case(string&& name,
            vector<int_t>&& ar,
            size_t expected)
            : base_test_case(forward<string>(name)),
            ar(forward<vector<int_t>>(ar)),
            expected(expected)
        {
        }

        void Validate() const override
        {
            base_test_case::Validate();
            Assert::GreaterOrEqual(ar.size(), expected, "expected");
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
        test_cases.push_back({ "4", { -100,-1,-1,5,1,9,10,200 }, 4 });
        test_cases.push_back({ "0", {}, 0 });
        test_cases.push_back({ "1", { 1 }, 1 });
        test_cases.push_back({ "same", { 2,2,2,2,2,2 }, 1 });
        test_cases.push_back({ "same2", { 2,2,2,2,2,2,2 }, 1 });
        test_cases.push_back({ "2", { 1,2 }, 2 });
        test_cases.push_back({ "-2", { 2,1 }, 2 });
        test_cases.push_back({ "3", { 4,5,4,4 }, 3 });
        test_cases.push_back({ "-3", { 4,3,3,4 }, 3 });

        constexpr auto neg_inf = numeric_limits<int_t>::min(), inf = numeric_limits<int_t>::max();

        test_cases.push_back({ "inf", { neg_inf,inf,neg_inf,neg_inf,inf,neg_inf,neg_inf}, 5 });

        test_cases.push_back({ "neg_inf", { inf,inf,neg_inf,neg_inf,inf,neg_inf,neg_inf,inf,neg_inf}, 6 });
    }

    void run_test_case(const test_case& test_case)
    {
        const auto actual = longest_alternative_subsequence(test_case.ar.begin(), test_case.ar.end());
        Assert::AreEqual(test_case.expected, actual, "longest_alternative_subsequence");

        const list<int_t> li(test_case.ar.begin(), test_case.ar.end());
        const auto actual_li = longest_alternative_subsequence(li.begin(), li.end());
        Assert::AreEqual(actual, actual_li, "longest_alternative_subsequence on list");
    }
}

void Privet::Algorithms::Numbers::Tests::longest_alternative_subsequence_tests()
{
    test_utilities<test_case>::run_tests(run_test_case, generate_test_cases);
}