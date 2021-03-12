#include"count_partitions_tests.h"
#include"../Utilities/test_utilities.h"
#include"count_partitions.h"
#include"default_modulus.h"

namespace
{
    using int_t = std::int32_t;
    using long_t = std::int64_t;

    constexpr auto mod = Standard::Algorithms::Numbers::default_modulus;
} // namespace

void Standard::Algorithms::Numbers::Tests::count_partitions_tests()
{
    const std::vector<int_t> expected_zero_twenty{ 1, 1,
        2, // 2,11
        3, // 3,21,111
        // NOLINTNEXTLINE
        5, // 4,31,22,211,1111
        // NOLINTNEXTLINE
        7, 11, 15, 22, 30, 42, // 10
        // NOLINTNEXTLINE
        56, 77, 101, 135, 176, 231, 297, 385, 490, 627 };

    constexpr auto count_zero = 1U;

    const auto n_max = static_cast<int_t>(expected_zero_twenty.size() - count_zero);

    {
        const auto actual = Standard::Algorithms::Numbers::count_partitions<long_t, int_t, mod>(n_max);

        ::Standard::Algorithms::ert::are_equal(expected_zero_twenty, actual, "count_partitions");
    }
    {
        const auto actual = Standard::Algorithms::Numbers::count_partitions_still_slow<long_t, int_t, mod>(n_max);

        ::Standard::Algorithms::ert::are_equal(expected_zero_twenty, actual, "count_partitions_still_slow");
    }

    const auto n_max_slow = std::min(static_cast<int_t>(5), n_max);

    const auto slow = count_partitions_slow<long_t, int_t, mod>(n_max_slow);

    ::Standard::Algorithms::ert::are_equal(expected_zero_twenty.data(), n_max_slow + 1LL, slow, "count_partitions_slow");
}
