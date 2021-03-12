#include"drop_floor_tests.h"
#include"../Utilities/test_utilities.h"
#include"drop_floor.h"

namespace
{
    using int_t = std::uint64_t;

    struct test_case final : Standard::Algorithms::Tests::base_test_case
    {
        constexpr test_case(const std::pair<int_t, int_t> &balls_floors, const int_t &expected) noexcept(false)
            : base_test_case(::Standard::Algorithms::Utilities::zu_string(balls_floors.first) + "_" +
                  ::Standard::Algorithms::Utilities::zu_string(balls_floors.second))
            , Balls(balls_floors.first)
            , Floors(balls_floors.second)
            , Expected(expected)
        {
        }

        [[nodiscard]] constexpr auto balls() const &noexcept -> const int_t &
        {
            return Balls;
        }

        [[nodiscard]] constexpr auto floors() const &noexcept -> const int_t &
        {
            return Floors;
        }

        [[nodiscard]] constexpr auto expected() const &noexcept -> const int_t &
        {
            return Expected;
        }

        void print(std::ostream &str) const override
        {
            str << ", Balls " << Balls << ", Floors " << Floors << ", Expected " << Expected;
        }

private:
        int_t Balls;
        int_t Floors;
        int_t Expected;
    };

    constexpr void generate_test_cases(std::vector<test_case> &test_cases)
    {
        test_cases.insert(test_cases.end(), // NOLINTNEXTLINE
            { { { 1, 1 }, 1 }, { { 1, 2 }, 2 }, { { 2, 1 }, 1 }, // NOLINTNEXTLINE
                { { 2, 2 }, 2 }, { { 2, 3 }, 2 }, { { 2, 4 }, 3 }, // NOLINTNEXTLINE
                { { 2, 5 }, 3 }, { { 2, 6 }, 3 }, // NOLINTNEXTLINE
                { { 2, 7 }, 4 }, { { 2, 8 }, 4 }, { { 2, 10 }, 4 }, // NOLINTNEXTLINE
                { { 2, 11 }, 5 }, { { 2, 12 }, 5 }, { { 2, 15 }, 5 }, // NOLINTNEXTLINE
                { { 2, 16 }, 6 }, { { 2, 17 }, 6 }, // NOLINTNEXTLINE
                { { 2, 35 }, 8 }, { { 2, 36 }, 8 }, // NOLINTNEXTLINE
                { { 2, 37 }, 9 }, { { 2, 38 }, 9 }, { { 2, 45 }, 9 }, // NOLINTNEXTLINE
                { { 2, 46 }, 10 }, // NOLINTNEXTLINE
                { { 2, 90 }, 13 }, { { 2, 91 }, 13 }, // NOLINTNEXTLINE
                { { 2, 92 }, 14 }, { { 2, 100 }, 14 } });

        if constexpr (!::Standard::Algorithms::is_debug)
        {
            test_cases.insert(test_cases.end(), // NOLINTNEXTLINE
                { { { 2, 703 }, 37 }, { { 2, 704 }, 38 }, // NOLINTNEXTLINE
                    { { 2, 10'011 }, 141 }, { { 2, 10'012 }, 142 }, // NOLINTNEXTLINE
                    { { 3, 2 }, 2 }, { { 3, 4 }, 3 }, { { 3, 7 }, 3 }, // NOLINTNEXTLINE
                    { { 3, 8 }, 4 }, { { 3, 9 }, 4 }, { { 3, 14 }, 4 }, // NOLINTNEXTLINE
                    { { 3, 15 }, 5 }, { { 3, 16 }, 5 }, { { 3, 25 }, 5 }, // NOLINTNEXTLINE
                    { { 3, 26 }, 6 }, { { 3, 27 }, 6 }, { { 3, 41 }, 6 }, // NOLINTNEXTLINE
                    { { 3, 42 }, 7 }, { { 3, 63 }, 7 }, // NOLINTNEXTLINE
                    { { 3, 64 }, 8 }, { { 3, 92 }, 8 }, // NOLINTNEXTLINE
                    { { 3, 93 }, 9 }, { { 3, 100 }, 9 }, // NOLINTNEXTLINE
                    { { 3, 1'000 }, 19 }, // NOLINTNEXTLINE
                    { { 4, 2 }, 2 }, { { 4, 3 }, 2 }, { { 4, 5 }, 3 }, // NOLINTNEXTLINE
                    { { 4, 6 }, 3 }, { { 4, 7 }, 3 }, // NOLINTNEXTLINE
                    { { 4, 8 }, 4 }, { { 4, 9 }, 4 }, { { 4, 15 }, 4 }, // NOLINTNEXTLINE
                    { { 4, 16 }, 5 }, { { 4, 17 }, 5 }, // NOLINTNEXTLINE
                    { { 4, 10'000 }, 23 }, { { 5, 5'000 }, 16 }, { { 7, 10'000 }, 15 }, // NOLINTNEXTLINE
                    { { 9, 10 }, 4 }, { { 10, 5'000 }, 13 }, { { 10, 10'000 }, 14 }, // NOLINTNEXTLINE
                    { { 22, 8 }, 4 }, { { 22, 10 }, 4 }, { { 32, 3 }, 2 }, { { 50, 5'000 }, 13 }, // NOLINTNEXTLINE
                    { { 100, 4 }, 3 }, { { 100, 5'000 }, 13 }, // NOLINTNEXTLINE
                    { { 102, 7 }, 3 }, // NOLINTNEXTLINE
                    { { 1'000, 1'000 }, 10 }, { { 1'000, 5'000 }, 13 }, // NOLINTNEXTLINE
                    { { 5'000, 5'000 }, 13 } });
        }
    }

    constexpr void run_test_case(const test_case &test)
    {
        const auto fast = Standard::Algorithms::Numbers::drop_floor<int_t>({ test.balls(), test.floors() });

        ::Standard::Algorithms::ert::are_equal(test.expected(), fast, "drop_floor");

        if (test.floors() < Standard::Algorithms::Numbers::drop_floor_slow_maxim)
        {
            const auto slow = Standard::Algorithms::Numbers::drop_floor_slow<int_t>({ test.balls(), test.floors() });

            ::Standard::Algorithms::ert::are_equal(fast, slow, "drop_floor_slow");
        }
    }
} // namespace

void Standard::Algorithms::Numbers::Tests::drop_floor_tests()
{
    Standard::Algorithms::Tests::test_utilities<test_case>::run_tests<all_cores>(run_test_case, generate_test_cases);
}
