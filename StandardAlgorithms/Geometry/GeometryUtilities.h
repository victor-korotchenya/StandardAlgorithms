#pragma once
#include <vector>
#include "../Utilities/StreamUtilities.h"
#include "point_utilities.h"

namespace Privet
{
    namespace Algorithms
    {
        namespace Geometry
        {
            enum class PointPosition
            {
                //The point is strictly outside the region.
                Outside,
                //The point is strictly inside the region.
                Inside,
                //The point belongs to a side.
                AlongSide,
            };

            inline std::ostream& operator << (
                std::ostream& str, PointPosition const& position)
            {
                switch (position)
                {
                case PointPosition::Outside: str << "Outside"; break;
                case PointPosition::Inside: str << "Inside"; break;
                case PointPosition::AlongSide: str << "Along a side"; break;
#ifdef _DEBUG
                default:
                {
                    std::ostringstream ss;
                    ss << "Unknown value of PointPosition="
                        << std::to_string(static_cast<int>(position));

                    StreamUtilities::throw_exception<std::out_of_range>(ss);
                }
#endif
                }

                return str;
            }

            //Computes relative position of a point
            // to a convex region consisting of 3 or more points.
            //
            //The first point is to be checked whether it is inside a convex.
            //The remaining points are assumed to form a convex.
            //All the points must be distinct.
            //
            //When the "first" of the returned value is "AlongSide",
            // the "second" is which segment the "checkPoint" belongs to.
            template <class Coordinate, class Point>
            std::pair<PointPosition, size_t> ConvexToPointPosition(
                const Point& checkPoint,
                const std::vector<Point>& otherPoints)
            {
                const size_t size = otherPoints.size();
                constexpr size_t minSize = 3;
                if (size < minSize)
                {
                    std::ostringstream ss;
                    ss << "ConvexPosition: The size("
                        << size << ") must be at least " << minSize << ".";
                    StreamUtilities::throw_exception<std::out_of_range>(ss);
                }

                auto position = PointPosition::Outside;
                size_t index = 0;
                auto positive = true;
                auto negative = true;

                const Point firstPoint = otherPoints[0];
                auto previous = firstPoint;

                for (;;)
                {
                    const Point current = otherPoints[1 + index];

                    if (is_along_side<Coordinate, Point>(
                        checkPoint, previous, current,
                        positive, negative))
                    {
                        position = PointPosition::AlongSide;
                        break;
                    }

                    if (size - 1 //The first point is already taken.
                        == ++index)
                    {//Check last - first.
                        if (is_along_side<Coordinate, Point>(
                            checkPoint, current, firstPoint,
                            positive, negative))
                        {
                            position = PointPosition::AlongSide;
                        }

                        break;
                    }

                    previous = current;
                }

                if (PointPosition::AlongSide == position)
                {
#ifdef _DEBUG
                    if (!(positive || negative))
                    {
                        throw std::runtime_error(
                            "The point is Outside and AlongSide - the convex might be concave.");
                    }
#endif
                    return { PointPosition::AlongSide, index };
                }

                if (positive || negative)
                {
                    return{ PointPosition::Inside, 0 };
                }

                return{ PointPosition::Outside, 0 };
            }

            // Points are distinct.
            template<class product_t, class point_t>
            bool is_simple_convex_polygon(const std::vector<point_t>& points)
            {
                const auto size = static_cast<int>(points.size());
                auto prev_angle = 0;

                for (auto i = 0; i < size; i++)
                {
                    const auto& a = points[i], & b = points[(i + 1) % size], & c = points[(i + 2) % size];
                    assert(!(a == b) && !(b == c) && !(a == c));

                    const auto angle = calc_angle<product_t, point_t>(a, b, c);
                    if (!angle)
                        continue;

                    if (!prev_angle)
                    {
                        prev_angle = angle > 0 ? 1 : -1;
                        continue;
                    }

                    if (prev_angle * angle < 0)
                        return false; // Orientation change is not allowed.
                }

                return true;
            }
        }
    }
}