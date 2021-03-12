#include "../Utilities/Random.h"
#include "two_subsets_min_diff_knapsack_tests.h"
#include "two_subsets_min_diff_knapsack.h"
#include "../test_utilities.h"
#include "../Utilities/PrintUtilities.h"

using namespace std;
using namespace Privet::Algorithms::Numbers::Tests;
using namespace Privet::Algorithms::Numbers;
using namespace Privet::Algorithms::Tests;
using namespace Privet::Algorithms;

namespace
{
    using int_t = short;

    int_t sum(const vector<int_t>& source, const vector<bool>& negative_signs)
    {
        Assert::AreEqual(source.size(), negative_signs.size(), "negative_signs.size");

        int_t s{};
        for (int_t i = 0; i < static_cast<int_t>(source.size()); ++i)
            s += (negative_signs[i] ? -1 : 1) * source[i];

        s = static_cast<int_t>(abs(s));
        assert(s >= 0);
        return s;
    }

    struct TestCase final : base_test_case
    {
        const int_t expected;
        const vector<int_t> source;
        const vector<bool> negative_signs;

        TestCase(string&& name = {},
            vector<int_t>&& source = {},
            vector<bool>&& negative_signs = {})
            : base_test_case(forward<string>(name)),
            // must occur before move.
            expected(sum(source, negative_signs)),
            source(forward<vector<int_t>>(source)),
            negative_signs(forward<vector<bool>>(negative_signs))
        {
        }

        void Validate() const override
        {
            base_test_case::Validate();

            RequirePositive(source.size(), "source size.");
            Assert::AreEqual(source.size(), negative_signs.size(), "negative_signs size.");
        }

        void Print(ostream& str) const override
        {
            base_test_case::Print(str);

            Privet::Algorithms::Print("source", source, str);
            Privet::Algorithms::Print("negative_signs", negative_signs, str);
        }
    };

    void GenerateTestCaseRandom(vector<TestCase>& testCases)
    {
        IntRandom r;
#ifdef _DEBUG
        constexpr int_t power2 = 5;
#else
        constexpr int_t power2 = 8;
#endif
        vector<int_t> v(r.operator()(1, power2));
        FillRandom(v, v.size(), static_cast<int_t>(power2 * 2));
        for (auto& a : v)
            a = static_cast<int_t>(abs(a));

        vector<vector<int_t>> temp;
        vector<bool> negative_signs;

        const auto best = two_subsets_min_diff_knapsack(v, temp, negative_signs);

        testCases.emplace_back("random"s, move(v), move(negative_signs));
        Assert::AreEqual(best, testCases.back().expected, "random best");
    }

    void GenerateTestCases(vector<TestCase>& testCases)
    {
        GenerateTestCaseRandom(testCases);

        testCases.push_back({
            "simple",
            { 10, 40, 7, 20},
            { false, true, false , false }
            });
        Assert::AreEqual(3, testCases.back().expected, "expected 1");

        testCases.push_back({
            "trivial",
            { 5, },
            { false }
            });
        Assert::AreEqual(5, testCases.back().expected, "expected 0");
    }

    void RunTestCase(const TestCase& testCase)
    {
        vector<bool> negative_signs;
        {
            vector<vector<int_t>> temp;
            const auto actual = two_subsets_min_diff_knapsack(
                testCase.source, temp,
                negative_signs);
            Assert::AreEqual(testCase.expected, actual, "actual");

            const auto actual_sum = sum(testCase.source, negative_signs);
            Assert::AreEqual(testCase.expected, actual_sum, "actual_sum");
        }
        {
            negative_signs.clear();
            const auto actual = two_subsets_min_diff_knapsack_slow<int_t>(
                testCase.source, negative_signs);
            Assert::AreEqual(testCase.expected, actual, "actual_slow");

            const auto actual_sum = sum(testCase.source, negative_signs);
            Assert::AreEqual(testCase.expected, actual_sum, "actual_sum_slow");
        }
    }
}

void Privet::Algorithms::Numbers::Tests::two_subsets_min_diff_knapsack_tests()
{
    test_utilities<TestCase>::run_tests(RunTestCase, GenerateTestCases);
}