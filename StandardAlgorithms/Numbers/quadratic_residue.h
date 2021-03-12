#pragma once
#include"number_utilities.h" // modular_power
#include<tuple>

namespace Standard::Algorithms::Numbers::Inner
{
    // Find x, x*x = n (mod p) for an odd prime p = 4k+3.
    // Let x = n**((p+1)/4) = n**(k+1). Note p-3 = 4k.
    // x*x (mod p) = n**(2k + 2) = n**((p-3)/2 + 2) =
    // = n**((p-1)/2 + 1) = n * n**((p-1)/2) =
    // = n, by Euler's criterion.
    // If not, return -1.
    template<std::signed_integral int_t>
    [[nodiscard]] constexpr auto quick_find_root_prime4k3(const int_t &nnn, const int_t &odd_prime) -> int_t
    {
        constexpr int_t one = 1;
        constexpr int_t four = 4;

        assert(!(nnn < int_t{}) && nnn < odd_prime);
        assert(3 <= odd_prime && 3 == (odd_prime % four));

        {
            constexpr auto maxi = std::numeric_limits<int_t>::max();

            assert(odd_prime < maxi);
        }

        const auto exponent = (odd_prime + one) / four;
        const auto xxx = modular_power(nnn, exponent, odd_prime);

        const auto doe = xxx * xxx % odd_prime;

        auto resol = doe == nnn ? static_cast<int_t>(xxx) : static_cast<int_t>(-one);
        return resol;
    }

    // Split (odd_prime - 1) = odd * (2**expon), where odd = 2k+1.
    template<std::signed_integral int_t>
    [[nodiscard]] constexpr auto prepare_prime4k1(const int_t &odd_prime) -> std::tuple<int_t, int_t, int_t>
    {
        constexpr int_t zero{};
        constexpr int_t one = 1;
        constexpr int_t two = 2;
        [[maybe_unused]] constexpr int_t four = 4;

        assert(four < odd_prime && one == (odd_prime % four));

        // No need to subtract 1 from odd_prime as the result will be the same.
        const auto half = static_cast<int_t>(odd_prime / two);
        auto odd = half;
        auto expon = one;

        while (zero == (odd % two))
        {
            ++expon;
            odd /= two;

            assert(zero < odd && zero < expon);
        }

        auto non_residue = two;

        while (modular_power(non_residue, half, odd_prime) == one)
        {// Usually it takes 2 iterations.
            ++non_residue;

            assert(zero < non_residue && non_residue < odd_prime);
        }

        auto non_residue_to_odd = modular_power(non_residue, odd, odd_prime);

        return { odd, expon, non_residue_to_odd };
    }
} // namespace Standard::Algorithms::Numbers::Inner

namespace Standard::Algorithms::Numbers
{
    // Whether n, 0 < n < p, is a quadratic residue (remainder) modulo an odd prime p=2*k+1.
    // If x exists s.t. (0 < x < p), and n=x*x (mod p), then taken  everything by mod p:
    // n**((p-1)/2) = (x*x)**((p-1)/2) = x**(2*((p-1)/2)) = x**(p-1) = 1.
    // If not, then number n is called quadratic nonresidue modulo p.
    //
    // Euler's theorem. For a value v, and a prime p s.t. (0 < v < p), v**(p-1) = 1 (mod p).
    template<std::signed_integral int_t>
    [[nodiscard]] constexpr auto euler_criterion(const int_t &nnn, const int_t &odd_prime) -> bool
    {
        [[maybe_unused]] constexpr int_t zero{};
        constexpr int_t one = 1;
        constexpr int_t two = 2;

        assert(!(nnn < zero) && nnn < odd_prime && two < odd_prime && zero != (odd_prime % two));

        const auto exponent = odd_prime / two;
        const auto xxx = modular_power(nnn, exponent, odd_prime);

        auto has = one == xxx;
        return has;
    }

    // odd_prime - 1 = odd * (2**expon), where odd = 2k+1.
    template<std::signed_integral int_t>
    [[nodiscard]] constexpr auto tonelli_shanks(const int_t &nnn, const int_t &odd_prime) -> int_t
    {
        constexpr int_t zero{};
        constexpr int_t one = 1;
        constexpr int_t two = 2;
        [[maybe_unused]] constexpr int_t four = 4;

        assert(zero < nnn && nnn < odd_prime);
        assert(four < odd_prime && one == (odd_prime % four));

        // These can be cached too:
        auto [odd, expon, non_residue_to_odd] = Inner::prepare_prime4k1<int_t>(odd_prime);

        assert(zero < odd && zero < (odd % two) && odd < odd_prime && zero < expon);
        assert(zero < non_residue_to_odd && non_residue_to_odd < odd_prime);

        int_t xxx = modular_power(nnn, (odd + one) / two, odd_prime);
        int_t nod = modular_power(nnn, odd, odd_prime);

        while (nod != one)
        {
            int_t index{};
            {
                auto squared = nod;

                while (squared != one && index + one < expon)
                {
                    ++index;
                    squared = static_cast<int_t>(squared * squared % odd_prime);
                }
            }

            auto nr2 = non_residue_to_odd;
            {
                auto rest = expon - index - one;

                while (zero < rest--)
                {
                    nr2 = static_cast<int_t>(nr2 * nr2 % odd_prime);
                }
            }

            xxx = static_cast<int_t>(xxx * nr2 % odd_prime);
            non_residue_to_odd = static_cast<int_t>(nr2 * nr2 % odd_prime);
            nod = static_cast<int_t>(nod * non_residue_to_odd % odd_prime);
            expon = index;
        }

        return xxx;
    }

    template<std::signed_integral int_t>
    [[nodiscard]] constexpr auto find_quadratic_residue_modulo(const int_t &nnn, const int_t &odd_prime) -> int_t
    {
        [[maybe_unused]] constexpr int_t zero{};
        constexpr int_t one = 1;
        [[maybe_unused]] constexpr int_t two = 2;
        constexpr int_t three = 3;
        constexpr int_t four = 4;

        assert(two < odd_prime && zero != (odd_prime % two));
        assert(zero < nnn && nnn < odd_prime);

        // It can be cached:
        const auto is_p3 = three == (odd_prime % four);

        if (is_p3)
        {
            auto resid = Inner::quick_find_root_prime4k3<int_t>(nnn, odd_prime);
            return resid;
        }

        if (!euler_criterion<int_t>(nnn, odd_prime))
        {
            return static_cast<int_t>(-one);
        }

        auto residue = tonelli_shanks<int_t>(nnn, odd_prime);
        return residue;
    }

    // Given an odd prime p, and 0 <= n < p, the Legendre symbol is:
    // 0 when (0 = n).
    // 1 when n is a quadratic residue modulo p.
    // -1 otherwise.
    template<std::signed_integral int_t>
    [[nodiscard]] constexpr auto legendre_symbol(int_t nnn, const int_t &odd_prime) -> int_t
    {
        constexpr int_t zero{};
        constexpr int_t one = 1;
        [[maybe_unused]] constexpr int_t two = 2;

        assert(two < odd_prime && zero != (odd_prime % two));

        // Cleaning ..
        nnn %= odd_prime;

        if (nnn < zero)
        {
            nnn += odd_prime;
        }

        assert(!(nnn < zero) && nnn < odd_prime);

        if (zero == nnn)
        {
            return zero;
        }

        const auto is_quadraric_residue = euler_criterion<int_t>(nnn, odd_prime);

        return is_quadraric_residue ? one : static_cast<int_t>(-one);
    }

    template<std::signed_integral int_t>
    [[nodiscard]] constexpr auto jacobi_symbol(int_t nnn, int_t odd_prime) -> int_t
    {
        constexpr int_t zero{};
        constexpr int_t one = 1;
        constexpr int_t two = 2;
        constexpr int_t three = 3;
        constexpr int_t four = 4;
        constexpr int_t five = 5;
        constexpr int_t eight = 8;

        assert(two < odd_prime && zero != (odd_prime % two));
        assert(!(nnn < zero) && nnn < odd_prime);

        auto sign1 = one;

        while (zero < nnn)
        {
            while (zero == (nnn % two))
            {
                nnn /= two;

                assert(zero < nnn);

                if (const auto remainder = odd_prime % eight; remainder == three || remainder == five)
                {
                    sign1 *= -one;
                }
            }

            std::swap(nnn, odd_prime);

            if ((nnn % four) == three && (odd_prime % four) == three)
            {
                sign1 *= -one;
            }

            nnn %= odd_prime;
        }

        return odd_prime == one ? sign1 : zero;
    }
} // namespace Standard::Algorithms::Numbers
