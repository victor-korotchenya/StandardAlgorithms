#pragma once
#include<cstdint>
#include<memory>
#include<utility>
#include<vector>

namespace Standard::Algorithms::Numbers
{
    // Get prime numbers (primes) <= num, and their min prime factors, where num >= 2.
    //
    // Use sample:
    //
    // constexpr auto n_max = 10'007;
    // auto s_ptr = Standard::Algorithms::Numbers::eratosthenes_sieve_cache(n_max);
    // assert(s_ptr);
    // const auto &sieve_divs = *s_ptr.get();
    [[nodiscard]] auto eratosthenes_sieve_cache(
        // The num had better be a prime.
        std::int32_t num) -> std::shared_ptr<const std::pair<std::vector<std::int32_t>, std::vector<std::int32_t>>>;
} // namespace Standard::Algorithms::Numbers
