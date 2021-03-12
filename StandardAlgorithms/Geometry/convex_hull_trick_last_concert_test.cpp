#include "convex_hull_trick_last_concert.h"
#include "../test_utilities.h"
#include "../Utilities/PrintUtilities.h"
#include "../Utilities/Random.h"

using namespace std;
using namespace Privet::Algorithms::Geometry::Tests;
using namespace Privet::Algorithms::Geometry;
using namespace Privet::Algorithms::Tests;
using namespace Privet::Algorithms;

namespace
{
    using pt = pair<double, double>;

    struct test_case final : base_test_case
    {
        const vector<pt> ticket_ratios;
        double expected;

        test_case(string&& name, vector<pt>&& ticket_ratios, const double expected)
            : base_test_case(forward<string>(name)),
            ticket_ratios(forward<vector<pt>>(ticket_ratios)), expected(expected)
        {
        }

        void Print(ostream& str) const override
        {
            base_test_case::Print(str);
            ::Print("ticket_ratios", ticket_ratios, str);
            str << ", expected=" << expected;
        }
    };

    test_case RandomTestCase()
    {
        IntRandom r;
        const auto n = r.operator()(1, 16);
        vector<pt> ticket_ratios(n);

        DoubleRandom dr;
        for (auto i = 0; i < n; ++i)
        {
            ticket_ratios[i].first = dr.operator()(0, 100);
            ticket_ratios[i].second = dr.operator()(0, 1);
        }

        const auto actual = convex_hull_trick_last_concert(ticket_ratios);
        return test_case("random"s, move(ticket_ratios), actual);
    }

    void GenerateTestCases(vector<test_case>& testCases)
    {
        testCases.push_back(RandomTestCase());
        testCases.push_back({ "trivial", {{10, 0.5}}, 10 });
        testCases.push_back({ "last_only", {{10, 0.5}, {10, 0.5}}, 10 });
        testCases.push_back({ "two", {{4, 0.5}, {10, 0.5}}, 9 });
        testCases.push_back({ "three", {{1, 0.9}, {20, 0.5}, {100, 0.5}}, 69 });
    }

    void RunTestCase(const test_case& testCase)
    {
        const auto actual_slow = convex_hull_trick_last_concert_slow(testCase.ticket_ratios);
        Assert::AreEqualWithEpsilon(testCase.expected, actual_slow, "convex_hull_trick_last_concert_slow");

        const auto actual = convex_hull_trick_last_concert(testCase.ticket_ratios);
        Assert::AreEqualWithEpsilon(testCase.expected, actual, "convex_hull_trick_last_concert");
    }
}

void Privet::Algorithms::Geometry::Tests::convex_hull_trick_last_concert_tests()
{
    test_utilities<test_case>::run_tests(RunTestCase, GenerateTestCases);
}