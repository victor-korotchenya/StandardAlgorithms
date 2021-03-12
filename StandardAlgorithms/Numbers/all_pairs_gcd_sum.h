#pragma once
#include <algorithm>
#include <cassert>
#include <vector>

namespace Privet
{
    namespace Algorithms
    {
        namespace Numbers
        {
            //Given a non-negative array {0, 15, 20, 25 },
            //return sum(gcd(a[i], a[j}} for all valid (i,j).
            // Assuming values are in [0, 1e6].
            template<class value2_t = int64_t, class value_t = int>
            value2_t all_pairs_gcd_sum(const std::vector<value_t>& v,
                value_t(*)(value_t, value_t),
                std::vector<value2_t>& temp)
            {
                static_assert(sizeof value_t <= sizeof value2_t);
                // todo: del unused param: value_t(*)(value_t, value_t)

                if (v.empty()) return 0;

                const auto ma = *std::max_element(v.begin(), v.end());
                temp.assign(ma + 1, value2_t());

                for (const auto& a : v)
                {
                    assert(a >= 0 && a <= 1e6);
                    ++temp[a];
                }

                assert(temp.back() > 0);

                value2_t total_sum{};
                for (value2_t i = 1; i <= ma; ++i)
                {
                    total_sum += static_cast<value2_t>(i) * temp[i];

                    auto j = i;
                    while ((j += i) <= ma)
                        temp[i] += temp[j];
                }

                const auto& zeroes = temp[0];
                auto r = zeroes * total_sum << 1;
                for (auto i = ma; i > 0; --i)
                {
                    temp[i] *= temp[i];

                    auto j = i;
                    while ((j += i) <= ma)
                        temp[i] -= temp[j];

                    assert(temp[i] >= 0);
                    r += static_cast<value2_t>(i) * temp[i];
                }

                return r;
            }

            template<class value2_t = int64_t, class value_t = int>
            value2_t all_pairs_gcd_sum_slow(const std::vector<value_t>& v,
                value_t(*g)(value_t, value_t))
            {
                static_assert(sizeof value_t <= sizeof value2_t);
                assert(g);
                value2_t r{};
                for (const auto& a : v)
                {
                    for (const auto& b : v)
                    {
                        r += g(a, b);
                    }
                }
                return r;
            }
        }
    }
}