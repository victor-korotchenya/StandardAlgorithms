#pragma once
#include<concepts>
#include<type_traits>

// todo(p3): #include<stdfloat>

namespace Standard::Algorithms
{
#ifndef __clang__
    // A quad precision floating-point type might be more accurate, bur probably is slower than float or double.
    using floating_point_type = long double;

    static_assert(sizeof(floating_point_type) == // NOLINTNEXTLINE
        16);

    static_assert(sizeof(floating_point_type) == sizeof(__float128));
#else
    // Clang prints __float128 as double.
    using floating_point_type = double;
#endif

    static_assert(std::is_floating_point_v<floating_point_type>);
    static_assert(sizeof(double) <= sizeof(floating_point_type));
} // namespace Standard::Algorithms
