#include"knight_min_moves_tests.h"
#include"knight_chess_tests.h"
#include"knight_min_moves.h"

namespace
{
    using int_t = Standard::Algorithms::Numbers::Tests::int_t;
    using point_t = Standard::Algorithms::Numbers::Tests::point_t;
    using long_int_t = Standard::Algorithms::Numbers::Tests::long_int_t;
    using test_case = Standard::Algorithms::Numbers::Tests::test_case;

    void generate_test_cases(std::vector<test_case> &tests)
    {
        constexpr point_t erde{};
        // 0 3 2
        // 3 x 1
        // 2 1 4
        tests.emplace_back("base31", 3, erde, point_t{ 1, 2 }, 1);
        tests.emplace_back("base31A", 3, point_t{ 2, 1 }, erde, 1);
        tests.emplace_back("base32", 3, erde, point_t{ 0, 2 }, 2);
        tests.emplace_back("base33", 3, point_t{ 0, 1 }, erde, 3);
        tests.emplace_back("base34", 3, point_t{ 2, 2 }, erde, 4);
        // him
        // 3 0 3
        // 2 x 2
        // 1 4 1
        tests.emplace_back("base32A", 3, point_t{ 0, 1 }, point_t{ 1, 0 }, 2);
        tests.emplace_back("base34A", 3, point_t{ 2, 1 }, point_t{ 0, 1 }, 4);

        constexpr int_t five = 5;
        // 0 3 2 5
        // 3 4 1 2
        // 2 1 4 3
        // 5 2 3 2
        tests.emplace_back("base41", 4, erde, point_t{ 2, 1 }, 1);
        tests.emplace_back("base41A", 4, point_t{ 1, 2 }, erde, 1);
        tests.emplace_back("base42", 4, point_t{ 1, 3 }, erde, 2);
        tests.emplace_back("base43", 4, erde, point_t{ 2, 3 }, 3);
        tests.emplace_back("base44", 4, erde, point_t{ 2, 2 }, 4);
        tests.emplace_back("base44A", 4, point_t{ 1, 1 }, erde, 4);
        tests.emplace_back("base45", 4, point_t{ 0, 3 }, erde, five);
        tests.emplace_back("base45A", 4, erde, point_t{ 3, 0 }, five);
        // him
        // 3 0 3 2
        // 2 3 2 1
        // 1 2 1 4
        // 2 3 2 3
        tests.emplace_back("base41B", 4, point_t{ 0, 1 }, point_t{ 2, 0 }, 1);
        tests.emplace_back("base42A", 4, point_t{ 0, 1 }, point_t{ 1, 2 }, 2);
        tests.emplace_back("base43A", 4, point_t{ 0, 2 }, point_t{ 0, 1 }, 3);
        tests.emplace_back("base44B", 4, point_t{ 0, 1 }, point_t{ 2, 3 }, 4);

        Standard::Algorithms::Numbers::Tests::add_random_test_cases(tests);
    }

    constexpr void run_test_case(const test_case &test)
    {
        const auto fast =
            Standard::Algorithms::Numbers::knight_min_moves_slow<long_int_t, int_t>(test.from(), test.size(), test.tod());

        if (test.is_computed())
        {
            ::Standard::Algorithms::ert::are_equal(test.expected(), fast, "knight_min_moves_slow");
        }
    }
} // namespace

void Standard::Algorithms::Numbers::Tests::knight_min_moves_tests()
{
    Standard::Algorithms::Tests::test_utilities<test_case>::run_tests(run_test_case, generate_test_cases);
}
