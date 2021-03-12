#include"mobius_binary_sequence_period_tests.h"
#include"../Utilities/test_utilities.h"
#include"mobius_binary_sequence_period.h"
#include<array>

namespace
{
    using int_t = std::int64_t;
} // namespace

void Standard::Algorithms::Numbers::Tests::mobius_binary_sequence_period_tests()
{
    std::vector<int_t> expecteds{// NOLINTNEXTLINE
        2, 1, 2, 3, 6, 9, 18
    };

    if constexpr (!::Standard::Algorithms::is_debug)
    {
        expecteds.insert(expecteds.end(),
            // NOLINTNEXTLINE
            { 30, 56, 99 });
    }

    int_t numb{};

    for (const auto &expected : expecteds)
    {
        ++numb;

        const auto name = std::to_string(numb);

        {
            const auto actual = bin_sequence_period(numb);

            ::Standard::Algorithms::ert::are_equal(expected, actual, "bin_sequence_period " + name);
        }
        {
            const auto actual = bin_sequence_period_slow(numb);

            ::Standard::Algorithms::ert::are_equal(expected, actual, "bin_sequence_period_slow " + name);
        }
    }
}
