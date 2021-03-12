#pragma once
// "geometry_utilities.h"
#include"../Utilities/is_debug.h"
#include"point_utilities.h"
#include<cmath>
#include<expected>
#include<vector>

namespace Standard::Algorithms::Geometry
{
    enum class [[nodiscard]] point_position : std::uint8_t
    {
        // The point is strictly outside the region.
        outside,
        // The point is strictly inside the region.
        inside,
        // The point belongs to a side.
        along_side,
    };

    inline auto operator<< (std::ostream &str, const point_position &position) -> std::ostream &
    {
        switch (position)
        {
            using enum point_position;

        case outside:
            str << "outside";
            break;
        case inside:
            str << "inside";
            break;
        case along_side:
            str << "Along a side";
            break;
        default:
            if constexpr (::Standard::Algorithms::is_debug)
            {
                auto str2 = ::Standard::Algorithms::Utilities::w_stream();
                str2 << "Unknown value of point_position " << std::to_underlying(position);

                throw std::out_of_range(str2.str());
            }
            else
            {
                std::unreachable();
            }
        }

        return str;
    }

    enum class [[nodiscard]] convex_error : std::uint8_t
    {
        // A convex must have 3 or more points.
        too_small_size,
        // A convex might be concave.
        possibly_concave,
    };

    inline auto operator<< (std::ostream &str, const convex_error &err) -> std::ostream &
    {
        switch (err)
        {
            using enum convex_error;

        case too_small_size:
            str << "Too small size";
            break;
        case possibly_concave:
            str << "Convex might be concave";
            break;
        default:
            if constexpr (::Standard::Algorithms::is_debug)
            {
                auto str2 = ::Standard::Algorithms::Utilities::w_stream();
                str2 << "Unknown value of convex_error " << std::to_underlying(err);

                throw std::out_of_range(str2.str());
            }
            else
            {
                std::unreachable();
            }
        }

        return str;
    }

    // todo(p2): query time O(log(n)). Line sweep: pre-split the convex into distinct vertical lines once for all
    // requests in O(n), binary find maximum 1 rectangle in O(log(n)) where the point could fit, check the rectangle in
    // O(1) if any. Special cases: leftmost and/or rightmost vertical lines might be vertical segments.


    // Compute the relative position of a point
    // to a convex region consisting of 3 or more distinct points.
    // Slow time O(n).
    // When the "first" of the returned value is "along_side",
    // then the "second" is which segment the "point" belongs to.
    template<class coordinate_t, class point_t>
    [[nodiscard]] constexpr auto convex_to_point_position_slow(
        const point_t &point, const std::vector<point_t> &convex_region)
        -> std::expected<std::pair<point_position, std::size_t>, convex_error>
    {
        const auto size = convex_region.size();

        if (constexpr auto min_size = 3U; size < min_size)
        {
            return std::unexpected(convex_error::too_small_size);
        }

        auto position = point_position::outside;
        std::size_t index{};
        auto positive = true;
        auto negative = true;

        const auto &first_point = convex_region[0];
        auto previous = first_point;

        for (;;)
        {
            const auto &current = convex_region[1 + index];

            if (is_along_side<coordinate_t, point_t>(point, previous, current, positive, negative))
            {
                position = point_position::along_side;
                break;
            }

            if (size - 1U // The first point is already taken.
                == ++index)
            {// Check last - first.
                if (is_along_side<coordinate_t, point_t>(point, current, first_point, positive, negative))
                {
                    position = point_position::along_side;
                }

                break;
            }

            previous = current;
        }

        if (point_position::along_side == position)
        {
            if constexpr (::Standard::Algorithms::is_debug)
            {
                if (!(positive || negative))
                {// The point is outside and along side - the convex might be concave.
                    return std::unexpected(convex_error::possibly_concave);
                }
            }

            return std::make_pair(point_position::along_side, index);
        }

        return std::make_pair(positive || negative ? point_position::inside : point_position::outside, 0ZU);
    }

    // Points are distinct.
    template<class product_t, class point_t>
    [[nodiscard]] constexpr auto is_simple_convex_polygon(const std::vector<point_t> &points) -> bool
    {
        const auto size = static_cast<std::int32_t>(points.size());
        std::int32_t prev_angle{};

        for (std::int32_t index{}; index < size; ++index)
        {
            const auto &point_a = points[index];
            const auto &point_b = points[(index + 1) % size];
            const auto &point_c = points[(index + 2) % size];
            assert(!(point_a == point_b) && !(point_b == point_c) && !(point_a == point_c));

            const auto angle = calc_angle<product_t, point_t>(point_a, point_b, point_c);
            if (angle == product_t{})
            {
                continue;
            }

            if (prev_angle == 0)
            {
                prev_angle = 0 < angle ? 1 : -1;
                continue;
            }

            if (prev_angle * angle < 0)
            {// Orientation change is not allowed.
                return false;
            }
        }

        return true;
    }

    template<class floating_t, class point_t>
    [[nodiscard]] constexpr auto perimeter(const std::vector<point_t> &points) -> floating_t
    {
        const auto size = points.size();
        floating_t sum{};

        for (std::size_t index{}; index < size; ++index)
        {
            const auto &point_a = points[(index == 0U ? size : index) - 1U];
            const auto &point_b = points[index];
            const auto diff = point_b - point_a;
            const auto float_diff = dist2<floating_t>(diff);

            assert(!(float_diff < floating_t{}));

            const auto root = static_cast<floating_t>(std::sqrt(float_diff));
            sum += root;

            assert(!(sum < floating_t{}) && !(root < floating_t{}));
        }

        return sum;
    }
} // namespace Standard::Algorithms::Geometry
