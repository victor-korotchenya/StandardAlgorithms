#pragma once
#include <stdexcept>
#include <type_traits>
#include "NumberUtilities.h"

namespace Privet
{
    namespace Algorithms
    {
        namespace Numbers
        {
            // A fraction like -5/3 with overflow check.
            template<class number_t>
            class fraction final
            {
                static_assert(std::is_signed_v<number_t>);

                number_t numerator;
                number_t denominator;

            public:
                explicit fraction(number_t num = {}, number_t den = 1)
                {
                    if (num == 0)
                    {
                        numerator = 0, denominator = 1;
                        return;
                    }

                    if (den == 0)
                        throw std::runtime_error("denominator cannot be zero.");

                    const auto g = std::abs(gcd(num, den));
                    if (g > 1)
                        num /= g, den /= g;

                    if (den < 0)
                        num = -num, den = -den;

                    numerator = num;
                    denominator = den;
                }

                number_t get_numerator() const
                {
                    return numerator;
                }

                number_t get_denominator() const
                {
                    return denominator;
                }

                number_t get_value() const
                {
                    if (numerator == 0)
                        return 0;

                    if (denominator == 1)
                        return numerator;

                    throw std::runtime_error("Cannot get_value because denominator=" + std::to_string(denominator));
                }

                fraction operator-() const
                {
                    return fraction(-numerator, denominator);
                }

                fraction operator-(const fraction& ot) const
                {
                    return (*this) + (-ot);
                }

                fraction operator+(const fraction& ot) const
                {
                    const auto a = multiply(numerator, ot.denominator),
                        b = multiply(denominator, ot.numerator),
                        c = multiply(denominator, ot.denominator),
                        d = add(a, b);
                    return fraction(d, c);
                }

                fraction operator*(const fraction& ot) const
                {
                    const auto a = multiply(numerator, ot.numerator),
                        b = multiply(denominator, ot.denominator);
                    return fraction(a, b);
                }

                bool operator==(const fraction& ot) const
                {
                    return numerator == ot.numerator && denominator == ot.denominator;
                }

                bool operator<(const fraction& ot) const
                {
                    if (denominator == ot.denominator)
                        return numerator < ot.numerator;

                    const auto a = multiply(numerator, ot.denominator),
                        b = multiply(ot.numerator, denominator);
                    return a < b;
                }

            private:
                template<typename tt = number_t>
                typename std::enable_if<std::is_signed_v<tt>, number_t>::type
                    add(tt const& a, tt const& b) const
                {
                    return AddSigned<number_t>(a, b);
                }

                template<typename tt = number_t>
                typename std::enable_if<!std::is_signed_v<tt>, number_t>::type
                    add(tt const& a, tt const& b) const
                {
                    return AddUnsigned<number_t>(a, b);
                }

                template<typename tt = number_t>
                typename std::enable_if<std::is_signed_v<tt>, number_t>::type
                    multiply(tt const a, tt const b) const
                {
                    return MultiplySigned<tt>(a, b);
                }

                template<typename tt = number_t>
                typename std::enable_if<!std::is_signed_v<tt>, number_t>::type
                    multiply(tt const a, tt const b) const
                {
                    return MultiplyUnsigned<tt>(a, b);
                }
            };

            template<class number_t, number_t mod, class number_t2>
            class fraction_mod final
            {
                static_assert(sizeof(number_t) <= sizeof(number_t2));
                static_assert(std::is_signed_v<number_t> == std::is_signed_v<number_t2>);

                number_t value;

            public:
                explicit fraction_mod(const number_t2 val = {}) : value(val% mod)
                {
                    assert(value >= 0 && value < mod);
                }

                fraction_mod(const number_t num, const number_t den)
                {
                    assert(num >= 0 && num < mod&& den > 0 && den < mod);

                    const auto inv = modular_power<number_t2>(den, mod - 2, mod);
                    value = static_cast<number_t>(static_cast<number_t2>(num) * inv);
                    assert(value >= 0 && value < mod);
                }

                number_t get_value() const
                {
                    return value;
                }

                fraction_mod operator+(const fraction_mod& ot) const
                {
                    const auto v = value + ot.value;
                    return fraction_mod(v >= mod ? v - mod : v);
                }

                fraction_mod operator*(const fraction_mod& ot) const
                {
                    return fraction_mod(static_cast<number_t>(static_cast<number_t2>(value) * ot.value % mod));
                }

                fraction_mod operator-() const
                {
                    return fraction_mod(value ? mod - value : 0);
                }

                fraction_mod operator-(const fraction_mod& ot) const
                {
                    const auto v = mod + value - ot.value;
                    return fraction_mod(v >= mod ? v - mod : v);
                }

                bool operator==(const fraction_mod& ot) const
                {
                    return value == ot.value;
                }
            };
        }
    }
}