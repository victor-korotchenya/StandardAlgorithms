#pragma once
#include <iomanip> // setprecision
#include "../Utilities/StreamUtilities.h"

namespace Privet
{
    namespace Algorithms
    {
        namespace Geometry
        {
            template <class TDistance>
            struct point2d final
            {
                //Row.
                TDistance X;

                //Column.
                TDistance Y;

                using distance_t = TDistance;

                constexpr point2d(TDistance x = {}, TDistance y = {})
                    : X(x), Y(y)
                {
                }

                bool operator < (const point2d& b) const
                {
                    const bool result = (X < b.X)
                        || (X == b.X) && (Y < b.Y);
                    return result;
                }

                bool operator == (point2d const& b) const
                {
                    const bool result = X == b.X && Y == b.Y;
                    return result;
                }

                friend std::ostream& operator <<
                    (std::ostream& str, point2d const& b)
                {
                    const auto prec = StreamUtilities::GetMaxDoublePrecision();

                    str << std::setprecision(prec)
                        << "X=" << b.X
                        << ", Y=" << b.Y;
                    return str;
                }
            };

            template <class TDistance>
            struct vector2d final
            {
                //Row.
                TDistance X;

                //Column.
                TDistance Y;

                using distance_t = TDistance;

                constexpr vector2d(TDistance x = {}, TDistance y = {})
                    : X(x), Y(y)
                {
                }

                constexpr vector2d(const point2d<TDistance>& a, const point2d<TDistance>& b)
                    : X(b.X - a.X), Y(b.Y - a.Y)
                {
                }

                bool operator == (vector2d const& b) const
                {
                    const bool result = X == b.X && Y == b.Y;
                    return result;
                }

                friend std::ostream& operator <<
                    (std::ostream& str, vector2d const& b)
                {
                    const auto prec = StreamUtilities::GetMaxDoublePrecision();

                    str << std::setprecision(prec)
                        << "X=" << b.X
                        << ", Y=" << b.Y;
                    return str;
                }
            };

            template <typename TDistance>
            vector2d<TDistance> operator - (const point2d<TDistance>& a, const point2d<TDistance>& b)
            {
                const vector2d<TDistance> result(a, b);
                return result;
            }

            template <class TPoint, class TWeight>
            struct PointAndWeight final
            {
                TPoint Point{};
                TWeight Weight{};

                bool operator < (const PointAndWeight& b) const
                {
                    return Point < b.Point;
                }

                void Validate() const
                {
                    if (Weight <= 0)
                    {
                        std::ostringstream ss;
                        ss << "The Weight (" << Weight
                            << ") must be positive."
                            << " Point=(" << Point << ")"
                            << ".";
                        StreamUtilities::throw_exception(ss);
                    }
                }

                friend std::ostream& operator <<
                    (std::ostream& str, PointAndWeight const& b)
                {
                    const auto prec = StreamUtilities::GetMaxDoublePrecision();

                    str << std::setprecision(prec)
                        << Point << ", Weight=" << b.Weight;
                    return str;
                }
            };
        }
    }
}

namespace std
{
    template <typename TDistance>
    struct hash<Privet::Algorithms::Geometry::point2d<TDistance>>
    {
        size_t operator()(const Privet::Algorithms::Geometry::point2d<TDistance>& p) const
        {
            const size_t a = std::hash<TDistance>()(p.X),
                b = std::hash<TDistance>()(p.Y),
                c = (a | (a << 5)) ^ b;
            return c;
        }
    };

    template <typename TDistance>
    struct hash<Privet::Algorithms::Geometry::vector2d<TDistance>>
    {
        size_t operator()(const Privet::Algorithms::Geometry::vector2d<TDistance>& p) const
        {
            const size_t a = std::hash<TDistance>()(p.X),
                b = std::hash<TDistance>()(p.Y),
                c = (a | (a << 5)) ^ b;
            return c;
        }
    };

    template <typename TPoint, typename TWeight>
    struct hash<Privet::Algorithms::Geometry::PointAndWeight<TPoint, TWeight>>
    {
        size_t operator()(const Privet::Algorithms::Geometry::PointAndWeight<TPoint, TWeight>& p) const
        {
            const size_t a = std::hash<TPoint>()(p.Point),
                b = std::hash<TWeight>()(p.Weight),
                c = (a | (a << 5)) ^ b;
            return c;
        }
    };

    template <class value_t>
    std::string to_string(const Privet::Algorithms::Geometry::point2d<value_t>& p)
    {
        std::stringstream ss;
        ss << p;
        return ss.str();
    }

    template <class TPoint, class TWeight>
    std::string to_string(const Privet::Algorithms::Geometry::PointAndWeight<TPoint, TWeight>& p)
    {
        std::stringstream ss;
        ss << p;
        return ss.str();
    }

    template <class value_t>
    std::string to_string(const Privet::Algorithms::Geometry::vector2d<value_t>& p)
    {
        std::stringstream ss;
        ss << p;
        return ss.str();
    }
}