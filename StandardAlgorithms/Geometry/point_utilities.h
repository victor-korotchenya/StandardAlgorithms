#pragma once
#include <cmath>
#include "epsilon_default.h"

namespace Privet
{
    namespace Algorithms
    {
        namespace Geometry
        {
            template<class product_t, class vector_t>
            product_t dot_product(const vector_t& a, const vector_t& b)
            {
                static_assert(sizeof(product_t) >= sizeof(vector_t::distance_t));

                const auto pr = static_cast<product_t>(static_cast<product_t>(a.X) * b.X +
                    static_cast<product_t>(a.Y) * b.Y);
                return pr;
            }

            template<class product_t, class vector_t>
            product_t cross_product(const vector_t& a, const vector_t& b)
            {
                static_assert(sizeof(product_t) >= sizeof(vector_t::distance_t));

                const auto pr = static_cast<product_t>(static_cast<product_t>(a.X) * b.Y -
                    static_cast<product_t>(a.Y) * b.X);
                return pr;
            }

            // a->b . a->c
            template<typename long_int_t, typename point_t>
            long_int_t cross_product3(const point_t& a,
                const point_t& b, const point_t& c)
            {
                const auto ab = a - b, ac = a - c;
                const auto r = cross_product<long_int_t>(ab, ac);
                return r;
            }

            template<class product_t, class point_t>
            product_t dist2(const point_t& a)
            {
                static_assert(sizeof(product_t) >= sizeof(point_t::distance_t));

                product_t x = a.X, y = a.Y;
                const auto res = static_cast<product_t>(x * x + y * y);
                return res;
            }

            //Calculate the angle/orientation between 3 points using cross-product.
            //   p3 - counterclockwise (the angle increases).
            //    p2
            // p1
            //
            //      p3 - clockwise (the angle decreases).
            //  p2
            // p1
            //
            //  if (result == 0) return 0;  // collinear
            //  return result > 0 ? 1 : -1; // counterclockwise (the angle increases) or clockwise.
            template <class long_int_t, class point_t>
            long_int_t calc_angle(const point_t& p1, const point_t& p2, const point_t& p3)
            {
                static_assert(sizeof(long_int_t) >= sizeof(point_t::distance_t));

                // angle(p1, p2) = (y2 - y1)/(x2 - x1)
                // angle(p1, p3) = (y3 - y1)/(x3 - x1)
                // difference = [angle(p1, p3) - angle(p1, p2)] * (x3 - x1) * (x2 - x1) =
                //  = (y3 - y1)*(x2 - x1) - (y2 - y1)*(x3 - x1).
                //
                // Another view via vectors: p1->p2, p1->p3
                // The cross product: (p2-p1).X * (p3-p1).Y - (p2-p1).Y * (p3-p1).X

                const long_int_t result = static_cast<long_int_t>(p3.Y - p1.Y) * (p2.X - p1.X)
                    - static_cast<long_int_t>(p2.Y - p1.Y) * (p3.X - p1.X);
                return result;
            }

            // Whether ab and ac are collinear.
            template <class vector_t, class distance_t = typename vector_t::distance_t>
            bool are_collinear(const vector_t& ab, const vector_t& ac, const distance_t epsilon = epsilon_default<distance_t>())
            {
                static_assert(std::is_floating_point_v<distance_t>);
                assert(epsilon > 0);

                const auto cr = cross_product<distance_t>(ab, ac);
                const auto result = fabs(cr) < epsilon;
                return result;
            }

            // Whether c lies in the line a->b.
            template <class point_t, class distance_t = typename point_t::distance_t>
            bool are_collinear(const point_t& a, const point_t& b, const point_t& c, const distance_t epsilon = epsilon_default<distance_t>())
            {
                static_assert(std::is_floating_point_v<distance_t>);
                assert(epsilon > 0);

                const auto ab = a - b, ac = a - c;
                const auto result = are_collinear<decltype(ab), distance_t>(ab, ac, epsilon);
                return result;
            }

            // Compute the angle abc in radians.
            template <class vector_t, class distance_t = typename vector_t::distance_t>
            distance_t calc_angle_radian(const vector_t& ba, const vector_t& bc)
            {
                static_assert(std::is_floating_point_v<distance_t>);

                const distance_t pr = dot_product<distance_t>(ba, bc),
                    ba2 = dist2<distance_t>(ba),
                    bc2 = dist2<distance_t>(bc),
                    s = static_cast<distance_t>(sqrt(ba2 * bc2));
                assert(s > 0);
                if (s == 0)
                    throw std::runtime_error("Vector length cannot be 0 in calc_angle_radian");

                distance_t ac = static_cast<distance_t>(pr / s);
                if (ac > 1)
                    ac = 1;
                else if (ac < -1)
                    ac = -1;

                const auto result = static_cast<distance_t>(acos(ac));
                return result;
            }

            // Compute the angle abc in radians.
            template <class point_t, class distance_t = typename point_t::distance_t>
            distance_t calc_angle_radian(
                const point_t& a,
                const point_t& b,
                const point_t& c)
            {
                static_assert(std::is_floating_point_v<distance_t>);
                assert(!(a == b) && !(b == c));

                const auto ba = b - a, bc = b - c;
                const auto result = calc_angle_radian<decltype(ba), distance_t>(ba, bc);
                return result;
            }

            template <typename Coordinate, typename Point>
            bool is_along_side(
                const Point& checkPoint,
                const Point& previous,
                const Point& current,
                bool& positive,
                bool& negative)
            {
                const auto angle = calc_angle<Coordinate, Point>(checkPoint, previous, current);
                if (0 == angle)
                    return true;

                (0 < angle ? negative : positive) = false;

                return false;
            }

            template <class point_t,
                //long double
                class double_t>
                point_t rotate(const point_t& point, const double_t theta)
            {
                static_assert(std::is_floating_point_v<typename point_t::distance_t> && std::is_floating_point_v<double_t>);

                const auto cos1 = std::cos(theta);
                const auto sin1 = std::sin(theta);

                point_t res(point.X * cos1 - point.Y * sin1,
                    point.X * sin1 + point.Y * cos1);
                return res;
            }

            template <class vector_t, class double_t>
            vector_t scale(const vector_t& v, const double_t s)
            {
                return vector_t(v.X * s, v.Y * s);
            }

            template <class point_t, class vector_t>
            point_t translate(const point_t& p, const vector_t& v)
            {
                return point_t(p.X + v.X, p.Y + v.Y);
            }

            // Points a and b, defining a line, must differ.
            template <class point_t, class distance_t = typename point_t::distance_t>
            point_t closest_point_to_line(const point_t& point,
                const point_t& a, const point_t& b)
            {
                static_assert(std::is_floating_point_v<distance_t>);
                assert(!(a == b));

                //     p
                //    ^
                //  /
                // a ->d -> b
                //
                // |ad| = |ap| * cos(pad)
                // ap . ab = |ap| * |ab| * cos(pab) = |ab| * |ad|
                // |ad| = ap . ab / |ab|
                // coef = |ad| / |ab| = ap . ab / |ab|**2
                //
                // closest = a + coef * ab
                const auto ap = a - point, ab = a - b;
                const auto ab2 = dist2<distance_t>(ab);
                assert(ab2 > 0);

                const auto coef = static_cast<distance_t>(dot_product<distance_t>(ap, ab) / ab2);
                const auto ad = scale(ab, coef);
                const auto closest = translate<point_t>(a, ad);
                return closest;
            }

            // A line segment has 2 different points.
            // Note. The point might lie on the line.
            template <class point_t, class distance_t = typename point_t::distance_t>
            point_t closest_point_to_line_segment(const point_t& point,
                const point_t& a, const point_t& b)
            {
                static_assert(std::is_floating_point_v<distance_t>);
                assert(!(a == b));

                const auto ap = a - point, ab = a - b;
                const auto ab2 = dist2<distance_t>(ab);
                assert(ab2 > 0);

                const auto coef = static_cast<distance_t>(dot_product<distance_t>(ap, ab) / ab2);
                if (coef < 0)
                    return a;

                if (coef > 1)
                    return b;

                const auto closest = closest_point_to_line<point_t, distance_t>(point, a, b);
                return closest;
            }
        }
    }
}