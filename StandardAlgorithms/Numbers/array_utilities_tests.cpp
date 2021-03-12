#include"array_utilities_tests.h"
#include"../Utilities/test_utilities.h"
#include"array_utilities.h"

namespace
{
    using val_t = std::int16_t;

    constexpr auto arr_size = 6U;
    using array_t = std::array<val_t, arr_size>;

    constexpr void normalize_cycle_tests()
    {
        const array_t expected{// NOLINTNEXTLINE
            3, 7, 100, 5, 9, 8
        };

        const std::vector<array_t> arrays{ expected,
            // NOLINTNEXTLINE
            { 8, 3, 7, 100, 5, 9 },
            // NOLINTNEXTLINE
            { 9, 8, 3, 7, 100, 5 },
            // NOLINTNEXTLINE
            { 5, 9, 8, 3, 7, 100 },
            // NOLINTNEXTLINE
            { 100, 5, 9, 8, 3, 7 },
            // NOLINTNEXTLINE
            { 7, 100, 5, 9, 8, 3 } };

        std::vector<val_t> temp;
        std::int32_t shift{};

        for (const auto &arr : arrays)
        {
            const auto shift_name = ", shift " + std::to_string(shift);
            {
                auto actual = arr;
                Standard::Algorithms::Numbers::rotate_array_left_slow(actual, shift, temp);
                ::Standard::Algorithms::ert::are_equal(expected, actual, "rotate_array_left_slow" + shift_name);
            }
            {
                auto actual = arr;
                Standard::Algorithms::Numbers::rotate_array_left(actual, shift);
                ::Standard::Algorithms::ert::are_equal(expected, actual, "rotate_array_left" + shift_name);
            }
            {
                auto actual = arr;
                Standard::Algorithms::Numbers::normalize_cycle_slow(actual, temp);
                ::Standard::Algorithms::ert::are_equal(expected, actual, "normalize_cycle_slow" + shift_name);
            }
            {
                auto actual = arr;
                Standard::Algorithms::Numbers::normalize_cycle(actual);
                ::Standard::Algorithms::ert::are_equal(expected, actual, "normalize_cycle" + shift_name);
            }

            ++shift;
        }
    }
} // namespace

void Standard::Algorithms::Numbers::Tests::array_utilities_tests()
{
    normalize_cycle_tests();
}
