#define _CRT_SECURE_NO_WARNINGS
#define D_SCL_SECURE_NO_WARNINGS
#define _USE_MATH_DEFINES

//Avoid errors like: Warning C4003 not enough actual parameters for macro 'min'.
#define NOMINMAX

#include <tuple>
#include "point.h"
#include "point_utilities.h"
#include "point_tests.h"
#include "../Utilities/PrintUtilities.h"
#include "../test_utilities.h"

using namespace std;
using namespace Privet::Algorithms::Geometry::Tests;
using namespace Privet::Algorithms::Geometry;
using namespace Privet::Algorithms::Tests;
using namespace Privet::Algorithms;

namespace
{
    using d_t = double;
    using point_d = point2d<d_t>;

    void rotate_tests()
    {
        const point_d source(1, 2);

        const vector<pair<d_t, point_d>> tests{
            { 1.2345,{ -1.5579734902104545, 1.6039696393016469 } },
            { 0.0, source },
            { M_PI * 2, source },
            { M_PI / 2,{ -2, 1 } },
            { M_PI,{ -1, -2 } },
            { M_PI * 3 / 2,{ 2, -1 } },
        };

        for (const auto& test : tests)
        {
            const auto actual = rotate(source, test.first);
            const auto name = "rotate " + to_string(test.first) + " ";
            Assert::AreEqualWithEpsilon(test.second.X, actual.X, name.c_str());
            Assert::AreEqualWithEpsilon(test.second.Y, actual.Y, name.c_str());
        }
    }

    void closest_to_line_tests()
    {
        const vector<tuple<point_d, point_d, point_d, point_d, point_d>> tests{
            // point, line_point1, line_point2, closest
            { { 1, 1 }, { 0, 0 }, { 3, 4 }, { 0.84, 1.12 }, { 0.84, 1.12 } },
            { { 1, -7 }, { 0, 0 }, { 3, 4 }, { -3, -4 }, { 0, 0 } },
            { { 5, -10 }, { 0, 0 }, { 3, 4 }, { -3, -4 }, { 0, 0 } },
            { { 7, -7 }, { 2, 3 }, { 5, 7 }, { -1, -1 }, { 2, 3 } },
            { { 2, 2 }, { 0, 1 }, { 1, 4 }, { 0.5, 2.5 }, { 0.5, 2.5 } },
            { { 1, 1 }, { 0, 1 }, { 1, 4 }, { 0.1, 1.3 }, { 0.1, 1.3 } },
            { { 4, 4 }, { 0, 2 }, { 2, 8 }, { 1, 5 }, { 1, 5 } },
            { { 0, 1 }, { 0, 0 }, { 1, 1 }, { 0.5, 0.5 }, { 0.5, 0.5 } },
            { { 1, 0 }, { 0, 0 }, { 1, 1 }, { 0.5, 0.5 }, { 0.5, 0.5 } },
            { { 0, 0 }, { 0, 0 }, { 1, 0 }, { 0, 0 }, { 0, 0 } }, // 1st
            { { 1, 0 }, { 0, 0 }, { 1, 0 }, { 1, 0 }, { 1, 0 } }, // 2nd
            { { 1, 1 }, { 0, 0 }, { 1, 0 }, { 1, 0 }, { 1, 0 } }, // y=0
        };

        auto i = -1;
        for (const auto& test : tests)
        {
            ++i;
            const auto is = to_string(i);
            const auto& point = get<0>(test), & a = get<1>(test), & b = get<2>(test);
            {
                const auto actual = closest_point_to_line(point, a, b);
                const auto name = "closest_point_to_line at " + is + ", " + to_string(actual);

                const auto& expected = get<3>(test);

                Assert::AreEqualWithEpsilon(expected.X, actual.X, name.c_str());
                Assert::AreEqualWithEpsilon(expected.Y, actual.Y, name.c_str());
            }
            {
                const auto actual = closest_point_to_line_segment(point, a, b);
                const auto name = "closest_point_to_line_segment at " + is + ", " + to_string(actual);

                const auto& expected = get<4>(test);

                Assert::AreEqualWithEpsilon(expected.X, actual.X, name.c_str());
                Assert::AreEqualWithEpsilon(expected.Y, actual.Y, name.c_str());
            }
        }
    }

    void calc_angle_radian_tests()
    {
        const point_d a(101, 100), b(100, 100), c(101, 101);
        {
            const auto actual = calc_angle_radian(a, b, c);
            const auto expected = M_PI_4;
            Assert::AreEqualWithEpsilon(expected, actual, "calc_angle_radian 3");
        }
        {
            const auto actual = calc_angle_radian(c, b, a);
            const auto expected = M_PI_4;
            Assert::AreEqualWithEpsilon(expected, actual, "calc_angle_radian 3 cba");
        }
        {
            const auto actual = calc_angle_radian(a, b, a);
            Assert::AreEqualWithEpsilon(0, actual, "calc_angle_radian 3 aba");
        }
        {
            const auto actual = calc_angle_radian(b, a, b);
            Assert::AreEqualWithEpsilon(0, actual, "calc_angle_radian 3 bab");
        }

        const auto expected3 = 1.5707963257958957;
        const point_d d(100.0001, -1e5);
        {
            const auto actual = calc_angle_radian(a, b, d);
            Assert::AreEqualWithEpsilon(expected3, actual, "calc_angle_radian abd");
        }
        {
            const auto actual = calc_angle_radian(d, b, a);
            Assert::AreEqualWithEpsilon(expected3, actual, "calc_angle_radian dba");
        }
    }

    void are_collinear_tests()
    {
        const point_d a(101, 100), b(100, 100), c(101, 101);
        {
            const auto actual = are_collinear(a, b, c);
            Assert::AreEqualWithEpsilon(false, actual, "are_collinear abc");
        }
        {
            const auto actual = are_collinear(a, b, a);
            Assert::AreEqualWithEpsilon(true, actual, "are_collinear aba");
        }
        {
            const auto actual = are_collinear(a, a, b);
            Assert::AreEqualWithEpsilon(true, actual, "are_collinear aab");
        }
        {
            const auto actual = are_collinear(b, a, a);
            Assert::AreEqualWithEpsilon(true, actual, "are_collinear baa");
        }
        {
            const auto actual = are_collinear(a, a, a);
            Assert::AreEqualWithEpsilon(true, actual, "are_collinear aaa");
        }
        {
            const auto actual = are_collinear(b, b, b);
            Assert::AreEqualWithEpsilon(true, actual, "are_collinear bbb");
        }

        constexpr auto epsilon = epsilon_default<double>() / 2;
        const point_d d(a.X + epsilon, a.Y - epsilon);
        {
            const auto actual = d.X != a.X && d.Y != a.Y && are_collinear(a, b, d);
            Assert::AreEqualWithEpsilon(true, actual, "are_collinear abd");
        }
    }
}

void Privet::Algorithms::Geometry::Tests::point_tests()
{
    rotate_tests();
    closest_to_line_tests();
    calc_angle_radian_tests();
    are_collinear_tests();
}