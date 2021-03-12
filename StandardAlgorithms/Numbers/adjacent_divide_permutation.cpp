#include <algorithm>
#include <cassert>
#include "adjacent_divide_permutation.h"
#include "NumberUtilities.h"
#include "bits/stdc++.h" // __builtin_popcount todo: p2. use c++20.

namespace
{
    bool can_use(const char* const allowed, const int prev, const int cur)
    {
        assert(prev > 0);
        const auto y = cur % prev == 0 && allowed[cur / prev];
        return y;
    }
}

std::vector<int> Privet::Algorithms::Numbers::adjacent_divide_permutation(const char* const allowed,
    const int n, const int mod, const bool is_debug)
{
    constexpr auto n_max = 40;
    assert(allowed && n > 0 && n <= 40 && mod > 0);

    // For debugging only.
    std::vector<std::vector<int>> ms;
    if (is_debug) ms.reserve(n + 1ll);

    const auto half = (n + 1) >> 1;

    std::vector<int> cur(1 << half | 1), prev(cur.size());
    cur[0] = 1;  // Empty mask.
    if (is_debug) ms.push_back(cur);
    if (is_debug) ms.push_back(cur);

    for (auto to = 2; to <= n; ++to)
    {// (2**n * n**2) main cycle.
        std::swap(cur, prev);
        std::fill(cur.begin(), cur.end(), 0);

        const auto max_mask = 1 << std::min(to, half);
        for (auto mask = 0; mask < max_mask; ++mask)
        {
            const auto& old = prev[mask];
            if (!old)
                continue;

            auto& val0 = cur[mask];
            sum_modulo(old, val0, mod);

            for (auto from = 1; from < to; ++from)
            {
                auto new_mask = mask;
                if (from <= n_max / 2)
                {// Avoid 2**40 TLE.
                    const auto from_mask = 1 << (from - 1);
                    const auto is_valid = can_use(allowed, from, to);
                    is_valid ? new_mask |= from_mask : new_mask &= ~from_mask;
                }

                assert(new_mask < max_mask);
                auto& val = cur[new_mask];
                sum_modulo(old, val, mod);
            }
        }

        if (is_debug)
            ms.push_back(cur);
    }

    std::vector<int> sol(n);
    const auto edge_mask = 1 << half;
    for (auto mask = 0; mask < edge_mask; ++mask)
    {
        assert(mask < edge_mask);
        const auto& old = cur[mask];
        if (!old)
            continue;

        const auto cnt = __builtin_popcount(mask);
        auto& val = sol[cnt];
        sum_modulo(old, val, mod);
    }

    return sol;
}

// slow.
std::vector<int> Privet::Algorithms::Numbers::adjacent_divide_permutation_slow(const char* const allowed,
    const int n, const int mod)
{
    assert(allowed && n > 0 && mod > 0);

    std::vector<int> ans(n), temp(n);
    std::iota(temp.begin(), temp.end(), 1);
    do
    {
        auto cost = 0;
        for (auto j = 0; j < n - 1; ++j)
        {
            const auto& prev = temp[j], & cur = temp[j + 1ll];
            assert(prev > 0);
            if (can_use(allowed, prev, cur))
                ++cost;
        }

        auto& val = ans[cost];
        if (++val >= mod)
            val -= mod;
    } while (std::next_permutation(temp.begin(), temp.end()));

    return ans;
}