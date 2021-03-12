#pragma once
#include"../Utilities/clang_constexpr.h"
#include"../Utilities/epsilon_default.h"
#include<cassert>
#include<cmath>
#include<concepts>

namespace Standard::Algorithms::Geometry
{
    template<class line_t, std::floating_point distance_t = typename line_t::distance_t>
    [[nodiscard]] constexpr auto are_parallel(
        const line_t &line, const line_t &line2, distance_t epsilon = epsilon_default<distance_t>()) -> bool
    {
        assert(distance_t{} < epsilon && check_line(line) && check_line(line2));

        const auto dix = line.A - line2.A;
        const auto diy = line.B - line2.B;
        auto result = ::Standard::Algorithms::fabs(dix) < epsilon && ::Standard::Algorithms::fabs(diy) < epsilon;
        return result;
    }

    template<class line_t, class distance_t = typename line_t::distance_t>
    [[nodiscard]] constexpr auto are_equal(
        const line_t &line, const line_t &line2, distance_t epsilon = epsilon_default<distance_t>()) -> bool
    {
        auto result = ::Standard::Algorithms::fabs(line.C - line2.C) < epsilon &&
            are_parallel<line_t, distance_t>(line, line2, epsilon);
        return result;
    }

    // If two lines neither parallel nor same, they intersect at a point:
    // a*x + b*y + c = a2*x + b2*y + c2 = 0.
    // y = -a*x/b - c/b
    // 0 = a2*x + b2*y + c2 = a2*x + -a*x/b*b2 - c/b*b2 + c2
    // x = (c/b*b2 - c2) / (a2 - a*b2/b) = (c*b2 - c2*b) / (a2*b - a*b2)
    // y = -a*x/b - c/b
    template<class line_t, class point_t, class distance_t = typename line_t::distance_t>
    [[nodiscard]] constexpr auto intersect_at_one_point(const line_t &line, const line_t &line2, point_t &point,
        distance_t epsilon = epsilon_default<distance_t>()) -> bool
    {
        if (are_parallel<line_t, distance_t>(line, line2, epsilon))
        {
            return false;
        }

        const auto dis = line2.A * line.B - line.A * line2.B;
        assert(dis != 0);

        point.x = (line.C * line2.B - line2.C * line.B) / dis;

        const auto is_vertical_line = ::Standard::Algorithms::fabs(line.B) < epsilon;

        point.y = is_vertical_line ? -line2.A * point.x - line2.C : // y = -a*x/b - c/b
            -line.A * point.x - line.C;

        return true;
    }
} // namespace Standard::Algorithms::Geometry
