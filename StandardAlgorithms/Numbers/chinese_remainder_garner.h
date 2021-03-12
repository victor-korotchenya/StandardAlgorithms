#pragma once
#include<vector>
#include"NumberUtilities.h"
#include"../Utilities/ExceptionUtilities.h"

namespace Privet::Algorithms::Numbers
{
    //Given n>0 co-prime numbers m[i] >= 2,
    //and n remainders 0 <= r[i] < m[i],
    //find x that (x = r[i]) modulo m[i]?
    //
    //Let p[k] = m[0]*m[1]*..*m[k].
    //x % p[k] = y[0] + y[1]*m[0] + .. + y[k]*p[k-1] % p[k]
    //y[k] = (r[k] - (y[0] + y[1]*m[0] + .. + y[k-1]*p[k-2])) *
    //       * mod_inv(p[k-1], m[k]).
    //
    // Example x=1 mod 2, x=4 mod 5.
    // 2 methods:
    // 1) x=1 mod 2; x = 1 + 2*y
    //    x = 4 = 1 + 2*y mod 5, 2*y = 3 mod 5, y = 9 = 4 mod 5, y = 4 + 5*t
    //    x = 1 + 2*y = 1 + 2*(4 + 5*t) = 9 + 10*t.
    // 2) mod_prod = prod(m[i], i=0..n-1) = 2*5 = 10,
    //    x = sum(r[i] * mod_prod / m[i] * modulo_reverse(mod_prod / m[i], m[i]), i=0..n-1) % mod_prod + mod_prod*t
    //    x = (1* 10/2 * modulo_reverse(10/2, 2) + 4 * 10/5 * modulo_reverse(10/5, 5)) % 10 + 10*t =
    //      = (1 * 5 * 1 + 4 * 2 * 3) % 10 + 10*t = (1 * 5 * 1 + 24) % 10 + 10*t = 9 + 10*t.
    // Example answer x = 9 + 10*t.
    template <typename number, typename number2>
    void chinese_remainder_garner(const std::vector<number>& modules,
        const std::vector<number>& remainders, number2& result)
    {
        const auto n = modules.size();
        RequirePositive(n, "modules");
        if (n != remainders.size())
            throw std::runtime_error("modules and remainders sizes must be equal.");

        for (size_t i = 0; i < n; ++i)
        {
            const auto ids = std::to_string(i);
            RequireGreater(modules[i], std::max<number>(1, remainders[i]), ids);
            RequireNonNegative(remainders[i], ids);

            for (auto j = i + 1u; j < n; ++j)
            {
                const auto g = gcd(modules[i], modules[j]);
                if (g > 1)
                {
                    const auto err = "moduli " + std::to_string(modules[i])
                        + ", " + std::to_string(modules[j])
                        + " must be coprime.";
                    throw std::runtime_error(err);
                }
            }
        }

        result = static_cast<number2>(remainders[0]);
        auto p = static_cast<number2>(1);
        for (auto i = 1u; i < n; ++i)
        {
            p *= modules[i - 1];

            const auto mi = static_cast<number2>(modules[i]);
            const auto inv = modulo_inverse<number2>(p, mi);
            auto k = static_cast<number2>(inv * (static_cast<number2>(remainders[i]) - result));

            make_positive(k, mi);
            result += k * p;
        }

#ifdef _DEBUG
        if (result < 0)
            throw std::runtime_error("Result " + std::to_string(result) + " must be nonnegative.");
        auto prod = static_cast<number2>(1);
        for (size_t i = 0; i < n; ++i)
        {
            const auto c = result % static_cast<number2>(modules[i]) - static_cast<number2>(remainders[i]);
            if (c)
                throw std::runtime_error("Result % mod != remainder at " + std::to_string(i));
            prod *= modules[i];
        }

        if (prod <= result)
            throw std::runtime_error("Result " + std::to_string(result) + " cannot exceed modules prod " + std::to_string(prod) + ".");
#endif
    }
}