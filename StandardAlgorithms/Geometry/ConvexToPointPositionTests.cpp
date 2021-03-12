#include <vector>
#include <utility>
#include <tuple>
#include "ConvexToPointPositionTests.h"
#include "GeometryUtilities.h"
#include "point.h"
#include "../test_utilities.h"

using namespace std;
using namespace Privet::Algorithms::Geometry;
using namespace Privet::Algorithms;

namespace
{
    using Coordinate = int;
    using TPoint = point2d<Coordinate>;
    using PointPositionSide = tuple<TPoint, PointPosition, size_t>;
    using TestData = tuple<TPoint, TPoint, TPoint, vector<PointPositionSide>>;

    TestData CreateTestCases()
    {
        // p3
        //
        //        middle
        //
        // p1                  p2

        constexpr Coordinate shift = 123;
        constexpr Coordinate quarter = 5;

        const TPoint p1{ shift,shift },
            p2{ shift + quarter * 4, shift },
            p3{ shift,shift + quarter * 2 };

        constexpr auto x = shift + quarter * 2;
        constexpr auto y = shift + quarter;

        const vector<PointPositionSide> pointPositionSides{
          PointPositionSide({ x,y - 1 }, PointPosition::Inside, 0),
          PointPositionSide({ x,y },
            PointPosition::AlongSide, 1 //Along points p2 and p3.
            ),
          PointPositionSide({ x,y + 1 }, PointPosition::Outside, 0)
        };

        return TestData(p1, p2, p3, pointPositionSides);
    }

    void RunPointPosition(
        const PointPositionSide& pointPositionSide,
        const TPoint& p1,
        const TPoint& p2,
        const TPoint& p3)
    {
        const auto& point = get<0>(pointPositionSide);
        const auto& position = get<1>(pointPositionSide);
        const auto& side = get<2>(pointPositionSide);

        ostringstream ss;
        ss << "Point= " << point
            << ", Position= " << position
            << ", side= " << side;
        const auto description = ss.str();

        const auto actual = ConvexToPointPosition<Coordinate>(
            point, { p1, p2, p3 });
        Assert::AreEqual(position, actual.first, description);
        Assert::AreEqual(side, actual.second, description);
    }
}

void Privet::Algorithms::Geometry::Tests::ConvexToPointPositionTests()
{
    const auto data = CreateTestCases();
    const auto& p1 = get<0>(data);
    const auto& p2 = get<1>(data);
    const auto& p3 = get<2>(data);
    const auto& pointPositionSides = get<3>(data);

    for (const auto& p : pointPositionSides)
    {
        RunPointPosition(p, p1, p2, p3);
    }
}