#pragma once
#include <cassert>
#include <cmath>
#include "epsilon_default.h"

namespace Privet
{
    namespace Algorithms
    {
        namespace Geometry
        {
            template<class line_t, class distance_t = typename line_t::distance_t>
            bool are_parallel(const line_t& line, const line_t& line2,
                distance_t epsilon = epsilon_default<distance_t>())
            {
                static_assert(std::is_floating_point_v<distance_t>);
                assert(epsilon > 0 && line.check() && line2.check());

                const auto dx = line.A - line2.A,
                    dy = line.B - line2.B;
                const auto result = fabs(dx) < epsilon && fabs(dy) < epsilon;
                return result;
            }

            template<class line_t, class distance_t = typename line_t::distance_t>
            bool are_equal(const line_t& line, const line_t& line2,
                distance_t epsilon = epsilon_default<distance_t>())
            {
                const auto result = are_parallel<line_t, distance_t>(line, line2, epsilon) &&
                    fabs(line.C - line2.C) < epsilon;
                return result;
            }

            // If two lines neither parallel nor same, they intersect at a point:
            // a*x + b*y + c = a2*x + b2*y + c2 = 0.
            // y = -a*x/b - c/b
            // 0 = a2*x + b2*y + c2 = a2*x + -a*x/b*b2 - c/b*b2 + c2
            // x = (c/b*b2 - c2) / (a2 - a*b2/b) = (c*b2 - c2*b) / (a2*b - a*b2)
            // y = -a*x/b - c/b
            template<class line_t, class point_t, class distance_t = typename line_t::distance_t>
            bool intersect_at_one_point(const line_t& line, const line_t& line2,
                point_t& p, distance_t epsilon = epsilon_default<distance_t>())
            {
                if (are_parallel<line_t, distance_t>(line, line2, epsilon))
                    return false;

                const auto d = line2.A * line.B - line.A * line2.B;
                assert(d != 0);

                // x = (c*b2 - c2*b) / (a2*b - a*b2)
                p.X = (line.C * line2.B - line2.C * line.B) / d;

                p.Y = fabs(line.B) < epsilon
                    // A vertical line
                    ? -line2.A * p.X - line2.C
                    : // y = -a*x/b - c/b
                    p.Y = -line.A * p.X - line.C;

                return true;
            }
        }
    }
}