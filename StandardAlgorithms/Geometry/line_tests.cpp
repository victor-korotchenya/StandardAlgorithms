#include"line_tests.h"
#include"../Utilities/test_utilities.h"
#include"line.h"
#include"line_utilities.h"
#include"point.h"
#include<tuple>

namespace
{
    using floating_t = Standard::Algorithms::floating_point_type;
    using point_d = Standard::Algorithms::Geometry::point2d<floating_t>;
    using line_d = Standard::Algorithms::Geometry::line2d<floating_t>;

    template<class t>
    constexpr void require_equal2(const t &expected, const t &actual, const std::string &message)
    {
        assert(!message.empty());

        if (are_equal<t>(expected, actual)) [[likely]]
        {
            return;
        }

        auto str = ::Standard::Algorithms::Utilities::w_stream();
        str << message << " got \n" << actual << "\nexpect\n" << expected;

        throw std::runtime_error(str.str());
    }

    constexpr void are_equal_test_impl(const line_d &one, const line_d &two)
    {
        assert(&one != &two);
        assert(Standard::Algorithms::Geometry::check_line(one));
        assert(Standard::Algorithms::Geometry::check_line(two));
        {
            const auto actual = are_equal<line_d, floating_t>(one, one);
            ::Standard::Algorithms::ert::are_equal(true, actual, "are_equal with self");
        }
        {
            const auto actual = are_equal<line_d, floating_t>(one, two);
            ::Standard::Algorithms::ert::are_equal(true, actual, "are_equal with a copy");
        }
        {
            const auto clone = one;
            const auto actual = are_equal<line_d, floating_t>(one, clone);
            ::Standard::Algorithms::ert::are_equal(true, actual, "are_equal with a clone");
        }
        {
            const line_d three{ one.A, one.B, one.C + 1 };
            assert(Standard::Algorithms::Geometry::check_line(three));

            const auto actual = are_equal<line_d, floating_t>(one, three);
            ::Standard::Algorithms::ert::are_equal(false, actual, "are_equal with C+1");
        }
    }

    constexpr void are_equal_tests_plain()
    {
        constexpr auto aaa = 3;
        constexpr auto bbb = 1;
        constexpr auto ccc = 50;
        const line_d one{ aaa, bbb, ccc };
        const line_d two{ aaa, bbb, ccc };
        are_equal_test_impl(one, two);

        {
            const line_d three{ one.A + 1, one.B, one.C };
            assert(Standard::Algorithms::Geometry::check_line(three));

            const auto actual = are_equal<line_d, floating_t>(one, three);
            ::Standard::Algorithms::ert::are_equal(false, actual, "are_equal with A+1");
        }
    }

    constexpr void are_equal_tests_vertical()
    {
        constexpr auto aaa = 1;
        constexpr auto bbb = 0;
        constexpr auto ccc = 39;
        const line_d one{ aaa, bbb, ccc };
        const line_d two{ aaa, bbb, ccc };
        are_equal_test_impl(one, two);
    }

    [[nodiscard]] constexpr auto are_equal_tests() -> bool
    {
        are_equal_tests_plain();

        try
        {
            are_equal_tests_vertical();
        }
        catch (const std::exception &exc)
        {
            const auto err = std::string("Vertical line error. ") + exc.what();
            throw std::runtime_error(err);
        }

        return true;
    }

    // Ensure that <compare> is correct.
    [[nodiscard]] constexpr auto line_compare_tests() -> bool
    {
        constexpr auto aaa = 3;
        constexpr auto bbb = 10;
        constexpr auto ccc = 500;

        const line_d sample{ aaa, bbb, ccc };

        const line_d one{ aaa - 1, bbb, ccc };
        const line_d two{ aaa, bbb - 1, ccc };
        const line_d three{ aaa, bbb, ccc - 1 };

        std::int32_t index{};

        for (const auto &cand : { one, two, three })
        {
            const auto name = ::Standard::Algorithms::Utilities::zu_string(index);
            {
                const auto les = (sample < cand) || (cand > sample);
                ::Standard::Algorithms::ert::are_equal(false, les, "les " + name);
            }
            {
                const auto les_equ = (sample <= cand) || (cand >= sample);
                ::Standard::Algorithms::ert::are_equal(false, les_equ, "les_equ " + name);
            }
            {
                const auto gre = (sample > cand) && cand < sample;
                ::Standard::Algorithms::ert::are_equal(true, gre, "gre " + name);
            }
            {
                const auto gre_eq = (sample >= cand) && cand <= sample;
                ::Standard::Algorithms::ert::are_equal(true, gre_eq, "gre_eq " + name);
            }
            {
                const auto equ = (sample == cand) || cand == sample;
                ::Standard::Algorithms::ert::are_equal(false, equ, "equ " + name);
            }
            {
                const auto neq = (sample != cand) && cand != sample;
                ::Standard::Algorithms::ert::are_equal(true, neq, "neq " + name);
            }

            ++index;
        }

        return true;
    }

    constexpr auto len5 = 5;

    [[nodiscard]] constexpr auto to_line_tests() -> bool
    {
        const std::initializer_list<std::tuple<point_d, point_d, line_d>> tests{// x=0, a vertical line
            { { 0, 0 }, { 0, 1 }, { 1, 0, 0 } }, { { 0, 1 }, { 0, 0 }, { 1, 0, 0 } },
            { { 0, 0 }, { 0, len5 }, { 1, 0, 0 } }, { { 0, len5 }, { 0, 0 }, { 1, 0, 0 } },
            // y=0
            { { 0, 0 }, { 1, 0 }, { 0, 1, 0 } }, { { 1, 0 }, { 0, 0 }, { 0, 1, 0 } },
            { { 0, 0 }, { len5, 0 }, { 0, 1, 0 } }, { { len5, 0 }, { 0, 0 }, { 0, 1, 0 } },
            // x + y = 0
            { { 0, 0 }, { 1, -1 }, { 1, 1, 0 } }, { { 1, -1 }, { 0, 0 }, { 1, 1, 0 } },
            { { 0, 0 }, { len5, -len5 }, { 1, 1, 0 } }, { { len5, -len5 }, { 0, 0 }, { 1, 1, 0 } },
            {// NOLINTNEXTLINE
                { 0, 0 }, { 5.2357891, -5.2357891 }, { 1, 1, 0 } },
            {// NOLINTNEXTLINE
                { 5.2357891, -5.2357891 }, { 0, 0 }, { 1, 1, 0 } },
            // x - y = 0
            { { 0, 0 }, { 1, 1 }, { -1, 1, 0 } }, // NOLINTNEXTLINE
            { { 1, 1 }, { 0, 0 }, { -1, 1, 0 } }, { { 0, 0 }, { len5, len5 }, { -1, 1, 0 } },
            { { len5, len5 }, { 0, 0 }, { -1, 1, 0 } },
            {// NOLINTNEXTLINE
                { 0, 0 }, { 5.2357891, 5.2357891 }, { -1, 1, 0 } },
            {// NOLINTNEXTLINE
                { 5.2357891, 5.2357891 }, { 0, 0 }, { -1, 1, 0 } },
            // 2x - 3y - 7 = 0
            // -2x/3 + y + 7/3 = 0
            {// NOLINTNEXTLINE
                { 2, -1 }, { 5, 1 }, { 2 / -3.0, 1, -7 / -3.0 } },
            {// NOLINTNEXTLINE
                { 5, 1 }, { 2, -1 }, { 2 / -3.0, 1, -7 / -3.0 } },
            {// NOLINTNEXTLINE
                { 5, 1 }, { 8, 3 }, { 2 / -3.0, 1, 7 / 3.0 } }
        };

        std::size_t index{};

        for (const auto &test : tests)
        {
            const auto &pnt = std::get<0>(test);
            const auto &pnt2 = std::get<1>(test);
            const auto &expected = std::get<2>(test);

            assert(Standard::Algorithms::Geometry::check_line(expected));

            const auto actual = Standard::Algorithms::Geometry::to_line<point_d, floating_t, line_d>(pnt, pnt2);

            require_equal2(expected, actual, "to_line at " + ::Standard::Algorithms::Utilities::zu_string(index));

            ++index;
        }

        return true;
    }

    constexpr auto len10 = 10;

    [[nodiscard]] constexpr auto intersect_lines_tests() -> bool
    {
        {
            // 3x + y + 5 = 4x + y + 10 = 0
            // 5-10 = x = -5
            //  y = 0 -5 -3x = -5 + 15 = 10
            const line_d line{ 3, 1, len5 };
            assert(Standard::Algorithms::Geometry::check_line(line));

            const line_d line2{ 4, 1, len10 };
            assert(Standard::Algorithms::Geometry::check_line(line2));

            point_d cross_point{};
            const auto actual = intersect_at_one_point(line, line2, cross_point);
            const auto *const name = "intersect_at_one_point 1";
            ::Standard::Algorithms::ert::are_equal(true, actual, name);

            const point_d expected{ -len5, len10 };
            ::Standard::Algorithms::ert::are_equal(expected, cross_point, name);
        }
        {
            // x - 50 = y + 123 = 0
            // NOLINTNEXTLINE
            const line_d line{ 1, 0, -50 };
            assert(Standard::Algorithms::Geometry::check_line(line));

            // NOLINTNEXTLINE
            const line_d line2{ 0, 1, 123 };
            assert(Standard::Algorithms::Geometry::check_line(line));

            point_d cross_point{};
            const auto actual = intersect_at_one_point(line, line2, cross_point);
            const auto *const name = "intersect_at_one_point 2";
            ::Standard::Algorithms::ert::are_equal(true, actual, name);

            // NOLINTNEXTLINE
            const point_d expected{ 50, -123 };
            ::Standard::Algorithms::ert::are_equal(expected, cross_point, name);
        }
        {
            // x + 5 = x + 10 = 0
            // No sol
            const line_d line{ 1, 0, len5 };
            assert(Standard::Algorithms::Geometry::check_line(line));

            const line_d line2{ 1, 0, len10 };
            assert(Standard::Algorithms::Geometry::check_line(line));

            point_d cross_point{};
            const auto actual = intersect_at_one_point(line, line2, cross_point);
            const auto *const name = "intersect_at_one_point 20";
            ::Standard::Algorithms::ert::are_equal(false, actual, name);
        }

        const std::vector<line_d> sames{
            { 1, 0, len5 },
            // NOLINTNEXTLINE
            { 1, 1, -0.3 },
        };

        for (std::size_t index{}; index < sames.size(); ++index)
        {
            const auto &line = sames[index];
            const auto line2 = line;
            assert(Standard::Algorithms::Geometry::check_line(line));

            point_d cross_point{};
            const auto actual = intersect_at_one_point(line, line2, cross_point);

            const auto name = "intersect_at_one_point - no unique sol for the same lines at " +
                ::Standard::Algorithms::Utilities::zu_string(index);

            ::Standard::Algorithms::ert::are_equal(false, actual, name);
        }

        return true;
    }
} // namespace

void Standard::Algorithms::Geometry::Tests::line_tests()
{
    static_assert(are_equal_tests());
    static_assert(line_compare_tests());
    static_assert(to_line_tests());
    static_assert(intersect_lines_tests());
}
