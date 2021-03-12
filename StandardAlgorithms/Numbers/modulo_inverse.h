#pragma once
#include"number_utilities.h" // make_non_negative
#include"totient.h"

namespace Standard::Algorithms::Numbers
{
    // r * a = 1 mod m, where r is called multiplicative modular inverse (multiplicative inverse) of a modulo m >=2,
    // where 0 < a < mod, and 0 < r < mod.
    // That is, x * inv(x) = 1 modulo m.
    // m = (m/a) * a + (m % a), where (m/a) is integer division e.g. 5/2 = 2.
    //
    // (m % a) = m - (m/a) * a + 0 =
    //         = m - (m/a) * a + m*a - m*a =
    //         = m*a + m - (m/a) * a - m*a =
    //         = m*a - (m/a) * a + m - a*m =
    //         = (m - (m/a)) * a + m - m*a
    // Take modulo m:
    // (m % a) = ((m - (m/a)) * a) mod m
    //
    // Multiply both sides by inv(m % a):
    // (m % a) * inv(m % a) = ((m - (m/a)) * a) * inv(m % a) mod m
    // (m % a) * inv(m % a) = (m - (m/a)) * inv(m % a) * a mod m
    //
    // The left side is 1 by definition:
    // 1 = (m - (m/a)) * inv(m % a) * a mod m
    //
    // Thus,
    // inv(a) = ((m - (m/a)) * inv(m % a)) % m
    template<std::integral int_t, std::integral long_int_t = int_t>
    requires(same_sign_leq_size<int_t, long_int_t>)
    constexpr void modular_inverses_first_n(
        const int_t &num, std::vector<int_t> &result, const int_t &modulus_prime) noexcept(false)
    {
        constexpr int_t zero{};
        constexpr int_t one = 1;

        if (!(zero < num)) [[unlikely]]
        {
            throw std::runtime_error("num must be positive.");
        }

        if (!(one < modulus_prime)) [[unlikely]]
        {
            throw std::runtime_error("modulus must be 2 or greater.");
        }

        result.resize(num + 1LL);

        result.at(0) = result.at(num) = zero;
        result.at(1) = one;

        for (int_t div = 2; div < num; ++div)
        {
            // inv(d) = [(m - (m/d)) * inv(m % d)] % m
            const auto left = static_cast<long_int_t>(modulus_prime - (modulus_prime / div));
            const auto remainder = modulus_prime % div;
            const auto &right = result.at(remainder);
            const auto prod = left * static_cast<long_int_t>(right);

            assert(!(prod < zero));

            result[div] = static_cast<int_t>(prod % modulus_prime);
        }
    }

    // When modulus is a prime number.
    template<std::integral int_t>
    [[nodiscard]] constexpr auto modulo_inverse_prime(int_t value, int_t modulus) noexcept -> int_t
    {
        assert(2 <= modulus);

        // Fermat's theorem: 1 === power(a, mod-1) modulo m.
        // Divide by a:
        // inv(a) === power(a, -1) === power(a, mod-2) modulo m.
        auto result = modular_power(value, static_cast<int_t>(modulus - 2), modulus);
        return result;
    }

    // Use Euclid's GCD.
    template<std::signed_integral int_t>
    [[nodiscard]] constexpr auto modulo_inverse(int_t value, const int_t &modulus) noexcept -> int_t
    {
        assert(int_t{} < modulus);

        make_non_negative<int_t>(value, modulus);

        constexpr int_t one{ 1 };

        auto result = one;
        {
            auto bus = modulus;
            int_t remainder{};

            while (one < value)
            {
                const auto quotient = static_cast<int_t>(value / bus);
                {
                    auto old_bus = bus;
                    bus = static_cast<int_t>(value % bus);
                    value = old_bus;
                }

                auto old_remainder = remainder;
                remainder = static_cast<int_t>(result - (quotient * remainder)); // Might overflow.
                result = old_remainder;
            }
        }

        make_non_negative<int_t>(result, modulus);

        return result;
    }

    // When modulus is prime or not prime.
    template<std::integral long_int_t, std::integral int_t, int_t modulus>
    requires(same_sign_leq_size<int_t, long_int_t> && int_t{ 1 } < modulus)
    [[nodiscard]] constexpr auto modulo_inverse_slow_but_checked(
        int_t value, std::vector<int_t> &prime_factors) noexcept(false) -> int_t
    {
        make_non_negative<int_t>(value, modulus);

        const auto phi = euler_phi_totient<long_int_t, int_t>(modulus, prime_factors);

        auto result = modular_power(value, phi - 1, modulus);

        return result;
    }
} // namespace Standard::Algorithms::Numbers
