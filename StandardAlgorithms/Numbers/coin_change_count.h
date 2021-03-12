#pragma once
#include"../Utilities/require_utilities.h"
#include"../Utilities/same_sign_leq_size.h"
#include"default_modulus.h"
#include<map>
#include<set>
#include<vector>

namespace Standard::Algorithms::Numbers::Inner
{
    template<std::signed_integral int_t, int_t mod>
    constexpr void coin_change_count_slow_rec(const std::vector<int_t> &coins, std::set<std::map<int_t, int_t>> &uniq,
        std::map<int_t, int_t> &buf, const int_t &sum)
    {
        assert(!coins.empty() && 0 < sum);

        for (const auto &coin : coins)
        {
            const int_t sum2 = sum - coin;
            if (sum2 < 0)
            {
                break;
            }

            auto &temp = buf[coin];
            assert(!(temp < 0));

            ++temp;
            assert(0 < temp);

            if (sum2 == 0)
            {
                uniq.insert(buf);
            }
            else
            {
                coin_change_count_slow_rec<int_t, mod>(coins, uniq, buf, sum2);
            }

            if (--temp == 0)
            {
                buf.erase(coin);
            }

            if (sum2 == 0)
            {
                break;
            }
        }
    }
} // namespace Standard::Algorithms::Numbers::Inner

namespace Standard::Algorithms::Numbers
{
    // Count ways, ignoring order, to make change for sum>0
    // using sorted unique coins[n] where n>0, and coins[0] might be 2.
    //
    // Here sorting is not required, but the coins must be unique.
    //
    // Time O(coins.size*sum), space O(sum).
    template<std::signed_integral int_t, int_t mod = default_modulus>
    requires(2 <= mod)
    [[nodiscard]] constexpr auto coin_change_count(const std::vector<int_t> &coins, const int_t &sum) -> int_t
    {
        auto size = require_positive(static_cast<int_t>(coins.size()), "coins size");
        require_positive(coins[0], "coins[0]");
        require_positive(sum, "sum");

        while (0 < size && sum < coins[size - 1])
        {
            --size;
        }

        if (size == 0)
        {// All are too large.
            return 0;
        }

        // sum -> count
        std::vector<int_t> buf(sum + 1LL);
        buf[0] = 1;

        for (int_t index{}; index < size; ++index)
        {
            const auto &coin = coins[index];

            assert(0 < coin && (0 == index || coins[index - 1] < coin));

            const auto lim_sum = sum - coin;

            for (int_t prev_sum{}; prev_sum <= lim_sum; ++prev_sum)
            {
                const auto &prev_cnt = buf.at(prev_sum);
                assert(!(prev_cnt < 0) && prev_cnt < mod);

                const auto next_sum = prev_sum + coin;
                assert(next_sum <= sum);

                auto &cur_cnt = buf.at(next_sum);
                cur_cnt += prev_cnt;

                if (!(cur_cnt < mod))
                {
                    cur_cnt -= mod;
                }

                assert(!(cur_cnt < 0) && cur_cnt < mod);
            }
        }

        const auto &result = buf.back();
        assert(!(result < 0) && result < mod);

        return result;
    }

    // Slow time O((coins.size)**sum).
    template<std::signed_integral int_t, int_t mod = default_modulus>
    requires(2 <= mod)
    [[nodiscard]] constexpr auto coin_change_count_slow_still(const std::vector<int_t> &coins, const int_t &sum,
        // Uniqueness (ignore order) is guaranteed.
        const int_t &pos) -> int_t
    {
        auto size = require_positive(static_cast<int_t>(coins.size()), "coins size");
        require_positive(coins[0], "coins[0]");

        {
            constexpr auto max_size = 20;

            assert(size <= max_size && coins.back() <= max_size);
        }

        if (sum == 0)
        {
            return 1;
        }

        if (sum < 0 || pos < 0)
        {
            return 0;
        }

        assert(!(pos < 0) && pos < size);

        const auto exclude_pos = coin_change_count_slow_still<int_t, mod>(coins, sum, pos - 1);

        const auto include_pos = coin_change_count_slow_still<int_t, mod>(coins, sum - coins[pos], pos);

        auto result = exclude_pos + include_pos;

        if (!(result < mod))
        {
            result -= mod;
        }

        assert(!(result < 0) && result < mod);

        return result;
    }

    // Slow time O((coins.size)**sum).
    template<std::signed_integral long_int_t, std::signed_integral int_t, int_t mod = default_modulus>
    requires(same_sign_leq_size<int_t, long_int_t> && 2 <= mod)
    [[nodiscard]] constexpr auto coin_change_count_slow(const std::vector<int_t> &coins, const int_t &sum) -> int_t
    {
        const auto size = require_positive(static_cast<int_t>(coins.size()), "coins size");
        require_positive(coins[0], "coins[0]");
        require_positive(sum, "sum");


        {
            constexpr auto max_size = 10;
            constexpr auto max_back = 12;

            assert(size <= max_size && coins.back() <= max_back);
        }

        std::set<std::map<int_t, int_t>> uniq{};
        std::map<int_t, int_t> buf;

        Inner::coin_change_count_slow_rec<int_t, mod>(coins, uniq, buf, sum);

        auto result = static_cast<int_t>(static_cast<long_int_t>(uniq.size()) % mod);

        assert(!(result < 0) && result < mod);

        return result;
    }
} // namespace Standard::Algorithms::Numbers
