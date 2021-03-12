#include"matrix_determinant_tests.h"
#include"../Utilities/iota_vector.h"
#include"../Utilities/random_matrix.h"
#include"../Utilities/test_utilities.h"
#include"matrix_determinant.h"

namespace
{
    using int_t = std::uint16_t;
    using perm_t = std::vector<int_t>;
    using floating_t = Standard::Algorithms::floating_point_type;

    constexpr auto min_size = 1U;
    constexpr auto max_size = 6U;
    static_assert(0U < min_size && min_size < max_size);

    constexpr void simple_det_tests()
    {
        constexpr floating_t expected_det = // NOLINTNEXTLINE
            2 * 7 * 11;
        static_assert(floating_t{} != expected_det);

        const std::vector<std::vector<floating_t>> source_matrix{// NOLINTNEXTLINE
            { 2, 3, 4 }, // NOLINTNEXTLINE
            { 12, 25, 32 }, // NOLINTNEXTLINE
            { 20, 93, 123 }
        };

        {
            constexpr floating_t det_2 = // NOLINTNEXTLINE
                2 * (25 * 123 - 32 * 93) + // NOLINTNEXTLINE
                -3 * (12 * 123 - 32 * 20) + // NOLINTNEXTLINE
                4 * (12 * 93 - 20 * 25);
            static_assert(det_2 == expected_det);
        }

        const auto size = source_matrix.size();

        // { 1, 0, 0 } * { 2, 3, 4 } = { 2, 3, 4 }
        // { 6, 1, 0 } * { 0, 7, 8 } = { 12, 25, 32 }
        // { 10, 9, 1} * { 0, 0, 11} = { 20, 93, 123 }
        const std::vector<std::vector<floating_t>> lower_upper_compot{// NOLINTNEXTLINE
            { 2, 3, 4 }, // NOLINTNEXTLINE
            { 6, 7, 8 }, // NOLINTNEXTLINE
            { 10, 9, 11 }
        };

        {
            const auto actual = Standard::Algorithms::Numbers::matrix_determinant_slow<floating_t>(source_matrix);

            ::Standard::Algorithms::ert::are_equal_with_epsilon(expected_det, actual, "matrix determinant slow");
        }
        {
            const auto permutation = ::Standard::Algorithms::Utilities::iota_vector<int_t>(size);

            const auto actual =
                Standard::Algorithms::Numbers::matrix_determinant_lup_quick<floating_t>(permutation, lower_upper_compot);

            ::Standard::Algorithms::ert::are_equal_with_epsilon(expected_det, actual, "matrix determinant via quick LUP");
        }
        {
            const std::vector<int_t> permutation{ 0, 2, 1 }; // (0)(21) has sign = 1*-1 = -1

            const auto actual =
                Standard::Algorithms::Numbers::matrix_determinant_lup_quick<floating_t>(permutation, lower_upper_compot);

            ::Standard::Algorithms::ert::are_equal_with_epsilon(-expected_det, actual, "matrix determinant *-1 via LUP");
        }
        {
            const auto actual = Standard::Algorithms::Numbers::matrix_determinant_via_lup<floating_t>(source_matrix);

            ::Standard::Algorithms::ert::are_equal_with_epsilon(expected_det, actual, "matrix determinant via LUP");
        }
    }

    void random_det_test(std::vector<std::vector<floating_t>> &source_matrix)
    {
        {
            constexpr auto add_extra_column = true;

            source_matrix =
                Standard::Algorithms::Utilities ::random_matrix<floating_t>(add_extra_column, min_size, max_size);
        }

        const auto expected_det = Standard::Algorithms::Numbers::matrix_determinant_slow<floating_t>(source_matrix);

        const auto actual = Standard::Algorithms::Numbers::matrix_determinant_via_lup<floating_t>(source_matrix);

        ::Standard::Algorithms::ert::are_equal_with_epsilon(expected_det, actual, "matrix_determinant via LUP");
    }

    void random_det_tests_many()
    {
        std::vector<std::vector<floating_t>> source_matrix(max_size, std::vector<floating_t>(max_size, floating_t{}));

        try
        {
            constexpr auto max_attempts = 1;

            for (std::int32_t att{}; att < max_attempts; ++att)
            {
                random_det_test(source_matrix);
            }
        }
        catch (const std::exception &exc)
        {
            auto str = ::Standard::Algorithms::Utilities::w_stream();
            ::Standard::Algorithms::print("Random source matrix", source_matrix, str);
            str << "\nError: " << exc.what();

            throw std::runtime_error(str.str());
        }
    }
} // namespace

void Standard::Algorithms::Numbers::Tests::matrix_determinant_tests()
{
    simple_det_tests();
    random_det_tests_many();
}
