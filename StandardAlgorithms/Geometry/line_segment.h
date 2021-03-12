#pragma once
// "line_segment.h"
#include"../Numbers/hash_utilities.h"
#include"../Utilities/throw_exception.h"

namespace Standard::Algorithms::Geometry
{
    template<class point_t1>
    struct line_segment final
    {
        using point_t = point_t1;

        // Remove, when '<=>' can be implemented outside class.
        // NOLINTNEXTLINE
        point_t left_lower{};

        // NOLINTNEXTLINE
        point_t right_upper{};

        [[nodiscard]] constexpr auto operator<=> (const line_segment &) const noexcept = default;
    };

    template<class point_t>
    [[nodiscard]] constexpr auto check_line_segment(const line_segment<point_t> &segment) -> bool
    {
        auto good = !(segment.right_upper < segment.left_lower);
        return good;
    }

    template<class point_t>
    auto operator<< (std::ostream &str, const line_segment<point_t> &segment) -> std::ostream &
    {
        str << "left lower " << segment.left_lower << ", right upper " << segment.right_upper;

        return str;
    }
} // namespace Standard::Algorithms::Geometry

template<class point_t>
struct std::hash<Standard::Algorithms::Geometry::line_segment<point_t>>
{
    [[nodiscard]] constexpr auto operator() (const Standard::Algorithms::Geometry::line_segment<point_t> &segment) const
        -> std::size_t
    {
        const auto one = hash<point_t>()(segment.left_lower);
        const auto two = hash<point_t>()(segment.right_upper);
        auto three = ::Standard::Algorithms::Numbers::combine_hashes(one, two);
        return three;
    }
};
