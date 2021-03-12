#pragma once
// "is_class_helper.h"
#include<algorithm>
#include<concepts>
#include<type_traits>
#include<utility>

namespace Standard::Algorithms
{
    template<class t>
    inline constexpr bool is_move_constructor_noexcept = std::is_nothrow_move_constructible_v<t>;

    template<class t>
    inline constexpr bool is_assignment_operator_noexcept = noexcept(std::declval<t &>() = std::declval<const t &>());

    template<class t>
    inline constexpr bool is_move_assignment_operator_noexcept =
        noexcept(std::declval<t &>() = std::move(std::declval<t &&>()));
} // namespace Standard::Algorithms
