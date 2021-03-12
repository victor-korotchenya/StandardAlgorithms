#pragma once
#include<cassert>
#include<concepts>
#include<cstdint>

namespace Standard::Algorithms::Numbers
{
    // 4'294'967'295 and 4'294'967'293 are not primes.
    constexpr std::uint32_t largest_prime_uint32 = 4'294'967'291;

    // 2 is a generator, but {1, 2, 4, 8, ..} looks deterministic.
    // 4294967279, 4294967282, 4294967284, 4294967286,
    // 4'294'967'287 and 4'294'967'288 are generators for the largest_prime_uint32.
    constexpr std::uint32_t some_generator = 4'294'967'279;
    static_assert(1U < some_generator && some_generator < largest_prime_uint32);

    // Generate pseudo-random numbers.
    // The prime must be an odd prime number.
    template<std::uint32_t prime = largest_prime_uint32, std::uint32_t generator = some_generator>
    requires(1U < generator && generator < prime && prime <= largest_prime_uint32 && 1U == (prime & 1U))
    struct random_not_degenerator final
    {
        // Time O(1).
        [[nodiscard]] constexpr auto operator() () noexcept -> std::uint32_t
        {
            const auto prev_value = value;
            assert(0U < value && value < prime);

            const auto next_value = static_cast<std::uint64_t>(value) * generator % prime;
            assert(0U < next_value && value != next_value && next_value < prime);

            value = static_cast<std::uint32_t>(next_value);
            return prev_value;
        }

        // Time O(1).
        [[nodiscard]] constexpr auto operator() (const std::uint32_t mod) noexcept -> std::uint32_t
        {
            assert(0U < mod);

            auto result = this->operator() () % mod;
            return result;
        }

private:
        std::uint32_t value = prime / 2;
    };

    static_assert(sizeof(std::uint32_t) == sizeof(random_not_degenerator<>));
} // namespace Standard::Algorithms::Numbers
