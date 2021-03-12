#include"rectangle_common_area_tests.h"
#include"../Graphs/segment_tree_add_bool.h"
#include"../Utilities/random.h"
#include"../Utilities/test_utilities.h"
#include"rectangle_common_area.h"

namespace
{
    using int_t = std::int32_t;
    using long_int_t = std::int64_t;
    using tree_t = Standard::Algorithms::Trees::segment_tree_add_bool<std::uint32_t, std::uint64_t>;

    using point_t = Standard::Algorithms::Geometry::point2d<int_t>;
    using rectangle_t = std::pair<point_t, point_t>;

    struct test_case final : Standard::Algorithms::Tests::base_test_case
    {
        constexpr test_case(
            std::string &&name, std::vector<rectangle_t> &&rectangles, long_int_t expected = {}) noexcept
            : base_test_case(std::move(name))
            , Rectangles(std::move(rectangles))
            , Expected(expected)
        {
        }

        [[nodiscard]] constexpr auto rectangles() const &noexcept -> const std::vector<rectangle_t> &
        {
            return Rectangles;
        }

        [[nodiscard]] constexpr auto expected() const &noexcept -> const long_int_t &
        {
            return Expected;
        }

        void print(std::ostream &str) const override
        {
            ::Standard::Algorithms::print("Rectangles", Rectangles, str);

            if (long_int_t{} < Expected)
            {
                ::Standard::Algorithms::print_value("expected", str, Expected);
            }
            else
            {
                str << "no expected";
            }
        }

        constexpr void validate() const override
        {
            base_test_case::validate();

            const auto size = static_cast<std::int32_t>(Rectangles.size());
            Standard::Algorithms::require_positive(size, "rectangles.size");

            for (std::int32_t index{}; index < size; ++index)
            {
                const auto iss = "rectangle" + std::to_string(index);
                const auto &rectangle = Rectangles[index];

                ::Standard::Algorithms::ert::greater(rectangle.second.x, rectangle.first.x, iss + " x");
                ::Standard::Algorithms::ert::greater(rectangle.second.y, rectangle.first.y, iss + " y");
                ::Standard::Algorithms::ert::greater_or_equal(rectangle.first.x, 0, iss + " x");
                ::Standard::Algorithms::ert::greater_or_equal(rectangle.first.y, 0, iss + " y");
            }

            Standard::Algorithms::require_non_negative(Expected, "Expected");
        }

private:
        std::vector<rectangle_t> Rectangles;
        long_int_t Expected;
    };

    void generate_random(std::vector<test_case> &test_cases)
    {
        constexpr auto coord_min = 0;
        constexpr auto coord_max = 17;

        Standard::Algorithms::Utilities::random_t<std::int32_t> rnd(coord_min, coord_max);

        constexpr auto max_attempts = 1;

        for (std::int32_t att{}; att < max_attempts; ++att)
        {
            constexpr auto min_size = 1;
            constexpr auto max_size = 15;

            const auto size = rnd.operator() (min_size, max_size);
            std::vector<rectangle_t> rectangles(size);

            for (std::int32_t index{}; index < size; ++index)
            {
                auto &rectangle = rectangles[index];
                {
                    rectangle.first.x = rnd.operator() ();
                    rectangle.first.y = rnd.operator() ();
                }
                {
                    constexpr auto delta_min = 1;
                    constexpr auto delta_max = 6;

                    rectangle.second.x = rectangle.first.x + rnd.operator() (delta_min, delta_max);
                    rectangle.second.y = rectangle.first.y + rnd.operator() (delta_min, delta_max);
                }
            }

            test_cases.emplace_back("Random" + std::to_string(att), std::move(rectangles));
        }
    }

    void generate_test_cases(std::vector<test_case> &test_cases)
    {
        generate_random(test_cases);

        test_cases.emplace_back("bug1",
            std::vector<rectangle_t>({// NOLINTNEXTLINE
                { { 1, 4 }, { 2, 6 } }, // useless

                // NOLINTNEXTLINE
                { { 0, 1 }, { 5, 6 } },
                // NOLINTNEXTLINE
                { { 5, 1 }, { 9, 4 } } }),

            // NOLINTNEXTLINE
            5 * 5 + 3 * 4);

        test_cases.emplace_back("bug2",
            std::vector<rectangle_t>({// NOLINTNEXTLINE
                { { 4, 1 }, { 9, 4 } },
                // NOLINTNEXTLINE
                { { 0, 4 }, { 4, 8 } },
                // NOLINTNEXTLINE
                { { 5, 2 }, { 9, 7 } },
                // NOLINTNEXTLINE
                { { 5, 1 }, { 7, 3 } } }), // useless
            0);

        test_cases.emplace_back("touch+useless",
            std::vector<rectangle_t>({// NOLINTNEXTLINE
                { { 0, 0 }, { 2, 2 } },
                // NOLINTNEXTLINE
                { { 2, 2 }, { 3, 3 } },
                // NOLINTNEXTLINE
                { { 1, 1 }, { 2, 2 } },
                // NOLINTNEXTLINE
                { { 1, 1 }, { 3, 3 } },
                // NOLINTNEXTLINE
                { { 1, 1 }, { 3, 3 } }, // a copy

                // NOLINTNEXTLINE
                { { 5, 5 }, { 6, 6 } } }),
            // NOLINTNEXTLINE
            8);

        test_cases.emplace_back("2 y, many x",
            std::vector<rectangle_t>({// NOLINTNEXTLINE
                { { 3, 0 }, { 4, 1 } },
                // NOLINTNEXTLINE
                { { 5, 0 }, { 6, 1 } },
                // NOLINTNEXTLINE
                { { 7, 0 }, { 8, 1 } },
                // NOLINTNEXTLINE
                { { 9, 0 }, { 10, 1 } },
                // NOLINTNEXTLINE
                { { 11, 0 }, { 12, 1 } },
                // NOLINTNEXTLINE
                { { 13, 0 }, { 14, 1 } },
                // NOLINTNEXTLINE
                { { 15, 0 }, { 16, 1 } } }),
            // NOLINTNEXTLINE
            7);

        test_cases.emplace_back("2 x, many y",
            std::vector<rectangle_t>({// NOLINTNEXTLINE
                { { 0, 0 }, { 1, 1 } },
                // NOLINTNEXTLINE
                { { 0, 2 }, { 1, 3 } },
                // NOLINTNEXTLINE
                { { 0, 5 }, { 1, 6 } },
                // NOLINTNEXTLINE
                { { 0, 8 }, { 1, 10 } }, // 2

                // NOLINTNEXTLINE
                { { 0, 11 }, { 1, 12 } },
                // NOLINTNEXTLINE
                { { 0, 12 }, { 1, 13 } },
                // NOLINTNEXTLINE
                { { 0, 15 }, { 1, 16 } },
                // NOLINTNEXTLINE
                { { 0, 15 }, { 1, 17 } },
                // NOLINTNEXTLINE
                { { 0, 17 }, { 1, 18 } } }),
            // NOLINTNEXTLINE
            10);

        test_cases.emplace_back("two",
            std::vector<rectangle_t>({// NOLINTNEXTLINE
                { { 5, 6 }, { 6, 7 } },
                // NOLINTNEXTLINE
                { { 1, 3 }, { 2, 4 } } }),
            2);

        test_cases.emplace_back("one",
            std::vector<rectangle_t>({// NOLINTNEXTLINE
                { { 0, 4 }, { 2, 6 } } }),
            // NOLINTNEXTLINE
            4);

        test_cases.emplace_back("trivial2",
            std::vector<rectangle_t>({// NOLINTNEXTLINE
                { { 2, 7 }, { 3, 8 } } }),
            1);

        test_cases.emplace_back("trivial1",
            std::vector<rectangle_t>({// NOLINTNEXTLINE
                { { 0, 1 }, { 1, 2 } } }),
            1);

        test_cases.emplace_back("trivial",
            std::vector<rectangle_t>({// NOLINTNEXTLINE
                { { 0, 0 }, { 1, 1 } } }),
            1);
    }

    constexpr void run_test_case(const test_case &test)
    {
        const auto fast =
            Standard::Algorithms::Geometry::rectangle_common_area<tree_t, int_t, long_int_t, point_t, rectangle_t>(
                test.rectangles());

        if (long_int_t{} < test.expected())
        {
            ::Standard::Algorithms::ert::are_equal(test.expected(), fast, "rectangle_common_area");
        }

        const auto slow = Standard::Algorithms::Geometry::rectangle_common_area_slow<long_int_t, point_t, rectangle_t>(
            test.rectangles());

        ::Standard::Algorithms::ert::are_equal(fast, slow, "rectangle_common_area_slow");
    }
} // namespace

void Standard::Algorithms::Geometry::Tests::rectangle_common_area_tests()
{
    Standard::Algorithms::Tests::test_utilities<test_case>::run_tests(run_test_case, generate_test_cases);
}
