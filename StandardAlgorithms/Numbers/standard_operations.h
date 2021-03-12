#pragma once
// "standard_operations.h"
#include"gcd.h"
#include"modulo_inverse.h"

namespace Standard::Algorithms::Numbers
{
    // There is no overflow check.
    template<class int_t>
    [[nodiscard]] constexpr auto sum_unchecked(const int_t &one, const int_t &two) -> int_t
    {
        auto result = int_t(one + two);
        return result;
    }

    // There is no overflow check.
    template<class int_t>
    [[nodiscard]] constexpr auto subtract_unchecked(const int_t &one, const int_t &two) -> int_t
    {
        auto result = int_t(one - two);
        return result;
    }

    template<class int_t>
    [[nodiscard]] constexpr auto min(const int_t &one, const int_t &two)
        // todo(p4): -> const int_t&
        -> int_t
    {
        const auto &result = std::min(one, two);
        return result;
    }

    template<class int_t>
    [[nodiscard]] constexpr auto max(const int_t &one, const int_t &two)
        // todo(p4): -> const int_t&
        -> int_t
    {
        const auto &result = std::max(one, two);
        return result;
    }

    // It seams that "xor" is not a valid name - let's use "xorr" instead.
    template<std::integral int_t>
    [[nodiscard]] constexpr auto xorr(const int_t &one, const int_t &two) -> int_t
    {
        auto result = int_t(one ^ two);
        return result;
    }

    // Greatest common divisor.
    template<std::unsigned_integral int_t>
    [[nodiscard]] constexpr auto gcd(const int_t &one, const int_t &two) -> int_t
    {
        auto result = gcd_unsigned<int_t>(one, two);
        return result;
    }

    // Least common multiple.
    // There is no overflow check.
    // See also std::lcm
    template<std::unsigned_integral int_t>
    [[nodiscard]] constexpr auto lcm(const int_t &one, const int_t &two) -> int_t
    {
        if (0U == one || 0U == two)
        {
            return 0U;
        }

        const auto gcd1 = gcd_unsigned<int_t>(one, two);
        assert(0U < gcd1);

        auto result = static_cast<int_t>(one / gcd1 * two);
        return result;
    }

    // There is no overflow check.
    template<std::integral int_t, int_t modulus>
    requires(int_t{} < modulus)
    [[nodiscard]] constexpr auto sum_modulo_unchecked(const int_t &one, const int_t &two) -> int_t
    {
        auto result = int_t(((one % modulus) + (two % modulus)) % modulus);
        return result;
    }

    // There is no overflow check.
    template<std::integral int_t, int_t modulus>
    requires(int_t{} < modulus)
    [[nodiscard]] constexpr auto subtract_modulo_unchecked(const int_t &one, const int_t &two) -> int_t
    {
        auto result = int_t(((one % modulus) - (two % modulus) + modulus) % modulus);
        return result;
    }

    // Multiply modulo.
    // There is no overflow check.
    template<std::integral int_t, int_t modulus>
    requires(int_t{} < modulus)
    [[nodiscard]] constexpr auto product_modulo_unchecked(const int_t &one, const int_t &two) -> int_t
    {
        auto result = int_t(((one % modulus) * (two % modulus)) % modulus);
        return result;
    }

    // There is no overflow check.
    //  result = dividend / divisor.
    template<std::signed_integral int_t, int_t modulus>
    requires(int_t{} < modulus)
    [[nodiscard]] constexpr auto divide_modulo_unchecked(
        // Two same type adjacent parameters suit well here.
        // NOLINTNEXTLINE
        const int_t &dividend, const int_t &divisor) -> int_t
    {
        constexpr int_t zero{};

        const auto rem_1 = dividend % modulus;
        const auto rem_2 = divisor % modulus;

        if (zero == rem_1 || zero == rem_2)
        {
            return zero;
        }

        const auto inve = modulo_inverse<int_t>(rem_2, modulus);
        auto result = static_cast<int_t>(rem_1 * inve % modulus);

        return result;
    }
} // namespace Standard::Algorithms::Numbers
