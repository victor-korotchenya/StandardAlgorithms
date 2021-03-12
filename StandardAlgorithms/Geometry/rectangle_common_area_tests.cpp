#include "rectangle_common_area_tests.h"
#include "rectangle_common_area.h"
#include "../Graphs/segment_tree_add_bool.h"
#include "../Utilities/Random.h"
#include "../test_utilities.h"
#include "../Utilities/PrintUtilities.h"

using namespace std;
using namespace Privet::Algorithms::Trees;
using namespace Privet::Algorithms::Geometry::Tests;
using namespace Privet::Algorithms::Geometry;
using namespace Privet::Algorithms::Tests;
using namespace Privet::Algorithms;

namespace
{
    using distance_t = int;
    using long_int_t = int64_t;
    using tree_t = segment_tree_add_bool<distance_t>;

    using point_t = point2d<distance_t>;
    using rectangle_t = pair<point_t, point_t>;

    struct test_case final : base_test_case
    {
        const vector<rectangle_t> rectangles;
        const long_int_t expected;

        test_case(string&& name, vector<rectangle_t>&& rectangles, long_int_t expected)
            : base_test_case(forward<string>(name)),
            rectangles(forward<vector<rectangle_t>>(rectangles)),
            expected(expected)
        {
        }

        void Print(ostream& str) const override
        {
            base_test_case::Print(str);
            ::Print("rectangles", rectangles, str);
            if (expected > 0)
                ::PrintValue(str, "expected", expected);
            else
                str << "no expected";
        }

        void Validate() const override
        {
            base_test_case::Validate();

            RequirePositive(rectangles.size(), "rectangles.size");
            const auto size = static_cast<int>(rectangles.size());
            for (auto i = 0; i < size; ++i)
            {
                const auto is = "rectangle" + to_string(i);
                const auto& rectangle = rectangles[i];
                Assert::Greater(rectangle.second.X, rectangle.first.X, is + "X");
                Assert::Greater(rectangle.second.Y, rectangle.first.Y, is + "Y");
                Assert::GreaterOrEqual(rectangle.first.X, 0, is + "X");
                Assert::GreaterOrEqual(rectangle.first.Y, 0, is + "Y");
            }

            RequireNonNegative(expected, "expected");
        }
    };

    void generate_random(vector<test_case>& test_cases)
    {
        IntRandom r;
        for (auto att = 0, att_max = 1; att < att_max; ++att)
        {
            const auto n = r.operator()(1, 15);
            vector<rectangle_t> rectangles(n);

            for (auto i = 0; i < n; ++i)
            {
                auto& rectangle = rectangles[i];

                constexpr auto coord_max = 17;
                rectangle.first.X = r.operator()(0, coord_max);
                rectangle.first.Y = r.operator()(0, coord_max);

                constexpr auto delta_max = 6;
                rectangle.second.X = rectangle.first.X + r.operator()(1, delta_max);
                rectangle.second.Y = rectangle.first.Y + r.operator()(1, delta_max);
            }

            test_cases.emplace_back("random" + to_string(att), move(rectangles), 0);
        }
    }

    void generate_test_cases(vector<test_case>& test_cases)
    {
        generate_random(test_cases);

        test_cases.push_back(
            { "bug1",
            {{{1, 4},{2, 6}},// useless
                {{0, 1},{5, 6}},
                {{5, 1},{9, 4}}},
                5 * 5 + 3 * 4 });

        test_cases.push_back(
            { "bug2",
            {{{4, 1},{9, 4}},
                {{0, 4},{4, 8}},
                {{5, 2},{9, 7}},
                {{5, 1},{7, 3}}},// useless
                0 });

        test_cases.push_back(
            { "touch+useless",
            { { { 0, 0 },{ 2, 2 } },
            { { 2, 2 },{ 3, 3 } },
            { { 1, 1 },{ 2, 2 } },
            { { 1, 1 },{ 3, 3 } },
            { { 1, 1 },{ 3, 3 } }, // a copy
            { { 5, 5 },{ 6, 6 } } },
                8 });

        test_cases.push_back(
            { "2 y, many x",
            { { { 3, 0 },{ 4, 1 } },
            { { 5, 0 },{ 6, 1 } },
            { { 7, 0 },{ 8, 1 } },
            { { 9, 0 },{ 10, 1 } },
            { { 11, 0 },{ 12, 1 } },
            { { 13, 0 },{ 14, 1 } },
            { { 15, 0 },{ 16, 1 } } },
                7 });

        test_cases.push_back(
            { "2 x, many y",
            { { { 0, 0 },{ 1, 1 } },
            { { 0, 2 },{ 1, 3 } },
            { { 0, 5 },{ 1, 6 } },
            { { 0, 8 },{ 1, 10 } }, // 2
            { { 0, 11 },{ 1, 12 } },
            { { 0, 12 },{ 1, 13 } },
            { { 0, 15 },{ 1, 16 } },
            { { 0, 15 },{ 1, 17 } },
            { { 0, 17 },{ 1, 18 } } },
                10 });

        test_cases.push_back(
            { "two",
            { { { 5, 6 },{ 6, 7 } },
            { { 1, 3 },{ 2, 4 } } },
                2 });

        test_cases.push_back(
            { "one",
            {{ { 0, 4 }, { 2, 6 }}},
                4 });

        test_cases.push_back(
            { "trivial2",
            {{ { 2, 7 }, { 3, 8 }}},
                1 });

        test_cases.push_back(
            { "trivial1",
            {{ { 0, 1 }, { 1, 2 }}},
                1 });

        test_cases.push_back(
            { "trivial",
            {{ { 0, 0 }, { 1, 1 }}},
                1 });
    }

    void run_test_case(const test_case& test_case)
    {
        const auto actual = rectangle_common_area<tree_t, distance_t, long_int_t, point_t, rectangle_t>(test_case.rectangles);
        if (test_case.expected > 0)
            Assert::AreEqual(test_case.expected, actual, "rectangle_common_area");

        const auto slow = rectangle_common_area_slow<long_int_t, point_t, rectangle_t>(test_case.rectangles);
        Assert::AreEqual(actual, slow, "rectangle_common_area_slow");
    }
}

void Privet::Algorithms::Geometry::Tests::rectangle_common_area_tests()
{
    test_utilities<test_case>::run_tests(run_test_case, generate_test_cases);
}