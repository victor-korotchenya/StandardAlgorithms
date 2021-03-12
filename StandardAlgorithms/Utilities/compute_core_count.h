#pragma once
#include<cstddef>

namespace Standard::Algorithms
{
    constexpr auto all_cores = 0U;

    constexpr auto single_core = 1U;
} // namespace Standard::Algorithms

namespace Standard::Algorithms::Utilities
{
    [[nodiscard]] auto compute_core_count(std::size_t input_size = 0, std::size_t cpu_count = all_cores) noexcept
        -> std::size_t;
} // namespace Standard::Algorithms::Utilities
