#include "point.h"
#include "closest_pair_of_points_in_2d_plane_tests.h"
#include "closest_pair_of_points_in_2d_plane.h"
#include "../test_utilities.h"
#include "../Utilities/Random.h"
#include "../Utilities/VectorUtilities.h"
#include "../Utilities/PrintUtilities.h"

//#define Print_closest_pair_of_points_in_2d_plane 1

#ifdef Print_closest_pair_of_points_in_2d_plane
#include "../elapsed_time_ns.h"
#include <iostream>
#endif

using namespace std;
using namespace Privet::Algorithms::Geometry::Tests;
using namespace Privet::Algorithms::Geometry;
using namespace Privet::Algorithms::Tests;
using namespace Privet::Algorithms;

namespace
{
    using int_t = int;
    using point_t = point2d<int_t>;
    using float_t = int64_t;
    constexpr auto not_computed = -1;

    struct test_case final : base_test_case
    {
        vector<point_t> points;

        // Use the squared distance.
        pair<float_t, pair<point_t, point_t>> expected;

        test_case(string&& name,
            vector<point_t>&& points,
            pair<float_t, pair<point_t, point_t>> expected = { not_computed, {} })
            : base_test_case(forward<string>(name)),
            points(forward<vector<point_t>>(points)),
            expected(expected)
        {
            VectorUtilities::sort_remove_duplicates(this->points);
        }

        void Validate() const override
        {
            base_test_case::Validate();

            const auto size = static_cast<int_t>(points.size());
            RequirePositive(size, "points.size");

            //if (expected.first < 0)
            //    return;
        }

        void Print(ostream& str) const override
        {
            base_test_case::Print(str);
            ::Print("points", points, str);
            PrintValue(str, "expected", expected);
        }
    };

    void generate_test_cases(vector<test_case>& test_cases)
    {
        IntRandom r;
        vector<point_t> points;

        for (auto att = 0, att_max = 1; att < att_max; ++att)
        {
            constexpr auto size_max = 10, value_max = size_max << 2;
            const auto n = r(1, size_max);

            points.resize(n);
            for (auto& point : points)
                point = { r(-value_max, value_max), r(-value_max, value_max) };

            points.emplace_back(points[0].X + 1, points[0].Y);

            test_cases.emplace_back("random" + to_string(att), move(points));
        }

        test_cases.push_back({ "base2", { {1,2}, {3, 5} }, {13,{{ 1,2 },{ 3, 5 }} } });
    }

    void run_test_case(const test_case& test)
    {
        const auto dist_func = [](const point_t& a, const point_t& b) {
            const auto dx = a.X - b.X, dy = a.Y - b.Y;
            const auto dd = static_cast<int64_t>(dx) * dx + static_cast<int64_t>(dy) * dy;
            return dd;
        };
        const auto dist_func1 = [](const int_t& a) {
            const auto di = static_cast<float_t>(a) * a;
            return di;
        };
        using best_t = pair<float_t, pair<point_t, point_t>>;

#ifdef Print_closest_pair_of_points_in_2d_plane
        elapsed_time_ns t0;
#endif
        const auto actual = closest_pair_sweep_line<float_t>(test.points, dist_func, dist_func1);

#ifdef Print_closest_pair_of_points_in_2d_plane
        const auto elapsed0 = t0.elapsed();
#endif

        if (test.expected.first >= 0)
            Assert::AreEqual(test.expected.first, actual.first, "closest_pair_sweep_line");

#ifdef Print_closest_pair_of_points_in_2d_plane
        elapsed_time_ns t1;
#endif
        best_t slow = { numeric_limits<float_t>::max(), {} };
        const auto shall_run = test.points.size() <= 1 << 10;
        if (shall_run)
            closest_pair_of_points_in_2d_plane_slow<float_t>(test.points, dist_func, 0, static_cast<int>(test.points.size()), slow);

#ifdef Print_closest_pair_of_points_in_2d_plane
        const auto elapsed1 = t1.elapsed();
#endif
        if (shall_run)
            Assert::AreEqual(actual.first, slow.first, "closest_pair_of_points_in_2d_plane_slow");

#ifdef Print_closest_pair_of_points_in_2d_plane
        elapsed_time_ns t2;
#endif
        const auto sweep = closest_pair_of_points_in_2d_plane_slow_still<float_t>(test.points, dist_func, dist_func1);

#ifdef Print_closest_pair_of_points_in_2d_plane
        const auto elapsed2 = t2.elapsed();
#endif
        Assert::AreEqual(actual.first, sweep.first, "closest_pair_of_points_in_2d_plane_slow_still");

#ifdef Print_closest_pair_of_points_in_2d_plane
        const auto ratio10 = elapsed0 > 0 ? static_cast<double>(elapsed1) / elapsed0 : 0.0;
        const auto ratio20 = elapsed0 > 0 ? static_cast<double>(elapsed2) / elapsed0 : 0.0;

        const auto rep = " _ " + to_string(test.points.size()) +
            ", slow " + to_string(elapsed1) +
            ", t2 " + to_string(elapsed2) +
            ", t0 " + to_string(elapsed0) +
            ", slow/t0 " + to_string(ratio10) +
            ", t2/t0 " + to_string(ratio20) +
            "\n";
        cout << rep;
#endif
    }
}

void Privet::Algorithms::Geometry::Tests::closest_pair_of_points_in_2d_plane_tests()
{
    test_utilities<test_case>::run_tests(run_test_case, generate_test_cases);
}