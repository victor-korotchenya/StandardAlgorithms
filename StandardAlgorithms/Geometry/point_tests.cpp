#include"point_tests.h"
#include"../Utilities/test_utilities.h"
#include"point.h"
#include"point_utilities.h"
#include<numbers>
#include<tuple>

namespace
{
    using floating_t = Standard::Algorithms::floating_point_type;
    using point_d = Standard::Algorithms::Geometry::point2d<floating_t>;

    // C was 100 (one hundred) in Roman Empire.
    constexpr auto c_roman = 100; // NOLINTNEXTLINE
    static_assert(0x64 == c_roman);

    void rotate_tests()
    {
        const point_d source{ 1, 2 };

        const std::vector<std::pair<floating_t, point_d>> tests{
            // NOLINTNEXTLINE
            { 1.2345, { -1.5579734902104545, 1.6039696393016469 } },
            { 0.0, source },
            { std::numbers::pi * 2, source },
            { std::numbers::pi / 2, { -2, 1 } },
            { std::numbers::pi, { -1, -2 } },
            { std::numbers::pi * 3 / 2, { 2, -1 } },
        };

        for (const auto &test : tests)
        {
            const auto actual = Standard::Algorithms::Geometry::rotate(source, test.first);
            const auto name = "rotate " + std::to_string(test.first) + " ";
            ::Standard::Algorithms::ert::are_equal_with_epsilon(test.second.x, actual.x, name);
            ::Standard::Algorithms::ert::are_equal_with_epsilon(test.second.y, actual.y, name);
        }
    }

    void closest_to_line_tests()
    {
        const std::vector<std::tuple<point_d, point_d, point_d, point_d, point_d>> tests{
            // point, line_point1, line_point2, closest
            // NOLINTNEXTLINE
            { { 1, 1 }, { 0, 0 }, { 3, 4 }, { 0.84, 1.12 }, { 0.84, 1.12 } },
            // NOLINTNEXTLINE
            { { 1, -7 }, { 0, 0 }, { 3, 4 }, { -3, -4 }, { 0, 0 } },
            // NOLINTNEXTLINE
            { { 5, -10 }, { 0, 0 }, { 3, 4 }, { -3, -4 }, { 0, 0 } },
            // NOLINTNEXTLINE
            { { 7, -7 }, { 2, 3 }, { 5, 7 }, { -1, -1 }, { 2, 3 } },
            // NOLINTNEXTLINE
            { { 2, 2 }, { 0, 1 }, { 1, 4 }, { 0.5, 2.5 }, { 0.5, 2.5 } },
            // NOLINTNEXTLINE
            { { 1, 1 }, { 0, 1 }, { 1, 4 }, { 0.1, 1.3 }, { 0.1, 1.3 } },
            // NOLINTNEXTLINE
            { { 4, 4 }, { 0, 2 }, { 2, 8 }, { 1, 5 }, { 1, 5 } },
            // NOLINTNEXTLINE
            { { 0, 1 }, { 0, 0 }, { 1, 1 }, { 0.5, 0.5 }, { 0.5, 0.5 } },
            // NOLINTNEXTLINE
            { { 1, 0 }, { 0, 0 }, { 1, 1 }, { 0.5, 0.5 }, { 0.5, 0.5 } },
            // NOLINTNEXTLINE
            { { 0, 0 }, { 0, 0 }, { 1, 0 }, { 0, 0 }, { 0, 0 } }, // 1st
            { { 1, 0 }, { 0, 0 }, { 1, 0 }, { 1, 0 }, { 1, 0 } }, // 2nd
            { { 1, 1 }, { 0, 0 }, { 1, 0 }, { 1, 0 }, { 1, 0 } }, // y=0
        };

        auto str = ::Standard::Algorithms::Utilities::w_stream();
        auto index = -1;

        for (const auto &test : tests)
        {
            ++index;
            const auto iss = std::to_string(index);
            const auto &point = std::get<0>(test);
            const auto &aaa = std::get<1>(test);
            const auto &bbb = std::get<2>(test);
            {
                const auto actual = Standard::Algorithms::Geometry::closest_point_to_line(point, aaa, bbb);

                Standard::Algorithms::Utilities::reset(str);
                str << "closest_point_to_line at " << iss << ", " << actual;

#ifndef __clang__
                const auto *const name = str.view().data();
#else
                const auto buff = str.str();
                const auto *const name = buff.data();
#endif

                const auto &expected = std::get<3>(test);

                ::Standard::Algorithms::ert::are_equal_with_epsilon(expected.x, actual.x, name);
                ::Standard::Algorithms::ert::are_equal_with_epsilon(expected.y, actual.y, name);
            }
            {
                const auto actual = Standard::Algorithms::Geometry::closest_point_to_line_segment(point, aaa, bbb);

                Standard::Algorithms::Utilities::reset(str);
                str << "closest_point_to_line_segment at " << iss << ", " << actual;

#ifndef __clang__
                const auto *const name = str.view().data();
#else
                const auto buff = str.str();
                const auto *const name = buff.data();
#endif

                const auto &expected = std::get<4>(test);

                ::Standard::Algorithms::ert::are_equal_with_epsilon(expected.x, actual.x, name);
                ::Standard::Algorithms::ert::are_equal_with_epsilon(expected.y, actual.y, name);
            }
        }
    }

    void calc_angle_radian_tests()
    {
        constexpr point_d aaa{ c_roman + 1, c_roman };
        constexpr point_d bbb{ c_roman, c_roman };
        constexpr point_d ccc{ c_roman + 1, c_roman + 1 };
        {
            const auto actual = Standard::Algorithms::Geometry::calc_angle_radian(aaa, bbb, ccc);
            const auto expected = std::numbers::pi / 4;
            ::Standard::Algorithms::ert::are_equal_with_epsilon(expected, actual, "calc_angle_radian 3");
        }
        {
            const auto actual = Standard::Algorithms::Geometry::calc_angle_radian(ccc, bbb, aaa);
            constexpr auto expected = std::numbers::pi / 4;
            ::Standard::Algorithms::ert::are_equal_with_epsilon(expected, actual, "calc_angle_radian 3 cba");
        }
        {
            const auto actual = Standard::Algorithms::Geometry::calc_angle_radian(aaa, bbb, aaa);
            ::Standard::Algorithms::ert::are_equal_with_epsilon(0, actual, "calc_angle_radian 3 aba");
        }
        {
            const auto actual = Standard::Algorithms::Geometry::calc_angle_radian(bbb, aaa, bbb);
            ::Standard::Algorithms::ert::are_equal_with_epsilon(0, actual, "calc_angle_radian 3 bab");
        }

        constexpr auto expected3 = 1.5707963257958957;
        constexpr point_d ddd{ c_roman + 0.0001, -1e5 };
        {
            const auto actual = Standard::Algorithms::Geometry::calc_angle_radian(aaa, bbb, ddd);
            ::Standard::Algorithms::ert::are_equal_with_epsilon(expected3, actual, "calc_angle_radian abd");
        }
        {
            const auto actual = Standard::Algorithms::Geometry::calc_angle_radian(ddd, bbb, aaa);
            ::Standard::Algorithms::ert::are_equal_with_epsilon(expected3, actual, "calc_angle_radian dba");
        }
    }

    void are_collinear_tests()
    {
        constexpr point_d aaa{ c_roman + 1, c_roman };
        constexpr point_d bbb{ c_roman, c_roman };
        constexpr point_d ccc{ c_roman + 1, c_roman + 1 };
        {
            const auto actual = Standard::Algorithms::Geometry::are_collinear(aaa, bbb, ccc);
            ::Standard::Algorithms::ert::are_equal(false, actual, "are_collinear abc");
        }
        {
            const auto actual = Standard::Algorithms::Geometry::are_collinear(aaa, bbb, aaa);
            ::Standard::Algorithms::ert::are_equal(true, actual, "are_collinear aba");
        }
        {
            const auto actual = Standard::Algorithms::Geometry::are_collinear(aaa, aaa, bbb);
            ::Standard::Algorithms::ert::are_equal(true, actual, "are_collinear aab");
        }
        {
            const auto actual = Standard::Algorithms::Geometry::are_collinear(bbb, aaa, aaa);
            ::Standard::Algorithms::ert::are_equal(true, actual, "are_collinear baa");
        }
        {
            const auto actual = Standard::Algorithms::Geometry::are_collinear(aaa, aaa, aaa);
            ::Standard::Algorithms::ert::are_equal(true, actual, "are_collinear aaa");
        }
        {
            const auto actual = Standard::Algorithms::Geometry::are_collinear(bbb, bbb, bbb);
            ::Standard::Algorithms::ert::are_equal(true, actual, "are_collinear bbb");
        }

        constexpr auto epsilon = Standard::Algorithms::epsilon_default<floating_t>() / 2;

        constexpr point_d ddd{ aaa.x + epsilon, aaa.y - epsilon };
        {
            const auto actual =
                ddd.x != aaa.x && ddd.y != aaa.y && Standard::Algorithms::Geometry::are_collinear(aaa, bbb, ddd);
            ::Standard::Algorithms::ert::are_equal(true, actual, "are_collinear abd");
        }
    }
} // namespace

void Standard::Algorithms::Geometry::Tests::point_tests()
{
    rotate_tests();
    closest_to_line_tests();
    calc_angle_radian_tests();
    are_collinear_tests();
}
