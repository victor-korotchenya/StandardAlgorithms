#pragma once
// "line.h"
#include"../Utilities/clang_constexpr.h"
#include"../Utilities/epsilon_default.h"
#include"../Utilities/throw_exception.h"
#include<cmath>
#include<ostream>
#include<string>

namespace Standard::Algorithms::Geometry
{
    // A * x + B * y + C = 0.
    // A vertical line has A=1, B=0,
    // otherwise B=1.
    template<class distance_t1>
    struct line2d final
    {
        using distance_t = distance_t1;

        // Remove, when '<=>' can be implemented outside class.
        // NOLINTNEXTLINE
        distance_t A{};

        // NOLINTNEXTLINE
        distance_t B{};

        // NOLINTNEXTLINE
        distance_t C{};

        [[nodiscard]] constexpr auto operator<=> (const line2d &) const noexcept = default;
    };

    template<class distance_t>
    [[nodiscard]] constexpr auto check_line(const line2d<distance_t> &line) -> bool
    {
        auto good = line.B == 1 || (line.B == distance_t{} && line.A == 1);
        return good;
    }

    template<class distance_t>
    auto operator<< (std::ostream &str, const line2d<distance_t> &line) -> std::ostream &
    {
        str << "A " << line.A << ", B " << line.B << ", C " << line.C;
        return str;
    }

    template<class point_t, std::floating_point distance_t = typename point_t::distance_t,
        class line_t = line2d<distance_t>>
    [[nodiscard]] constexpr auto to_line(
        const point_t &one, const point_t &two, distance_t epsilon = epsilon_default<distance_t>()) -> line_t
    {
        assert(0 < epsilon);

        const distance_t dix = two.x - one.x;

        if (const auto is_vertical_line = ::Standard::Algorithms::fabs(dix) < epsilon; is_vertical_line)
        {
            return { 1, 0, -one.x };
        }

        assert(dix != 0);
        // a * x + b * y + c = 0, where b=1.
        //
        // a * one.x + one.y + c = 0.
        // a * two.x + two.y + c = 0.

        const distance_t aaa = (one.y - two.y) / dix;
        const distance_t ccc = -aaa * one.x - one.y;
        return { aaa, 1, ccc };
    }
} // namespace Standard::Algorithms::Geometry

template<class distance_t>
struct std::hash<Standard::Algorithms::Geometry::line2d<distance_t>>
{
    [[nodiscard]] constexpr auto operator() (const Standard::Algorithms::Geometry::line2d<distance_t> &line) const
        -> std::size_t
    {
        constexpr auto three = 3;
        constexpr auto five = 5;

        const auto one = hash<distance_t>()(line.A);
        const auto two = hash<distance_t>()(line.B);
        const auto four = hash<distance_t>()(line.C);
        const auto six = (one | (one << three)) ^ two;
        auto heaven = (six | (six << five)) ^ four;
        return heaven;
    }
};
