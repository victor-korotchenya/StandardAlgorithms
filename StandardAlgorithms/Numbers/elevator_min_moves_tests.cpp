#include"elevator_min_moves_tests.h"
#include"../Utilities/test_utilities.h"
#include"elevator_min_moves.h"

void Standard::Algorithms::Numbers::Tests::elevator_min_moves_tests()
{
    const std::vector<std::int32_t> weights{// NOLINTNEXTLINE
        8, 6, 1, 1, 4, // NOLINTNEXTLINE
        3, 2, 2, 9, 4
    };

    constexpr auto max_weight = 10;

    const auto actual = elevator_min_moves(weights, max_weight);

    // 8 2, 6 4, 1 3 2 4, 9 1
    constexpr auto expect = 4;

    ::Standard::Algorithms::ert::are_equal(expect, actual, "elevator_min_moves");
}
