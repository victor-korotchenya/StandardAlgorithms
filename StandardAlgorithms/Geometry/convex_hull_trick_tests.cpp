#include"convex_hull_trick_tests.h"
#include"../Utilities/random.h"
#include"../Utilities/test_utilities.h"
#include"convex_hull_trick.h"

namespace
{
    using int_t = std::int32_t;
    using long_int_t = std::int64_t;
    using pos_t = std::uint32_t;
    using pair_t = std::pair<int_t, int_t>;
    using coll_t = std::vector<pair_t>;

    struct test_case final : Standard::Algorithms::Tests::base_test_case
    {
        constexpr test_case(std::string &&name, coll_t &&rectangles, long_int_t expected = {}) noexcept
            : base_test_case(std::move(name))
            , Rectangles(std::move(rectangles))
            , Expected(expected)
        {
        }

        [[nodiscard]] constexpr auto rectangles() const &noexcept -> const coll_t &
        {
            return Rectangles;
        }

        [[nodiscard]] constexpr auto expected() const &noexcept -> const long_int_t &
        {
            return Expected;
        }

        void print(std::ostream &str) const override
        {
            ::Standard::Algorithms::append_separator(str);
            ::Standard::Algorithms::print("Rectangles", Rectangles, str);
            ::Standard::Algorithms::print_value("Expected", str, Expected);
        }

private:
        coll_t Rectangles;
        long_int_t Expected;
    };

    void generate_test_cases(std::vector<test_case> &test_cases)
    {
        test_cases.emplace_back("One group better than two ones",
            // NOLINTNEXTLINE
            coll_t({ { 3, 5 }, { 5, 3 } }), 25);

        test_cases.emplace_back("One group over many",
            // NOLINTNEXTLINE
            coll_t({ { 3, 10 }, { 5, 11 }, { 8, 9 }, { 10, 4 } }), 110);

        test_cases.emplace_back("Trivial brain composter",
            // NOLINTNEXTLINE
            coll_t({ { 3, 5 } }), 15);

        test_cases.emplace_back("Two groups",
            // NOLINTNEXTLINE
            coll_t({ { 3, 100 }, { 500, 4 } }), 300 + 2000);

        test_cases.emplace_back("Covered by bigger",
            // NOLINTNEXTLINE
            coll_t({ { 3, 5 }, { 2, 3 }, { 1, 1 }, { 2, 4 }, { 2, 5 }, { 3, 3 }, { 1, 1 } }), 15);

        test_cases.emplace_back("3 groups",
            // NOLINTNEXTLINE
            coll_t({ { 3, 100 }, // 300
                                 // NOLINTNEXTLINE
                { 5, 11 }, { 8, 9 }, // 88
                // NOLINTNEXTLINE
                { 4, 7 }, { 8, 8 }, // 0
                // NOLINTNEXTLINE
                { 20, 4 } }), // 80
            // NOLINTNEXTLINE
            468);

        test_cases.emplace_back("2 groups test 2",
            // NOLINTNEXTLINE
            coll_t({ { 100, 105 }, { 111, 8 },
                // useless
                // NOLINTNEXTLINE
                { 97, 18 } }),
            // NOLINTNEXTLINE
            11388);

        constexpr auto min_side = 1;
        constexpr auto max_side = 100;

        Standard::Algorithms::Utilities::random_t<int_t> rnd(min_side, max_side);

        constexpr auto max_attempts = 1;

        for (std::int32_t att{}; att < max_attempts; ++att)
        {
            constexpr auto siz = 8;

            coll_t rectangles(siz);

            for (auto &rect : rectangles)
            {
                rect.first = rnd();
                rect.second = rnd();
            }

            test_cases.emplace_back("Random conv hull tr" + std::to_string(att), std::move(rectangles));
        }
    }

    constexpr void run_test_case(const test_case &test)
    {
        const auto &rectangles = test.rectangles();
        const auto slow = ::Standard::Algorithms::Geometry::convex_hull_trick_slow<int_t, long_int_t, pos_t>(rectangles);

        if (const auto &expected = test.expected(); 0 < expected)
        {
            ::Standard::Algorithms::ert::are_equal(expected, slow, "convex_hull_trick_slow");
        }

        {
            const auto fast = ::Standard::Algorithms::Geometry::convex_hull_trick<int_t, long_int_t, pos_t>(rectangles);
            ::Standard::Algorithms::ert::are_equal(slow, fast, "convex_hull_trick");
        }
    }
} // namespace

void Standard::Algorithms::Geometry::Tests::convex_hull_trick_tests()
{
    Standard::Algorithms::Tests::test_utilities<test_case>::run_tests(run_test_case, generate_test_cases);
}
