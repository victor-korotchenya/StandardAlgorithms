#include"convex_to_point_position_tests.h"
#include"../Utilities/test_utilities.h"
#include"geometry_utilities.h"
#include"point.h"
#include<tuple>
#include<vector>

namespace
{
    using int_t = std::int32_t;
    using point_t = Standard::Algorithms::Geometry::point2d<int_t>;
    using point_position_side_t = std::tuple<point_t, Standard::Algorithms::Geometry::point_position, std::size_t>;

    using test_data_t = std::tuple<point_t, point_t, point_t, std::vector<point_position_side_t>>;

    [[nodiscard]] constexpr auto create_test_cases() -> test_data_t
    {
        // point3
        //
        //        middle
        //
        // point1                  point2

        constexpr int_t shift = 123;
        constexpr int_t quarter = 5;

        constexpr point_t point1{ shift, shift };
        constexpr point_t point2{ shift + quarter * 4, shift };
        constexpr point_t point3{ shift, shift + quarter * 2 };

        constexpr auto x_pos = shift + quarter * 2;
        constexpr auto y_pos = shift + quarter;

        std::vector<point_position_side_t> point_position_sides{
            point_position_side_t({ x_pos, y_pos - 1 }, Standard::Algorithms::Geometry::point_position::inside, 0),
            point_position_side_t(
                { x_pos, y_pos }, Standard::Algorithms::Geometry::point_position::along_side, 1 // Along points 2 and 3.
                ),
            point_position_side_t({ x_pos, y_pos + 1 }, Standard::Algorithms::Geometry::point_position::outside, 0)
        };

        return { point1, point2, point3, std::move(point_position_sides) };
    }

    constexpr void run_point_position(const point_position_side_t &point_position_side, const point_t &point1,
        const point_t &point2, const point_t &point3)
    {
        const auto &[point, position, side] = point_position_side;
        const auto actual =
            Standard::Algorithms::Geometry::convex_to_point_position_slow<int_t>(point, { point1, point2, point3 });

        if (!actual.has_value()) [[unlikely]]
        {
            auto str = ::Standard::Algorithms::Utilities::w_stream();
            str << "Point " << point << ", position " << position << ", side " << side;
            str << ". Error: " << actual.error();
            throw std::runtime_error(str.str());
        }

        const auto &[actual_pos, actual_size] = actual.value();
        ::Standard::Algorithms::ert::are_equal(position, actual_pos, "slow position");
        ::Standard::Algorithms::ert::are_equal(side, actual_size, "slow side");
    }

    [[nodiscard]] constexpr auto run_pp_tests() -> bool
    {
        const auto test = create_test_cases();
        const auto &[point1, point2, point3, point_position_sides] = test;

        for (const auto &point : point_position_sides)
        {
            run_point_position(point, point1, point2, point3);
        }

        return true;
    }
} // namespace

void Standard::Algorithms::Geometry::Tests::convex_to_point_position_tests()
{
    static_assert(run_pp_tests());
}
