#pragma once
// "is_integral_pure.h"
#include<concepts>
#include<type_traits>
#include<utility>

namespace Standard::Algorithms
{
    // std::is_integral<std::int32_t&>::value is false.
    template<class item_t>
    inline constexpr bool is_integral_pure = std::is_integral_v<std::remove_cvref_t<item_t>>;
} // namespace Standard::Algorithms
