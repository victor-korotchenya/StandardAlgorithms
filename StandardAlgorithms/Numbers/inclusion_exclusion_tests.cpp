#include"inclusion_exclusion_tests.h"
#include"../Utilities/test_utilities.h"
#include"inclusion_exclusion.h"

namespace
{
    constexpr void all_subsets_tests()
    {
        const std::vector<std::int16_t> data{// NOLINTNEXTLINE
            2, 7, 3
        };

        const std::vector<std::vector<std::int16_t>> expected{// NOLINTNEXTLINE
            {}, { 2 }, { 7 }, { 2, 7 }, { 3 }, { 2, 3 }, { 7, 3 }, { 2, 7, 3 }
        };

        const auto actual = Standard::Algorithms::Numbers::all_subsets(data);

        ::Standard::Algorithms::ert::are_equal(expected, actual, "all_subsets");
    }

    constexpr void coprimes_in_interval_tests()
    {
        std::vector<std::int32_t> factors;

        constexpr auto small_int = 10;

        for (auto divisor = 1; divisor <= small_int; ++divisor)
        {
            const auto name = std::to_string(divisor) + "_coprimes_in_interval_";

            for (auto right = 1; right <= small_int; ++right)
            {
                const auto expected =
                    Standard::Algorithms::Numbers::coprimes_in_interval_slow<std::int32_t>({ divisor, right });

                const auto actual =
                    Standard::Algorithms::Numbers::coprimes_in_interval<std::int64_t>(divisor, factors, right);

                ::Standard::Algorithms::ert::are_equal(expected, actual, name + std::to_string(right));
            }
        }
    }
} // namespace

void Standard::Algorithms::Numbers::Tests::inclusion_exclusion_tests()
{
    coprimes_in_interval_tests();
    all_subsets_tests();
}
