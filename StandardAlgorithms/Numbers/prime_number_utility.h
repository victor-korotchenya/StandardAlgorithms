#pragma once
// "prime_number_utility.h"
#include"../Utilities/same_sign_leq_size.h"
#include"arithmetic.h"
#include"eratosthenes_sieve.h"
#include"number_utilities.h" // modular_power
#include"power_root.h"
#include<unordered_map>

namespace Standard::Algorithms::Numbers::Inner
{
    template<std::integral long_int_t, std::integral int_t>
    requires(same_sign_leq_size<int_t, long_int_t>)
    [[nodiscard]] constexpr auto all_combinations_ps_choose_n(const int_t &power_set_size, const auto &primes,
        std::vector<bool> &includes, const int_t &nnn) noexcept -> long_int_t
    {
        constexpr int_t zero{};

        const auto primes_size = static_cast<int_t>(primes.size());

        assert(int_t{} < power_set_size && int_t{} < primes_size &&
            static_cast<std::size_t>(primes_size) == includes.size() && !(primes_size < power_set_size));

        assert(!includes[primes_size - power_set_size]);
        includes[primes_size - power_set_size] = true;

        long_int_t sum{};

        do
        {
            auto quotient = nnn;

            for (int_t index{}; index < primes_size; ++index)
            {
                if (const auto skip = !includes[index]; skip)
                {
                    continue;
                }

                quotient /= primes[index];

                if (zero == quotient)
                {
                    break;
                }
            }

            assert(!(quotient < zero) && quotient < nnn);

            sum += quotient;
        } while (std::next_permutation(includes.begin(), includes.end()));

        assert(zero < sum);

        return sum;
    }
} // namespace Standard::Algorithms::Numbers::Inner

namespace Standard::Algorithms::Numbers
{
    // Primality testing. Whether the value is a prime number in slow time O(sqrt(value)).
    // See also factoring_utilities, eratosthenes_sieve
    template<std::integral int_t>
    [[nodiscard]] constexpr auto is_prime_simple(const int_t &value) noexcept -> bool
    {
        constexpr int_t zero{};

        assert(zero < value);

        if (!(3 < value))
        {
            return !(value < 2);
        }

        if (const auto is_divisible_by_2_or_3 = zero == (value & 1) || zero == (value % 3); is_divisible_by_2_or_3)
        {
            return false;
        }

        const auto root = int_sqrt(value);

        constexpr int_t start = 5;
        constexpr int_t subrange = 6;

        for (auto denominator = start; denominator <= root; denominator += subrange)
        {
            if (zero == (value % denominator) || zero == (value % (denominator + 2)))
            {
                return false;
            }
        }

        return true;
    }

    // Pseudoprime? Pseudo-prime?
    // In case of a bad input, return false.
    // By Fermat or Euler, let m = x**(n - 1) % n, 0 < x < n. If m!=1, then n is composite, not prime.
    // Carmichael numbers e.g. 561, 1105, 1729, .. are no exception.
    template<std::integral int_t>
    [[nodiscard]] constexpr auto is_pseudo_prime(const int_t &value, const int_t &xxx) noexcept -> bool
    {
        constexpr int_t zero{};

        if (const auto is_good_input = zero < xxx && xxx < value; !is_good_input)
        {
            return false;
        }

        constexpr int_t one{ 1 };

        const auto exponent = value - one;
        const auto &modulus = value;
        const auto mmm = modular_power<int_t, int_t, int_t>(xxx, exponent, modulus);
        auto isp = one == mmm;
        return isp;
    }

    // Slow.
    // is_prime_simple is faster.
    template<std::integral int_t>
    [[nodiscard]] constexpr auto naive_pseudo_prime(const int_t &value) noexcept -> bool
    {
        constexpr int_t one{ 1 };

        if (!(one < value))
        {
            return false;
        }

        for (int_t xxx{ 2 }; xxx < value; ++xxx)
        {
            if (!is_pseudo_prime<int_t>(value, xxx))
            {
                return false;
            }
        }

        return true;
    }

    // Slow.
    template<std::integral int_t>
    [[nodiscard]] constexpr auto ensur_prima(const int_t &prim) -> int_t
    {
        if (const auto good = Standard::Algorithms::Numbers::naive_pseudo_prime(prim); !good) [[unlikely]]
        {
            throw std::runtime_error(std::to_string(prim) + " must be a pseudo-prime number");
        }

        if (const auto robin = is_prime_simple(prim); !robin) [[unlikely]]
        {
            throw std::runtime_error(std::to_string(prim) + " must be a prime number");
        }

        return prim;
    }

    // Given 56, return {7, 3} where 28 = 7 * (2**3).
    template<std::integral int_t>
    [[nodiscard]] constexpr auto into_odd_and_shifts(int_t value) noexcept -> std::pair<int_t, int_t>
    {
        constexpr int_t zero{};
        constexpr int_t one{ 1 };
        assert(zero < one);

        int_t shifts{};

        if (!(zero < value))
        {// Bad input
            return { value, shifts };
        }

        for (;;)
        {
            assert(zero < value);

            if (const auto min_bit = value & one; zero != min_bit)
            {
                return { value, shifts };
            }

            value >>= one;
            ++shifts;
            assert(zero < shifts);
        }
    }

    // Is pseudo-prime with high probability?
    template<std::integral int_t>
    [[nodiscard]] constexpr auto miller_rabin_test(const int_t &prime_cand, auto &rnd, const int_t &max_attempts)
        -> bool
    {
        require_positive(max_attempts, "max_attempts");

        constexpr int_t zero{};
        constexpr int_t one{ 1 };
        constexpr int_t four{ 4 };
        static_assert(zero < one && one < four);

        if (prime_cand < four)
        {
            return one < prime_cand;
        }

        if (const auto min_bit = prime_cand & one; zero == min_bit)
        {// Even, at least 4 -> must be composite.
            return false;
        }

        constexpr int_t two{ 2 };
        static_assert(one < two && two < four);

        const auto upper_lim = static_cast<int_t>(prime_cand - two);
        assert(one < upper_lim && upper_lim < prime_cand);

        const auto one_less = static_cast<int_t>(prime_cand - one);
        const auto [odds, shifts] = into_odd_and_shifts<int_t>(one_less);
        assert(zero < shifts);

        for (int_t attempt{}; attempt < max_attempts; ++attempt)
        {
            const auto xxx = rnd(two, upper_lim);
            assert(one < xxx && xxx < one_less);

            const auto &exponent = odds;
            auto xxx0 = modular_power<int_t, int_t, int_t>(xxx, exponent, prime_cand);

            if (one == xxx0 || one_less == xxx0)
            {
                continue;
            }

            for (int_t shi{};;)
            {
                auto xxx1 = static_cast<int_t>(xxx0 * xxx0 % prime_cand);

                if (one == xxx1)
                {
                    if (const auto is_trivial_square_root = one_less == xxx0; is_trivial_square_root)
                    {
                        break;
                    }

                    return false; // xxx0 is a non-trivial square root: (xxx0 != 1 && xxx0 != -1 && xxx0*xxx0 == 1).
                }

                if (++shi < shifts)
                {
                    xxx0 = std::move(xxx1);
                    continue;
                }

                if constexpr (::Standard::Algorithms::is_debug)
                {
                    const auto isp = is_pseudo_prime<int_t>(prime_cand, xxx);
                    assert(!isp);
                }

                return false; // xxx is not a pseudo-prime.
            }
        }

        return true;
    }

    // Return the prime number that is equal to or greater than the start
    // or throw "arithmetic_exception" if no such number exists.
    template<std::integral int_t>
    [[nodiscard]] constexpr auto calc_prime(const int_t &start) -> int_t
    {
        constexpr int_t two = 2;

        if (constexpr int_t three = 3; !(three < start))
        {
            return start < three ? two : start;
        }

        constexpr int_t max_value = std::numeric_limits<int_t>::max() - two;
        static_assert(int_t{} < max_value);

        for (auto candidate = static_cast<int_t>(start | 1); candidate <= max_value; candidate += two)
        {
            if (is_prime_simple(candidate))
            {
                return candidate;
            }
        }

        throw arithmetic_exception("No prime number has been found.");
    }

    // Find maximum x s.t. p**x | n! where p is a given prime.
    // E.g. 5**249 divides (*1000!).
    template<std::integral int_t>
    [[nodiscard]] constexpr auto factorial_prime_max_power(
        int_t nnn, const int_t &prime, const bool check_prime = true) noexcept -> int_t
    {
        constexpr int_t zero{};
        constexpr int_t two = 2;

        if (const auto bad_input = prime < two || nnn < prime || (check_prime && !is_prime_simple(prime)); bad_input)
        {
            return zero;
        }

        int_t power{};

        do
        {
            nnn /= prime;
            power += nnn;

            assert(zero < power);
        } while (zero < nnn);

        return power;
    }

    // Slow
    template<std::integral int_t>
    [[nodiscard]] constexpr auto factorial_prime_max_power_slow(int_t nnn, const int_t &prime) -> int_t
    {
        constexpr int_t zero{};
        constexpr int_t two = 2;

        if (const auto bad_input = prime < two || nnn < prime || !is_prime_simple(prime); bad_input)
        {
            return zero;
        }

        int_t power{};

        do
        {
            if (zero < nnn % prime)
            {
                continue;
            }

            auto quot = nnn;

            do
            {
                ++power;
                quot /= prime;

                assert(zero < power && zero < quot);
            } while (zero == quot % prime);
        } while (!(--nnn < prime));

        assert(zero < power);

        return power;
    }

    // Count Euler's phi(n): how many primes are there in the range [2..n].
    template<std::integral long_int_t, std::integral int_t>
    requires(same_sign_leq_size<int_t, long_int_t>)
    [[nodiscard]] constexpr auto prime_number_count_simple(const int_t &nnn) noexcept(false) -> int_t
    {
        constexpr int_t two = 2;

        if (!(two < nnn))
        {
            return nnn == two ? 1 : 0;
        }

        const auto prime_factors = eratosthenes_sieve<long_int_t, int_t>(nnn);
        auto size = static_cast<int_t>(prime_factors.first.size());

        assert(two <= size && prime_factors.first.size() == static_cast<std::size_t>(size));

        return size;
    }

    // Power set on sqrt(n) using inclusion-exclusion is slow.
    // Example: n=17, integer square root of n is 4.
    // There is 1 neither prime nor composite: {1}.
    // 2 primes in [2..root]: {2, 3}.
    // 13 divisible singles: integer(17/2)=8, 17/3=5, in total 8+5 = 13.
    // 2 divisible doubles: (17/2/3)=2.
    // 0 divisible triples, stop iterating.
    // Prime count in [2..n]: 17 - 1 + 2 - 13 + 2 = 18 - 11 = 7.
    // Check that 7 actual primes are in [2..n]: {2, 3, 5, 7, 11,13,17}.
    //
    // Another example: n=20, integer square root of n is 4.
    // There is 1 neither prime nor composite: {1}.
    // 2 primes in [2..root]: {2, 3}.
    // 16 divisible singles: 20/2=10, 20/3=6, in total 10+6.
    // 3 divisible doubles: 20/2/3=3.
    // 0 divisible triples, stop iterating.
    // Prime count in [2..n]: 20 - 1 + 2 - 16 + 3 = 19 - 11 = 8.
    // Check that 8 actual primes are in [2..n]: {2, 3, 5, 7, 11,13,17,19}.
    //
    // A trivial example: n=2, integer square root of n is 1.
    // There is 1 neither prime nor composite: {1}.
    // 0 primes in [2..root], stop iterating.
    // Prime count in [2..n]: 2 - 1 = 1.
    // Check that 1 actual prime is in [2..n]: {2}.
    //
    // Another trivial example: n=4, integer square root of n is 2.
    // There is 1 neither prime nor composite: {1}.
    // 1 prime in [2..root]: {2}.
    // 2 divisible singles: 4/2=2.
    // 0 divisible doubles, stop iterating.
    // Prime count in [2..n]: 4 - 1 + 1 - 2 = 2.
    // Check that 2 actual primes are in [2..n]: {2, 3}.
    //
    // Troy example: n=30, integer square root of n is 5.
    // There is 1 neither prime nor composite: {1}.
    // 3 primes in [2..root]: {2, 3, 5}.
    // 31 divisible singles: 30/2=15, 30/3=10, 30/5=6, in total 15+10+6.
    // 10 divisible doubles: 30/2/3=5, 30/2/5=3, 30/3/5=2, in total 5+3+2.
    // 1 divisible triple: 30/2/3/5=1.
    // 0 divisible quads, stop iterating.
    // Prime count in [2..n]: 30 - 1 + 3 - 31 + 10 - 1 = 1 + 9 = 10.
    // Check that 10 actual primes are in [2..n]: {2, 3, 5, 7, 11,13,17,19, 23,29}.
    //
    // Quadruple example: n=53, integer square root of n is 7.
    // There is 1 neither prime nor composite: {1}.
    // 4 primes in [2..root]: {2, 3, 5, 7}.
    // 60 divisible singles: 53/2=26, 53/3=17, 53/5=10, 53/7=7, in total 26+17+10+7 = 43 + 17.
    // 22 divisible doubles: 53/2/3=8, 53/2/5=5, 53/2/7=3, 53/3/5=3, 53/3/7=2, 53/5/7=1, in total 8+5+3+3+2+1 = 13 + 9.
    // 2 divisible triples: 53/2/3/5=1, 53/2/3/7=1, 53/2/5/7=0, 53/3/5/7=0, stop iterating.
    // Prime count in [2..n]: 53 - 1 + 4 - 60 + 22 - 2 = -4 + 20 = 16.
    // Check that 16 actual primes are in [2..n]: {2, 3, 5, 7, 11,13,17,19, 23,29, 31,37,39, 41, 43, 47}.
    template<std::integral long_int_t, std::integral int_t>
    requires(same_sign_leq_size<int_t, long_int_t>)
    [[nodiscard]] constexpr auto prime_number_count_slow(std::unordered_map<int_t, int_t> &cache, const int_t &nnn)
        -> int_t
    {
        constexpr int_t zero{};
        constexpr int_t one = 1;
        constexpr int_t two = 2;

        if (const auto iter = cache.find(nnn); iter != cache.end())
        {
            assert(zero < iter->second);

            return iter->second;
        }

        if (nnn < two)
        {
            return zero;
        }

        cache[two] = one;
        cache[4] = cache[3] = two;

        if (constexpr int_t five = 5; nnn < five)
        {
            auto cnt = cache[nnn];
            assert(zero < cnt);

            return cnt;
        }

        const auto root = int_sqrt(nnn);
        const auto primes = eratosthenes_sieve<long_int_t, int_t>(root).first; // Might throw.
        const auto primes_size = cache[root] = static_cast<int_t>(primes.size());
        assert(int_t{} < primes_size);

        std::vector<bool> includes(primes_size, false);
        auto ad_max = nnn;

        auto count = static_cast<long_int_t>(static_cast<long_int_t>(nnn) - one // neither prime nor composite.
            + static_cast<long_int_t>(primes_size) // primes in [2..root]
        );

        for (int_t power_set_size = one, sign1 = -one; power_set_size <= primes_size; ++power_set_size)
        {
            ad_max /= primes.at(power_set_size - one);

            if (zero == ad_max)
            {// If multiplying the first power_set_size primes produces a greater number.
                break;
            }

            auto sum = Inner::all_combinations_ps_choose_n<long_int_t, int_t>(power_set_size, primes, includes, nnn);

            count += static_cast<long_int_t>(sign1 * sum);
            sign1 *= -one;
        }

        assert(two < count && count < nnn);

        return cache[nnn] = static_cast<int_t>(count);
    }
} // namespace Standard::Algorithms::Numbers
