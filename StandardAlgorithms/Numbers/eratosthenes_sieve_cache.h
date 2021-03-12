#pragma once
#include <memory>
#include <utility>
#include <vector>

namespace Privet::Algorithms::Numbers
{
    // Get prime numbers (primes) <= n,
    // and min prime factors for all numbers <= n, where n >= 2.
    //
    // Usage sample:
    //
    // constexpr auto n_max = 10007;
    // auto s_ptr = Privet::Algorithms::Numbers::eratosthenes_sieve_cache(n_max);
    // assert(s_ptr);
    // const auto &sieve_divs = *s_ptr.get();
    std::shared_ptr<std::pair<std::vector<int>, std::vector<int>>> eratosthenes_sieve_cache(
        // The n had better be a prime.
        int n);
}