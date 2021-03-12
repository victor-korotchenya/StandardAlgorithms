#include <tuple>
#include "point.h"
#include "line.h"
#include "line_utilities.h"
#include "line_tests.h"
#include "../test_utilities.h"

using namespace std::literals;
using namespace std;
using namespace Privet::Algorithms::Geometry;
using namespace Privet::Algorithms::Tests;
using namespace Privet::Algorithms;

namespace
{
    using d_t = double;
    using point_d = point2d<d_t>;
    using line_d = line2d<d_t>;

    template<class t>
    void RequireEqual(const t& expected, const t& actual, const string& message)
    {
        assert(message.size());

        if (are_equal<t>(expected, actual))
            return;

        throw runtime_error(message + " got \n" +
            to_string(actual) + "\nexpect\n" +
            to_string(expected));
    }

    void are_equal_test_impl(const line_d& l1, const line_d& l2)
    {
        assert(&l1 != &l2);
        {
            const auto actual = are_equal<line_d, d_t>(l1, l1);
            Assert::AreEqual(true, actual, "are_equal with self");
        }
        {
            const auto actual = are_equal<line_d, d_t>(l1, l2);
            Assert::AreEqual(true, actual, "are_equal with a copy");
        }
        {
            const auto clone = l1;
            const auto actual = are_equal<line_d, d_t>(l1, clone);
            Assert::AreEqual(true, actual, "are_equal with a clone");
        }
        {
            const line_d l3(l1.A, l1.B, l1.C + 1);

            const auto actual = are_equal<line_d, d_t>(l1, l3);
            Assert::AreEqual(false, actual, "are_equal with C+1");
        }
    }

    void are_equal_tests_plain()
    {
        const line_d l1(3, 1, 50), l2(3, 1, 50);
        are_equal_test_impl(l1, l2);

        {
            const line_d l3(l1.A + 1, l1.B, l1.C);

            const auto actual = are_equal<line_d, d_t>(l1, l3);
            Assert::AreEqual(false, actual, "are_equal with A+1");
        }
    }

    void are_equal_tests_vertical()
    {
        const line_d l1(1, 0, 39), l2(1, 0, 39);
        are_equal_test_impl(l1, l2);
    }

    void are_equal_tests()
    {
        are_equal_tests_plain();
        try
        {
            are_equal_tests_vertical();
        }
        catch (const exception& ex)
        {
            const auto err = "Vertical line error. "s + ex.what();
            throw runtime_error(err);
        }
    }

    void to_line_tests()
    {
        const initializer_list<tuple<point_d, point_d, line_d>> tests{
            //x=0, a vertical line
            { {0,0},{0,1}, {1, 0, 0}, },
            { {0,1},{0,0}, {1, 0, 0}, },
            { {0,0},{0,5}, {1, 0, 0}, },
            { {0,5},{0,0}, {1, 0, 0}, },
            //y=0
            { {0,0},{1,0}, {0, 1, 0}, },
            { {1,0},{0,0}, {0, 1, 0}, },
            { {0,0},{5,0}, {0, 1, 0}, },
            { {5,0},{0,0}, {0, 1, 0}, },
            // x + y = 0
            { {0,0},{1,-1}, {1, 1, 0}, },
            { {1,-1},{0,0}, {1, 1, 0}, },
            { {0,0},{5,-5}, {1, 1, 0}, },
            { {5,-5},{0,0}, {1, 1, 0}, },
            { {0,0},{5.2357891,-5.2357891 }, {1, 1, 0}, },
            { {5.2357891,-5.2357891 },{0,0}, {1, 1, 0}, },
            // x - y = 0
            { {0,0},{1,1}, {-1, 1, 0}, },
            { {1,1},{0,0}, {-1, 1, 0}, },
            { {0,0},{5,5}, {-1, 1, 0}, },
            { {5,5},{0,0}, {-1, 1, 0}, },
            { {0,0},{5.2357891,5.2357891 }, {-1, 1, 0}, },
            { {5.2357891,5.2357891 },{0,0}, {-1, 1, 0}, },
            // 2x - 3y - 7 = 0
            // -2x/3 + y + 7/3 = 0
            { { 2,-1 },{ 5,1 }, { 2 / -3.0, 1, -7 / -3.0 }, },
            { { 5,1 },{ 2,-1 }, { 2 / -3.0, 1, -7 / -3.0 }, },
            { { 5,1 },{ 8,3 }, { 2 / -3.0, 1, 7 / 3.0 }, },
        };
        auto i = 0u;
        for (auto it = tests.begin(); it != tests.end(); ++it, ++i)
        {
            const auto& test = *it;
            const auto& p = get<0>(test), & p2 = get<1>(test);
            const auto& expected = get<2>(test);

            const auto actual = to_line<point_d, d_t, line_d>(p, p2);
            RequireEqual(expected, actual, "to_line at " + to_string(i));
        }
    }

    void intersect_lines_tests()
    {
        {
            //3x + y + 5 = 4x + y + 10 = 0
            //5-10 = x = -5
            // y = 0 -5 -3x = -5 + 15 = 10
            const line_d line(3, 1, 5), line2(4, 1, 10);

            point_d p(1000000, 100);
            const auto actual = intersect_at_one_point(line, line2, p);
            const auto name = "intersect_at_one_point 1";
            Assert::AreEqual(true, actual, name);

            const point_d expected(-5, 10);
            Assert::AreEqual(expected, p, name);
        }
        {
            //x - 50 = y + 123 = 0
            const line_d line(1, 0, -50), line2(0, 1, 123);

            point_d p(1000000, 100);
            const auto actual = intersect_at_one_point(line, line2, p);
            const auto name = "intersect_at_one_point 2";
            Assert::AreEqual(true, actual, name);

            const point_d expected(50, -123);
            Assert::AreEqual(expected, p, name);
        }
        {
            //x + 5 = x + 10 = 0
            //No sol
            const line_d line(1, 0, 5), line2(1, 0, 10);

            point_d p(1000000, 100);
            const auto actual = intersect_at_one_point(line, line2, p);
            const auto name = "intersect_at_one_point 20";
            Assert::AreEqual(false, actual, name);
        }

        const vector<line_d> sames{ {1, 0, 5}, {1, 1, -0.3}, };
        for (auto i = 0u; i < sames.size(); ++i)
        {
            //
            const auto& line = sames[i], line2 = line;

            point_d p(1000000, 100);
            const auto actual = intersect_at_one_point(line, line2, p);
            const auto name = "intersect_at_one_point - no unique sol for the same lines at " + to_string(i);
            Assert::AreEqual(false, actual, name);
        }
    }
}

void Privet::Algorithms::Geometry::Tests::line_tests()
{
    are_equal_tests();
    to_line_tests();
    intersect_lines_tests();
}