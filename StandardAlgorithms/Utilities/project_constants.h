#pragma once
// "project_constants.h"
#include<cassert>
#include<concepts>
#include<cstddef>
#include<cstdint>
#include<limits>
#include<utility>

namespace Standard::Algorithms::Utilities
{
    constexpr auto throw_on_fail_default = true;
    constexpr auto stack_max_size = 1'000;
    constexpr auto max_logn_algorithm_depth = 64 - 1;
    constexpr auto factorial_max_slow = 12;
    constexpr auto max_attempts = 100;

    constexpr auto min_char = 33; // 0x21 !
    constexpr auto max_char = 122; // 0x7A z
    static_assert(min_char <= max_char);

    template<std::integral int_t = std::int32_t>
    constexpr int_t max_size = std::numeric_limits<int_t>::max() - 5;

    template<std::integral int_t = std::int32_t>
    constexpr int_t graph_max_nodes = (sizeof(int_t) << 3U) - 1;
} // namespace Standard::Algorithms::Utilities
