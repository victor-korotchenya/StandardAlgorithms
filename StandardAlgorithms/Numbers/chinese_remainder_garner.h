#pragma once
#include"../Utilities/is_debug.h"
#include"gcd.h"
#include"modulo_inverse.h"
#include<vector>

namespace Standard::Algorithms::Numbers
{
    // Given n>0 co-prime numbers m[i] >= 2,
    // and n remainders, 0 <= r[i] < m[i],
    // find x that (x = r[i]) modulo m[i]?
    //
    // Let p[k] = m[0]*m[1]*..*m[k].
    // x % p[k] = y[0] + y[1]*m[0] + .. + y[k]*p[k-1] % p[k]
    // y[k] = (r[k] - (y[0] + y[1]*m[0] + .. + y[k-1]*p[k-2])) *
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
    template<std::signed_integral int_t, std::signed_integral int_t_2>
    constexpr void chinese_remainder_garner(
        const std::vector<int_t> &modules, int_t_2 &result, const std::vector<int_t> &remainders)
    {
        assert(&modules != &remainders);

        const auto size = require_positive(modules.size(), "modules size");

        require_equal(size, "modules and remainders sizes", remainders.size());

        for (std::size_t index{}; index < size; ++index)
        {
            const auto ids = std::to_string(index);

            require_non_negative(remainders[index], ids);

            constexpr int_t one = 1;

            require_greater(modules[index], std::max(one, remainders[index]), ids);

            for (auto ind_2 = index + 1U; ind_2 < size; ++ind_2)
            {
                const auto gr_div = gcd_int(modules[index], modules[ind_2]);
                if (one < gr_div) [[unlikely]]
                {
                    const auto err = "moduli " + std::to_string(modules[index]) + ", " +
                        std::to_string(modules[ind_2]) + " must be coprime.";

                    throw std::runtime_error(err);
                }
            }
        }

        result = static_cast<int_t_2>(remainders[0]);

        auto prod = static_cast<int_t_2>(1);

        for (std::size_t index = 1; index < size; ++index)
        {
            prod *= modules[index - 1U];

            const auto moi = static_cast<int_t_2>(modules[index]);
            const auto inv = modulo_inverse<int_t_2>(prod, moi);
            auto kkk = static_cast<int_t_2>(inv * (static_cast<int_t_2>(remainders[index]) - result));

            make_non_negative(kkk, moi);
            result += kkk * prod;
        }

        if constexpr (::Standard::Algorithms::is_debug)
        {
            if (result < int_t_2{}) [[unlikely]]
            {
                throw std::runtime_error("Result " + std::to_string(result) + " must be nonnegative.");
            }

            auto prod_2 = static_cast<int_t_2>(1);

            for (std::size_t index{}; index < size; ++index)
            {
                const auto diff =
                    result % static_cast<int_t_2>(modules[index]) - static_cast<int_t_2>(remainders[index]);

                if (diff != int_t_2{}) [[unlikely]]
                {
                    throw std::runtime_error("Result % mod != remainder at " + std::to_string(index));
                }

                prod_2 *= modules[index];
            }

            if (!(result < prod_2)) [[unlikely]]
            {
                throw std::runtime_error(
                    "Result " + std::to_string(result) + " cannot exceed modules prod " + std::to_string(prod_2) + ".");
            }
        }
    }
} // namespace Standard::Algorithms::Numbers
