#include"closest_pair_of_points_in_2d_plane_tests.h"
#include"../Numbers/shift.h"
#include"../Utilities/elapsed_time_ns.h"
#include"../Utilities/random.h"
#include"../Utilities/remove_duplicates.h"
#include"../Utilities/test_utilities.h"
#include"closest_pair_of_points_in_2d_plane.h"
#include"point.h"
#include<iostream>

// todo(p4): #include<syncstream> // clang ?? // std::cout

namespace
{
    using int_t = std::int32_t;
    using point_t = Standard::Algorithms::Geometry::point2d<int_t>;
    using long_int_t = std::int64_t;
    using best_t = std::pair<long_int_t, std::pair<point_t, point_t>>;

    constexpr auto print_closest_pair_of_points_in_2d_plane = false;
    constexpr long_int_t not_computed = -1LL;

    struct test_case final : Standard::Algorithms::Tests::base_test_case
    {
        constexpr test_case(
            std::string &&name, std::vector<point_t> &&points, best_t &&expected = { not_computed, {} }) noexcept
            : base_test_case(std::move(name))
            , Points(std::move(points))
            , Expected(std::move(expected))
        {
            Standard::Algorithms::sort_remove_duplicates(Points);
        }

        [[nodiscard]] constexpr auto points() const &noexcept -> const std::vector<point_t> &
        {
            return Points;
        }

        [[nodiscard]] constexpr auto expected() const &noexcept -> const best_t &
        {
            return Expected;
        }

        constexpr void validate() const override
        {
            base_test_case::validate();

            Standard::Algorithms::require_positive(static_cast<int_t>(Points.size()), "points.size");

            // if (Expected.first < 0) return;
        }

        void print(std::ostream &str) const override
        {
            ::Standard::Algorithms::print("Points", Points, str);
            ::Standard::Algorithms::print_value("Expected", str, Expected);
        }

private:
        std::vector<point_t> Points;
        best_t Expected;
    };

    void generate_test_cases(std::vector<test_case> &test_cases)
    {
        constexpr auto min_size = 1;
        constexpr auto max_size = 10;
        constexpr auto value_max = ::Standard::Algorithms::Numbers::shift_left(max_size, 2U);

        static_assert(0 < value_max);

        Standard::Algorithms::Utilities::random_t<std::int32_t> rnd(-value_max, value_max);

        auto gena = [&rnd] -> point_t
        {
            return { rnd(), rnd() };
        };

        constexpr auto max_attempts = 1;

        for (std::int32_t att{}; att < max_attempts; ++att)
        {
            const auto size = rnd(min_size, max_size);

            std::vector<point_t> points(size);
            std::generate(points.begin(), points.end(), gena);

            points.push_back(point_t{ points.at(0).x + 1, points.at(0).y });

            test_cases.emplace_back("Random" + std::to_string(att), std::move(points));
        }

        constexpr auto len5 = 5;
        constexpr auto len13 = 13;

        test_cases.push_back({ "base2", { { 1, 2 }, { 3, len5 } }, { len13, { { 1, 2 }, { 3, len5 } } } });
    }

    // A non constexpr because of timing.
    void run_test_case(const test_case &test)
    {
        const auto dist_func = [] [[nodiscard]] (const point_t &one, const point_t &two)
        {
            const auto dix = one.x - two.x;
            const auto diy = one.y - two.y;
            const auto ddd = static_cast<std::int64_t>(dix) * dix + static_cast<std::int64_t>(diy) * diy;
            return ddd;
        };
        const auto dist_func1 = [] [[nodiscard]] (const int_t &val)
        {
            const auto dis = static_cast<long_int_t>(val) * val;
            return dis;
        };

        const Standard::Algorithms::elapsed_time_ns ti0;

        const auto actual =
            Standard::Algorithms::Geometry::closest_pair_sweep_line<long_int_t>(test.points(), dist_func, dist_func1);

        [[maybe_unused]] const auto elapsed0 = ti0.elapsed();

        if (0 <= test.expected().first)
        {
            ::Standard::Algorithms::ert::are_equal(test.expected().first, actual.first, "closest_pair_sweep_line");
        }

        const Standard::Algorithms::elapsed_time_ns ti1;

        best_t slow = { std::numeric_limits<long_int_t>::max(), {} };

        constexpr auto large = 128U;

        const auto shall_run = test.points().size() <= large;

        if (shall_run)
        {
            Standard::Algorithms::Geometry::closest_pair_of_points_in_2d_plane_slow<long_int_t>(
                test.points(), dist_func, 0, static_cast<std::int32_t>(test.points().size()), slow);
        }

        [[maybe_unused]] const auto elapsed1 = ti1.elapsed();

        if (shall_run)
        {
            ::Standard::Algorithms::ert::are_equal(actual.first, slow.first, "closest_pair_of_points_in_2d_plane_slow");
        }

        const Standard::Algorithms::elapsed_time_ns ti2;

        const auto sweep = Standard::Algorithms::Geometry::closest_pair_of_points_in_2d_plane_slow_still<long_int_t>(
            test.points(), dist_func, dist_func1);

        [[maybe_unused]] const auto elapsed2 = ti2.elapsed();

        ::Standard::Algorithms::ert::are_equal(
            actual.first, sweep.first, "closest_pair_of_points_in_2d_plane_slow_still");

        if constexpr (print_closest_pair_of_points_in_2d_plane)
        {
            const auto ratio10 = ::Standard::Algorithms::ratio_compute(elapsed0, elapsed1);
            const auto ratio20 = ::Standard::Algorithms::ratio_compute(elapsed0, elapsed2);

            const auto rep = " _ " + std::to_string(test.points().size()) + ", slow " + std::to_string(elapsed1) +
                ", ti2 " + std::to_string(elapsed2) + ", ti0 " + std::to_string(elapsed0) + ", slow/ti0 " +
                std::to_string(ratio10) + ", ti2/ti0 " + std::to_string(ratio20) + "\n";

            std::cout << rep;
        }
    }
} // namespace

void Standard::Algorithms::Geometry::Tests::closest_pair_of_points_in_2d_plane_tests()
{
    Standard::Algorithms::Tests::test_utilities<test_case>::run_tests(run_test_case, generate_test_cases);
}
