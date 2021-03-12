#include <vector>
#include "CalcAngleTests.h"
#include "GeometryUtilities.h"
#include "point.h"
#include "../test_utilities.h"

using namespace std;
using namespace Privet::Algorithms::Geometry;
using namespace Privet::Algorithms::Tests;
using namespace Privet::Algorithms;

namespace
{
    using Coordinate = int;
    using TPoint = point2d<Coordinate>;

    struct TestCase final : base_test_case
    {
        TPoint P1, P2, P3;
        Coordinate Expected;

        TestCase(string&& name,
            const TPoint& p1,
            const TPoint& p2,
            const TPoint& p3,
            Coordinate expected)
            : base_test_case(forward<string>(name)),
            P1(p1), P2(p2), P3(p3), Expected(expected)
        {
        }

        void Print(ostream& str) const override
        {
            base_test_case::Print(str);

            str << " P1=" << P1
                << ", P2=" << P2
                << ", P3=" << P3
                << ", Expected=" << Expected;
        }
    };

    TPoint NegateX(const TPoint& point)
    {
        return{ -point.X, point.Y };
    }

    void GenerateTestCases(vector<TestCase>& testCases)
    {
        const TestCase simple = { "Simple",{ 1,2 },{ 10, 30 },{ 7, 50 }, 264 };

        //Changing the sign of X coordinates must negate the result.
        const TestCase simpleNegated = { "SimpleNegated",
          NegateX(simple.P1),
          NegateX(simple.P2),
          NegateX(simple.P3),
          -simple.Expected };

        testCases.insert(testCases.end(),
            {
              simple,
              simpleNegated,
              { "ZeroTrivial",{ 1,1 },{ 2,2 },{ 5,5 }, 0 },
              { "ZeroTwo",{ 1,1 },{ 2, 4 },{ 5, 13 }, 0 },
            });
    }

    void RunTestCase(
        const TestCase& testCase)
    {
        const auto actual = calc_angle<Coordinate>(
            testCase.P1,
            testCase.P2,
            testCase.P3);

        Assert::AreEqual(testCase.Expected, actual, "Expected");
    }
}

void Privet::Algorithms::Geometry::Tests::CalcAngleTests()
{
    test_utilities<TestCase>::run_tests(RunTestCase, GenerateTestCases);
}