#include"is_simple_convex_polygon_tests.h"
#include"../Utilities/test_utilities.h"
#include"geometry_utilities.h"
#include"point.h"

namespace
{
    using int_t = std::int16_t;
    using point_t = Standard::Algorithms::Geometry::point2d<int_t>;

    struct test_case final : Standard::Algorithms::Tests::base_test_case
    {
        constexpr test_case(std::string &&name, std::vector<point_t> &&points, bool expected) noexcept
            : base_test_case(std::move(name))
            , Points(std::move(points))
            , Expected(expected)
        {
        }

#if defined(__GNUG__) && !defined(__clang__) // todo(p3): del this garbage after G++ is fixed.
        // Avoid an error: 'virtual constexpr {anonymous}::test_case::~test_case()' used before its definition.
        // The "= default" will not compile because of the bug. // NOLINTNEXTLINE
        constexpr ~test_case() noexcept override {}
#endif // todo(p3): End of: del this garbage after G++ is fixed.

        [[nodiscard]] constexpr auto points() const &noexcept -> const std::vector<point_t> &
        {
            return Points;
        }

        [[nodiscard]] constexpr auto expected() const noexcept -> bool
        {
            return Expected;
        }

        void print(std::ostream &str) const override
        {
            ::Standard::Algorithms::print("Points", Points, str);
            str << ", Expected " << Expected;
        }

private:
        std::vector<point_t> Points;
        bool Expected;
    };

    constexpr void generate_test_cases(std::vector<test_case> &test_cases)
    {
        {
            test_cases.emplace_back("quad", std::vector<point_t>{ { 1, 1 }, { 2, 1 }, { 2, 2 }, { 1, 2 } }, true);
        }
        {
            test_cases.emplace_back("zig+zag", std::vector<point_t>{ { 1, 1 }, { 2, 2 }, { 2, 1 }, { 1, 2 } }, false);
        }
    }

    constexpr void run_test_case(const test_case &test)
    {
        const auto actual = Standard::Algorithms::Geometry::is_simple_convex_polygon<std::int64_t>(test.points());

        ::Standard::Algorithms::ert::are_equal(test.expected(), actual, "is_simple_convex_polygon");
    }
} // namespace

void Standard::Algorithms::Geometry::Tests::is_simple_convex_polygon_tests()
{
    static_assert(Standard::Algorithms::Tests::test_utilities<test_case>::run_tests(run_test_case, generate_test_cases));
}
