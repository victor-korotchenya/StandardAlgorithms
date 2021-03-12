#pragma once
#ifdef _OPENMP
#include<omp.h>
#endif

namespace Standard::Algorithms
{
    [[nodiscard]] inline constexpr auto has_openmp() noexcept -> bool
    {// -fopenmp
#ifdef _OPENMP
        return true;
#else
        return false;
#endif
    }

    [[nodiscard]] auto has_openmp_computer() -> bool;
} // namespace Standard::Algorithms
