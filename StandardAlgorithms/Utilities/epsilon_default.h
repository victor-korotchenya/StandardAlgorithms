#pragma once
// "epsilon_default.h"
#include"floating_point_type.h"
#include<concepts>

namespace Standard::Algorithms
{
    template<std::floating_point floating_t = floating_point_type>
    [[nodiscard]] inline constexpr auto epsilon_default() -> floating_t
    {
        constexpr auto eps = static_cast<floating_t>(1e-8);
        static_assert(floating_t{} < eps);

        return eps;
    }
} // namespace Standard::Algorithms
