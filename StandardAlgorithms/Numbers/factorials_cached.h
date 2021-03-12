#pragma once
#include <vector>
#include "NumberUtilities.h"

template<typename number_t = unsigned,
    typename long_number_t = long long unsigned,
    number_t modulus = 1000 * 1000 * 1000 + 7>
    class factorials_cached final
{
    static_assert(sizeof(number_t) <= sizeof(long_number_t),
        "long_number_t is too small vs. number_t.");

    std::vector<number_t> factorials;
    std::vector<number_t> factorial_inverses;

    void check(unsigned n) const
    {
        if (factorials.size() <= n)
        {
            assert(0);
            std::string s("Too large value(");
            s += std::to_string(n)
                + ") for factorials_cached("
                + std::to_string(factorials.size() - 1)
                + ").";
            throw std::runtime_error(s.c_str());
        }
    }

public:
    explicit factorials_cached(unsigned n)
        : factorials(RequirePositive(1
            + RequirePositive(n, "The size(n) must be positive."),
            "(1 + n) must be positive.")),
        factorial_inverses(1llu + n)
    {
        factorials[0] = 1;
        long_number_t f = 1;
        for (unsigned i = 1; i <= n; ++i)
        {
            f = (f * i) % modulus;
            factorials[i] = static_cast<number_t>(f);

            //Fermat's theorem: 1 (mod) === power(a, mod-1).
            //Divide by a: power(a, -1) === power(a, mod-2).
            //factorial_inverses[i] = static_cast<number_t>(
            //  modular_power<long_number_t>(f, modulus - 2, modulus));
        }

        factorial_inverses[n] = static_cast<number_t>(modular_power(static_cast<long_number_t>(factorials[n]), modulus - 2, modulus));

        for (int64_t i = n; i > 0; --i)
            factorial_inverses[i - 1] = static_cast<number_t>(factorial_inverses[i] * i % modulus);
    }

    // (n choose k) modulo mod.
    number_t choose_modulo(number_t n, number_t k) const
    {
        check(n);
        check(k);

        const auto a = static_cast<long_number_t>(factorials[n]) * factorial_inverses[k] % modulus,
            b = a * factorial_inverses[n - k] % modulus;
        return static_cast<number_t>(b);
    }

    number_t factorial_modulo(number_t n) const
    {
        check(n);
        return factorials[n];
    }

    number_t inverse_factorial_modulo(number_t n) const
    {
        check(n);
        return factorial_inverses[n];
    }

    size_t size() const noexcept
    {
        return factorials.size();
    }
};