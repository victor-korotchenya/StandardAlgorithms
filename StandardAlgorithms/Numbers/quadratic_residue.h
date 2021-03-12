#pragma once
#include <cassert>
#include "NumberUtilities.h"

namespace
{
    // Whether x*x = n (mod p), when odd prime p = 4k+3.
    // Let x = n**((p+1)/4) = n**(k+1).
    // x*x (mod p) = n ** (2k + 2) = n ** ((p-3)/2 + 2)
    //  = n ** ((p-1)/2 + 1) = n * n ** ((p-1)/2)
    //  = n, by Euler's criterion.
    template<class number_t>
    int find_root_prime4k3(const number_t n, const int odd_prime)
    {
        static_assert(sizeof(int) <= sizeof(number_t));
        assert(n >= 0 && odd_prime >= 3 && 3 == (odd_prime & 3));

        const auto exponent = (odd_prime + 1) >> 2;
        const auto x = modular_power(n, exponent, odd_prime);
        const auto x2 = x * x % odd_prime;
        return x2 == n ? static_cast<int>(x) : -1;
    }

    // odd_prime - 1 = odd * (2 ** expon), where odd = 2k+1.
    template<class number_t>
    void prepare_prime4k1(number_t& non_residue_to_odd, int& odd, int& expon, const int odd_prime)
    {
        static_assert(sizeof(int) <= sizeof(number_t));
        assert(odd_prime >= 5 && 1 == (odd_prime & 3));

        odd = odd_prime >> 1, expon = 1;
        while (!(odd & 1))
        {
            ++expon;
            odd >>= 1;
        }

        assert(odd > 0 && expon > 0);

        const auto minus_1 = odd_prime ^ 1,
            minus_half = minus_1 >> 1;

        number_t non_residue = 2;
        while (modular_power(non_residue, minus_half, odd_prime) == 1)
            // Usually it takes 2 iterations.
            ++non_residue;

        assert(non_residue > 0 && non_residue < odd_prime);
        non_residue_to_odd = modular_power(non_residue, odd, odd_prime);
    }
}

namespace Privet::Algorithms::Numbers
{
    // Whether n, 0 < n < p, is a quadratic residue modulo an odd prime p.
    // If x exists s.t. n=x*x (mod p), then n**((p-1)/2) (mod p) = x**(2*(p-1)/2 = x**(p-1) = 1.
    // If not, n is called quadratic nonresidue modulo p.
    template<class number_t>
    bool euler_criterion(const number_t n, const int odd_prime)
    {
        static_assert(sizeof(int) <= sizeof(number_t));
        assert(n >= 0 && odd_prime >= 3 && odd_prime & 1);

        const auto exponent = (odd_prime ^ 1) >> 1;
        const auto one = modular_power(n, exponent, odd_prime);
        return 1 == one;
    }

    // odd_prime - 1 = odd * (2 ** expon), where odd = 2k+1.
    template<class number_t>
    int tonelli_shanks(const number_t n, number_t non_residue_to_odd,
        const int odd, int expon, const int odd_prime)
    {
        static_assert(sizeof(int) <= sizeof(number_t));
        assert(n > 0 && non_residue_to_odd > 0 && non_residue_to_odd < odd_prime&& odd & 1 && odd > 0 && odd < odd_prime&& expon > 0 && odd_prime >= 5 && 1 == (odd_prime & 3));

        number_t x = modular_power(n, (odd + 1) / 2, odd_prime),
            t = modular_power(n, odd, odd_prime);
        while (t != 1)
        {
            auto i = 0;
            {
                auto tt = t;
                while (tt != 1 && i + 1 < expon)
                {
                    ++i;
                    tt = tt * tt % odd_prime;
                }
            }

            auto nr2 = non_residue_to_odd;
            {
                auto e = expon - i - 1;
                while (e-- > 0)
                    nr2 = nr2 * nr2 % odd_prime;
            }

            x = x * nr2 % odd_prime;
            non_residue_to_odd = nr2 * nr2 % odd_prime;
            t = t * non_residue_to_odd % odd_prime;
            expon = i;
        }

        return static_cast<int>(x);
    }

    template<class number_t>
    int find_quadratic_residue_modulo(const number_t n, const int odd_prime)
    {
        static_assert(sizeof(int) <= sizeof(number_t));
        assert(odd_prime >= 3 && 1 == (odd_prime & 1));
        assert(n > 0 && n < odd_prime);

        // These can be cached:
        const auto is_p3 = 3 == (odd_prime & 3);
        int64_t non_residue_to_odd = 0;
        auto odd = 0, expon = 9;
        if (!is_p3)
            prepare_prime4k1<number_t>(non_residue_to_odd, odd, expon, odd_prime);

        // Solve.
        int r;
        if (is_p3)
            r = find_root_prime4k3<number_t>(n, odd_prime);
        else
        {
            if (!euler_criterion<number_t>(n, odd_prime))
                return -1;

            r = tonelli_shanks<number_t>(n, non_residue_to_odd, odd, expon, odd_prime);
        }
        return r;
    }

    // Given an odd prime p, and 0 <= n < p, the Legendre symbol is
    // (n)    {0 when 0==n.
    // (-) is {1 when n is a quadratic residue modulo p.
    // (p)    {-1 otherwise.
    template<class number_t>
    int legendre_symbol(number_t n, const int odd_prime)
    {
        assert(odd_prime >= 3 && (odd_prime & 1) == 1);

        // Cleaning...
        n %= odd_prime;
        if (n < 0)
            n += odd_prime;
        assert(n >= 0 && n < odd_prime);

        if (!n)
            return 0;

        const auto is_quadraric_residue = euler_criterion(n, odd_prime);
        return is_quadraric_residue ? 1 : -1;
    }

    template<class number_t>
    int jacobi_symbol(number_t n, number_t odd_prime)
    {
        assert(odd_prime >= 3 && (odd_prime & 1) == 1);
        assert(n >= 0 && n < odd_prime);

        auto t = 1;
        while (n)
        {
            while (!(n & 1))
            {
                n >>= 1;

                const auto r = odd_prime & 7;
                if (r == 3 || r == 5)
                    t *= -1;
            }

            std::swap(n, odd_prime);
            if ((n & 3) == 3 && (odd_prime & 3) == 3)
                t *= -1;

            n %= odd_prime;
        }

        return odd_prime == 1 ? t : 0;
    }
}