#pragma once
// "clang_constexpr.h"
// #if defined(__clang__)
#include"w_stream.h"
#include<concepts>
#include<stdexcept>
#include<string>
#include<utility>
// #endif

#include<cmath>
#include<type_traits> // std::is_constant_evaluated

// todo(p3): del when ready. GCC, Clang both miss constexpr - let's make it ourselves.

namespace Standard::Algorithms
{
    template<std::floating_point floating_t>
    [[nodiscard]] inline constexpr auto fabs(floating_t value) -> floating_t
    {
        // #if defined(__clang__)
        // if consteval // 'readability-simplify-boolean-expr' crashes in Clang, oh.
        if (std::is_constant_evaluated())
        {
            if (constexpr const floating_t zero{}; value < zero)
            {
                value = -value;

                if (value < zero) [[unlikely]]
                {
                    auto str = ::Standard::Algorithms::Utilities::w_stream();
                    str << "Cannot negate a floating value: " << value;

                    throw std::runtime_error(str.str());
                }
            }

            return value;
        }

        return std::fabs(value);
        // #endif
    }
} // namespace Standard::Algorithms
