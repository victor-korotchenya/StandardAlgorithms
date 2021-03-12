#include <iostream>
#include "convex_hull_tests.h"
#include "convex_hull.h"
#include "point.h"
#include "../Utilities/Random.h"
#include "../test_utilities.h"
#include "../Utilities/PrintUtilities.h"

using namespace std::chrono;
using namespace std;
using namespace Privet::Algorithms::Geometry::Tests;
using namespace Privet::Algorithms::Geometry;
using namespace Privet::Algorithms::Tests;
using namespace Privet::Algorithms;

namespace
{
    using point_t = point2d<int>;
    using long_int_t = int64_t;

    struct test_case final : base_test_case
    {
        const vector<point_t> source_points, expected;

        test_case(string&& name, vector<point_t>&& source_points, vector<point_t>&& expected)
            : base_test_case(forward<string>(name)),
            source_points(forward<vector<point_t>>(source_points)),
            expected(forward<vector<point_t>>(expected))
        {
        }

        void Print(ostream& str) const override
        {
            base_test_case::Print(str);
            ::Print("source_points", source_points, str);
            ::Print("expected", expected, str);
        }
    };

    test_case RandomTestCase()
    {
        IntRandom r;
#ifdef _DEBUG
        constexpr auto mu = 1;
#else
        constexpr auto mu = 1 << 3;
#endif
        const auto n = r.operator()(mu, 32 * mu);
        vector<point_t> source_points(n), expected;

        for (auto i = 0; i < n; ++i)
        {
            constexpr auto d = 1 << 30;
            source_points[i].X = r.operator()(-d, d);
            source_points[i].Y = r.operator()(-d, d);
        }

        convex_hull_graham_scan<long_int_t>(source_points, expected);
        RequirePositive(expected.size(), "random expected size");

        return test_case("random"s, move(source_points), move(expected));
    }

    void GenerateTestCases(vector<test_case>& testCases)
    {
        testCases.push_back(
            { "remove inner points",
            { { 1, 1 },{ 2, 3 },{ 3, 2 },
            // duplicate
            { 3, 2 },
            { 0, 5 },{ 6, 0 },{ 7, 1 }, {1,6}, {2,7} },
        { { 0, 5 },{ 1, 1 },{ 6, 0 },{ 7, 1 }, {2,7} } });

        testCases.push_back(RandomTestCase());

        testCases.push_back({ "trivial",{ { 10, 20 } },{ { 10, 20 } } });

        testCases.push_back({ "segment",{ { 10, 20 },{ 10, 300 } },{ { 10, 20 },{ 10, 300 } } });

        testCases.push_back(
            { "line remove middle",
            { { 10, 20 },{ 10, 30 },{ 10, 50 } },
            { { 10, 20 },{ 10, 50 } } });
    }

    void RunTestCase(const test_case& testCase)
    {
        const auto size = testCase.source_points.size();

        vector<point_t> actual, source(size), source2(size);
        int64_t a0 = 0, a1 = 0;

        const auto attempts = 1;
        for (auto att = 0; att < attempts; ++att)
        {
            copy(testCase.source_points.begin(), testCase.source_points.end(), source.begin());

            copy(testCase.source_points.begin(), testCase.source_points.end(), source2.begin());

            const auto t0 = high_resolution_clock::now();
            actual.clear();

            convex_hull_graham_scan<long_int_t>(source, actual);
            Assert::AreEqual(testCase.expected, actual, "convex_hull_graham_scan_" + testCase.get_Name());

            const auto t1 = high_resolution_clock::now();
            actual.clear();

            convex_hull_low_upper<long_int_t>(source2, actual);
            Assert::AreEqual(testCase.expected, actual, "convex_hull_low_upper_" + testCase.get_Name());

            const auto t2 = high_resolution_clock::now();
            if (!att)
                continue;

            const auto elapsed1 = duration_cast<nanoseconds>(t1 - t0).count();
            a0 += elapsed1;

            const auto elapsed2 = duration_cast<nanoseconds>(t2 - t1).count();
            a1 += elapsed2;

            const auto rat = a0 ? a1 * 1.0 / a0 : 0.0;
            // a1/a0 = 1.7 on 1M.
            cout << att
                << ", n=" << size
                << ", a0=" << a0
                << ", a1=" << a1
                << ", a1/a0="
                << rat << '\n';
        }
    }
}

void Privet::Algorithms::Geometry::Tests::convex_hull_tests()
{
    test_utilities<test_case>::run_tests(RunTestCase, GenerateTestCases);
}