#include"has_openmp.h"
#include<cstdint>

[[nodiscard]] auto Standard::Algorithms::has_openmp_computer() -> bool
{
    if constexpr (!has_openmp())
    {
        return false;
    }

    std::int32_t value{};

#pragma omp parallel num_threads(2)
    {
#pragma omp critical
        ++value;
    }

    auto has = 1 < value;
    return has;
}
