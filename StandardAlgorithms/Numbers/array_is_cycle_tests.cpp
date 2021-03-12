#include "../Utilities/Random.h"
#include "array_is_cycle_tests.h"
#include "array_is_cycle.h"
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
        char expected;

        test_case(string&& name,
            vector<int_t>&& ar,
            int expected)
            : base_test_case(forward<string>(name)),
            ar(forward<vector<int_t>>(ar)),
            expected(static_cast<char>(expected))
        {
        }

        void Validate() const override
        {
            base_test_case::Validate();

            const auto size = static_cast<int_t>(ar.size());
            RequirePositive(size, "ar.size");
        }

        void Print(ostream& str) const override
        {
            base_test_case::Print(str);
            ::Print("ar", ar, str);
            PrintValue(str, "expected", static_cast<int>(expected));
        }
    };

    void generate_test_cases(vector<test_case>& test_cases)
    {
        test_cases.push_back({ "1", { 1 }, 1 });
        test_cases.push_back({ "2", { 2 }, 1 });
        test_cases.push_back({ "2 ones", { 1,3 }, 1 });
        test_cases.push_back({ "3 ones", { 7,4,4 }, 1 });
        test_cases.push_back({ "4 ones", { 1,5,1,1 }, 1 });
        test_cases.push_back({ "5 ones", { 1,101,-10024,6,-4 }, 1 });
        test_cases.push_back({ "2 0s", { 0,0 }, 0 });
        test_cases.push_back({ "2 2-cycles", { 1,-1, 1,-1 }, 0 });

        IntRandom r;
        vector<int_t> ar;

        for (auto att = 0, att_max = 1; att < att_max; ++att)
        {
            constexpr auto size_max = 10, value_max = 1000;
            const auto n = r(1, size_max);

            FillRandom(ar, n, value_max);

            test_cases.emplace_back("random" + to_string(att), move(ar), not_computed);
        }
    }

    void run_test_case(const test_case& test_case)
    {
        const auto actual = array_is_cycle<int_t>(test_case.ar);
        if (test_case.expected != not_computed)
            Assert::AreEqual(test_case.expected != 0, actual, "array_is_cycle");

        const auto slow = array_is_cycle_slow<int_t>(test_case.ar);
        Assert::AreEqual(actual, slow, "array_is_cycle_slow");
    }
}

void Privet::Algorithms::Numbers::Tests::array_is_cycle_tests()
{
    test_utilities<test_case>::run_tests(run_test_case, generate_test_cases);
}