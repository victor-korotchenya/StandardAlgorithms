#pragma once
#include<array>
#include<cassert>
#include<vector>
#include "bernoulli.h"
#include "Permutation.h"

namespace Privet
{
    namespace Algorithms
    {
        namespace Numbers
        {
            // Faulhaber: 1 + pow(2, power) + ... + pow(n, power).
            template<class number_t2, class number_t, class fraction_t>
            number_t2 faulhaber_power_sum(const std::vector<std::vector<std::pair<fraction_t, number_t>>>& coefs,
                const number_t power, const number_t2 n)
            {
                static_assert(sizeof(number_t) <= sizeof(number_t2));
                assert(power >= 0 && power < static_cast<number_t>(coefs.size()) && n >= 0);

                const auto& co = coefs[power];
                const auto m = fraction_t(n);
                fraction_t result{}, prod(1);
                number_t2 p{};

                for (const auto& x : co)
                {
                    while (p < x.second)
                    {
                        ++p;
                        prod = prod * m;
                    }

                    result = x.first * prod + result;
                }

                return result.get_value();
            }

            // For usual (not modular) fraction, the power cannot be > 30 as an exception is thrown.
            template<class number_t2, class number_t, class multiply_t, class divide_t, class fraction_t>
            std::vector<std::pair<fraction_t, number_t>> faulhaber_coef(const multiply_t& multiply, const divide_t& divide, std::vector<fraction_t>& temp, const number_t p)
            {
                static_assert(sizeof(number_t) <= sizeof(number_t2));
                assert(p >= 0);
                assert(multiply);
                std::vector<std::pair<fraction_t, number_t>> r;

                const std::array<fraction_t, 2> signs{ fraction_t(1), -fraction_t(1) };
                const auto f = fraction_t(1, p + 1);
                for (number_t i = 0, si = 0; i <= p; ++i, si ^= 1)
                {
                    const auto b = bernoulli<fraction_t, number_t>(temp, i);
                    const auto c = binomial_choose_naive<multiply_t, divide_t, number_t2>(multiply, divide, p + 1, i);
                    const auto prod = f * b * signs[si] * fraction_t(c);
                    if (prod == fraction_t(0))
                        continue;

                    const auto p2 = p + 1 - i;
                    r.emplace_back(prod, p2);
                }

                std::reverse(r.begin(), r.end());
                return r;
            }

            template<class number_t2, class number_t, class multiply_t, class divide_t, class fraction_t>
            std::vector<std::vector<std::pair<fraction_t, number_t>>> faulhaber_coef_range(
                const multiply_t& multiply, const divide_t& divide, std::vector<fraction_t>& temp, const number_t max_power)
            {
                static_assert(sizeof(number_t) <= sizeof(number_t2));

                std::vector<std::vector<std::pair<fraction_t, number_t>>> r(max_power + 1);
                for (number_t p = 0; p <= max_power; ++p)
                    r[p] = faulhaber_coef<number_t2, number_t, multiply_t, divide_t, fraction_t>(multiply, divide, temp, p);

                return r;
            }

            template<class number_t2, class number_t, class add_t, class multiply_t>
            number_t2 power_sum_slow(const add_t& add, const multiply_t& multiply, const number_t power, const number_t2 n)
            {
                static_assert(sizeof(number_t) <= sizeof(number_t2));
                assert(add);
                assert(multiply);
                assert(power >= 0 && n >= 0);

                number_t2 result{};

                for (number_t2 i = 1; i <= n; ++i)
                {
                    const number_t2 m = add(i, 0);
                    number_t2 prod = 1;
                    number_t p{};

                    while (p++ < power)
                        prod = multiply(prod, m);

                    result = add(prod, result);
                }

                return result;
            }
        }
    }
}