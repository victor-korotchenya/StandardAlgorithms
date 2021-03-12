#pragma once
// "primitive_root.h"
#include"../Utilities/zu_string.h"
#include"factoring_utilities.h" // decompose_into_prime_divisors
#include"number_utilities.h" // modular_power
#include<cassert>
#include<concepts>
#include<cstdint>
#include<limits>
#include<vector>

namespace Standard::Algorithms::Numbers
{
    // The base may be composite (not prime).
    // Please ensure there is no overflow.
    // Slow time O(n).
    template<std::integral int_t>
    requires(sizeof(std::uint64_t) <= sizeof(int_t))
    [[nodiscard]] constexpr auto is_generator_slow(const int_t &base1, const int_t &root) -> bool
    {
        constexpr int_t zero{};
        constexpr int_t one{ 1 };
        static_assert(zero < one);

        {
            constexpr int_t maxi = std::numeric_limits<std::uint32_t>::max();
            static_assert(int_t{} < maxi);

            if (!(base1 < maxi)) [[unlikely]]
            {
                return false;
            }
        }

        if (const auto quick_test = zero < root && root < base1 && root == root % base1; !quick_test)
        {
            auto isroo = zero == root && one == base1;
            return isroo;
        }

        std::vector<bool> useds(base1, false);
        useds[zero] = true;

        for (int_t index = 1, value = 1; index < base1; ++index)
        {
            const auto next_value = static_cast<int_t>(value * root % base1); // may overflow.
            if (!(zero < next_value) || !(next_value < base1) || useds[next_value])
            {
                return false;
            }

            useds[next_value] = true;
            value = next_value;
        }

        if (const auto iter = std::find(useds.cbegin(), useds.cend(), false); iter != useds.cend()) [[unlikely]]
        {
            return false;
        }

        return true;
    }

    // The prime must be a prime number.
    // The smallest possible number, generating all the numbers [1..prime-1] modulo prime, returned.
    // Or, all the values (number ** k % modulo) must be unique for k in [1..prime-1].
    template<std::integral int_t>
    requires(sizeof(std::int64_t) <= sizeof(int_t))
    [[nodiscard]] constexpr auto primitive_root_generator_slow(const int_t &prime) -> int_t
    {
        [[maybe_unused]] constexpr int_t zero{};
        constexpr int_t one{ 1 };
        constexpr int_t two{ 2 };

        if (constexpr int_t five{ 5 }; !(five < prime))
        {
            return two < prime ? two : one;
        }

        const auto eu_totient = static_cast<int_t>(prime - one);

        std::vector<int_t> prime_factors;
        decompose_into_prime_divisors<int_t>(eu_totient, prime_factors);

        assert(!prime_factors.empty() && two == prime_factors[0]); // (17 - 1) == 2 ** 4.

        for (auto root1 = two;;)
        {
            bool has_one{};

            for (const auto &factor : prime_factors)
            {
                assert(one < factor && zero == (eu_totient % factor) && factor < eu_totient);

                const auto exponent1 = static_cast<int_t>(eu_totient / factor);
                const auto power_rem = modular_power<int_t>(root1, exponent1, prime);

                if (power_rem == one)
                {
                    has_one = true;
                    break;
                }
            }

            if (!has_one)
            {
                return root1;
            }

            if (++root1 < prime) [[likely]]
            {
                continue;
            }

            {
                auto err = "A prime number must have been used in primitive_root_generator_slow " +
                    ::Standard::Algorithms::Utilities::zu_string(prime);

                throw std::runtime_error(err);
            }
        }
    }
} // namespace Standard::Algorithms::Numbers
