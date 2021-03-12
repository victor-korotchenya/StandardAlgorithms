#include"convex_min_weight_triangulation_tests.h"
#include"../Utilities/random.h"
#include"../Utilities/test_utilities.h"
#include"convex_min_weight_triangulation.h"
#include<numbers>

namespace
{
    using distance_t = std::int32_t;
    using point_t = ::Standard::Algorithms::Geometry::point2d<distance_t>;
    using coll_t = std::vector<point_t>;
    using floating_t = Standard::Algorithms::floating_point_type;
    constexpr floating_t inf = 1e50;

    struct test_case final : Standard::Algorithms::Tests::base_test_case
    {
        constexpr test_case(std::string &&name, coll_t &&polygon, const floating_t &min_cost = {}) noexcept
            : base_test_case(std::move(name))
            , Polygon(std::move(polygon))
            , Min_cost(min_cost)
        {
        }

        [[nodiscard]] constexpr auto polygon() const &noexcept -> const coll_t &
        {
            return Polygon;
        }

        [[nodiscard]] constexpr auto min_cost() const &noexcept -> const floating_t &
        {
            return Min_cost;
        }

        void print(std::ostream &str) const override
        {
            ::Standard::Algorithms::print("polygon", Polygon, str);
            str << ", min cost " << Min_cost;
        }

private:
        coll_t Polygon;
        floating_t Min_cost;
    };

    constexpr void generate_test_cases(std::vector<test_case> &test_cases)
    {
        test_cases.emplace_back("segment", coll_t({ { 0, 1 }, { 1, 0 } }), 0);

        test_cases.emplace_back("triangle", // NOLINTNEXTLINE
            coll_t({ { 0, 1 }, { 1, 0 }, { 1, 1 } }), 3.414213562373);

        // 1  2
        // 0      3
        // Perimeter length Per = 1 + 1 + sqrt(2) + 2 = 5.41421356
        // Cutting through 1-3 costs C1 = Per + 2*sqrt(5) = 9.88634951737267444162
        // Cutting through 0-2 costs C2 = Per + 2*sqrt(2) = 8.2426406871192851464
        // Thus, it is cheaper to cut through 0-2 which costs C2.
        test_cases.emplace_back("bucket", // NOLINTNEXTLINE
            coll_t({ { 0, 0 }, { 1, 0 }, { 1, 1 }, { 0, 2 } }), // NOLINTNEXTLINE
            8.2426406871192851464);

        test_cases.emplace_back("quadrilateral", coll_t({ { 0, 0 }, { 1, 0 }, { 1, 1 }, { 0, 1 } }),
            // 4 + 2*sqrt(2). // NOLINTNEXTLINE
            6.82842712474619);

        {
            constexpr auto cnt = 5;
            constexpr auto radius = 100;

            coll_t polygon{};

            for (std::int32_t index{}; index < cnt; ++index)
            {
                const auto angle = std::numbers::pi_v<floating_t> * 2 / cnt * index;
                const auto xxx = radius * std::sin(angle);
                const auto yyy = radius * std::cos(angle);

                polygon.push_back(point_t{ static_cast<distance_t>(xxx), static_cast<distance_t>(yyy) });
            }

            constexpr auto min_cst = 1339.224598776549;
            test_cases.emplace_back("5-gone", std::move(polygon), min_cst);
        }
    }

    constexpr void run_test_case(const test_case &test)
    {
        std::vector<std::pair<std::size_t, std::size_t>> cross_edges{};

        const auto actual_slow =
            ::Standard::Algorithms::Geometry::convex_min_weight_triangulation_slow<point_t, floating_t>(
                test.polygon(), inf, cross_edges);

        ::Standard::Algorithms::ert::are_equal_with_epsilon(
            test.min_cost(), actual_slow, "convex_min_weight_triangulation_slow cost");
    }
} // namespace

void Standard::Algorithms::Geometry::Tests::convex_min_weight_triangulation_tests()
{
    ::Standard::Algorithms::Tests::test_utilities<test_case>::run_tests(run_test_case, generate_test_cases);
}
