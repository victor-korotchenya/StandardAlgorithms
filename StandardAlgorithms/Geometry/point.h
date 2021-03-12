#pragma once
// "point.h"
#include"../Numbers/hash_utilities.h"
#include"../Utilities/throw_exception.h"

namespace Standard::Algorithms::Geometry
{
    template<class distance_t1>
    struct point2d final
    {
        using distance_t = distance_t1;

        // Remove, when '<=>' can be implemented outside class.
        // NOLINTNEXTLINE
        distance_t x{};

        // NOLINTNEXTLINE
        distance_t y{};

        [[nodiscard]] constexpr auto operator<=> (const point2d &) const noexcept = default;
    };

    template<class distance_t>
    [[nodiscard]] constexpr auto operator- (const point2d<distance_t> &one, const point2d<distance_t> &two)
        -> point2d<distance_t>
    {
        return { static_cast<distance_t>(one.x - two.x), static_cast<distance_t>(one.y - two.y) };
    }

    template<class distance_t>
    auto operator<< (std::ostream &str, const point2d<distance_t> &point) -> std::ostream &
    {
        str << "x " << point.x << ", y " << point.y;
        return str;
    }
} // namespace Standard::Algorithms::Geometry

template<class distance_t>
struct std::hash<Standard::Algorithms::Geometry::point2d<distance_t>>
{
    [[nodiscard]] constexpr auto operator() (
        const Standard::Algorithms::Geometry::point2d<distance_t> &pnt) const noexcept -> std::size_t
    {
        const auto one = hash<distance_t>()(pnt.x);
        const auto two = hash<distance_t>()(pnt.y);
        auto three = ::Standard::Algorithms::Numbers::combine_hashes(one, two);

        return three;
    }
};
