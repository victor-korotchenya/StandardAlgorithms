#pragma once
#include <cassert>
#include<map>
#include <type_traits>
#include<set>
#include <vector>
#include "../Utilities/ExceptionUtilities.h"

namespace
{
    template<class int_t, int_t mod>
    void coin_change_count_slow_rec(const std::vector<int_t>& coins,
        std::set<std::map<int_t, int_t>>& uniq,
        std::map<int_t, int_t>& buf,
        const int_t sum)
    {
        static_assert(std::is_signed_v<int_t>);
        assert(coins.size() && sum > 0);

        for (const auto& c : coins)
        {
            const auto sum2 = sum - c;
            if (sum2 < 0)
                break;

            auto& v = buf[c];
            assert(v >= 0);

            ++v;
            assert(v >= 0);

            if (sum2)
                coin_change_count_slow_rec<int_t, mod>(coins, uniq, buf, sum2);
            else
                uniq.insert(buf);

            if (!--v)
                buf.erase(c);

            if (!sum2)
                break;
        }
    }
}

namespace Privet::Algorithms::Numbers
{
    // Count ways, ignoring order, to make change for sum>0
    // using sorted unique coins[n] where n>0, and coins[0] might be 2.
    //
    // Here sorting is not required, but the coins must be unique still.
    //
    // Time O(coins.size*sum), space O(sum).
    template<class int_t, int_t mod = 1000 * 1000 * 1000 + 7>
    int_t coin_change_count(const std::vector<int_t>& coins, const int_t sum)
    {
        static_assert(std::is_signed_v<int_t> && mod >= 2);

        auto size = static_cast<int_t>(coins.size());
        RequirePositive(size, "coins.size");
        RequirePositive(coins[0], "coins[0]");
        RequirePositive(sum, "sum");

        while (size && coins[size - 1] > sum)
            --size;

        if (!size) // All are too large.
            return 0;

        // sum -> count
        std::vector<int_t> buf(sum + 1);
        buf[0] = 1;

        for (int_t i = 0; i < size; ++i)
        {
            const auto& c = coins[i];
            assert(c > 0 && (!i || coins[i - 1] < c));

            const auto lim_sum = sum - c;
            for (int_t prev_sum = 0; prev_sum <= lim_sum; ++prev_sum)
            {
                const auto& prev_cnt = buf[prev_sum];

                const auto next_sum = prev_sum + c;
                assert(next_sum <= sum);

                auto& cur_cnt = buf[next_sum];
                cur_cnt += prev_cnt;

                if (cur_cnt >= mod)
                    cur_cnt -= mod;
                assert(prev_cnt >= 0 && prev_cnt < mod&& cur_cnt >= 0 && cur_cnt < mod);
            }
        }

        const auto& result = buf.back();
        assert(result >= 0 && result < mod);
        return result;
    }

    // Slow O((coins.size)**sum).
    template<class int_t, int_t mod = 1000 * 1000 * 1000 + 7>
    int_t coin_change_count_slow_still(const std::vector<int_t>& coins,
        const int_t sum,
        // Uniqueness (ignore order) is guaranteed.
        const int_t pos)
    {
        static_assert(std::is_signed_v<int_t> && mod >= 2);

        const auto size = static_cast<int_t>(coins.size());
        RequirePositive(size, "coins.size");
        RequirePositive(coins[0], "coins[0]");
        assert(size <= 20 && coins.back() <= 20);

        if (!sum)
            return 1;
        if (sum < 0 || pos < 0)
            return 0;

        assert(pos >= 0 && pos < size);

        const auto exclude_pos = coin_change_count_slow_still<int_t, mod>(coins, sum, pos - 1),
            include_pos = coin_change_count_slow_still<int_t, mod>(coins, sum - coins[pos], pos);

        auto result = exclude_pos + include_pos;
        if (result >= mod)
            result -= mod;

        assert(result >= 0 && result < mod);
        return result;
    }

    // Slow O((coins.size)**sum).
    template<class long_int_t, class int_t, int_t mod = 1000 * 1000 * 1000 + 7>
    int_t coin_change_count_slow(const std::vector<int_t>& coins, const int_t sum)
    {
        static_assert(std::is_signed_v<long_int_t> && std::is_signed_v<int_t> && sizeof(int_t) <= sizeof(long_int_t) && mod >= 2);

        const auto size = static_cast<int_t>(coins.size());
        RequirePositive(size, "coins.size");
        RequirePositive(coins[0], "coins[0]");
        RequirePositive(sum, "sum");
        assert(size <= 10 && coins.back() <= 12);

        std::set<std::map<int_t, int_t>> uniq;
        std::map<int_t, int_t> buf;

        coin_change_count_slow_rec<int_t, mod>(coins, uniq, buf, sum);

        const auto result = static_cast<int_t>(
            static_cast<long_int_t>(uniq.size()) % mod);

        assert(result >= 0 && result < mod);
        return result;
    }
}