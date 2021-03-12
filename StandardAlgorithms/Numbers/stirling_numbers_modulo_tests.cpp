#include"stirling_numbers_modulo_tests.h"
#include"../Utilities/test_utilities.h"
#include"default_modulus.h"
#include"stirling_numbers_modulo.h"

namespace
{
    constexpr auto size = 10;

    constexpr void test_stirling_1()
    {
        const auto actual = Standard::Algorithms::Numbers::stirling_numbers_1_permut_cycle_cnt_modulo<
            Standard::Algorithms::Numbers::default_modulus>(size);

        const std::vector<std::vector<std::int32_t>> expected = { { 1, 0 }, // 0
            { 0, 1, 0 }, // 1
            { 0, 1, 1, 0 }, // 2
            { 0, 2, 3, 1, 0 }, // NOLINTNEXTLINE
            { 0, 6, 11, 6, 1, 0 }, // NOLINTNEXTLINE
            { 0, 24, 50, 35, 10, 1, 0 }, // NOLINTNEXTLINE
            { 0, 120, 274, 225, 85, 15, 1, 0 }, // NOLINTNEXTLINE
            { 0, 720, 1764, 1624, 735, 175, 21, 1, 0 }, // NOLINTNEXTLINE
            { 0, 5040, 13068, 13132, 6769, 1960, 322, 28, 1, 0 }, // NOLINTNEXTLINE
            { 0, 40320, 109584, 118124, 67284, 22449, 4536, 546, 36, 1, 0 }, // NOLINTNEXTLINE
            { 0, 362880, 1026576, 1172700, 723680, 269325, 63273, 9450, 870, 45, 1, 0 } };

        ::Standard::Algorithms::ert::are_equal(expected, actual, "stirling_numbers_1_permut_cycle_cnt_modulo");
    }

    constexpr void test_stirling_2()
    {
        constexpr auto mod = 999'979;

        const auto actual = Standard::Algorithms::Numbers::stirling_numbers_2_partition_modulo<mod>(size);

        const std::vector<std::vector<std::int32_t>> expected{
            { 1, 0 }, // 0
            { 0, 1, 0 }, // 1
            { 0, 1, 1, 0 }, // 2
            { 0, 1, 3, 1, 0 }, // NOLINTNEXTLINE
            { 0, 1, 7, 6, 1, 0 }, // NOLINTNEXTLINE
            { 0, 1, 15, 25, 10, 1, 0 }, // NOLINTNEXTLINE
            { 0, 1, 31, 90, 65, 15, 1, 0 }, // NOLINTNEXTLINE
            { 0, 1, 63, 301, 350, 140, 21, 1, 0 }, // NOLINTNEXTLINE
            { 0, 1, 127, 966, 1701, 1050, 266, 28, 1, 0 }, // NOLINTNEXTLINE
            { 0, 1, 255, 3025, 7770, 6951, 2646, 462, 36, 1, 0 }, // NOLINTNEXTLINE
            { 0, 1, 511, 9330, 34105, 42525, 22827, 5880, 750, 45, 1, 0 } // NOLINTNEXTLINE
        };

        ::Standard::Algorithms::ert::are_equal(expected, actual, "stirling_numbers_2_partition_modulo");
    }
} // namespace

void Standard::Algorithms::Numbers::Tests::stirling_numbers_modulo_tests()
{
    test_stirling_1();
    test_stirling_2();
}
