#pragma once
// "digita.h"
#include<algorithm>
#include<concepts>
#include<cstddef>
#include<cstdint>
#include<limits>
#include<type_traits>

namespace Standard::Algorithms::Numbers
{
    template<class digit_t>
    // It had better be "unsigned char" for a string
    // because ((char)255) is -1, that leads to index overflow.
    concept digita = std::unsigned_integral<digit_t> || std::same_as<std::byte, digit_t>;

    static_assert(std::byte{} == std::numeric_limits<std::byte>::max(), "It is hard to believe.");

    template<digita digit_t>
    constexpr auto max_digit = std::max<digit_t>( // NOLINTNEXTLINE
        { static_cast<digit_t>(127), static_cast<digit_t>(255), // NOLINTNEXTLINE
            static_cast<digit_t>(32'767), static_cast<digit_t>(65'536), std::numeric_limits<digit_t>::max() });
} // namespace Standard::Algorithms::Numbers
