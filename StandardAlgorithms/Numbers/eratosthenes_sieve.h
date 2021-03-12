#pragma once
#include <utility>
#include <cmath>
#include <vector>
#include "../Utilities/ExceptionUtilities.h"

namespace Privet
{
    namespace Algorithms
    {
        namespace Numbers
        {
            // Return prime numbers <= n,
            // and min prime factors for all numbers <= n,
            // in time O(n) where n >= 2.
            //
            // Min prime factor samples: 10 -> 2, 15 -> 3, 25 -> 5, 26 -> 2.
            template <class long_int_t, class int_t>
            std::pair<std::vector<int_t>, std::vector<int_t>> eratosthenes_sieve(const int_t n)
            {
                static_assert(sizeof(int_t) <= sizeof(long_int_t));
                assert(n >= 2);
                RequireGreater(n, 1, "n");

                std::vector<int_t> primes, min_factors(n + long_int_t(1));
                if (n > 10)
                {
                    const auto estimate = (n >= 100 * 1000 ? 1.1 : 1.3) * n / std::log(static_cast<long double>(n));
                    const auto estimate_int = static_cast<long_int_t>(estimate);
                    primes.reserve(estimate_int);
                }

                min_factors[0] = 1; // A prime number == its min divisor.

                for (int_t i = 2; i <= n; ++i)
                {
                    auto& factor = min_factors[i];
                    if (!factor)
                    {
                        factor = i;
                        primes.push_back(i);
                    }

                    assert(factor >= 2 && !(i % factor));

                    for (const auto& p : primes)
                    {
                        if (p > factor)
                            // The key stop.
                            break;

                        const auto prod = static_cast<long_int_t>(i) * p;
                        if (prod > n)
                            break;

                        assert(!min_factors[prod]);
                        min_factors[prod] = p;
                    }
                }

                return { primes, min_factors };
            }

            // For every int <= n, have a min primary factor.
            // Then a number can be factorized in time O(log(n)).
            // Min prime factor samples: 10 -> 2, 15 -> 3, 25 -> 5, 26 -> 2.
            // See also decompose_into_divisors_with_powers
            template <class int_t, class pair_t>
            void decompose_eratosthenes_sieve_factoring(
                const std::vector<int_t>& min_prime_factors,
                std::vector<pair_t>& factor_powers,
                int_t a)
            {
                assert(a >= 0);
                factor_powers.clear();

                while (a >= 2)
                {
                    assert(a < static_cast<int_t>(min_prime_factors.size()));
                    const auto& f = min_prime_factors[a];
                    assert(2 <= f && f <= a);

                    factor_powers.emplace_back(f, 1);
                    a /= f;

                    while (!(a % f))
                        a /= f, ++factor_powers.back().second;
                    assert(a > 0);

                    assert(factor_powers.size() == 1 ||
                        factor_powers[factor_powers.size() - 2].first < factor_powers.back().first);
                }
            }

            // Slow time O(n*log(log(n))).
            template <class long_int_t, class int_t>
            std::vector<bool> eratosthenes_sieve_slow(const int_t n)
            {
                static_assert(sizeof(int_t) <= sizeof(long_int_t));
                assert(n >= 2);
                RequireGreater(n, 1, "n");

                std::vector<bool> flags(n + long_int_t(1), true);
                flags[0] = flags[1] = false;

                for (long_int_t i = 4; i <= n; i += 2)
                    flags[i] = false;

                for (long_int_t i = 3; i * i <= n; i += 2)
                {
                    if (!flags[i])
                        continue;

                    for (long_int_t j = i * i; j <= n; j += i)
                        flags[j] = false;
                }

                return flags;
            }
        }
    }
}