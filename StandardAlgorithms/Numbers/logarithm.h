#pragma once
// "logarithm.h"
#include<algorithm>
#include<bit>
#include<cassert>
#include<concepts>
#include<cstddef>
#include<cstdint>
#include<tuple>
#include<utility>

namespace Standard::Algorithms::Numbers
{
    // See also "power_root.h"

    // Logarithm base 2, rounded down.
    // 1 => 0.
    // 2 => 1, 3 => 1.
    // 4 => 2, 7 => 2.
    // 8 => 3, 15 => 3.
    // 16 => 4.
    [[nodiscard]] inline constexpr auto log_base_2_down(std::uint64_t value) noexcept -> std::int32_t
    {
        constexpr auto bits = sizeof(std::uint64_t) << 3U;
        constexpr auto max_val = static_cast<std::int32_t>(bits - 1);
        static_assert(0 < max_val);

        auto logarithm = max_val - std::countl_zero(value);
        return logarithm;
    }

    // Logarithm base 2, rounded up.
    // 1 => 0, 2 => 1.
    // 3 => 2, 4 => 2.
    // 5 => 3, 8 => 3.
    // 9 => 4, 16 => 4.
    [[nodiscard]] inline constexpr auto log_base_2_up(std::uint64_t value) noexcept -> std::int32_t
    {
        constexpr std::uint64_t one = 1;

        value = std::max(value, one);

        const auto is_power_of_2 = 0U == (value & (value - one));

        auto logarithm = std::bit_width(value) - (is_power_of_2 ? 1 : 0);

        return logarithm;
    }

    // Logarithm base 10:
    // 1000..9999 - return 3,
    // 100..999 - 2,
    // 10..99 - 1,
    // 0..9 - 0.
    template<class int_t>
    [[nodiscard]] constexpr auto log10_slow(int_t numb) -> // logarithm, 10**logarithm, highest digit
        std::tuple<int_t, int_t, int_t>
    {
        assert(int_t{} <= numb);

        constexpr int_t ten{ 10 };
        static_assert(int_t{} < ten);

        int_t logar{};
        int_t pow10 = 1;

        while (ten <= numb)
        {
            numb /= ten;
            ++logar;
            pow10 *= ten;
        }

        assert(int_t{} <= numb && numb < ten && int_t{} < pow10);

        return std::make_tuple(logar, pow10, numb);
    }
} // namespace Standard::Algorithms::Numbers
