#pragma once
#include <cmath>
#include <iomanip> // setprecision
#include "epsilon_default.h"
#include "../Utilities/StreamUtilities.h"

namespace Privet
{
    namespace Algorithms
    {
        namespace Geometry
        {
            // A * x + B * y + C = 0.
            // A vertical line has A=1, B=0,
            // otherwise B=1.
            template <class value_t>
            struct line2d final
            {
                value_t A, B, C;

                using distance_t = value_t;

                constexpr line2d(value_t a = {}, value_t b = {}, value_t c = {})
                    : A(a), B(b), C(c)
                {
                    check();
                }

                bool check() const
                {
                    const auto good = B == 0 && A == 1 || B == 1;
                    assert(good);
                    return good;
                }

                friend std::ostream& operator <<
                    (std::ostream& str, line2d const& b)
                {
                    const auto prec = StreamUtilities::GetMaxDoublePrecision();

                    str << std::setprecision(prec)
                        << "A=" << b.A
                        << ", B=" << b.B
                        << ", C=" << b.C;
                    return str;
                }
            };

            template<class point_t, class distance_t = typename point_t::distance_t,
                class line_t = line2d<distance_t>>
                line_t to_line(const point_t& p, const point_t& p2,
                    distance_t epsilon = epsilon_default<distance_t>())
            {
                static_assert(std::is_floating_point_v<distance_t>);
                assert(epsilon > 0);

                const distance_t dx = p2.X - p.X;
                if (fabs(dx) < epsilon) // vertical line
                    return line_t(1, 0, -p.X);

                assert(dx != 0);
                // a * x + b * y + c = 0, where b=1.
                //
                // a * p.x + p.y + c = 0.
                // a * p2.x + p2.y + c = 0.

                const distance_t a = (p.Y - p2.Y) / dx,
                    c = -a * p.X - p.Y;
                return line_t(a, 1, c);
            }
        }
    }
}

namespace std
{
    template <class value_t>
    struct hash<Privet::Algorithms::Geometry::line2d<value_t>>
    {
        size_t operator()(const Privet::Algorithms::Geometry::line2d<value_t>& line) const
        {
            const size_t a = std::hash<value_t>()(line.A),
                b = std::hash<value_t>()(line.B),
                c = std::hash<value_t>()(line.C),
                d = (a | a << 5) ^ b,
                e = (d | d << 5) ^ c;
            return e;
        }
    };

    template <class value_t>
    std::string to_string(const Privet::Algorithms::Geometry::line2d<value_t>& line)
    {
        std::stringstream ss;
        ss << line;
        return ss.str();
    }
}