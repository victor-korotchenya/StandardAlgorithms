#include "../Utilities/Random.h"
#include "../Utilities/PrintUtilities.h"
#include "../test_utilities.h"
#include "next_greater_element.h"
#include "next_greater_element_tests.h"

using namespace std;
using namespace Privet::Algorithms::Numbers;
using namespace Privet::Algorithms::Tests;
using namespace Privet::Algorithms;

namespace
{
    struct TestCase final : base_test_case
    {
        const vector<short> source;
        const vector<int> expected;

        TestCase(string&& name = {},
            vector<short>&& source = {},
            vector<int>&& expected = {})
            : base_test_case(forward<string>(name)),
            source(forward<vector<short>>(source)),
            expected(forward<vector<int>>(expected))
        {
        }

        void Validate() const override
        {
            base_test_case::Validate();

            RequirePositive(source.size(), "source size.");
            Assert::AreEqual(source.size(), expected.size(), "expected size.");
        }

        void Print(ostream& str) const override
        {
            base_test_case::Print(str);

            Privet::Algorithms::Print("source", source, str);
            Privet::Algorithms::Print("expected", expected, str);
        }
    };

    void GenerateTestCaseRandom(vector<TestCase>& testCases)
    {
        IntRandom r;
#ifdef _DEBUG
        constexpr auto m = 10;
#else
        constexpr auto m = 50;
#endif
        vector<short> v(r.operator()(1, m));
        FillRandom(v, v.size());

        vector<int> actual;
        next_greater_element(v, actual);

        testCases.emplace_back("random"s, move(v), move(actual));
    }

    void GenerateTestCases(vector<TestCase>& testCases)
    {
        GenerateTestCaseRandom(testCases);

        testCases.push_back({
            "simple",
            { 10, 40, 7, 20 },
            { 1, -1, 3 , -1 }
            });

        testCases.push_back({
            "trivial",
            { 5, },
            { -1 }
            });

        testCases.push_back({
            "pair",
            { 5, 8 },
            { 1, -1 }
            });

        testCases.push_back({
            "pair reversed",
            { 8, 5 },
            { -1, -1 }
            });
    }

    void RunTestCase(const TestCase& testCase)
    {
        vector<int> actual;
        next_greater_element(testCase.source, actual);
        Assert::AreEqual(testCase.expected, actual, "next_greater_element");

        actual.clear();
        next_greater_element_slow(testCase.source, actual);
        Assert::AreEqual(testCase.expected, actual, "next_greater_element_slowe");

        actual.clear();
    }
}

void Privet::Algorithms::Numbers::Tests::next_greater_element_tests()
{
    test_utilities<TestCase>::run_tests(RunTestCase, GenerateTestCases);
}