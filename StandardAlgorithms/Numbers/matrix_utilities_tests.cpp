#include"matrix_utilities_tests.h"
#include"../Utilities/ert.h"
#include"../Utilities/stringer.h"
#include"default_modulus.h"
#include"matrix.h"
#include"number_mod.h"

namespace
{
    using int_t = std::uint32_t;
    using long_int_t = std::uint64_t;

    constexpr auto modulo = static_cast<int_t>(Standard::Algorithms::Numbers::default_modulus);

    using num_mod_t = Standard::Algorithms::Numbers::number_mod<int_t, modulo, long_int_t>;

    constexpr auto size = 4U;

    using line_t = std::array<num_mod_t, size>;
    using array_t = std::array<line_t, size>;

    using matrix_t = Standard::Algorithms::Numbers::matrix<num_mod_t, size, size>;

    constexpr void matrix_power_tests()
    {
        const array_t source{ {// NOLINTNEXTLINE
            { 159, 16, 4, 3 }, // NOLINTNEXTLINE
            { 1, 15, 153, 985'625 }, // NOLINTNEXTLINE
            { 235, 15'362, 657, 32'645 }, // NOLINTNEXTLINE
            { 2'467, 565, 2'347, 956'804 } } };

        // NOLINTNEXTLINE
        constexpr int_t power = 1'000'000'123;

        const array_t expecteds{ {// NOLINTNEXTLINE
            { 284253588, 226920766, 817067932, 102261027 }, // NOLINTNEXTLINE
            { 440236870, 10749691, 182101956, 931651544 }, // NOLINTNEXTLINE
            { 668022195, 713435927, 647142552, 13969634 }, // NOLINTNEXTLINE
            { 266652219, 473563980, 753381942, 450274607 } } };

        matrix_t actuals{};
        {
            matrix_t source_matr(source);

            matrix_power<num_mod_t, size, int_t>(source_matr, power, actuals);
        }

        Standard::Algorithms::Utilities::stringer stinger;

        for (int_t row{}; row < size; ++row)
        {
            stinger.initialize(std::to_string(row) + ", ");

            for (int_t col{}; col < size; col++)
            {
                stinger.reset_append(std::to_string(col));

                const auto &expected = expecteds[row][col];

                const auto &actual = actuals[row, col];
                ::Standard::Algorithms::ert::are_equal(expected, actual, stinger);

                ::Standard::Algorithms::ert::greater(modulo, actual.value(), stinger);
            }
        }
    }
} // namespace

void Standard::Algorithms::Numbers::Tests::matrix_utilities_tests()
{
    matrix_power_tests();
}
