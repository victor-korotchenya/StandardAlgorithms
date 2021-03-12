#pragma once
#include"to_unsigned.h"

namespace Standard::Algorithms::Numbers
{
    // Use only if the type might be signed.
    // For an unsigned type, the result is (value & 1U).
    template<std::integral int_t>
    [[nodiscard]] static constexpr auto xor_1(int_t value) noexcept -> int_t
    {
        auto xorred = static_cast<int_t>(to_unsigned(value) ^ 1U);

        assert(xorred != value);

        return xorred;
    }
} // namespace Standard::Algorithms::Numbers
