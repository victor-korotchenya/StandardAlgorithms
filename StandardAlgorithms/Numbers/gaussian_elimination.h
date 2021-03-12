#pragma once
#include <vector>
#include "../Utilities/ExceptionUtilities.h"

namespace Privet::Algorithms::Numbers
{
    enum class gaussian_elimination_result
    {
        none,
        one_solution,
        singular,
        inf_many_solutions
    };

    // Gaussian elimination is used to solve a system of linear equations and
    // for calculating: matrix rank, determinant, invertible square matrice inverse.
    // Note. The matrix will be edited!
    // Time O(n**3).
    template<class float_t, class matrix_t>
    std::pair<gaussian_elimination_result, std::vector<float_t>> gaussian_elimination(matrix_t& matr, const int size)
    {
        RequirePositive(size, "size");
        assert(size <= matr.size() && size < matr[0].size());

        for (auto col = 0; col < size; ++col)
        {
            auto max_col = col; // Find max to reduce error.
            for (auto j = col + 1; j < size; ++j)
                if (fabs(matr[j][col]) > fabs(matr[max_col][col]))
                    max_col = j;

            auto& m = matr[col];
            if (float_t() == m[max_col])
            {
                const auto has = float_t() == m[size];
                return { has ? gaussian_elimination_result::inf_many_solutions
                    : gaussian_elimination_result::singular, {} };
            }

            if (max_col != col)
                for (auto j = col; j <= size; ++j)
                    std::swap(m[j], matr[max_col][j]);

            const auto& mi = static_cast<float_t>(m[col]);

            for (auto j = col + 1; j < size; ++j)
            {
                auto& mj = matr[j];
                const auto coef = mj[col] / mi;

                mj[col] = 0;

                for (auto k = col + 1; k <= size; ++k)
                    mj[k] -= m[k] * coef;
            }
        }

        std::vector<float_t> x(size);

        for (auto i = size - 1; i >= 0; --i)
        {
            const auto& m = matr[i];

            float_t t = static_cast<float_t>(m[size]);
            for (auto j = i + 1; j < size; ++j)
                t -= m[j] * x[j];

            x[i] = t / m[i];
        }

        return { gaussian_elimination_result::one_solution, x };
    }
}