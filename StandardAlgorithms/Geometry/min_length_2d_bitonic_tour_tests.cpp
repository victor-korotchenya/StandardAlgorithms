#include"min_length_2d_bitonic_tour_tests.h"
#include"../Utilities/random.h"
#include"../Utilities/remove_duplicates.h"
#include"../Utilities/test_utilities.h"
#include"min_length_2d_bitonic_tour.h"
#include<cmath>

namespace
{
    using point_t = std::pair<std::int32_t, std::int32_t>;
    using floating_t = Standard::Algorithms::floating_point_type;
    constexpr auto not_computed = -1;

    struct test_case final : Standard::Algorithms::Tests::base_test_case
    {
        test_case(std::string &&name, std::vector<point_t> &&points, floating_t expected_distance = not_computed,
            std::vector<std::int32_t> &&expected_indexes = {}) noexcept
            : base_test_case(std::move(name))
            , Points(std::move(points))
            , Expected_distance(expected_distance)
            , Expected_indexes(std::move(expected_indexes))
        {
            Standard::Algorithms::sort_remove_duplicates(this->Points);
        }

        [[nodiscard]] auto points() const &noexcept -> const std::vector<point_t> &
        {
            return Points;
        }

        [[nodiscard]] auto expected_distance() const &noexcept -> const floating_t &
        {
            return Expected_distance;
        }

        [[nodiscard]] auto expected_indexes() const &noexcept -> const std::vector<std::int32_t> &
        {
            return Expected_indexes;
        }

        void validate() const override
        {
            base_test_case::validate();

            const auto size = static_cast<std::int32_t>(Points.size());
            Standard::Algorithms::require_positive(size, "points.size");

            if (Expected_distance < 0)
            {
                return;
            }

            ::Standard::Algorithms::ert::greater(Expected_indexes.size(), 0U, "expected_indexes.size");
            ::Standard::Algorithms::ert::are_equal(0, Expected_indexes[0], "expected_indexes[0]");
            ::Standard::Algorithms::ert::are_equal(size - 1, Expected_indexes.back(), "expected_indexes.back");

            ::Standard::Algorithms::ert::are_equal(
                true, std::is_sorted(Expected_indexes.cbegin(), Expected_indexes.cend()), "expected_indexes is_sorted");
        }

        void print(std::ostream &str) const override
        {
            ::Standard::Algorithms::print("Points", Points, str);
            ::Standard::Algorithms::print_value("Expected distance", str, Expected_distance);
            ::Standard::Algorithms::print("Expected indexes", Expected_indexes, str);
        }

private:
        std::vector<point_t> Points;
        floating_t Expected_distance;
        std::vector<std::int32_t> Expected_indexes;
    };

    void generate_test_cases(std::vector<test_case> &test_cases)
    {
        test_cases.emplace_back("base4",
            std::vector<point_t>({
                { 0, 0 },
                { 0, 1 },
                { 1, 0 },
                { 1, 1 },
            }),
            4, std::vector<std::int32_t>({ 0, 1, 2, 3 }));

        test_cases.emplace_back("base3",
            // NOLINTNEXTLINE
            std::vector<point_t>({ { 1, -2 }, { 1, 1 }, { 3, 2 } }), std::sqrt(5) * 3 + 3,
            std::vector<std::int32_t>({ 0, 1, 2 }));

        test_cases.emplace_back("base2",
            // NOLINTNEXTLINE
            std::vector<point_t>({ { 1, -2 }, { 5, 1 } }), 10, std::vector<std::int32_t>({ 0, 1 }));

        test_cases.emplace_back("base1",
            // NOLINTNEXTLINE
            std::vector<point_t>({ { 10, 20 } }), 0, std::vector<std::int32_t>({ 0 }));

        constexpr auto value_max = 10;

        Standard::Algorithms::Utilities::random_t<std::int32_t> rnd(-value_max, value_max);

        constexpr auto max_attempts = 1;

        for (std::int32_t att{}; att < max_attempts; ++att)
        {
            constexpr auto min_size = 1;
            constexpr auto max_size = 11;

            const auto size = rnd(min_size, max_size);
            std::vector<point_t> points(size);

            for (std::int32_t index{}; index < size; ++index)
            {
                points[index] = { rnd(), rnd() };
            }

            test_cases.emplace_back("Random" + std::to_string(att), std::move(points));
        }
    }

    void run_test_case(const test_case &test)
    {
        const auto dist_func = [] [[nodiscard]] (const point_t &one, const point_t &two)
        {
            const auto dix = one.first - two.first;
            const auto diy = one.second - two.second;
            const auto dis = static_cast<std::int64_t>(dix) * dix + static_cast<std::int64_t>(diy) * diy;

            auto root = std::sqrt(dis);
            return root;
        };

        const auto actual =
            Standard::Algorithms::Geometry::min_length_2d_bitonic_tour<floating_t, point_t>(test.points(), dist_func);

        if (0 <= test.expected_distance())
        {
            ::Standard::Algorithms::ert::are_equal_with_epsilon(
                test.expected_distance(), actual.first, "min_length_2d_bitonic_tour");
        }

        // todo(p3): do indexes.

        const auto slow2 = Standard::Algorithms::Geometry::min_length_2d_bitonic_tour_slow2<floating_t, point_t>(
            test.points(), dist_func);
        ::Standard::Algorithms::ert::are_equal_with_epsilon(
            actual.first, slow2.first, "min_length_2d_bitonic_tour_slow2");

        const auto slow3 = Standard::Algorithms::Geometry::min_length_2d_bitonic_tour_slow3<floating_t, point_t>(
            test.points(), dist_func);
        ::Standard::Algorithms::ert::are_equal_with_epsilon(
            actual.first, slow3.first, "min_length_2d_bitonic_tour_slow3");

        const auto slow = Standard::Algorithms::Geometry::min_length_2d_bitonic_tour_slow<floating_t, point_t>(
            test.points(), dist_func);
        ::Standard::Algorithms::ert::are_equal_with_epsilon(actual.first, slow.first, "min_length_2d_bitonic_tour_slow");
    }
} // namespace

void Standard::Algorithms::Geometry::Tests::min_length_2d_bitonic_tour_tests()
{
    Standard::Algorithms::Tests::test_utilities<test_case>::run_tests(run_test_case, generate_test_cases);
}
