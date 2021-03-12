#include"convex_hull_tests.h"
#include"../Numbers/shift.h"
#include"../Utilities/elapsed_time_ns.h"
#include"../Utilities/random.h"
#include"../Utilities/test_utilities.h"
#include"convex_hull.h"
#include"point.h"
#include<iostream>

namespace
{
    using long_int_t = std::int64_t;
    using point_t = Standard::Algorithms::Geometry::point2d<std::int32_t>;
    using coll_t = std::vector<point_t>;

    struct test_case final : Standard::Algorithms::Tests::base_test_case
    {
        constexpr test_case(std::string &&name, coll_t &&source_points, coll_t &&expected) noexcept
            : base_test_case(std::move(name))
            , Source_points(std::move(source_points))
            , Expected(std::move(expected))
        {
        }

        [[nodiscard]] constexpr auto source_points() const &noexcept -> const coll_t &
        {
            return Source_points;
        }

        [[nodiscard]] constexpr auto expected() const &noexcept -> const coll_t &
        {
            return Expected;
        }

        void print(std::ostream &str) const override
        {
            ::Standard::Algorithms::print("Source points", Source_points, str);
            ::Standard::Algorithms::print("Expected", Expected, str);
        }

private:
        coll_t Source_points;
        coll_t Expected;
    };

    [[nodiscard]] auto random_test_case() -> test_case
    {
        // NOLINTNEXTLINE
        constexpr auto min_mud = ::Standard::Algorithms::is_debug ? 1 : 8;
        constexpr auto shift = 30U;
        constexpr auto ddd = ::Standard::Algorithms::Numbers::shift_left(static_cast<std::int32_t>(1), shift);
        constexpr auto max_mud = min_mud * 16;

        static_assert(0 < ddd);

        Standard::Algorithms::Utilities::random_t<std::int32_t> rnd(-ddd, ddd);

        const auto size = rnd.operator() (min_mud, max_mud);
        coll_t source_points(size);
        coll_t expected;

        for (std::int32_t index{}; index < size; ++index)
        {
            source_points[index].x = rnd.operator() ();
            source_points[index].y = rnd.operator() ();
        }

        std::pair<coll_t &, coll_t &> source_result = { source_points, expected };
        Standard::Algorithms::Geometry::convex_hull_graham_scan<long_int_t, point_t>(source_result);

        Standard::Algorithms::require_positive(expected.size(), "random expected size");

        return { "Random", std::move(source_points), std::move(expected) };
    }

    void generate_test_cases(std::vector<test_case> &test_cases)
    {
        test_cases.emplace_back("remove inner points",
            coll_t({ { 1, 1 }, { 2, 3 }, { 3, 2 },
                // duplicate
                { 3, 2 },
                // NOLINTNEXTLINE
                { 0, 5 }, { 6, 0 }, { 7, 1 }, { 1, 6 }, { 2, 7 } }),
            // NOLINTNEXTLINE
            coll_t({ { 0, 5 }, { 1, 1 }, { 6, 0 }, { 7, 1 }, { 2, 7 } }));

        test_cases.push_back(random_test_case());

        test_cases.emplace_back("trivial",
            // NOLINTNEXTLINE
            coll_t({ { 10, 20 } }), coll_t({ { 10, 20 } }));

        test_cases.emplace_back("segment",
            // NOLINTNEXTLINE
            coll_t({ { 10, 20 }, { 10, 300 } }), coll_t({ { 10, 20 }, { 10, 300 } }));

        test_cases.emplace_back("line remove middle",
            // NOLINTNEXTLINE
            coll_t({ { 10, 20 }, { 10, 30 }, { 10, 50 } }),
            // NOLINTNEXTLINE
            coll_t({ { 10, 20 }, { 10, 50 } }));
    }

    void run_test_case(const test_case &test)
    {
        const auto &src = test.source_points();
        const auto size = src.size();

        coll_t source(size);
        coll_t source2(size);
        coll_t actual;
        std::int64_t ai0{};
        std::int64_t ai1{};

        constexpr auto attempts = 1;

        for (std::int32_t att{}; att < attempts; ++att)
        {
            std::copy(src.cbegin(), src.cend(), source.begin());
            std::copy(src.cbegin(), src.cend(), source2.begin());
            actual.clear();

            std::pair<coll_t &, coll_t &> source_result = { source, actual };

            const Standard::Algorithms::elapsed_time_ns tim0;
            Standard::Algorithms::Geometry::convex_hull_graham_scan<long_int_t, point_t>(source_result);
            const auto elapsed0 = tim0.elapsed();

            ::Standard::Algorithms::ert::are_equal(test.expected(), actual, "convex_hull_graham_scan");

            actual.clear();

            const Standard::Algorithms::elapsed_time_ns tim1;
            convex_hull_low_upper<long_int_t>(source2, actual);
            const auto elapsed1 = tim1.elapsed();

            ::Standard::Algorithms::ert::are_equal(test.expected(), actual, "convex_hull_low_upper");

            if (att == 0)
            {
                continue;
            }

            ai0 += elapsed0;
            ai1 += elapsed1;

            const auto rat = ::Standard::Algorithms::ratio_compute(ai0, ai1);
            // ai1/ai0 = 1.7 on 1M.

            auto str = ::Standard::Algorithms::Utilities::w_stream();
            str << att << ", size " << size << ", ai0 " << ai0 << ", ai1 " << ai1 << ", ai1/ai0 " << rat << '\n';

            std::cout << str.str();
        }
    }
} // namespace

void Standard::Algorithms::Geometry::Tests::convex_hull_tests()
{
    Standard::Algorithms::Tests::test_utilities<test_case>::run_tests(run_test_case, generate_test_cases);
}
