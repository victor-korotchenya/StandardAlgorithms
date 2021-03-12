#pragma once
// #include"max_double_precision.h"
#include<cstdint>
#include<limits>

// todo(p3): #include<stdfloat>

namespace Standard::Algorithms
{
    inline static constexpr std::int32_t max_double_precision =
        // static_cast<std::int32_t>(
        // 1 + std::numeric_limits<floating_point_type>::max_digits10);
        // todo(p4): Why is it incorrect? Set manually. // NOLINTNEXTLINE
        40;

    static_assert(0 < max_double_precision);
    static_assert(std::numeric_limits<long double>::max_digits10 < max_double_precision);
    static_assert(std::numeric_limits<__float128>::max_digits10 < max_double_precision);
} // namespace Standard::Algorithms
