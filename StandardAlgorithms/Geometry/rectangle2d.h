#pragma once
#include <iomanip> // setprecision

#include "../Utilities/StreamUtilities.h"

namespace Privet
{
    namespace Algorithms
    {
        namespace Geometry
        {
            template <typename TDistance,
                typename TDistanceSquared,
                typename TPoint>
                struct rectangle2d final
            {
                static_assert(sizeof(TDistance) <= sizeof(TDistanceSquared));

                rectangle2d(
                    //The rightBottom must have coordinates greater than that of leftTop.
                    TPoint leftTop = {},
                    TPoint rightBottom = {})
                    : _LeftTop(leftTop), _RightBottom(rightBottom)
                {
                    Check(leftTop, rightBottom);
                }

                TPoint LeftTop() const
                {
                    return _LeftTop;
                }

                TPoint RightBottom() const
                {
                    return _RightBottom;
                }

                TDistance Height() const
                {
                    const TDistance result = _RightBottom.X - _LeftTop.X;
                    return result;
                }

                TDistance Width() const
                {
                    const TDistance result = _RightBottom.Y - _LeftTop.Y;
                    return result;
                }

                TDistanceSquared Area() const
                {
                    const TDistanceSquared x = TDistanceSquared(Height());
                    const TDistanceSquared y = TDistanceSquared(Width());

                    const TDistanceSquared result = x * y;
                    return result;
                }

                static void Check(
                    //The rightBottom must have coordinates greater than that of leftTop.
                    const TPoint& leftTop,
                    const TPoint& rightBottom)
                {
                    //#ifdef _DEBUG
                    if (rightBottom.X < leftTop.X)
                    {
                        std::ostringstream ss;
                        ss << "Error in rectangle2d: rightBottom.X (" << rightBottom.X
                            << ") < leftTop.X (" << leftTop.X << ").";
                        StreamUtilities::throw_exception(ss);
                    }

                    if (rightBottom.Y < leftTop.Y)
                    {
                        std::ostringstream ss;
                        ss << "Error in rectangle2d: rightBottom.Y (" << rightBottom.Y
                            << ") < leftTop.Y (" << leftTop.Y << ").";
                        StreamUtilities::throw_exception(ss);
                    }
                }

                bool operator == (rectangle2d const& b) const
                {
                    const bool result = (_LeftTop == b._LeftTop)
                        && (_RightBottom == b._RightBottom);
                    return result;
                }

                friend std::ostream& operator <<
                    (std::ostream& str, rectangle2d const& b)
                {
                    const auto prec = StreamUtilities::GetMaxDoublePrecision();

                    str << std::setprecision(prec)
                        << "LeftTop = (" << b.LeftTop()
                        << "), RightBottom = (" << b.RightBottom() << ")";
                    return str;
                }

            private:
                TPoint _LeftTop, _RightBottom;
            };
        }
    }
}

namespace std
{
    template <typename TDistance, typename TDistanceSquared, typename TPoint>
    struct hash<Privet::Algorithms::Geometry::rectangle2d<TDistance, TDistanceSquared, TPoint>>
    {
        size_t operator()(const Privet::Algorithms::Geometry::rectangle2d<TDistance, TDistanceSquared, TPoint>& p) const
        {
            const size_t a = std::hash<TPoint>()(p.LeftTop()),
                b = std::hash<TPoint>()(p.RightBottom()),
                c = (a | (a << 5)) ^ b;
            return c;
        }
    };
}