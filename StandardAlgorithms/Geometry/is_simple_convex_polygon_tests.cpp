#include "GeometryUtilities.h"
#include "is_simple_convex_polygon_tests.h"
#include "point.h"
#include "../Utilities/PrintUtilities.h"
#include "../test_utilities.h"

using namespace std;
using namespace Privet::Algorithms::Geometry::Tests;
using namespace Privet::Algorithms::Geometry;
using namespace Privet::Algorithms::Tests;
using namespace Privet::Algorithms;

namespace
{
    using Coordinate = short;
    using point_t = point2d<Coordinate>;

    struct test_case final : base_test_case
    {
        const vector<point_t> points;
        bool expected;

        test_case(string&& name,
            vector<point_t>&& points,
            bool expected)
            : base_test_case(forward<string>(name)),
            points(forward<vector<point_t>>(points)),
            expected(expected)
        {
        }

        void Print(ostream& str) const override
        {
            base_test_case::Print(str);
            ::Print("points", points, str);
            str << ", expected=" << expected;
        }
    };

    void generate_test_cases(vector<test_case>& test_cases)
    {
        {
            test_cases.emplace_back("quad", vector<point_t>{
                {1,1},
                {2,1},
                {2,2},
                {1,2},
            }, true);
        }
        {
            test_cases.emplace_back("zigzag", vector<point_t> {
                {1,1},
                {2,2},
                {2,1},
                {1,2},
            }, false);
        }
    }

    void run_test_case(const test_case& testCase)
    {
        const auto actual = is_simple_convex_polygon<int64_t>(testCase.points);
        Assert::AreEqual(testCase.expected, actual, "expected");
    }
}

void Privet::Algorithms::Geometry::Tests::is_simple_convex_polygon_tests()
{
    test_utilities<test_case>::run_tests(run_test_case, generate_test_cases);
}