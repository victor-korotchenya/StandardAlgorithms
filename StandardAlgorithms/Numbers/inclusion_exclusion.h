#pragma once

#include <cassert>
#include <vector>
#include "NumberUtilities.h"

namespace Privet
{
    namespace Algorithms
    {
        namespace Numbers
        {
            template<int dim, class number_t>
            void inclusion_exclusion(number_t* a, const number_t mod)
            {
                static_assert(dim > 0, "size");

                constexpr auto s0 = 2;
                for (auto i = dim / s0; i >= s0; --i)
                    for (auto j = i * s0; j < dim; j += i)
                    {
                        a[i] -= a[j];
                        if (a[i] < 0)
                            a[i] += mod;
                    }
            }

            template<class data_t>
            std::vector<std::vector<data_t>> all_subsets(const std::vector<data_t>& data)
            {
                std::vector<std::vector<data_t>> r(1);
                const auto size = static_cast<int>(data.size());

                for (auto i = 0; i < size; i++)
                {
                    const auto& datum = data[i];
                    const auto size2 = static_cast<int>(r.size());
                    for (auto j = 0; j < size2; ++j)
                    {
                        //adding {2} to {{},{1}} producing {{2},{1,2}}.
                        auto t = r[j];
                        t.resize(t.size() + 1);
                        t.back() = datum;
                        r.push_back(std::move(t));
                    }
                }

                return r;
            }

            // Count the ints, coprime to n, in the interval [1, r].
            // n, r must be small.
            // Time compexity sqrt(n + r).
            template<class int_t2, class int_t>
            int_t coprimes_in_interval(std::vector<int_t>& factors, const int_t n, const int_t r)
            {
                static_assert(sizeof(int_t) <= sizeof(int_t2));

                assert(n > 0 && r > 0);
                decompose_into_prime_divisors<int_t2, int_t>(n, factors);

                auto ans = r; // Divisable counts will be subtracted/added.
                const auto size = static_cast<int>(factors.size());
                const auto lim_mask = 1u << size;
                for (auto mask = 1u; mask < lim_mask; ++mask)
                {
                    int_t prod = 1;
                    auto par = 0;

                    for (auto i = 0; i < size; ++i)
                        if (mask & 1u << i)
                        {
                            prod *= factors[i];
                            par ^= 1;
                        }

                    // Subtract/add divisables.
                    ans += r / prod * (par ? -1 : 1);
                }

                return ans;
            }

            template<class number_t>
            number_t coprimes_in_interval_slow(const number_t n, const number_t r)
            {
                number_t expected{};
                for (number_t i = 1; i <= r; ++i)
                {
                    const auto g = gcd(n, i);
                    expected += g == 1;
                }

                return expected;
            }
        }
    }
}