#include <cmath>
#include "../Utilities/Random.h"
#include "min_length_2d_bitonic_tour_tests.h"
#include "min_length_2d_bitonic_tour.h"
#include "../test_utilities.h"
#include "../Utilities/VectorUtilities.h"
#include "../Utilities/PrintUtilities.h"

using namespace std;
using namespace Privet::Algorithms::Geometry::Tests;
using namespace Privet::Algorithms::Geometry;
using namespace Privet::Algorithms::Tests;
using namespace Privet::Algorithms;

namespace
{
    using point_t = pair<int, int>;
    using float_t = double;
    constexpr auto not_computed = -1;

    struct test_case final : base_test_case
    {
        vector<point_t> points;
        float_t expected_distance;
        vector<int> expected_indexes;

        test_case(string&& name,
            vector<point_t>&& points,
            float_t expected_distance = not_computed,
            vector<int>&& expected_indexes = {})
            : base_test_case(forward<string>(name)),
            points(forward<vector<point_t>>(points)),
            expected_distance(expected_distance),
            expected_indexes(forward<vector<int>>(expected_indexes))
        {
            VectorUtilities::sort_remove_duplicates(this->points);
        }

        void Validate() const override
        {
            base_test_case::Validate();

            const auto size = static_cast<int>(points.size());
            RequirePositive(size, "points.size");

            if (expected_distance < 0)
                return;

            Assert::Greater(expected_indexes.size(), 0u, "expected_indexes.size");
            Assert::AreEqual(0, expected_indexes[0], "expected_indexes[0]");
            Assert::AreEqual(size - 1, expected_indexes.back(), "expected_indexes.back");
            Assert::AreEqual(true, is_sorted(expected_indexes.begin(), expected_indexes.end()), "expected_indexes is_sorted");
        }

        void Print(ostream& str) const override
        {
            base_test_case::Print(str);
            ::Print("points", points, str);
            PrintValue(str, "expected_distance", expected_distance);
            ::Print("expected_indexes", expected_indexes, str);
        }
    };

    void generate_test_cases(vector<test_case>& test_cases)
    {
        test_cases.push_back({ "base4", { {0,0}, {0,1}, {1,0}, {1,1}, }, 4, {0, 1, 2, 3} });
        test_cases.push_back({ "base3", { {1,-2}, {1,1}, {3,2} }, sqrt(5) * 3 + 3, {0, 1, 2} });
        test_cases.push_back({ "base2", { {1,-2}, {5,1} }, 10, {0, 1} });
        test_cases.push_back({ "base1", { {10,20} }, 0, {0} });

        IntRandom r;
        vector<point_t> points;

        for (auto att = 0, att_max = 1; att < att_max; ++att)
        {
            constexpr auto size_max = 11, value_max = 10;
            const auto n = r(1, size_max);

            points.resize(n);
            for (auto i = 0; i < n; ++i)
                points[i] = { r(-value_max, value_max), r(-value_max, value_max) };

            test_cases.emplace_back("random" + to_string(att), move(points));
        }
    }

    void run_test_case(const test_case& test)
    {
        const auto dist_func = [](const point_t& a, const point_t& b) {
            const auto dx = a.first - b.first, dy = a.second - b.second;
            const auto di = static_cast<int64_t>(dx) * dx + static_cast<int64_t>(dy) * dy;
            const auto q = sqrt(di);
            return q;
        };

        const auto actual = min_length_2d_bitonic_tour<float_t, point_t>(test.points, dist_func);
        if (test.expected_distance >= 0)
            Assert::AreEqualWithEpsilon(test.expected_distance, actual.first, "min_length_2d_bitonic_tour");

        // todo: p3. do indexes.

        const auto slow2 = min_length_2d_bitonic_tour_slow_they2<float_t, point_t>(test.points, dist_func);
        Assert::AreEqualWithEpsilon(actual.first, slow2.first, "min_length_2d_bitonic_tour_slow_they2");

        const auto slow3 = min_length_2d_bitonic_tour_slow3<float_t, point_t>(test.points, dist_func);
        Assert::AreEqualWithEpsilon(actual.first, slow3.first, "min_length_2d_bitonic_tour_slow3");

        const auto slow = min_length_2d_bitonic_tour_slow<float_t, point_t>(test.points, dist_func);
        Assert::AreEqualWithEpsilon(actual.first, slow.first, "min_length_2d_bitonic_tour_slow");
    }
}

void Privet::Algorithms::Geometry::Tests::min_length_2d_bitonic_tour_tests()
{
    test_utilities<test_case>::run_tests(run_test_case, generate_test_cases);
}