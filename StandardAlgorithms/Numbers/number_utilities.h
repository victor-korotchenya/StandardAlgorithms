#pragma once
#include"../Utilities/same_sign_leq_size.h"
#include"arithmetic.h"
#include<vector>

namespace Standard::Algorithms::Numbers
{
    template<std::integral int_t, bool is_normalized = false>
    constexpr void make_non_negative(int_t &value, const int_t &modulus) noexcept
    {
        assert(&value != &modulus && int_t{} < modulus);

        if constexpr (!is_normalized)
        {
            value = static_cast<int_t>(value % modulus);
        }

        if constexpr (std::is_signed_v<int_t>)
        {
            if (value < int_t{})
            {
                value += modulus;
            }
        }

        assert(int_t{} <= value && value < modulus);
    }

    template<std::integral int_t, std::integral long_int_t>
    requires(same_sign_leq_size<int_t, long_int_t>)
    constexpr void product_modulo(const long_int_t &value, long_int_t &result, const int_t &modulus) noexcept
    {
        assert(&value != &modulus && &result != &modulus);
        assert(int_t{} < modulus);

        result = static_cast<long_int_t>((result * value) % modulus);
    }

    template<std::integral int_t>
    constexpr void sum_modulo(const int_t &value, int_t &result, const int_t &modulus) noexcept
    {
        assert(&value != &result && &value != &modulus && &result != &modulus);
        assert(int_t{} < modulus);

        result = (value + result) % modulus;
    }

    // Return the total number of numbers from 1 to number inclusively,
    //  having the reminder == "reminder".
    //
    //  "reminder" must belong to [0, divisor).
    //  "number" and "divisor" must be positive.
    template<std::integral int_t, int_t divisor>
    requires(int_t{} < divisor)
    [[nodiscard]] constexpr auto numbers_having_reminder(const int_t reminder, const int_t number) noexcept(
        !::Standard::Algorithms::is_debug) -> int_t
    {
        if constexpr (::Standard::Algorithms::is_debug)
        {
            if (reminder < int_t{}) [[unlikely]]
            {
                auto message = "The reminder (" + std::to_string(reminder) + ") must be non-negative";
                throw std::runtime_error(message);
            }

            if (divisor <= reminder) [[unlikely]]
            {
                auto message = "The reminder (" + std::to_string(reminder) + ") must be smaller than divisor(" +
                    std::to_string(divisor) + ")";
                throw std::runtime_error(message);
            }

            if (number <= int_t{}) [[unlikely]]
            {
                auto message = "The number (" + std::to_string(number) + ") must be strictly positive";

                throw std::runtime_error(message);
            }
        }

        auto ret = static_cast<int_t>((number + divisor - reminder) / divisor - (int_t{} == reminder ? 1 : 0));

        return ret;
    }

    // It is assumed that beginInclusive <= endInclusive,
    //  and the "divisor" is not zero.
    template<std::integral int_t>
    [[nodiscard]] constexpr auto divisors_in_range(
        const int_t &begin_inclusive, const int_t &end_inclusive, const int_t &divisor) noexcept -> int_t
    {
        assert(int_t{} < divisor && !(end_inclusive < begin_inclusive));
        // [[assume(int_t{} < divisor && !(endInclusive < beginInclusive))]];

        auto result = static_cast<int_t>(end_inclusive / divisor - (begin_inclusive - 1) / divisor);

        return result;
    }

    // Moebius function is defined for an integer n >0:
    // -1 when n is a product of an odd number of distinct primes e.g. 30=2*3*5, 7=7**1.
    // +1 when n is a product of an even number of distinct primes e.g. 1=2**0, 15=3*5.
    // 0 otherwise. Here n has not distinct primes e.g. n=24=2*2*2*3 has three (more than 1) factors of 2.
    // It is multiplicative for coprime x and y: u(x*y) = u(x)*u(y), gcd(x, y) = 1.
    template<std::integral int_t>
    [[nodiscard]] constexpr auto mobius(const int_t &size) noexcept(false) -> std::vector<std::int8_t>
    {
        require_positive(size, "size");

        {
            constexpr int_t max_size = std::numeric_limits<int_t>::max() / 3;

            require_greater(max_size, size, "size");
        }

        std::vector<std::int8_t> mob(size + 1LL);
        mob.at(1) = 1;

        for (int_t index = 1; index <= size; ++index)
        {
            const auto &mobi = mob[index];

            if (int_t{} == mobi)
            {
                continue;
            }

            for (auto ind_2 = index << 1U; ind_2 <= size; ind_2 += index)
            {
                // 2**26 has max(abs(mob)) = 18
                // 2**28 has max(abs(mob)) = 20
                // For a moderate size, here should be no overflow.
                mob[ind_2] -= mobi;
            }
        }

        return mob;
    }

    // The "modulus" must be not zero.
    // modular exponentiation, pow, power.
    template<std::integral int_t, std::integral long_int_t, std::integral modi_t>
    requires(same_sign_leq_size<modi_t, int_t>)
    [[nodiscard]] constexpr auto modular_power(int_t base, long_int_t exponent, const modi_t &modulus) noexcept -> int_t
    {
        assert(modi_t{} < modulus && !(exponent < long_int_t{}));

        base %= modulus;

        if (int_t{} == base)
        {
            return {};
        }

        int_t result{ 1 };

        constexpr long_int_t zero_exp{};

        while (zero_exp < exponent)
        {
            if (constexpr long_int_t one = 1; zero_exp != (exponent & one))
            {// A particular bit is set in the "exponent".
                product_modulo<int_t>(base, result, modulus);
            }

            product_modulo<int_t>(base, base, modulus);
            exponent >>= 1U;
        }

        make_non_negative<int_t, true>(result, modulus);

        return result;
    }

    // Sum of powers from 0 to "power_exclusive - 1".
    // E.g. for base = 2, and modulus = a large prime,
    // power exclusive, result:
    //  0, 0 - nothing to sum in the range [0, -1]
    //  1, 1 = 2**0 = 1
    //  2, 3 = 1 + 2
    //  3, 7 = 1 + 2 + 4
    //  4, 15 = 1 + 2 + 4 + 8
    //  10, 1023
    template<std::integral int_t, int_t modulus>
    requires(int_t{} < modulus)
    [[nodiscard]] constexpr auto powers_sum_modulo(int_t base, int_t power_exclusive) noexcept -> int_t
    {
        constexpr int_t zero{};

        base %= modulus;

        assert(!(base < zero) && !(power_exclusive < zero));

        if (zero == base)
        {
            return zero;
        }

        int_t result{};
        int_t temp = 1;
        int_t digit = 1;

        while (zero < power_exclusive)
        {
            if (zero != (power_exclusive & digit))
            {
                power_exclusive &= ~digit;

                auto add = static_cast<int_t>((temp * modular_power(base, power_exclusive, modulus)) % modulus);

                sum_modulo<int_t>(add, result, modulus);
            }

            sum_modulo<int_t>(((temp * modular_power(base, digit, modulus)) % modulus), temp, modulus);

            digit <<= 1U;
        }

        return result;
    }
} // namespace Standard::Algorithms::Numbers
