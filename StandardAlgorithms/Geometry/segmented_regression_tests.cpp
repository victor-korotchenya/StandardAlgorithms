#include"segmented_regression_tests.h"
#include"../Numbers/number_comparer.h"
#include"../Utilities/has_openmp.h"
#include"../Utilities/test_utilities.h"
#include"segmented_regression.h"
#include"segmented_regression_old.h"
#include<vector>

namespace
{
    // todo(p3): Compare classes with approx members.
    using small_floating_t = float;
    using point_t = Standard::Algorithms::Geometry::point2d<small_floating_t>;

    // todo(p4): using floating_t = Standard::Algorithms::floating_point_type;
    using floating_t = double;
    static_assert(sizeof(small_floating_t) <= sizeof(floating_t));

    using segment_info_t = Standard::Algorithms::Geometry::segment_info<floating_t>;
    using segment_result_t = Standard::Algorithms::Geometry::segment_result<floating_t>;

    constexpr floating_t epsilon{ 1.0e-3 };

    constexpr Standard::Algorithms::Numbers::number_comparer<floating_t> comparer{ epsilon,
        Standard::Algorithms::Numbers::absolute_value<floating_t> };

    [[nodiscard]] inline auto are_equal_numbers(const floating_t &one, const floating_t &two) -> bool
    {
        auto result = comparer.are_equal(one, two);
        return result;
    }

    [[nodiscard]] inline auto are_equal_results(
        const segment_result_t *const a_ptr, const segment_result_t *const b_ptr) -> bool
    {
        if (a_ptr == b_ptr)
        {
            return true;
        }

        if (a_ptr == nullptr || b_ptr == nullptr) [[unlikely]]
        {
            return false;
        }

        const auto &one = *a_ptr;
        const auto &two = *b_ptr;

        const auto &info_a = one.info;
        const auto &info_b = two.info;

        auto result = one.point1 == two.point1 && one.point2 == two.point2 &&
            comparer.are_equal(info_a.slope, info_b.slope) && comparer.are_equal(info_a.intercept, info_b.intercept) &&
            comparer.are_equal(info_a.error, info_b.error);

        return result;
    }

    struct test_case final : Standard::Algorithms::Tests::base_test_case
    {
        constexpr test_case(std::string &&name, floating_t segment_cost, std::vector<point_t> &&points = {},
            floating_t expected_cost = {}, std::vector<segment_result_t> &&expected_segments = {}) noexcept
            : base_test_case(std::move(name))
            , Segment_cost(segment_cost)
            , Points(std::move(points))
            , Expected_cost(expected_cost)
            , Expected_segments(std::move(expected_segments))
        {
        }

        [[nodiscard]] constexpr auto segment_cost() const &noexcept -> const floating_t &
        {
            return Segment_cost;
        }

        [[nodiscard]] constexpr auto segment_cost() &noexcept -> floating_t &
        {
            return Segment_cost;
        }

        [[nodiscard]] constexpr auto points() const &noexcept -> const std::vector<point_t> &
        {
            return Points;
        }

        [[nodiscard]] constexpr auto points() &noexcept -> std::vector<point_t> &
        {
            return Points;
        }

        [[nodiscard]] constexpr auto expected_cost() const &noexcept -> const floating_t &
        {
            return Expected_cost;
        }

        [[nodiscard]] constexpr auto expected_cost() &noexcept -> floating_t &
        {
            return Expected_cost;
        }

        [[nodiscard]] constexpr auto expected_segments() const &noexcept -> const std::vector<segment_result_t> &
        {
            return Expected_segments;
        }

        [[nodiscard]] constexpr auto expected_segments() &noexcept -> std::vector<segment_result_t> &
        {
            return Expected_segments;
        }

        void print(std::ostream &str) const override
        {
            str << " Segment cost " << Segment_cost;
            ::Standard::Algorithms::print("points", Points, str);

            ::Standard::Algorithms::print("expected segments", Expected_segments, str);

            str << " expected cost " << Expected_cost;
        }

private:
        floating_t Segment_cost;
        std::vector<point_t> Points;

        floating_t Expected_cost;
        std::vector<segment_result_t> Expected_segments;
    };

    constexpr void generate_test_cases(std::vector<test_case> &test_cases)
    {
        {// Put the longest running one first.
            constexpr auto segment_cost = 10'000;

            test_case test("Parabola", segment_cost);

            constexpr auto length = 20;

            for (std::int32_t index{}; index < length; ++index)
            {
                const auto xxx = static_cast<small_floating_t>(index);

                // NOLINTNEXTLINE
                const auto yyy = static_cast<small_floating_t>(xxx * xxx * 7 + 3.4F);
                // todo(p4): Clang bug? test.points().emplace_back(x, y);
                test.points().push_back(point_t{ xxx, yyy });
            }

            // NOLINTNEXTLINE
            test.expected_cost() = 40061.323095023399;

            // todo(p4): Clang bug? test.expected_segments().push_back({// todo(p4): Clang bug?
            // // NOLINTNEXTLINE
            // segment_info_t{41.999998177800862, -31.599995936666215, 4115.9994459152786}, 0, 6);

            test.expected_segments().push_back({// todo(p4): Clang bug?
                // NOLINTNEXTLINE
                segment_info_t{ 41.999998177800862, -31.599995936666215, 4115.9994459152786 }, 0, 6 });

            test.expected_segments().push_back({// todo(p4): Clang bug?
                // NOLINTNEXTLINE
                segment_info_t{ 133.00002615792411, -607.93356759207597, 1829.3321940172568 }, 7, 12 });

            test.expected_segments().push_back({// todo(p4): Clang bug?
                // NOLINTNEXTLINE
                segment_info_t{ 223.99996512276786, -1760.5994524274554, 4115.9914550908661 }, 13, 19 });

            test_cases.push_back(std::move(test));
        }
        {
            constexpr auto segment_cost = 0.1;
            test_case test("3Lines", segment_cost);

            const std::array points_c = { point_t{ 1, 1 }, point_t{ 2, 1 }, point_t{ 3, 1 },
                // NOLINTNEXTLINE
                point_t{ 4, 2 }, point_t{ 5, 3 }, point_t{ 6, 4 }, point_t{ 7, 5 },
                // NOLINTNEXTLINE
                point_t{ 8, 3 }, point_t{ 9, 1 }, point_t{ 10, -1 }, point_t{ 11, -3 }, point_t{ 12, -5 } };
            test.points().insert(test.points().end(), points_c.cbegin(), points_c.cend());

            test.expected_cost() = test.segment_cost() * 3;

            test.expected_segments().push_back({// todo(p4): Clang bug?
                // NOLINTNEXTLINE
                segment_info_t{ 0.0, 1.0, 0.0 }, 0, 1 });

            test.expected_segments().push_back({// todo(p4): Clang bug?
                // NOLINTNEXTLINE
                segment_info_t{ 1.0, -2.0, 0.0 }, 2, 5 });

            test.expected_segments().push_back({// todo(p4): Clang bug?
                // NOLINTNEXTLINE
                segment_info_t{ -2.0, 19.0, 0.0 }, 6, 11 });

            test_cases.push_back(std::move(test));
        }
        {// This test case must go right after the previous one.
            auto test = test_cases.back();
            test.name("3LinesAs2");

            // NOLINTNEXTLINE
            test.segment_cost() = 50.0;

            constexpr floating_t error = 1.0857142857142859;
            test.expected_cost() = 2 * test.segment_cost() + error;

            test.expected_segments().clear();
            test.expected_segments().push_back({// todo(p4): Clang bug?
                // NOLINTNEXTLINE
                segment_info_t{ 0.62857142857142856, -0.19999999999999987, error }, 0, 5 });

            test.expected_segments().push_back({// todo(p4): Clang bug?
                // NOLINTNEXTLINE
                segment_info_t{ -2.0, 19.0, 0.0 }, 6, 11 });

            test_cases.push_back(std::move(test));
        }
        {// This test case must go right after the previous one.
            auto test = test_cases.back();
            test.name("3LinesAs1");

            // NOLINTNEXTLINE
            test.segment_cost() = 500.0;

            constexpr floating_t error = 64.825174825174827;
            test.expected_cost() = test.segment_cost() + error;

            test.expected_segments().clear();
            test.expected_segments().push_back({// todo(p4): Clang bug?
                // NOLINTNEXTLINE
                segment_info_t{ -0.41958041958041958, 3.7272727272727271, error }, 0, 11 });

            test_cases.push_back(std::move(test));
        }
        {
            constexpr auto segment_cost = 0.1;
            test_case test("2Points", segment_cost);

            const std::array points_c = {// NOLINTNEXTLINE
                point_t{ 3, 1 }, point_t{ 6, 4 }
            };
            test.points().insert(test.points().end(), points_c.cbegin(), points_c.cend());

            test.expected_cost() = test.segment_cost();

            test.expected_segments().push_back({// todo(p4): Clang bug?
                // NOLINTNEXTLINE
                segment_info_t{ 1.0, -2.0, 0.0 }, 0, 1 });

            test_cases.push_back(std::move(test));
        }
        {
            constexpr auto segment_cost = 0.001;
            test_case test("Zigzag", segment_cost);

            const std::array points_c = {// NOLINTNEXTLINE
                point_t{ 1, 1 }, point_t{ 2, 3 }, point_t{ 3, 5 }, point_t{ 4, 3 },

                // NOLINTNEXTLINE
                point_t{ 5, 1 }, point_t{ 6, 3 }, point_t{ 7, 5 }, point_t{ 8, 3 },

                // NOLINTNEXTLINE
                point_t{ 9, 1 }, point_t{ 10, 3 }, point_t{ 11, 5 }, point_t{ 12, 3 },

                // NOLINTNEXTLINE
                point_t{ 13, 1 }, point_t{ 14, 3 }, point_t{ 15, 5 }, point_t{ 16, 3 },

                // // NOLINTNEXTLINE
                point_t{ 17, 1 }, point_t{ 18, 3 }, point_t{ 19, 5 }, point_t{ 20, 3 }
            };
            test.points().insert(test.points().end(), points_c.cbegin(), points_c.cend());

            // NOLINTNEXTLINE
            test.expected_cost() = 0.010000000000000002;

            constexpr auto max_index = 5U;

            for (std::size_t index{}; index < max_index; ++index)
            {
                const auto mult4 = index << 2U;
                const auto mult8 = static_cast<floating_t>(index << 3U);

                test.expected_segments().push_back({// todo(p4): Clang bug?
                    // NOLINTNEXTLINE
                    segment_info_t{ 2.0, -1.0 - mult8, 0 }, mult4, 1U + mult4 });

                test.expected_segments().push_back({// todo(p4): Clang bug?
                    // NOLINTNEXTLINE
                    segment_info_t{ -2.0, 11.0 + mult8, 0 }, 2U + mult4, 3U + mult4 });
            }

            test_cases.push_back(std::move(test));
        }
    }

    constexpr void run_test_case(const test_case &test)
    {
        constexpr auto infinity = std::numeric_limits<floating_t>::infinity();

        // todo(p2): Call non-static method. Comparer.are_equal,
        // #pragma omp parallel sections default(none) shared(test, infinity)
        {
            // #pragma omp section
            {
                std::vector<segment_result_t> actual_segments;

                const auto actual_cost = Standard::Algorithms::Geometry::segmented_regression_old<small_floating_t,
                    floating_t, point_t>::solve(infinity, test.points(), test.segment_cost(), actual_segments);

                ::Standard::Algorithms::ert::are_equal(test.expected_cost(), actual_cost, "Old total cost");

                ::Standard::Algorithms::ert::are_equal(test.expected_segments(), actual_segments, "Old segments");
            }
            // #pragma omp section
            {
                std::vector<segment_result_t> actual_segments;

                const auto actual_cost =
                    Standard::Algorithms::Geometry::segmented_regression<small_floating_t, floating_t, point_t>::solve(
                        infinity, Standard::Algorithms::Numbers::absolute_value<floating_t>, test.points(),
                        test.segment_cost(), actual_segments);

                ::Standard::Algorithms::ert::are_equal_with_epsilon<floating_t, floating_t>(
                    test.expected_cost(), actual_cost, are_equal_numbers, "Approximate total cost");

                ::Standard::Algorithms::ert::are_equal(
                    test.expected_segments(), actual_segments, "Approximate segments", are_equal_results);
            }
        } // sections
    }
} // namespace

void Standard::Algorithms::Geometry::Tests::segmented_regression_tests()
{
    Standard::Algorithms::Tests::test_utilities<test_case>::run_tests(run_test_case, generate_test_cases);
}
