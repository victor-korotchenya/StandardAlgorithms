#pragma once
#include"../Numbers/hash_utilities.h"
#include"../Utilities/static_false.h"
#include"../Utilities/throw_exception.h"

namespace Standard::Algorithms::Geometry
{
    template<class distance_t1, class squared_distance_t1, class point_t>
    requires(sizeof(typename point_t::distance_t) <= sizeof(distance_t1) &&
        sizeof(distance_t1) <= sizeof(squared_distance_t1))
    struct rectangle2d final
    {
        using distance_t = distance_t1;
        using squared_distance_t = squared_distance_t1;

        constexpr rectangle2d()
            : rectangle2d({}, {})
        {
        }

        constexpr rectangle2d( // The rightBottom coordinates cannot be less than that of the leftTop.
                               // todo(p4): fix 2 adjacent parameters.
                               // NOLINTNEXTLINE
            const point_t &leftTop, const point_t &rightBottom)
            : Left_top(leftTop)
            , Right_bottom(rightBottom)
        {
            validate(*this, "ctor");
        }

        [[nodiscard]] constexpr auto left_top() const &noexcept -> const point_t &
        {
            return Left_top;
        }

        [[nodiscard]] constexpr auto right_bottom() const &noexcept -> const point_t &
        {
            return Right_bottom;
        }

        // Enable the structured binding, unpacking, decoupling.
        template<std::size_t index, class some_t = bool>
        [[nodiscard]] constexpr auto get() const &noexcept -> const point_t &
        {
            if constexpr (index == 0U)
            {
                return Left_top;
            }
            else if constexpr (index == 1U)
            {
                return Right_bottom;
            }
            else
            {
                static_assert(static_false<some_t>);
            }
        }

        [[nodiscard]] constexpr auto height() const -> squared_distance_t
        {
            auto result = static_cast<squared_distance_t>(static_cast<squared_distance_t>(Right_bottom.x) - Left_top.x);

            return result;
        }

        [[nodiscard]] constexpr auto width() const -> squared_distance_t
        {
            auto result = static_cast<squared_distance_t>(static_cast<squared_distance_t>(Right_bottom.y) - Left_top.y);

            return result;
        }

        [[nodiscard]] constexpr auto area() const -> squared_distance_t
        {
            auto hei = height();
            auto wid = width();
            auto result = static_cast<squared_distance_t>(static_cast<squared_distance_t>(hei) * wid);

            return result;
        }

        static constexpr void validate(const rectangle2d &recta, const std::string &name) noexcept(false)
        {
            assert(!name.empty());

            if (recta.Right_bottom.x < recta.Left_top.x)
            {
                auto str = ::Standard::Algorithms::Utilities::w_stream();
                str << "Error in rectangle2d: rightBottom.x " << recta.Right_bottom.x << " < leftTop.x "
                    << recta.Left_top.x << ".";

                throw_exception(str, name);
            }

            if (recta.Right_bottom.y < recta.Left_top.y)
            {
                auto str = ::Standard::Algorithms::Utilities::w_stream();
                str << "Error in rectangle2d: rightBottom.y " << recta.Right_bottom.y << " < leftTop.y "
                    << recta.Left_top.y << ".";

                throw_exception(str, name);
            }
        }

        [[nodiscard]] constexpr auto operator<=> (const rectangle2d &) const noexcept = default;

private:
        point_t Left_top{};
        point_t Right_bottom{};
    };

    template<class distance_t, class squared_distance_t, class point_t>
    auto operator<< (std::ostream &str, const rectangle2d<distance_t, squared_distance_t, point_t> &rect)
        -> std::ostream &
    {
        str << "Left top " << rect.left_top() << ", Right bottom " << rect.right_bottom();

        return str;
    }
} // namespace Standard::Algorithms::Geometry

// Enable the structured binding, unpacking, decoupling.
template<class distance_t, class squared_distance_t, class point_t>
struct std::tuple_size<Standard::Algorithms::Geometry::rectangle2d<distance_t, squared_distance_t, point_t>>
    // It cannot be final.
    : std::integral_constant<std::size_t, 2>
{
};

template<class distance_t, class squared_distance_t, class point_t>
struct std::tuple_element<0, Standard::Algorithms::Geometry::rectangle2d<distance_t, squared_distance_t, point_t>> final
{
    using type = point_t;
};

template<class distance_t, class squared_distance_t, class point_t>
struct std::tuple_element<1, Standard::Algorithms::Geometry::rectangle2d<distance_t, squared_distance_t, point_t>> final
{
    using type = point_t;
};

template<class distance_t, class squared_distance_t, class point_t>
struct std::hash<Standard::Algorithms::Geometry::rectangle2d<distance_t, squared_distance_t, point_t>> final
{
    [[nodiscard]] constexpr auto operator() (
        const ::Standard::Algorithms::Geometry::rectangle2d<distance_t, squared_distance_t, point_t> &rect) const noexcept
        -> std::size_t
    {
        const auto one = std::hash<point_t>()(rect.left_top());
        const auto two = std::hash<point_t>()(rect.right_bottom());

        auto three = ::Standard::Algorithms::Numbers::combine_hashes(one, two);

        return three;
    }
};
