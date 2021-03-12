#pragma once
// "bit_utilities.h"
#include"arithmetic.h"
#include<bit> // std::popcount
#include<vector>

namespace Standard::Algorithms::Numbers
{
    // Given 12, return 4.
    // 10 -> 2.
    // 7 -> 1
    // 6 -> 2
    // 3 -> 1
    // 2 -> 2
    // 1 -> 1
    // 0 -> 0
    template<std::integral int_t>
    [[nodiscard]] constexpr auto lowest_bit(const int_t &value) noexcept -> int_t
    {
        auto result = static_cast<int_t>(value & (int_t{} - value));

        return result;
    }

    template<std::integral int_t>
    [[nodiscard]] constexpr auto has_zero_bit(const int_t &value) noexcept -> bool
    {
        constexpr int_t zero{};
        constexpr int_t one = 1;

        auto jazz = (value & one) != zero;
        return jazz;
    }

    // Find the smallest greater number with the same number of binary 1-bits.
    // Zero is not a valid input.
    // Watch out for an overflow!
    template<std::unsigned_integral int_t>
    [[nodiscard]] constexpr auto next_greater_same_pop_count(const int_t &value) noexcept(
        !::Standard::Algorithms::is_debug) -> int_t
    {
        [[maybe_unused]] constexpr int_t zero{};
        // [[assume(zero != value)]];

        [[maybe_unused]] constexpr auto max_value = std::numeric_limits<int_t>::max();

        const auto low = lowest_bit<int_t>(value);

        assert(zero < value && value < max_value && zero < low);

        if constexpr (::Standard::Algorithms::is_debug)
        {
            require_positive(low, "low");
            require_greater(max_value, low, "low");
        }

        const auto sum = static_cast<int_t>(value + low);

        auto result = static_cast<int_t>((((sum ^ value) >> 2U) / low) | sum);

        assert(zero < result && result != value);

        if constexpr (::Standard::Algorithms::is_debug)
        {
            const auto bits0 = pop_count(value);
            const auto bits1 = pop_count(result);

            assert(0 < bits0 && bits0 == bits1);
        }

        return result;
    }

    template<std::unsigned_integral int_t>
    [[nodiscard]] constexpr auto next_smaller_same_pop_count(const int_t &value) noexcept(
        !::Standard::Algorithms::is_debug) -> int_t
    {
        assert(value != 0U && value != std::numeric_limits<int_t>::max());

        auto result = ~next_greater_same_pop_count(~value);

        assert(result != 0U && value != result);

        if constexpr (::Standard::Algorithms::is_debug)
        {
            {
                const auto bits0 = pop_count(value);
                const auto bits1 = pop_count(result);

                assert(0 < bits0 && bits0 == bits1);
            }
            {
                const auto next = next_greater_same_pop_count(result);

                assert(value == next);
            }
        }

        return result;
    }

    constexpr std::uint64_t highest_high_mask = 0x8080808080808080ULL;

    template<std::unsigned_integral int_t>
    requires(sizeof(int_t) <= sizeof(std::uint64_t))
    [[nodiscard]] constexpr auto has_no_highest_byte_bit(const int_t &mask) noexcept -> bool
    {
        auto res = 0U == (mask & highest_high_mask);
        return res;
    }

    // To subtract spaces, 0x30 should be replicated eight times.
    constexpr std::uint64_t lower_high_mask = 0x3030303030303030ULL;
    static_assert(has_no_highest_byte_bit(lower_high_mask));

    constexpr std::uint64_t max_signed_high_mask = 0x7F7F7F7F7F7F7F7FULL;
    static_assert(has_no_highest_byte_bit(max_signed_high_mask));

    template<std::uint64_t subtrahend = lower_high_mask>
    requires(has_no_highest_byte_bit(subtrahend))
    constexpr void multi_byte_subtract(std::uint64_t &minuend) noexcept
    {
        const auto avoid_overflow = (minuend | highest_high_mask) - subtrahend;

        const auto high_bits = minuend | max_signed_high_mask;
        minuend = ~(avoid_overflow ^ high_bits);
    }

    // To find the first byte not greater than 9,
    //  (0x7F - 9) - 0x76 should be replicated eight times.
    constexpr std::uint64_t find_9_mask = 0x7676767676767676ULL;
    static_assert(has_no_highest_byte_bit(find_9_mask));

    template<std::uint64_t mask = find_9_mask>
    requires(has_no_highest_byte_bit(mask))
    [[nodiscard]] constexpr auto find_first_byte(const std::uint64_t &value) -> std::uint32_t
    {
        const auto eighties = ~(((value & max_signed_high_mask) + mask) | value | max_signed_high_mask);

        constexpr std::uint32_t zero_result = 8U;

        auto result = 0U == eighties ? zero_result : (most_significant_bit64(eighties) >> 3U);

        return result;
    }

    // Start from the value, then in the decreasing order.
    // E.g. given 6 = 0b110, return {6,4,2}.
    template<std::unsigned_integral int_t>
    constexpr void enumerate_all_subsets_of_mask(const int_t &value, std::vector<int_t> &result) noexcept(false)
    {// todo(p3): move to another file?
        result.clear();

        for (auto index = value; index != 0U; index = static_cast<int_t>((index - 1U) & value))
        {
            result.push_back(index);
        }
    }
} // namespace Standard::Algorithms::Numbers
