#pragma once
#include"../Utilities/require_utilities.h"
#include"gaussian_elimination.h"
#include"permutation.h"

namespace Standard::Algorithms::Numbers::Inner
{
    constexpr void hide_column_slow(const auto &source, const std::size_t excluded_column, auto &sub_result)
    {
        const auto size = source.size();
        assert(excluded_column < size && 1U < size);
        assert(size - 1U == sub_result.size() && size - 1U == sub_result.at(0).size());

        for (std::size_t col{}, col_2{}; col < size; ++col)
        {
            if (col == excluded_column)
            {
                continue;
            }

            assert(col_2 + 1U < size);

            for (std::size_t row = 1U; row < size; ++row)
            {
                sub_result[row - 1ZU][col_2] = source[row][col];
            }

            ++col_2;
        }
    }

    template<std::floating_point floating_t>
    [[nodiscard]] constexpr auto matrix_determinant_slow_rec(const auto &source) -> floating_t
    {
        const auto size = source.size();
        assert(0 < size);

        if (size <= 1U)
        {
            return source.at(0).at(0);
        }

        floating_t determinant{};
        std::vector<std::vector<floating_t>> sub_mat(size - 1U, std::vector<floating_t>(size - 1U, floating_t{}));
        std::int32_t sig = 1;

        for (std::size_t column{}; column < size; ++column, sig *= -1)
        {
            assert(size <= source.at(column).size());

            hide_column_slow(source, column, sub_mat);

            assert(size - 1U == sub_mat.size() && size - 1U <= sub_mat.at(0).size());

            auto temp = matrix_determinant_slow_rec<floating_t>(sub_mat);
            const auto &coef = source[0].at(column);
            determinant += static_cast<floating_t>(temp * coef * sig);
        }

        return determinant;
    }
} // namespace Standard::Algorithms::Numbers::Inner

namespace Standard::Algorithms::Numbers
{
    // Given a LUP decomposition, compute the determinant of the source matrix, which is not singular.
    // No LUP decomposition means the source matrix is singular, and its determinant is zero.
    // Time O(n).
    template<std::floating_point floating_t>
    [[nodiscard]] constexpr auto matrix_determinant_lup_quick(const auto &permutation, const auto &lower_upper_compot)
        -> floating_t
    {
        const auto size = Standard::Algorithms::require_positive(permutation.size(), "permutation size.");

        // todo(p4): validate the input thoroughly.
        assert(0U < size && size == lower_upper_compot.size() && size <= lower_upper_compot[0].size());

        floating_t determinant{ 1 };
        {
            const auto sig = permutation_sign(permutation);
            determinant *= sig;

            assert(floating_t{} != determinant);
        }

        for (std::size_t row{}; row < size; ++row)
        {
            const auto &linie = lower_upper_compot.at(row);
            assert(row < linie.size());

            const auto &coef = linie.at(row);
            determinant *= static_cast<floating_t>(coef);
            // The determinant might become close to 0.0.
        }

        return determinant;
    }

    template<std::floating_point floating_t, class matrix_t>
    [[nodiscard]] constexpr auto matrix_determinant_via_lup(const matrix_t &source_matrix) -> floating_t
    {
        const auto size = source_matrix.size();
        if (0U == size)
        {
            return {};
        }

        {
            const auto &linie = source_matrix.at(0);
            require_less_equal(size, linie.size(), "source matrix columns");
        }

        if (1U == size)
        {
            return source_matrix.at(0).at(0);
        }

        auto lower_upper_compot = source_matrix;
        std::vector<std::int32_t> permutation(size);
        {
            const auto res = bottom_left_top_right_permut_decomposition<floating_t, matrix_t>(
                static_cast<std::int32_t>(size), lower_upper_compot, permutation);

            if (gaussian_elimination_result::one_solution != res)
            {
                return {};
            }
        }

        auto determ = matrix_determinant_lup_quick<floating_t>(permutation, lower_upper_compot);

        return determ;
    }

    // Slow time O(n!).
    template<std::floating_point floating_t>
    [[nodiscard]] constexpr auto matrix_determinant_slow(const auto &source_matrix) -> floating_t
    {
        if (source_matrix.empty())
        {
            return {};
        }

        {
            constexpr auto max_size = 10U;
            constexpr const auto *const name = "source matrix size";

            const auto size = source_matrix.size();
            require_less_equal(size, max_size, name);
        }

        auto determinant = Inner::matrix_determinant_slow_rec<floating_t>(source_matrix);
        return determinant;
    }
} // namespace Standard::Algorithms::Numbers
