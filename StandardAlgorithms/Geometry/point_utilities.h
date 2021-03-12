#pragma once
#include"../Utilities/clang_constexpr.h"
#include"../Utilities/epsilon_default.h"
#include<cmath>
#include<concepts>
#include<stdexcept>
#include<utility>

namespace Standard::Algorithms::Geometry
{
    template<class long_t, class vector_t>
    requires(sizeof(typename vector_t::distance_t) <= sizeof(long_t))
    [[nodiscard]] constexpr auto dot_product(const vector_t &one, const vector_t &two) -> long_t
    {
        auto prod = static_cast<long_t>(static_cast<long_t>(one.x) * two.x + static_cast<long_t>(one.y) * two.y);

        return prod;
    }

    template<class long_t, class vector_t>
    requires(sizeof(typename vector_t::distance_t) <= sizeof(long_t))
    [[nodiscard]] constexpr auto cross_product(const vector_t &one, const vector_t &two) -> long_t
    {
        auto pro = static_cast<long_t>(static_cast<long_t>(one.x) * two.y - static_cast<long_t>(one.y) * two.x);

        return pro;
    }

    // a->b . a->c
    template<class long_t, class point_t, class point_t2>
    requires(sizeof(typename point_t::distance_t) <= sizeof(long_t) &&
        sizeof(typename point_t2::distance_t) <= sizeof(long_t))
    [[nodiscard]] constexpr auto cross_product3(const point_t &one, const point_t2 &two, const point_t &three) -> long_t
    {
        const auto a_b = two - one;
        const auto a_c = three - one;
        auto pro = cross_product<long_t>(a_b, a_c);
        return pro;
    }

    template<class long_t, class point_t>
    requires(sizeof(typename point_t::distance_t) <= sizeof(long_t))
    [[nodiscard]] constexpr auto dist2(const point_t &one) -> long_t
    {
        auto res = static_cast<long_t>(static_cast<long_t>(one.x) * static_cast<long_t>(one.x) +
            static_cast<long_t>(one.y) * static_cast<long_t>(one.y));

        return res;
    }

    template<class long_t, class point_t>
    [[nodiscard]] constexpr auto distance(const point_t &one) -> long_t
    {
        const auto squ = dist2<long_t, point_t>(one);
        auto root = std::sqrt(squ); // todo(p2): use a correct sqrt.
        return static_cast<long_t>(root);
    }

    // Calculate the angle/orientation between 3 points using cross-product.
    //    pt3 - counterclockwise (the angle increases).
    //     pt2
    //  pt1
    //
    //       pt3 - clockwise (the angle decreases).
    //   pt2
    //  pt1
    //
    //  if (result == 0) return 0;  // collinear
    //    return 0 < result ? 1 : -1; // counterclockwise (the angle increases) or clockwise.
    template<class long_t, class point_t, class point_t2>
    requires(sizeof(typename point_t::distance_t) <= sizeof(long_t) &&
        sizeof(typename point_t2::distance_t) <= sizeof(long_t))
    [[nodiscard]] constexpr auto calc_angle(const point_t &pt1, const point_t2 &pt2, const point_t &pt3) -> long_t
    {
        // angle(pt1, pt2) = (y2 - y1)/(x2 - x1)
        // angle(pt1, pt3) = (y3 - y1)/(x3 - x1)
        // difference = [angle(pt1, pt3) - angle(pt1, pt2)] * (x3 - x1) * (x2 - x1) =
        //  = (y3 - y1)*(x2 - x1) - (y2 - y1)*(x3 - x1).
        //
        // Another view via vectors: pt1->pt2, pt1->pt3
        // The cross product: (pt2-pt1).x * (pt3-pt1).y - (pt2-pt1).y * (pt3-pt1).x

        auto result = static_cast<long_t>(static_cast<long_t>(pt3.y - pt1.y) * (pt2.x - pt1.x) -
            static_cast<long_t>(pt2.y - pt1.y) * (pt3.x - pt1.x));

        return result;
    }

    // Whether ab and ac are collinear.
    template<class vector_t, std::floating_point distance_t = typename vector_t::distance_t>
    [[nodiscard]] constexpr auto are_collinear(
        const vector_t &a_b, const vector_t &a_c, const distance_t epsilon = epsilon_default<distance_t>()) -> bool
    {
        assert(distance_t{} < epsilon);

        const auto cro = cross_product<distance_t>(a_b, a_c);
        auto result = ::Standard::Algorithms::fabs(cro) < epsilon;
        return result;
    }

    // Whether c lies in the line a->b.
    template<class point_t, class point_t2, std::floating_point distance_t = typename point_t::distance_t>
    [[nodiscard]] constexpr auto are_collinear(const point_t &one, const point_t2 &two, const point_t &three,
        const distance_t epsilon = epsilon_default<distance_t>()) -> bool
    {
        assert(distance_t{} < epsilon);

        const auto a_b = two - one;
        const auto a_c = three - one;

        auto result = are_collinear<decltype(a_b), distance_t>(a_b, a_c, epsilon);

        return result;
    }

    // Compute the angle abc in radians.
    template<class vector_t, std::floating_point distance_t = typename vector_t::distance_t>
    [[nodiscard]] constexpr auto calc_angle_radian(const vector_t &b_a, const vector_t &b_c) -> distance_t
    {
        const auto pro = dot_product<distance_t>(b_a, b_c);
        const auto ba2 = dist2<distance_t>(b_a);
        const auto bc2 = dist2<distance_t>(b_c);
        const auto dis = static_cast<distance_t>(std::sqrt(ba2 * bc2));
        assert(distance_t{} < dis);

        if (dis == distance_t{}) [[unlikely]]
        {
            throw std::runtime_error("Vector length cannot be 0 in calc_angle_radian");
        }

        auto a_c = static_cast<distance_t>(pro / dis);

        if (1 < a_c)
        {
            a_c = 1;
        }
        else if (a_c < -1)
        {
            a_c = -1;
        }

        auto result = static_cast<distance_t>(std::acos(a_c));
        return result;
    }

    // Compute the angle abc in radians.
    template<class point_t, std::floating_point distance_t = typename point_t::distance_t>
    [[nodiscard]] constexpr auto calc_angle_radian(const point_t &one, const point_t &two, const point_t &three)
        -> distance_t
    {
        assert(!(one == two) && !(two == three));

        const auto b_a = one - two;
        const auto b_c = three - two;
        auto result = calc_angle_radian<decltype(b_a), distance_t>(b_a, b_c);

        return result;
    }

    template<class int_t, class point_t>
    [[nodiscard]] constexpr auto is_along_side(const point_t &check_point, const point_t &previous,
        const point_t &current, bool &positive, bool &negative) -> bool
    {
        const auto angle = calc_angle<int_t, point_t>(check_point, previous, current);

        constexpr int_t zero{};

        if (zero == angle)
        {
            return true;
        }

        (zero < angle ? negative : positive) = false;
        return false;
    }

    template<class point_t, std::floating_point floating_t>
    requires(std::is_floating_point_v<typename point_t::distance_t>)
    [[nodiscard]] constexpr auto rotate(const point_t &point, const floating_t &theta) -> point_t
    {
        const auto cos1 = std::cos(theta);
        const auto sin1 = std::sin(theta);

        point_t res{ point.x * cos1 - point.y * sin1, point.x * sin1 + point.y * cos1 };

        return res;
    }

    template<class vector_t, class int_t>
    [[nodiscard]] constexpr auto scale(const vector_t &vec, const int_t &coef) -> vector_t
    {
        return { vec.x * coef, vec.y * coef };
    }

    template<class point_t, class vector_t>
    [[nodiscard]] constexpr auto translate(const point_t &pnt, const vector_t &vec) -> point_t
    {
        return { pnt.x + vec.x, pnt.y + vec.y };
    }

    // Points a and b, defining a line, must differ.
    template<class point_t, std::floating_point distance_t = typename point_t::distance_t>
    [[nodiscard]] constexpr auto closest_point_to_line(const point_t &point, const point_t &one, const point_t &two)
        -> point_t
    {
        assert(!(one == two));

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
        const auto a_p = point - one;
        const auto a_b = two - one;
        const auto ab2 = dist2<distance_t>(a_b);
        assert(distance_t{} < ab2);

        const auto coef = static_cast<distance_t>(dot_product<distance_t>(a_p, a_b) / ab2);
        const auto add = scale(a_b, coef);
        auto closest = translate<point_t>(one, add);
        return closest;
    }

    // A line segment has 2 different points.
    // The point might lie on the line.
    template<class point_t, std::floating_point distance_t = typename point_t::distance_t>
    [[nodiscard]] constexpr auto closest_point_to_line_segment(
        const point_t &point, const point_t &one, const point_t &two) -> point_t
    {
        assert(!(one == two));

        const auto a_p = point - one;
        const auto a_b = two - one;
        const auto ab2 = dist2<distance_t>(a_b);
        assert(distance_t{} < ab2);

        const auto coef = static_cast<distance_t>(dot_product<distance_t>(a_p, a_b) / ab2);
        if (coef < distance_t{})
        {
            return one;
        }

        if (static_cast<distance_t>(1) < coef)
        {
            return two;
        }

        auto closest = closest_point_to_line<point_t, distance_t>(point, one, two);
        return closest;
    }
} // namespace Standard::Algorithms::Geometry
