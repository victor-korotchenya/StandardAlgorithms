#include "../Assert.h"
#include "PointTests.h"
#include "../Geometry/point.h"
#include "../Geometry/rectangle2d.h"

using namespace std;
using namespace Privet::Algorithms::Geometry;
using namespace Privet::Algorithms;

namespace
{
    void area_tests()
    {
        using TSize = unsigned int;
        using TSizeSquared = size_t;
        using TPoint = point2d<TSize>;
        using TRectangle = rectangle2d<TSize, TSizeSquared, TPoint>;

        {
            const TRectangle rectangle = TRectangle();
            const TSizeSquared area = rectangle.Area();
            Assert::AreEqual< TSizeSquared>(0, area, "Zero area1");
        }
        {
            const TPoint point = TPoint(3, 5);
            const TRectangle rectangle = TRectangle(point, point);
            const TSizeSquared area = rectangle.Area();
            Assert::AreEqual< TSizeSquared>(0, area, "Zero area2");
        }
        {
            const TPoint top = TPoint(3, 5);
            const TPoint bottom = TPoint(top.X + 1, top.Y);
            const TRectangle rectangle = TRectangle(top, bottom);
            const TSizeSquared area = rectangle.Area();
            Assert::AreEqual< TSizeSquared>(0, area, "Zero area3");
        }
        {
            const TSize delta = 1;
            const TPoint top = TPoint(3, 5);
            const TPoint bottom = TPoint(top.X, top.Y + delta);
            const TRectangle rectangle = TRectangle(top, bottom);
            const TSizeSquared area = rectangle.Area();
            Assert::AreEqual< TSizeSquared>(0, area, "Zero area4");
        }
        {
            const TSize delta = 1;
            const TPoint top = TPoint(30, 571);
            const TPoint bottom = TPoint(top.X + delta, top.Y + delta);
            const TRectangle rectangle = TRectangle(top, bottom);
            const TSizeSquared area = rectangle.Area();
            Assert::AreEqual< TSizeSquared>(delta * delta, area, "Area5");
        }
        {
            const TSize delta = 17;
            const TPoint top = TPoint(1, 572);
            const TPoint bottom = TPoint(top.X + delta, top.Y + delta);
            const TRectangle rectangle = TRectangle(top, bottom);
            const TSizeSquared area = rectangle.Area();
            Assert::AreEqual< TSizeSquared>(delta * delta, area, "Area6");
        }
        {
            const TSize deltaX = 17;
            const TSize deltaY = 256;
            const TPoint top = TPoint(102, 57209);
            const TPoint bottom = TPoint(top.X + deltaX, top.Y + deltaY);
            const TRectangle rectangle = TRectangle(top, bottom);
            const TSizeSquared area = rectangle.Area();
            Assert::AreEqual< TSizeSquared>(deltaX * deltaY, area, "Area7");
        }
    }
}

void PointTests::Test()
{
    area_tests();
}