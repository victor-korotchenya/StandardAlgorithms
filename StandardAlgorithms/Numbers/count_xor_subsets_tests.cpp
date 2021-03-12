#include "../Utilities/Random.h"
#include "count_xor_subsets_tests.h"
#include "count_xor_subsets.h"
#include "../test_utilities.h"
#include "../Utilities/PrintUtilities.h"

using namespace std;
using namespace Privet::Algorithms::Numbers::Tests;
using namespace Privet::Algorithms::Numbers;
using namespace Privet::Algorithms::Tests;
using namespace Privet::Algorithms;

namespace
{
    using int_t = unsigned;
    constexpr auto not_computed = -1, mod = 1000 * 1000 * 1000 + 7;

    struct test_case final : base_test_case
    {
        vector<int_t> ar;
        int_t x;
        int expected;

        test_case(string&& name,
            vector<int_t>&& ar,
            int_t x,
            int expected = not_computed)
            : base_test_case(forward<string>(name)),
            ar(forward<vector<int_t>>(ar)),
            x(x),
            expected(expected)
        {
        }

        void Validate() const override
        {
            base_test_case::Validate();

            Assert::Greater(mod, expected, "expected");
            Assert::GreaterOrEqual(expected, not_computed, "expected");
        }

        void Print(ostream& str) const override
        {
            base_test_case::Print(str);
            ::Print("ar", ar, str);
            PrintValue(str, "x", x);
            PrintValue(str, "expected", expected);
        }
    };

    void generate_test_cases(vector<test_case>& test_cases)
    {
        test_cases.push_back({ "base1", {0,1,2,3,4,0xD,0xE},
            // 0100 ^ 1101 ^ 1110 = 0111
            7,
            // with or without 0 -> times two
            // 124, 34, 4de, 123 ^ 4de
            4 * 2 });

        test_cases.push_back({ "empty 0", {}, 0, 1 }); // The empty subset counts.
        test_cases.push_back({ "empty 1", {}, 1, 0 });
        test_cases.push_back({ "empty 2", {}, 2, 0 });

        test_cases.push_back({ "one same", { 7 }, 7, 1 });
        test_cases.push_back({ "one 0", { 7 }, 0, 1 });
        test_cases.push_back({ "one different", { 7 }, 4, 0 });
        test_cases.push_back({ "zero 1", { 0 }, 1, 0 });
        test_cases.push_back({ "zero 0", { 0 }, 0, 2 });

        test_cases.push_back({ "two 0 0", { 0,0 }, 0, 4 });
        test_cases.push_back({ "two 1", { 0,0 }, 1, 0 });
        test_cases.push_back({ "two same 0", { 2,2 }, 0, 2 });
        test_cases.push_back({ "two same", { 2,2 }, 2, 2 });

        test_cases.push_back({ "three same", { 8,8,8 }, 8, 4 });
        test_cases.push_back({ "three same 0", { 0,0,0}, 0, 8 });
        test_cases.push_back({ "three same 0 1", { 0,0,0}, 1, 0 });
        test_cases.push_back({ "three", { 1,2,4}, 7, 1 });
        test_cases.push_back({ "three 2 same 0", { 1,2,2}, 0, 2 });
        test_cases.push_back({ "three 2 same 1", { 1,2,2}, 1, 2 });
        test_cases.push_back({ "three 2 same 2", { 1,2,2}, 2, 2 });
        test_cases.push_back({ "three 2 same 3", { 1,2,2}, 3, 2 });
        test_cases.push_back({ "three 2 same 4", { 1,2,2}, 4, 0 });

        IntRandom r;
        vector<int_t> ar;

        for (auto att = 0, att_max = 1; att < att_max; ++att)
        {
            constexpr auto size_max = 5, value_max = 15;
            const auto n = r(0, size_max);
            const auto x = static_cast<int_t>(r(0, value_max));

            ar.resize(n);
            for (auto& v : ar)
                v = static_cast<int_t>(r(0, value_max));

            test_cases.emplace_back("random" + to_string(att), move(ar), x);
        }
    }

    void run_test_case(const test_case& test)
    {
        const auto actual = count_xor_subsets<int_t, mod>(test.ar, test.x);
        if (test.expected >= 0)
            Assert::AreEqual(test.expected, actual, "count_xor_subsets");

        {
            const auto slow = count_xor_subsets_slow_still<int_t, mod>(test.ar, test.x);
            Assert::AreEqual(actual, slow, "count_xor_subsets_slow_still");
        }
        {
            const auto slow = count_xor_subsets_slow<int_t, mod>(test.ar, test.x);
            Assert::AreEqual(actual, slow, "count_xor_subsets_slow");
        }
    }
}

void Privet::Algorithms::Numbers::Tests::count_xor_subsets_tests()
{
    test_utilities<test_case>::run_tests(run_test_case, generate_test_cases);
}