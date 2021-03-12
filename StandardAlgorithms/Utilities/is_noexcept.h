#pragma once
// "is_noexcept.h"
#include<concepts>
#include<type_traits>
#include<utility>

namespace Standard::Algorithms
{
    // noexcept(noexcept(t + t))
    template<class item_t>
    inline constexpr bool is_noexcept_add = noexcept(std::declval<const item_t &>() + std::declval<const item_t &>());
} // namespace Standard::Algorithms
