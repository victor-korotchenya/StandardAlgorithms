#include"calc_angle_tests.h"
#include"../Utilities/test_utilities.h"
#include"geometry_utilities.h"
#include"point.h"
#include<vector>

namespace
{
    using coordinate_t = std::int32_t;
    using point_t = Standard::Algorithms::Geometry::point2d<coordinate_t>;

    struct test_case final : Standard::Algorithms::Tests::base_test_case
    {
        constexpr test_case(std::string &&name,
            // No need to create nothing to pass in 3 points.
            // NOLINTNEXTLINE
            const point_t &pt1, const point_t &pt2, const point_t &pt3, const coordinate_t &expected)
            : base_test_case(std::move(name))
            , Pt1(pt1)
            , Pt2(pt2)
            , Pt3(pt3)
            , Expected(expected)
        {
        }

        [[nodiscard]] constexpr auto point1() const &noexcept -> const point_t &
        {
            return Pt1;
        }

        [[nodiscard]] constexpr auto point2() const &noexcept -> const point_t &
        {
            return Pt2;
        }

        [[nodiscard]] constexpr auto point3() const &noexcept -> const point_t &
        {
            return Pt3;
        }

        [[nodiscard]] constexpr auto expected() const &noexcept -> const coordinate_t &
        {
            return Expected;
        }

        void print(std::ostream &str) const override
        {
            str << " 1st point " << Pt1 << ", 2nd point " << Pt2 << ", 3-rd point " << Pt3 << ", Expected " << Expected;
        }

private:
        point_t Pt1;
        point_t Pt2;
        point_t Pt3;
        coordinate_t Expected;
    };

    [[nodiscard]] constexpr auto negate_x(const point_t &point) -> point_t
    {
        return { -point.x, point.y };
    }

    constexpr void generate_test_cases(std::vector<test_case> &test_cases)
    {
        const test_case simple{ "Simple", // NOLINTNEXTLINE
            { 1, 2 }, { 10, 30 }, { 7, 50 }, 264 };

        // Changing the sign of x coordinates must negate the result.
        const test_case simple_negated{ "Simple negated", negate_x(simple.point1()), negate_x(simple.point2()),
            negate_x(simple.point3()), -simple.expected() };

        test_cases.insert(test_cases.end(),
            {
                simple,
                simple_negated,

                // NOLINTNEXTLINE
                { "ZeroTrivial", { 1, 1 }, { 2, 2 }, { 5, 5 }, 0 },

                // NOLINTNEXTLINE
                { "ZeroTwo", { 1, 1 }, { 2, 4 }, { 5, 13 }, 0 },
            });
    }

    constexpr void run_test_case(const test_case &test)
    {
        const auto actual =
            Standard::Algorithms::Geometry::calc_angle<coordinate_t>(test.point1(), test.point2(), test.point3());

        ::Standard::Algorithms::ert::are_equal(test.expected(), actual, "calc_angle");
    }
} // namespace

void Standard::Algorithms::Geometry::Tests::calc_angle_tests()
{
    Standard::Algorithms::Tests::test_utilities<test_case>::run_tests(run_test_case, generate_test_cases);
}
