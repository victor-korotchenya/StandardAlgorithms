#pragma once
// "polynomial_all_derivatives.h"
#include"polynomial_multiplier.h"

namespace Standard::Algorithms::Numbers
{
    // Given a polynomial, compute all derivatives at a point in time O(n*log(n)).
    // E.g. polynomial = (10 + 20*x + 50*x*x + 1000*x*x*x) has these derivatives:
    // polynomial' = (20 + 100*x + 3000*x*x)
    // polynomial'' = (100 + 6000*x)
    // polynomial''' = (6000)
    // polynomial'''' = (all higher order ..) = 0
    //
    // If point = 0, then the derivatives are:
    // polynomial(0) = 10, polynomial'(0) = 20, polynomial''(0) = 100, polynomial'''(0) = 6000.
    //
    // If point = -1, the derivatives are:
    // polynomial(-1) = -960,
    // polynomial'(-1) = (20 + 100*x + 3000*x*x)(-1) = 2920,
    // polynomial''(-1) = (100 + 6000*x)(-1) = -5900,
    // polynomial'''(-1) = (6000)(-1) = 6000.
    //
    // If point = 1, the derivatives:
    // polynomial(1) = 1080,
    // polynomial'(1) = (20 + 100*x + 3000*x*x)(1) = 3120,
    // polynomial''(1) = (100 + 6000*x)(1) = 6100,
    // polynomial'''(1) = (6000)(1) = 6000.
    //
    // If point = 2:
    // polynomial(2) = (10 + 20*x + 50*x*x + 1000*x*x*x)(2) = 10 + 40 + 200 + 8000 = 8250,
    // polynomial'(2) = (20 + 100*x + 3000*x*x)(2) = 12220,
    // polynomial''(2) = (100 + 6000*x)(2) = 12100,
    // polynomial'''(2) = (6000)(2) = 6000.

    /*
    p=0!*a0 + a1*x + a2*x**2 + a3*x**3 + a4*x**4 + .. + an*x**n
    p'=1!*a1 + 2*a2*x + 3*a3*x**2 + 4*a4*x**3 + .. + n*an*x**(n-1)
    p''=2!*a2 + 3!*a3*x + 12*a4*x**2 + .. + n*(n-1)*an*x**(n-2)
    p'''=3!*a3 + 4!*a4*x + .. + n*(n-1)*(n-2)*an*x**(n-3)
    p''''=4!*a4 + 5!*a5*x + .. + n*(n-1)*(n-2)*(n-3)*an*x**(n-4)
    ..
    p[n-2 der]=(n-3)!*a(n-3) + (n-2)!*a(n-2)*x + (n-1)!/2!*a(n-1)*x**2 + n!/3!*an*x**3
    p[n-2 der]=(n-2)!*a(n-2) + (n-1)!*a(n-1)*x + n!/2!*an*x**2
    p[n-1 der]=(n-1)!*a(n-1) + n!*an*x
    p[n der]=n!*an

    f=0!*v0 + v1*x + v2*x**2 + v3*x**3 + v4*x**4 + .. + vn*x**n
    g=0!*b0 + b1*x + b2*x**2 + b3*x**3 + b4*x**4 + .. + bn*x**n

    Convolution of (f * g) =
    v0*b0
    v1*b0 + v0*b1
    v2*b0 + v1*b1 + v0*b2
    v3*b0 + v2*b1 + v1*b2 + v0*b3
    v4*b0 + v3*b1 + v2*b2 + v1*b3 + v0*b4
    ..

    Let v0=n!*an, and b0=1. Then
    v0*b0 = n!*an
    v1*1 + v0*b1 = v1 + n!*an*b1 = (n-1)!*a(n-1) + n!*an*x.
     Thus, v1 = (n-1)!*a(n-1), b1 = x.
    v2*1 + v1*b1 + v0*b2 = v2 + (n-1)!*a(n-1)*x + n!*an*b2 = (n-2)!*a(n-2) + (n-1)!*a(n-1)*x + n!/2!*an*x**2.
      Thus, v2 = (n-2)!*a(n-2), b2 = n!/2!*an*x**2 / (n!*an) = 1/2!*x**2.
    v3*1 + v2*b1 + v1*b2 + v0*b3 = v3 + (n-2)!*a(n-2)*x + (n-1)!*a(n-1)*1/2!*x**2 + n!*an*b3 =
     = (n-3)!*a(n-3) + (n-2)!*a(n-2)*x + (n-1)!/2!*a(n-1)*x**2 + n!/3!*an*x**3
     Thus, v3 = (n-3)!*a(n-3), b3 = n!/3!*an*x**3 / (n!*an) = 1/3!*x**3.
    v4*1 + v3*b1 + v2*b2 + v1*b3 + v0*b4
    ..
     Thus, vi = (n-i)!*a(n-i), bi = 1/i!*x**i, assuming x**0 == 1, and 0! == 1.
    Let f = (1, x, 1/2!*x**2, 1/3!*x**3, .. , 1/i!*x**i, .. , 1/n!*x**n).
    Let g = (n!*a(n), (n-1)!*a(n-1), (n-2)!*a(n-2), .. , (n-i)!*a(n-i), .. , 2!*a(2), 1!*a(1), 0!*a0).
    Convolution of (f * g) = (p[n der], p[n-1 der], p[n-2 der], .. , p[n-i der], .. , p[2 der] , p[1 der] , p, ..).
*/
    // Time O(n*log(n)).
    template<std::floating_point floating_t, class complex_t = std::complex<floating_t>>
    constexpr void polynomial_all_derivatives(
        const auto &polynomial, const floating_t &point, std::vector<floating_t> &derivatives)
    {
        const auto size = static_cast<std::size_t>(polynomial.size());
        derivatives.resize(size);

        constexpr floating_t zero{};
        constexpr floating_t one{ 1 };
        constexpr floating_t half_flo{ one / (one + one) };
        static_assert(zero < half_flo && 2 * half_flo == one);

        auto xxx_prod = one;
        auto factorial1 = one;

        // f = (1, x, 1/2!*x**2, 1/3!*x**3, .. , 1/i!*x**i, ..  , 1/n!*x**n)
        std::vector<floating_t> fff(size);
        // g = (n!*a(n), (n-1)!*a(n-1), (n-2)!*a(n-2), .. , (n-i)!*a(n-i), .. , 2!*a(2), 1!*a(1), 0!*a0).
        std::vector<floating_t> ggg(size);

        for (std::size_t index{}; index < size;)
        {
            fff[index] = xxx_prod / factorial1; // 1/i!*x**i

            {
                const auto rev_index = size - 1ZU - index;
                assert(rev_index < size);

                const auto &coef = polynomial[index];
                ggg[rev_index] = static_cast<floating_t>(static_cast<floating_t>(coef) * factorial1); // (n-i)!*a(n-i)
            }

            xxx_prod *= point;
            ++index;
            factorial1 *= static_cast<floating_t>(index);
        }

        // Convolution of (f * g) = (p[n der], p[n-1 der], p[n-2 der], .. , p[n-i der], .. , p[2 der] , p[1 der] , p,
        // ..).
        std::vector<complex_t> convol;

        Standard::Algorithms::Numbers::polynomial_multiplier<floating_t, floating_t, complex_t>(fff, ggg, convol);

        for (std::size_t index{}; index < size; ++index)
        {
            const auto rev_index = size - 1ZU - index;
            const auto &approx = convol.at(rev_index);
            derivatives[index] = approx.real();
        }
    }

    // Slow time O(n*n).
    template<class floating_t>
    constexpr void polynomial_all_derivatives_slow(
        const auto &polynomial, const floating_t &point, std::vector<floating_t> &derivatives)
    {
        constexpr floating_t zero{};
        constexpr floating_t one{ 1 };
        static_assert(zero < one);

        const auto size = polynomial.size();
        derivatives.assign(size, zero);

        for (std::size_t order{}; order < size; ++order)
        {
            auto &deriv = derivatives[order];
            auto xxx = one;
            auto mult = one;

            // A order-th derivative at x's degree deg produces 0 if (deg < order)
            // else prod = polynomial[deg] * mult * (xxx**(deg - order))
            // where mult = deg! / (deg - order)!
            // E.g. c*x*x + d*x*x*x + e*x*x*x*x + f*x*x*x*x*x
            // Let order = 2, the second order derivative is:
            // c*2 + d*x*6 + e*x*x*12 + f*x*x*x*20;
            // going from 12, where deg=4, to 20 with deg=5,
            // coef = (deg + one) / (deg + one - order) = (deg + 1) / (deg - 1) = 5/3;
            // check 12*5/3 = 20 is indeed 20.

            for (std::size_t deg{}, rev_order = order; deg < size; ++deg)
            {
                if (deg < order)
                {// todo(p4): use a factorial cache.
                    assert(0U < rev_order);
                    mult *= rev_order;
                    --rev_order;
                    continue;
                }

                {
                    const auto prod = static_cast<floating_t>(polynomial[deg] * xxx * mult);
                    deriv += prod;
                }

                xxx *= point;
                mult = static_cast<floating_t>(mult * (deg + one) / (deg + one - order));

                assert(zero != mult);
            }
        }
    }
} // namespace Standard::Algorithms::Numbers
