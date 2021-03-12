#pragma once
#include <limits>
#include <unordered_map>
#include "Arithmetic.h"
#include "../Utilities/ExceptionUtilities.h"
#include "NumberUtilities.h"
#include "eratosthenes_sieve.h"

namespace Privet::Algorithms::Numbers
{
    // Whether the value is a prime number in O(sqrt(value)) time.
    template<class int_t>
    bool is_prime_simple(const int_t& value)
    {
        assert(value > 0);
        if (value <= 3)
            return value >= 2;

        const auto is_even = !(value & 1);
        if (is_even || !(value % 3))
            return false;

        const auto root = get_sqrt(value);

        for (int_t denominator = 5; denominator <= root; denominator += 6)
            if (!(value % denominator))
                return false;

        for (int_t denominator = 7; denominator <= root; denominator += 6)
            if (!(value % denominator))
                return false;

        return true;
    }

    //Return the prime number that is equal to or greater than the "value"
    // or throws "ArithmeticException" if no such number exists.
    //If the "value" is <= 0, std::out_of_range is thrown.
    template<typename int_t>
    int_t calc_prime(const int_t& value)
    {
        RequirePositive(value, "calc_prime");

        if (value <= 3)
            return 1 == value ? 2 : value;

        const auto maxValue = int_t(std::numeric_limits<int_t>::max() - int_t(2));
        for (int_t candidate = value | 1; candidate <= maxValue; candidate += 2)
        {
            if (is_prime_simple(candidate))
                return candidate;
        }

        throw ArithmeticException("No prime number has been found.");
    }

    // Find maximum x s.t. p**x | n! where p is a prime.
    // E.g. 5**249 divides 1000!.
    template<class int_t>
    int_t factorial_prime_max_power(int_t n, const int_t prime)
    {
        if (prime < 2 || n < prime || !is_prime_simple(prime))
            return {};

        int_t x{};
        do
        {
            n /= prime;
            x += n;
            assert(x > 0);
        } while (n);
        return x;
    }
    // Slow
    template<class int_t>
    int_t factorial_prime_max_power_slow(int_t n, const int_t prime)
    {
        if (prime < 2 || n < prime || !is_prime_simple(prime))
            return{};

        int_t x{};
        do
        {
            if (n % prime)
                continue;

            auto r = n;
            do
            {
                r /= prime;
                ++x;
                assert(x > 0 && r > 0);
            } while (0 == r % prime);
        } while (--n >= prime);

        assert(x > 0);
        return x;
    }

    // Count pi(n): how many primes are in the range [2..n].

    template <class long_int_t, class int_t>
    int_t prime_number_count_simple(const int_t n)
    {
        static_assert(sizeof(int_t) <= sizeof(long_int_t));

        if (n <= 2)
            return n == 2;

        const auto prime_factors = eratosthenes_sieve<long_int_t, int_t>(n);
        const auto c = static_cast<int_t>(prime_factors.first.size());
        assert(c >= 2);
        return c;
    }

    // Power set on sqrt(n) using inclusion-exclusion is slow.
    // Example. n=20, sqrt root=4, primes in [2..root] are pr={2,3}.
    // Divisible singles: 20/2=10, int(20/3)=6.
    // Divisible doubles: int(20/2/3)=3.
    // Neither prime nor composite {1} count: 1.
    // Prime count in [2..n]: 20 - (10+6 - (3) + 1) + 2 = 22 - (14) = 8.
    // Check primes in [2..n]: |{2,3,5,7, 11,13,17,19}| = 8.
    template <class long_int_t, class int_t>
    int_t prime_number_count_slow(std::unordered_map<int_t, int_t>& m, const int_t n)
    {
        static_assert(sizeof(int_t) <= sizeof(long_int_t));
        if (const auto it = m.find(n); it != m.end())
            return it->second;

        if (n <= 4)
            return n >= 3 ? 2 : n == 2;

        m[2] = 1;
        m[3] = 2;

        const auto root = static_cast<int_t>(sqrt(n));
        const auto primes = eratosthenes_sieve<long_int_t, int_t>(root).first;
        const auto size = m[root] = static_cast<int_t>(primes.size());
        assert(size > 0);

        std::vector<int_t> buf(size);
        long_int_t count = n - long_int_t(1) + size;
        auto ad_max = n;

        for (int_t power_set_size = 1, sign = -1; power_set_size <= size; ++power_set_size)
        {
            ad_max /= primes[power_set_size - 1];
            if (!ad_max)
                break;

            assert(!buf[size - power_set_size]);
            buf[size - power_set_size] = 1;

            long_int_t sum{};
            do
            {
                auto ad = n;
                for (int_t j = 0; j < size; ++j)
                    if (buf[j])
                    {
                        ad /= primes[j];
                        if (!ad)
                            break;
                    }

                sum += ad;
            } while (std::next_permutation(buf.begin(), buf.end()));

            count += sign * sum;
            sign *= -1;
        }

        assert(count >= 3 && count < n);
        return m[n] = static_cast<int_t>(count);
    }
}