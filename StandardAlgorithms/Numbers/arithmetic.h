#pragma once
#include"../Utilities/clang_constexpr.h"
#include"../Utilities/require_utilities.h"
#include"../Utilities/static_false.h"
#include"arithmetic_exception.h"
#include<array>
#include<bit>
#include<cmath>
#include<functional>
#include<numbers>
#include<numeric>

// todo(p2): look at #include<safeint.h>
// #include<intrin.h> // todo(p3): fix or use <bit>
// #pragma intrinsic(_BitScanForward64)
// #pragma intrinsic(_BitScanReverse)
// #pragma intrinsic(__popcnt)
// #pragma intrinsic(__popcnt64)

namespace Standard::Algorithms::Numbers
{
    // How to use:
    // noexcept(is_compare_noexcept<t>)
    template<class t>
    constexpr bool is_compare_noexcept = noexcept(std::declval<t>() < std::declval<t>());

    // How to use:
    // noexcept(is_add_noexcept<t>)
    template<class t>
    constexpr bool is_add_noexcept = noexcept(std::declval<t>() + std::declval<t>());

    template<std::floating_point floating_t>
    [[nodiscard]] inline constexpr auto absolute_value(const floating_t &val) noexcept(false) -> floating_t
    {
        auto result = ::Standard::Algorithms::fabs(val);
        return result;
    }

    template<std::floating_point floating_t>
    [[nodiscard]] inline constexpr auto is_infinite(const floating_t &val) -> bool
    {
        auto result = std::isinf(val);
        return result;
    }

    template<class t>
    constexpr void assert_signed(const std::string &function_name)
    {
        assert(!function_name.empty());

        const auto temp = static_cast<t>(t{} - static_cast<t>(1));
        if (t{} < temp) [[unlikely]]
        {
            const auto message = "The '" + function_name + "' function is for signed numbers only.";
            throw std::runtime_error(message);
        }
    }

    template<class t>
    constexpr void assert_unsigned(const std::string &function_name)
    {
        assert(!function_name.empty());

        const auto temp = static_cast<t>(t{} - static_cast<t>(1));
        if (temp < t{}) [[unlikely]]
        {
            const auto message = "The '" + function_name + "' function is for unsigned numbers only.";

            throw std::runtime_error(message);
        }
    }

    template<std::signed_integral t>
    [[nodiscard]] constexpr auto is_negative(const t &value) noexcept(is_compare_noexcept<t>) -> bool
    {
        return value < t{};
    }

    template<class t>
    [[nodiscard]] constexpr auto sign(const t &val) noexcept(is_compare_noexcept<t>) -> std::int32_t
    {
        constexpr t zero{};

        auto res = (zero < val ? 1 : 0) - (val < zero ? 1 : 0);
        return res;
    };

    // Throw an exception in case of an overflow.
    template<std::unsigned_integral t>
    [[nodiscard]] constexpr auto multiply_unsigned(const t &one, const t &two) noexcept(false) -> t
    {
        if constexpr (::Standard::Algorithms::is_debug)
        {
            const auto *const func_name = static_cast<const char *>(__FUNCTION__);

            assert_unsigned<t>(func_name);
        }

        if (0U == two) [[unlikely]]
        {
            return 0U;
        }

        constexpr auto max_value = std::numeric_limits<t>::max();

        if ((max_value) / two < one) [[unlikely]]
        {
            auto str = ::Standard::Algorithms::Utilities::w_stream();
            str << "Multiplying " << one << " and " << two << " will produce an overflow.";
            throw arithmetic_exception(str.str());
        }

        auto result = static_cast<t>(one * two);
        return result;
    }

    template<std::signed_integral t>
    [[nodiscard]] constexpr auto multiply_signed(const t &one, const t &two) noexcept(false) -> t
    {
        if constexpr (::Standard::Algorithms::is_debug)
        {
            const auto *const func_name = static_cast<const char *>(__FUNCTION__);

            assert_signed<t>(func_name);
        }

        if (0 == one) [[unlikely]]
        {
            return 0;
        }

        auto result = static_cast<t>(one * two);
        if (two == result / one) [[likely]]
        {
            return result;
        }

        {
            auto str = ::Standard::Algorithms::Utilities::w_stream();
            str << "Multiplying " << one << " and " << two << " will produce an overflow.";
            throw arithmetic_exception(str.str());
        }
    }

    template<std::unsigned_integral t>
    [[nodiscard]] constexpr auto add_unsigned(const t &one, const t &two) noexcept(false) -> t
    {
        if constexpr (::Standard::Algorithms::is_debug)
        {
            const auto *const func_name = static_cast<const char *>(__FUNCTION__);

            assert_unsigned<t>(func_name);
        }

        auto result = static_cast<t>(one + two);
        if (result < one) [[unlikely]]
        {
            auto str = ::Standard::Algorithms::Utilities::w_stream();
            str << "Adding " << one << " and " << two << " will produce an overflow.";
            throw arithmetic_exception(str.str());
        }

        return result;
    }

    template<std::signed_integral t>
    [[nodiscard]] constexpr auto add_signed(const t &one, const t &two) noexcept(false) -> t
    {
        if constexpr (::Standard::Algorithms::is_debug)
        {
            const auto *const func_name = static_cast<const char *>(__FUNCTION__);

            assert_signed<t>(func_name);
        }

        auto result = static_cast<t>(one + two);

        if (one < t{} ? two < result : result < two) [[unlikely]]
        {
            auto str = ::Standard::Algorithms::Utilities::w_stream();
            str << "Adding " << one << " and " << two << " will produce an overflow.";
            throw arithmetic_exception(str.str());
        }

        return result;
    }

    template<std::signed_integral t>
    [[nodiscard]] constexpr auto subtract_signed(const t &one, const t &two) noexcept(false) -> t
    {
        if constexpr (::Standard::Algorithms::is_debug)
        {
            const auto *const func_name = static_cast<const char *>(__FUNCTION__);

            assert_signed<t>(func_name);
        }

        auto result = static_cast<t>(one - two);

        if ((t{} < two) == (result < one)) [[likely]]
        {
            return result;
        }

        auto str = ::Standard::Algorithms::Utilities::w_stream();
        str << "Subtracting " << one << " minus " << two << " will produce an overflow.";
        throw arithmetic_exception(str.str());
    }

    // Zero must not be an input!
    // Numbers starts from 0, that is count_tail_zeros(2) == 1.
    // Given 8 == (1000 in binary), return 3.
    [[nodiscard]] inline constexpr auto count_tail_zeros(const std::uint64_t &value) // todo(p3): noexcept ?
        -> std::uint32_t
    {
        assert(value != 0U);

#ifdef _WIN64_todo_fix_it
        std::uint64_t result{};

        [[maybe_unused]] const char is_ok = _BitScanForward64(&result, value);

        if constexpr (::Standard::Algorithms::is_debug)
        {
            if (is_ok == 0)
            {
                auto str = ::Standard::Algorithms::Utilities::w_stream();

                const auto *const func_name = static_cast<const char *>(__FUNCTION__);

                str << "Error in " << func_name << "(" << value << ").";

                throw_exception(str);
            }
        }

        return static_cast<std::uint32_t>(result);
#else
        // Linux
        // std::int32_t __builtin_ffsl (std::uint64_t)
        //  Similar to __builtin_ffs, except the argument type is std::uint64_t.
        //
        // Built-in Function: std::int32_t __builtin_ffsll (std::uint64_t)
        //  Similar to __builtin_ffs, except the argument type is std::uint64_t.
        //
        // Built-in Function: std::int32_t __builtin_ffs (std::uint32_t x)
        //  Return one plus the index of the least significant 1-bit of x,
        // or if x is zero, return zero.

        auto ffs = __builtin_ffsll( // Sometimes the input is 'long long'.
            static_cast<std::int64_t>(value));

        auto result = static_cast<std::uint32_t>(ffs - 1);
        return result;
#endif
    }

    // Zero must not be an input!
    // Return the number of leading 0-bits in value,
    // starting at the most significant bit position.
    // E.g given 1, return 0,
    // 3 -> 1, 7 -> 2,
    // 8 -> 3, 15 -> 3, but 16 -> 4.
    //
    // _BitScanReverse, _BitScanReverse64
    // See also most_significant_bit64.
    [[nodiscard]] inline constexpr auto most_significant_bit(const std::uint32_t value) // todo(p3): noexcept ?
        -> std::uint32_t
    {
        assert(value != 0U);

#ifdef _WIN64_todo_fix_it
        std::uint64_t result{};

        [[maybe_unused]] char is_ok = _BitScanReverse(&result, value);

        if constexpr (::Standard::Algorithms::is_debug)
        {
            if (is_ok == 0)
            {
                auto str = ::Standard::Algorithms::Utilities::w_stream();

                const auto *const func_name = static_cast<const char *>(__FUNCTION__);

                str << "Error in " << func_name << "(" << value << ").";
                throw_exception(str);
            }
        }

        return static_cast<std::uint32_t>(result);
#else
        // Linux
        // Return the number of leading 0-bits in value,
        // starting at the most significant bit position.
        // If value is 0, the result is undefined.
        const std::uint32_t number_of_leading0 = __builtin_clz(value);

        constexpr auto max_val = 31U;
        auto result = max_val - number_of_leading0;
        return result;
#endif
    }

    // See also MostSignificantBit.
    [[nodiscard]] inline constexpr auto most_significant_bit64(const std::uint64_t value) // todo(p3): noexcept ?
        -> std::uint32_t
    {
        assert(value != 0U);

#ifdef _WIN64_todo_fix_it
        std::uint64_t result{};

        [[maybe_unused]] char is_ok = _BitScanReverse64(&result, value);

        if constexpr (::Standard::Algorithms::is_debug)
        {
            if (is_ok == 0)
            {
                auto str = ::Standard::Algorithms::Utilities::w_stream();

                const auto *const func_name = static_cast<const char *>(__FUNCTION__);

                str << "Error in " << func_name << "(" << value << ").";
                throw_exception(str);
            }
        }

        return static_cast<std::uint32_t>(result);
#else
        // Linux
        // Return the number of leading 0-bits in value,
        // starting at the most significant bit position.
        // If value is 0, the result is undefined.
        const std::uint32_t number_of_leading0 = __builtin_clzll(value);

        constexpr auto max_val = 63U;

        auto result = max_val - number_of_leading0;
        return result;
#endif
    }

    template<std::unsigned_integral t>
    [[nodiscard]] inline constexpr auto most_significant_bit(const t &value) // todo(p3): noexcept ?
        -> std::uint32_t
    {
        if constexpr (sizeof(t) <= sizeof(std::uint32_t))
        {
            auto res = most_significant_bit(static_cast<std::uint32_t>(value));
            return res;
        }
        else if constexpr (sizeof(t) <= sizeof(std::uint64_t))
        {
            auto res = most_significant_bit64(static_cast<std::uint64_t>(value));
            return res;
        }
        else
        {
            static_assert(static_false<t>);

            throw std::runtime_error("Error in most_significant_bit");
        }
    }

    // todo(p4): use std::popcount and similar.
    [[nodiscard]] inline constexpr auto pop_count(const std::uint32_t value) // todo(p3): noexcept ?
        -> std::uint32_t
    {
        const auto
#ifdef _WIN64_todo_fix_it
            result = static_cast<std::uint32_t>(__popcnt(value));
#else
            // Built-in Function: std::int32_t __builtin_popcountl (std::uint64_t)
            // Similar to __builtin_popcount, except the argument type is std::uint64_t.
            //
            // Built-in Function: std::int32_t __builtin_popcount (std::uint32_t x)
            // Return the number of 1-bits in x.
            //
            // Built-in Function: std::int32_t __builtin_popcountll (std::uint64_t)
            // Similar to __builtin_popcount, except the argument type is std::uint64_t.
            result = static_cast<std::uint32_t>(__builtin_popcount(value));
#endif

        return result;
    }

    // unsigned __int64 __popcnt64(unsigned __int64)
    [[nodiscard]] inline constexpr auto pop_count(const std::uint64_t &value) // todo(p3): noexcept ?
        -> std::uint32_t
    {
        const auto
#ifdef _WIN64_todo_fix_it
            result = static_cast<std::uint32_t>(__popcnt64(value));
#else
            // Built-in Function: std::int32_t __builtin_popcountl (std::uint64_t)
            // Similar to __builtin_popcount, except the argument type is std::uint64_t.
            //
            // Built-in Function: std::int32_t __builtin_popcount (std::uint32_t x)
            // Return the number of 1-bits in x.
            //
            // Built-in Function: std::int32_t __builtin_popcountll (std::uint64_t)
            // Similar to __builtin_popcount, except the argument type is std::uint64_t.
            result = static_cast<std::uint32_t>(__builtin_popcountll(value));
#endif

        return result;
    }

    template<class t>
    [[nodiscard]] constexpr auto compare(const t &one, const t &two) noexcept(is_compare_noexcept<t>) -> std::int32_t
    {
        return one < two ? -1 : (two < one ? 1 : 0);
    };

    template<class t>
    [[nodiscard]] constexpr auto greater_than_function(const t &one, const t &two) noexcept(is_compare_noexcept<t>)
        -> bool
    {
        auto result = two < one;
        return result;
    };

    // template<class t>
    // struct GreaterThanClass final
    //{
    //   [[nodiscard]] auto operator() (const t &one, const t &two) const -> bool
    //   {
    //     auto result = two < one;
    //     return result;
    //   }
    // };

    template<std::integral int_t>
    [[nodiscard]] constexpr auto is_power_of_two(const int_t &numb) -> bool
    {
        return int_t{} != numb &&
            // Delete the only 1-bit.
            int_t{} == (numb & (numb - static_cast<int_t>(1)));
    }

    // Given 0, return 0.
    // 1 -> 1.
    // 2..3 -> 2.
    // 4..7 -> 4.
    // 8..15 -> 8.
    // 16..31 -> 16.
    [[nodiscard]] inline constexpr auto round_to_lesser_power_of_two(const std::uint32_t numb) -> std::uint32_t
    // todo(p3): noexcept ?
    {
        if (numb == 0U)
        {
            return 0;
        }

        const auto hi_bit = most_significant_bit(numb);
        {
            constexpr auto max_value = 31U;
            assert(hi_bit <= max_value);
        }

        auto result = 1U << hi_bit;
        return result;
    }

    // Given 0 or 1, return 1.
    // 2 -> 2.
    // 3..4 -> 4.
    // 5..8 -> 8.
    // 9..16 -> 16.
    [[nodiscard]] inline constexpr auto round_to_greater_power_of_two(std::uint32_t numb) noexcept -> std::uint32_t
    {
        if (numb-- == 0U)
        {
            return 1;
        }

        numb |= numb >> 1U;
        numb |= numb >> 2U;

        // NOLINTNEXTLINE
        numb |= numb >> 4U;

        // NOLINTNEXTLINE
        numb |= numb >> 8U;

        // NOLINTNEXTLINE
        numb |= numb >> 16U;
        ++numb;

        return numb;
    }

    [[nodiscard]] inline constexpr auto round_to_greater_power_of_two(std::uint64_t numb) noexcept -> std::uint64_t
    {
        if (numb-- == 0U)
        {
            return 1;
        }

        numb |= numb >> 1U;
        numb |= numb >> 2U;

        // NOLINTNEXTLINE
        numb |= numb >> 4U;

        // NOLINTNEXTLINE
        numb |= numb >> 8U;

        // NOLINTNEXTLINE
        numb |= numb >> 16U;

        // NOLINTNEXTLINE
        numb |= numb >> 32U;
        ++numb;

        return numb;
    }

    // Given 1011001,
    // get back 1111111 where all 0s after the highest 1 bit are set to 1.
    // Given 0, return 0.
    [[nodiscard]] inline constexpr auto xor_max(std::uint32_t value) noexcept -> std::uint32_t
    {
        value |= value >> 1U;
        value |= value >> 2U;

        // NOLINTNEXTLINE
        value |= value >> 4U;

        // NOLINTNEXTLINE
        value |= value >> 8U;

        // NOLINTNEXTLINE
        value |= value >> 16U;

        return value;
    }

    [[nodiscard]] inline constexpr auto xor_max(std::uint64_t value) noexcept -> std::uint64_t
    {
        value |= value >> 1U;
        value |= value >> 2U;

        // NOLINTNEXTLINE
        value |= value >> 4U;

        // NOLINTNEXTLINE
        value |= value >> 8U;

        // NOLINTNEXTLINE
        value |= value >> 16U;

        // NOLINTNEXTLINE
        value |= value >> 32U;

        return value;
    }

    // Must be divisible by alignment.
    // See also std::assume_aligned
    [[nodiscard]] inline constexpr auto pad_by_alignment(
        const std::uint64_t &value, const std::uint32_t alignment) noexcept -> std::uint64_t
    {
        constexpr auto max_alignment = 256U;

        assert(is_power_of_two(alignment) && alignment <= max_alignment);

        const auto reminder = (value & (alignment - 1LLU)) & (max_alignment - 1U);

        auto result = reminder != 0U ? value + alignment - reminder : value;

        return result;
    }

    // Given 0x9A, return 0x9A9A9A9A9A9A9A9A.
    [[nodiscard]] inline constexpr auto convert_by_duplicating(std::uint8_t byt) noexcept -> std::uint64_t
    {
        constexpr auto length = static_cast<std::int32_t>(sizeof(std::uint64_t));

        auto uns = static_cast<std::uint64_t>(byt);
        auto result = uns;

        for (std::int32_t index = 1; index < length; ++index)
        {
            uns = uns << static_cast<std::uint32_t>(CHAR_BIT);
            result |= uns;
        }

        return result;
    }

    template<std::integral int_t>
    [[nodiscard]] constexpr auto is_even(const int_t &one) -> bool
    {
        auto result = int_t{} == (one & static_cast<int_t>(1));

        return result;
    }

    // Just 1 comparison branch.
    template<std::unsigned_integral t>
    [[nodiscard]] constexpr auto is_within_inclusive(const t &value, const std::pair<t, t> &min_max_inclusive) noexcept(
        false) -> bool
    {
        const auto &min_inclusive = min_max_inclusive.first;
        const auto &max_inclusive = min_max_inclusive.second;

        if (max_inclusive < min_inclusive)
        {
            auto str = ::Standard::Algorithms::Utilities::w_stream();
            str << "The minInclusive " << min_inclusive << " cannot exceed maxInclusive " << max_inclusive << ".";

            throw_exception(str);
        }

        const auto limits_delta = max_inclusive - min_inclusive;
        const auto value_delta = value - min_inclusive;

        auto result = !(limits_delta < value_delta);
        return result;
    }

    // Implement the <= comparison assuming t* is a pointer.
    template<class t1, class comparer_t1 = std::less_equal<t1>>
    struct less_equal_by_ptr final
    {
        using t = t1;
        using comparer_t = comparer_t1;

        [[nodiscard]] constexpr auto operator() (const t *left, const t *right) const
            noexcept(noexcept(std::declval<comparer_t>()(std::declval<const t &>(), std::declval<const t &>()))) -> bool
        {
            auto result = left == right || (nullptr != left && nullptr != right && (Comparer(*left, *right)));

            return result;
        }

private:
        comparer_t Comparer{};
    };

    template<class int_t>
    [[nodiscard]] constexpr auto abs_diff(const int_t &one, const int_t &two) -> int_t
    {
        auto result = static_cast<int_t>(two < one ? one - two : two - one);

        return result;
    }
} // namespace Standard::Algorithms::Numbers
