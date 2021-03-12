#include <tuple>
#include "../Utilities/Random.h"
#include "../Utilities/PrintUtilities.h"
#include "../test_utilities.h"
#include "difference_array__range_update_in_o1.h"
#include "difference_array__range_update_in_o1_tests.h"

using namespace std;
using namespace Privet::Algorithms::Trees;
using namespace Privet::Algorithms::Tests;
using namespace Privet::Algorithms;

namespace
{
    using tu = tuple<int, int, short>;

    struct TestCase final : base_test_case
    {
        const vector<short> source;
        const vector<tu> ops;
        const vector<short> expected;

        TestCase(string&& name = {},
            vector<short>&& source = {},
            vector<tu>&& ops = {},
            vector<short>&& expected = {})
            : base_test_case(forward<string>(name)),
            source(forward<vector<short>>(source)),
            ops(forward<vector<tu>>(ops)),
            expected(forward<vector<short>>(expected))
        {
        }

        void Validate() const override
        {
            base_test_case::Validate();

            RequirePositive(source.size(), "source size.");
            RequirePositive(ops.size(), "ops size.");
            Assert::AreEqual(source.size(), expected.size(), "expected size.");
        }

        void Print(ostream& str) const override
        {
            base_test_case::Print(str);

            Privet::Algorithms::Print("source", source, str);
            Privet::Algorithms::Print("ops", ops, str);
            Privet::Algorithms::Print("expected", expected, str);
        }
    };

    template<class da_t>
    void calc_actual(const vector<tu>& ops, da_t& da, vector<short>& actual)
    {
        actual.clear();
        for (const auto& t : ops)
            da.update(get<0>(t), get<1>(t), get<2>(t));

        da.restore(actual);
    }

    void GenerateTestCaseRandom(vector<TestCase>& testCases)
    {
        UnsignedIntRandom r;
#ifdef _DEBUG
        constexpr auto m = 6;
#else
        constexpr auto m = 20;
#endif
        const auto size = r.operator()(1, m);
        vector<short> v(size);

        constexpr auto cutter = static_cast<short>(100);
        FillRandom(v, size, cutter);

        vector<tuple<int, int, short>> ops;
        for (auto i = 0u; i < 20; ++i)
        {
            const auto val = static_cast<short>(r(0, cutter));
            unsigned left, right;

            do
            {
                left = r(0, size - 1u);
                right = r(0, size);
            } while (left == right);

            if (right < left)
                swap(left, right);

            ops.emplace_back(left, right, val);
        }

        vector<short> actual;
        difference_array__range_update_in_o1<short> temp(v);
        calc_actual(ops, temp, actual);

        testCases.push_back({
            "random",
            move(v),
            move(ops),
            move(actual) });
    }

    void GenerateTestCases(vector<TestCase>& testCases)
    {
        GenerateTestCaseRandom(testCases);

        testCases.push_back({
            "simple",
            { 10, 40, 7, 20 },
            {
                {0, 3, static_cast<short>(100)},
                {1, 2, static_cast<short>(-1)},
                {0, 4,static_cast<short>(25)},
                {2, 4, static_cast<short>(250)},
            },
            { 10 + 100 + 25, 40 + 100 - 1 + 25, 7 + 100 + 25 + 250, 20 + 25 + 250 }
            });
    }

    void RunTestCase(const TestCase& testCase)
    {
        difference_array__range_update_in_o1<short> da(testCase.source);

        vector<short> actual;
        calc_actual(testCase.ops, da, actual);
        Assert::AreEqual(testCase.expected, actual, "difference_array__range_update_in_o1");

        difference_array_slow<short> da_slow(testCase.source);
        actual.clear();
        calc_actual(testCase.ops, da_slow, actual);
        Assert::AreEqual(testCase.expected, actual, "difference_array_slow");
    }
}

void Privet::Algorithms::Trees::Tests::difference_array__range_update_in_o1_tests()
{
    test_utilities<TestCase>::run_tests(RunTestCase, GenerateTestCases);
}