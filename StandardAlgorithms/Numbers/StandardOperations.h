#pragma once

#include <algorithm>
#include "NumberUtilities.h" // gcd

namespace Privet
{
    namespace Algorithms
    {
        namespace Numbers
        {
            //There is no overflow check.
            template <typename Number>
            Number Sum_unchecked(const Number& a, const Number& b)
            {
                const auto result = Number(a + b);
                return result;
            }

            //There is no overflow check.
            template <typename Number>
            Number Subtract_unchecked(const Number& a, const Number& b)
            {
                const auto result = Number(a - b);
                return result;
            }

            template <typename Number>
            Number Min(const Number& a, const Number& b)
            {
                const auto& result = std::min(a, b);
                return result;
            }

            template <typename Number>
            Number Max(const Number& a, const Number& b)
            {
                const auto& result = std::max(a, b);
                return result;
            }

            template <typename Number>
            Number Xor(const Number& a, const Number& b)
            {
                const auto result = Number(a ^ b);
                return result;
            }

            //Greatest common divisor.
            template <typename Number>
            Number Gcd(const Number& a, const Number& b)
            {
                const auto result = gcd_unsigned(a, b);
                return result;
            }

            //Least common multiple.
            //There is no overflow check.
            template <typename Number>
            Number Lcm(const Number& a, const Number& b)
            {
                if (0 == a || 0 == b)
                {
                    return 0;
                }

                const auto g = gcd_unsigned(a, b);
                const auto result = (a / g) * b;
                return Number(result);
            }

            //There is no overflow check.
            template <typename Number, Number Modulus>
            Number SumModulo_unchecked(const Number& a, const Number& b)
            {
                const auto result = ((a % Modulus) + (b % Modulus)) % Modulus;
                return Number(result);
            }

            //There is no overflow check.
            template <typename Number, Number Modulus>
            Number SubtractModulo_unchecked(const Number& a, const Number& b)
            {
                const auto result = ((a % Modulus) - (b % Modulus) + Modulus) % Modulus;
                return Number(result);
            }

            //Multiply modulo.
            //There is no overflow check.
            template <typename Number, Number Modulus>
            Number ProductModulo_unchecked(const Number& a, const Number& b)
            {
                const auto result = ((a % Modulus) * (b % Modulus)) % Modulus;
                return Number(result);
            }

            //There is no overflow check.
            // x = a / b.
            template <typename Number, Number Modulus>
            Number DivideModulo_unchecked(const Number& a, const Number& b)
            {
                static_assert(0 < Modulus, "Modulus must be positive.");

                if (0 == (a % Modulus) || 0 == (b % Modulus))
                {
                    return 0;
                }

                const auto result = ((a % Modulus)
                    * modulo_inverse<Number>(b % Modulus, Modulus)) % Modulus;
                return Number(result);
            }
        }
    }
}