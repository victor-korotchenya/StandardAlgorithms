#pragma once
#include<array>
#include<vector>
#include "Arithmetic.h"
#include "../Utilities/ExceptionUtilities.h"

namespace Privet::Algorithms::Numbers
{
    // Given an array, count subsets whose items XOR is equal to x.
    //
    // Time pseudo-polynomial O(n * max_value), space O(max_value).
    template<class int_t, int mod = 1000 * 1000 * 1000 + 7>
    int count_xor_subsets(const std::vector<int_t>& ar, const int_t x)
    {
        static_assert(std::is_unsigned_v<int_t> && mod >= 2);

        if (ar.empty())
            return !x;

        const auto& largest = *std::max_element(ar.begin(), ar.end());
        const auto max_value = xor_max(largest);

        if (max_value < x) // Cannot set the highest bit.
            return 0;

        using vt = std::vector<int>;

        std::array<vt, 2> bufs{ vt(max_value + 1llu), vt(max_value + 1llu) };
        bufs[0][0] = 1;

        auto pos = 0;

        for (const auto& v : ar)
        {
            const auto& prev = bufs[pos];
            auto& cur = bufs[pos ^ 1];

            std::copy(prev.begin(), prev.end(), cur.begin());

            for (int_t s{}; s <= max_value; ++s)
            {
                const auto& p = prev[s];

                // todo: p1. When will it run faster on the newest CPUs, if the next line in removed?
                if (!p) continue;

                const int_t y = v ^ s;
                assert(y <= max_value);

                auto& count = cur[y];
                count += p;

                if (count >= mod)
                    count -= mod;
                assert(count >= 0 && count < mod);
            }

            pos ^= 1;
        }

        const auto& result = bufs[pos][x];
        assert(result >= 0 && result < mod);
        return result;
    }

    template<class int_t, int mod = 1000 * 1000 * 1000 + 7>
    int count_xor_subsets_slow_still(const std::vector<int_t>& ar, const int_t x)
    {
        static_assert(std::is_unsigned_v<int_t> && mod >= 2);

        if (ar.empty())
            return !x;

        const auto& largest = *std::max_element(ar.begin(), ar.end());
        const auto max_value = xor_max(largest);

        if (max_value < x) // Cannot set the highest bit.
            return 0;

        using vt = std::vector<std::pair<int, bool>>;

        std::array<vt, 2> bufs{ vt(max_value + 1llu), vt(max_value + 1llu) };
        bufs[0][0] = { 1, true };

        auto pos = 0;

        for (const auto& v : ar)
        {
            const auto& prev = bufs[pos];
            auto& cur = bufs[pos ^ 1];

            std::copy(prev.begin(), prev.end(), cur.begin());

            for (int_t s{}; s <= max_value; ++s)
            {
                const auto& prev_p = prev[s];
                if (!prev_p.second)
                    continue;

                const int_t y = v ^ s;
                assert(y <= max_value);

                auto& p = cur[y];
                auto& count = p.first;

                // Note. if (prev_p.first == 0), then (count + 0) = count.
                count += prev_p.first;

                if (count >= mod)
                    count -= mod;
                p.second = true;
                assert(count >= 0 && count < mod);
            }

            pos ^= 1;
        }

        const auto& result = bufs[pos][x].first;
        assert(result >= 0 && result < mod);
        return result;
    }

    // Slow time O(2**n).
    template<class int_t, int mod = 1000 * 1000 * 1000 + 7>
    int count_xor_subsets_slow(const std::vector<int_t>& ar, const int_t x)
    {
        static_assert(std::is_unsigned_v<int_t> && mod >= 2);

        const auto size = ar.size();
        assert(size <= 20);

        constexpr uint64_t one = 1;
        const auto edge_mask = one << size;
        int result{};

        for (uint64_t mask{}; mask < edge_mask; ++mask)
        {
            int_t s{};
            for (size_t i{}; i < size; ++i)
            {
                if ((mask >> i) & 1)
                    s ^= ar[i];
            }

            if (s == x && ++result == mod)
                result = 0;
        }

        assert(result >= 0 && result < mod);
        return result;
    }
}