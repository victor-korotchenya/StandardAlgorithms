#pragma once
#include"../Utilities/floating_point_type.h"
#include"../Utilities/require_utilities.h"
#include"../Utilities/same_sign_leq_size.h"
#include<cmath>
#include<vector>

namespace Standard::Algorithms::Numbers
{
    constexpr auto min_prime = 2;

    // Return prime numbers <= n,
    // and min prime factors for all numbers <= n,
    // in time O(n) where n >= 2.
    //
    // Min prime factor samples: 10 -> 2, 15 -> 3, 25 -> 5, 26 -> 2.
    template<std::integral long_int_t, std::integral int_t>
    requires(same_sign_leq_size<int_t, long_int_t> && sizeof(std::int32_t) <= sizeof(long_int_t))
    [[nodiscard]] constexpr auto eratosthenes_sieve(const int_t &num)
        -> std::pair<std::vector<int_t>, std::vector<int_t>>
    {
        require_greater(num, 1, "number");

        require_greater(std::numeric_limits<int_t>::max(), num, "number");

        std::vector<int_t> primes;

        std::vector<int_t> min_factors(num + 1LL);

        if (constexpr int_t ten = 10; ten < num)
        {
            constexpr long_int_t large = 100'000;

            const auto num_f = static_cast<Standard::Algorithms::floating_point_type>(num);
            const auto estimate = (large <= num ? 1.1 : 1.3) * num_f / std::log(num_f);

            const auto estimate_int = static_cast<long_int_t>(estimate);
            assert(long_int_t{} < estimate_int);

            primes.reserve(estimate_int);
        }

        min_factors[0] = 1; // A prime number == its min divisor.

        for (int_t index = 2; index <= num; ++index)
        {
            auto &factor = min_factors[index];

            if (int_t{} == factor)
            {
                factor = index;
                primes.push_back(index);
            }

            assert(static_cast<int_t>(2) <= factor && int_t{} == (index % factor));

            for (const auto &prim : primes)
            {
                if (factor < prim)
                {// The key stop.
                    break;
                }

                const auto prod = static_cast<long_int_t>(index) * prim;

                if (num < prod)
                {
                    break;
                }

                assert(min_factors[prod] == int_t{});

                min_factors[prod] = prim;
            }
        }

        return { std::move(primes), std::move(min_factors) };
    }

    // Every number (2 <= number <= n) has a min primary factor,
    // and can be factorized in time O(log(n)).
    // See also decompose_into_divisors_with_powers
    template<std::integral int_t, class pair_t>
    constexpr void decompose_eratosthenes_sieve_factoring(
        const std::vector<int_t> &min_prime_factors, std::vector<pair_t> &factor_powers, int_t num)
    {
        assert(int_t{} <= num);

        factor_powers.clear();

        while (static_cast<int_t>(min_prime) <= num)
        {
            assert(num < static_cast<int_t>(min_prime_factors.size()));

            const auto &facs = min_prime_factors.at(num);

            assert(static_cast<int_t>(min_prime) <= facs && facs <= num);

            factor_powers.emplace_back(facs, 1);
            num /= facs;

            while (int_t{} == (num % facs))
            {
                num /= facs;
                ++factor_powers.back().second;

                assert(int_t{} < num);
            }

            assert(int_t{} < num);

            assert(factor_powers.size() == 1 ||
                factor_powers[factor_powers.size() - 2].first < factor_powers.back().first);
        }
    }

    // Slow time O(n*log(log(n))).
    template<std::integral long_int_t, std::integral int_t>
    requires(same_sign_leq_size<int_t, long_int_t>)
    [[nodiscard]] constexpr auto eratosthenes_sieve_slow(const int_t &num) -> std::vector<bool>
    {
        require_greater(num, 1, "number");

        std::vector<bool> flags(num + 1LL, true);

        flags[0] = flags[1] = false;

        for (long_int_t index = 4; index <= num; index += 2)
        {
            flags[index] = false;
        }

        for (long_int_t index = 3; index * index <= num; index += 2)
        {
            if (!flags[index])
            {
                continue;
            }

            for (auto ind_2 = static_cast<long_int_t>(index * index); ind_2 <= num; ind_2 += index)
            {
                flags[ind_2] = false;
            }
        }

        return flags;
    }
} // namespace Standard::Algorithms::Numbers
