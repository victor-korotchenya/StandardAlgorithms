#include"swap_knights_min_moves_tests.h"
#include"../Utilities/elapsed_time_ns.h"
#include"../Utilities/is_debug.h"
#include"hash_utilities.h"
#include"knight_chess_tests.h"
#include"swap_knights_min_moves.h"

namespace
{
    using int_t = Standard::Algorithms::Numbers::Tests::int_t;
    using point_t = Standard::Algorithms::Numbers::Tests::point_t;
    using long_int_t = Standard::Algorithms::Numbers::Tests::long_int_t;
    using test_case = Standard::Algorithms::Numbers::Tests::test_case;

    void generate_test_cases(std::vector<test_case> &tests)
    {
        tests.emplace_back("base3", 3, point_t{}, point_t{ 0, 1 },
            // A B x // Begin
            // x x x
            // x x x
            //
            // x x x // moves 1, 2
            // x x x
            // B A x
            //
            // x x A // moves 3, 4
            // x x B
            // x x x
            //
            // B x x // moves 5, 6
            // A x x
            // x x x
            //
            // B x x // move 7
            // x x x
            // x x A
            //
            // B A x // move 8, the end.
            // x x x
            // x x x ; NOLINTNEXTLINE
            8);

        if constexpr (!::Standard::Algorithms::is_debug)
        {
            // tests.emplace_back("base4 10", 4, point_t{3, 3}, point_t{ 3, 0 }, // NOLINTNEXTLINE
            //  10);

            tests.emplace_back("base6", // NOLINTNEXTLINE
                6, point_t{ 4, 5 }, point_t{ 4, 1 }, 4);

            tests.emplace_back("base7", // NOLINTNEXTLINE
                7, point_t{}, point_t{ 3, 3 }, 4);

            // tests.emplace_back("base7A", // NOLINTNEXTLINE
            // 7, point_t{}, point_t{ 4, 6 }, 8); // long-running 100 ms.
        }

        Standard::Algorithms::Numbers::Tests::add_random_test_cases(tests);
    }

    void run_test_case(const test_case &test)
    {
        const Standard::Algorithms::elapsed_time_ns tim0;
        const auto fast = Standard::Algorithms::Numbers::swap_knights_min_moves<long_int_t, int_t>(
            test.from(), test.size(), test.tod());
        const auto elapsed0 = tim0.elapsed();

        // NOLINTNEXTLINE NOLINT
        if (elapsed0 != 0)
        {
        }

        // std::cout << test << ", time ns " << elapsed0 << ", fast " << fast << '\n';
        if (test.is_computed())
        {
            ::Standard::Algorithms::ert::are_equal(test.expected(), fast, "swap_knights_min_moves");
        }

        {
            const Standard::Algorithms::elapsed_time_ns tim1;
            const auto still = Standard::Algorithms::Numbers::swap_knights_min_moves_slow_still<long_int_t, int_t>(
                test.from(), test.size(), test.tod());
            [[maybe_unused]] const auto elapsed1 = tim1.elapsed();

            ::Standard::Algorithms::ert::are_equal(fast, still, "swap_knights_min_moves_slow_still");
        }

        if (!test.is_computed())
        {
            return;
        }

        {
            const Standard::Algorithms::elapsed_time_ns tim2;
            const auto [slow, calls] = Standard::Algorithms::Numbers::swap_knights_min_moves_slow<long_int_t, int_t,
                Standard::Algorithms::Numbers::pair_hash>(test.from(), test.size(), test.tod());

            [[maybe_unused]] const auto elapsed2 = tim2.elapsed();

            // NOLINTNEXTLINE NOLINT
            if (elapsed2 != 0 && calls != 0)
            {
            }

            ::Standard::Algorithms::ert::are_equal(fast, slow, "swap_knights_min_moves_slow");
        }
    }
} // namespace

void Standard::Algorithms::Numbers::Tests::swap_knights_min_moves_tests()
{
    Standard::Algorithms::Tests::test_utilities<test_case>::run_tests(run_test_case, generate_test_cases);
}
