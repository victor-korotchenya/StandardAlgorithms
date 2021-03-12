#pragma once
#include"to_unsigned.h"

namespace Standard::Algorithms::Numbers
{
    // Shift of signed numbers seams to be problematic in C++.
    template<std::integral int_t, std::integral int_t_2>
    [[nodiscard]] constexpr auto shift_left(const int_t &value, const int_t_2 &shift) noexcept -> int_t
    {
        auto res = static_cast<int_t>(to_unsigned(value) << to_unsigned(shift));
        return res;
    }

    template<std::integral int_t, std::integral int_t_2>
    [[nodiscard]] constexpr auto shift_right(const int_t &value, const int_t_2 &shift) noexcept -> int_t
    {
        auto res = static_cast<int_t>(to_unsigned(value) >> to_unsigned(shift));
        return res;
    }
} // namespace Standard::Algorithms::Numbers
