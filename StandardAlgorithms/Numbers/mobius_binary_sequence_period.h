#pragma once
#include "../Utilities/ExceptionUtilities.h"
#include "NumberUtilities.h"

namespace
{
    template<typename Number>
    bool is_full_period(const std::vector<Number> factors, const std::vector<bool>& str)
    {
        const auto n = static_cast<Number>(str.size());
        assert(n > 0);

        for (const auto& d : factors)
        {
            if (n == d)
                continue;

            const auto chucks = n / d;
            auto has = true;
            for (Number c = 1; c < chucks && has; ++c)
            {
                for (Number i = 0; i < d; ++i)
                    if (str[i] != str[c * d + i])
                    {
                        has = false;
                        break;
                    }
            }

            if (has)
                return false;
        }

        return true;
    }

    template<typename Number>
    Number enumerate_strings(const std::vector<Number>& factors, std::vector<bool>& str, const Number ind)
    {
        static_assert(std::is_signed<Number>::value);

        if (ind < 0)
        {
            const auto has = is_full_period(factors, str);
            return has;
        }

        assert(ind < static_cast<Number>(str.size()));

        str[ind] = true;
        auto result = enumerate_strings(factors, str, ind - 1);

        str[ind] = false;
        result += enumerate_strings(factors, str, ind - 1);

        return result;
    }
}

namespace Privet::Algorithms::Numbers
{
    // todo: p1. How many binary sequences (Lyndon words) of length n>0, ignoring cyclic permutations, have period exactly n?
    // The cycles "011", "101", "110" are considered equal.
    // https://matheducators.stackexchange.com/questions/2714/understandable-interesting-uses-of-m%C3%B6bius-inversion
    //
    // 2**n ==== sum(d * ans(d), d divides n).
    // Using Moebius inversion,
    // ans(n) = 1/n * sum(u(d) * 2**(n/d), d divides n).
    template<typename Number>
    Number bin_sequence_period(const Number n)
    {
        static_assert(std::is_signed<Number>::value, "Number must be signed.");
        RequirePositive<Number>(n, "n");
        assert(n < 64);

        std::vector<Number> factors;
        all_divisors<Number>(n, factors);

        const auto mobs = mobius(n);
        Number result{};
        for (const auto& d : factors)
            result += mobs[d] * (static_cast<Number>(1) << (n / d));

        assert(result > 0 && 0 == result % n);
        return result / n;
    }

    // Slow.
    template<typename Number>
    Number bin_sequence_period_slow(const Number n)
    {
        static_assert(std::is_signed<Number>::value, "Number must be signed.");
        RequirePositive<Number>(n, "n");
        assert(n < 64);

        std::vector<Number> factors;
        all_divisors<Number>(n, factors);

        std::vector<bool> str(n);
        const auto result = enumerate_strings<Number>(factors, str, n - 1);
        return result / n;
    }
}