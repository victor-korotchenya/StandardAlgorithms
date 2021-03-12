#pragma once
#include <algorithm>
#include <array>
#include <cassert>
#include <climits>
#include <tuple>
#include <type_traits>
#include <vector>

namespace
{
    template<class int_t, class int_t2>
    void maximum_share_one_tran(const std::vector<int_t>& prices,
        size_t min_index, const size_t size, int_t2& profit, std::vector<std::pair<size_t, size_t>>& indexes)
    {
        static_assert(sizeof(int_t) <= sizeof(int_t2));
        assert(min_index + 1 < size && prices[min_index] < prices[min_index + 1]);

        profit = 0;
        indexes.resize(1);

        for (size_t i = min_index + 1; i < size; ++i)
        {
            if (prices[min_index] > prices[i])
                min_index = i;
            else
            {
                const auto cand = prices[i] - prices[min_index];
                if (profit < cand)
                {
                    profit = cand;
                    indexes[0] = { min_index, i };
                }
            }
        }

        assert(profit > 0 && indexes[0].first < indexes[0].second);
    }

    // Given {8,7,7,3,5,7,3,9,9,3,1}
    // return the  {3,5,7,3,9}.
    template<class t>
    auto skip_non_increasing_ends(const std::vector<t>& data)
    {
        size_t start{}, size = data.size();

        while (start + 1 < size && data[start + 1] <= data[start])
            ++start;

        const auto has_at_least_two = start + 1 < size;
        if (has_at_least_two)
        {
            while (data[size - 1] <= data[size - 2])
            {
                assert(size > 2);
                --size;
            }
        }

        return std::make_tuple(start, size, has_at_least_two);
    }
}

namespace Privet::Algorithms::Numbers
{
    // Given share prices over a period of n days,
    // you can either buy or sell maximum 1 share a day.
    // A transaction occurs when a share is bought on day i, and then sold on a day j > i.
    // How much can you trade if you can make up to allowed transactions?
    // Time O(n*transactions).

    template<class int_t, class int_t2>
    void maximum_share_speculation_simple(const std::vector<int_t>& prices, const size_t allowed_transactions,
        int_t2& profit, std::vector<std::pair<size_t, size_t>>& indexes)
    {
        static_assert(sizeof(int_t) <= sizeof(int_t2) && std::is_signed_v<int_t> && std::is_signed_v<int_t2>);

        profit = 0;
        indexes.clear(); // todo: p2. indexes.

        if (!allowed_transactions)
            return;

        const auto [start, size, has] = skip_non_increasing_ends(prices);
        if (!has)
            return;// Cannot make even 1 profitable transaction.

        using vt = std::vector<int_t2>;
        std::array<vt, 2> buf{ vt(size + 1), vt(size + 1) };
        auto pos = 0;

        for (size_t tran{}; tran < allowed_transactions; ++tran)
        {
            const auto& prev = buf[pos];
            auto& cur = buf[pos ^ 1];

            auto max_dif = std::numeric_limits<int_t2>::min();
            for (auto i = start; i < size - 1; ++i)
            {
                const int_t2 dif = prev[i] - static_cast<int_t2>(prices[i]);
                max_dif = std::max(max_dif, dif);

                const int_t2 cand = static_cast<int_t2>(prices[i + 1]) + max_dif;
                cur[i + 1] = std::max(cur[i], cand);
            }

            pos ^= 1;
        }

        const auto& pr = buf[pos];
        profit = *std::max_element(pr.begin(), pr.end());
    }

    // Usually it is slightly faster than the simple code above.
    template<class int_t, class int_t2>
    void maximum_share_speculation(const std::vector<int_t>& prices, const size_t allowed_transactions,
        int_t2& profit, std::vector<std::pair<size_t, size_t>>& indexes)
    {
        static_assert(sizeof(int_t) <= sizeof(int_t2) && std::is_signed_v<int_t> && std::is_signed_v<int_t2>);

        profit = 0;
        indexes.clear();

        if (!allowed_transactions)
            return;

        const auto [start, size, has] = skip_non_increasing_ends(prices);
        if (!has)
            return;// Cannot make even 1 profitable transaction.

        if (allowed_transactions == 1)
        {
            maximum_share_one_tran<int_t, int_t2>(prices, start, size, profit, indexes);
            return;
        }

        std::vector<int_t> prices2;
        prices2.reserve(size - start);

        // If there are enough transactions, it can run quickly in O(n).
        for (size_t i = start, possible_transactions{};;)
        {
            // wave up
            ++possible_transactions;
            assert(i + 1 < size && prices[i] < prices[i + 1]);

            auto j = i;
            while (j + 1 < size && prices[j] <= prices[j + 1])
                ++j;

            assert(prices[i] < prices[j] && profit >= 0);
            profit += static_cast<int_t2>(prices[j]) - prices[i];
            assert(profit > 0);

            indexes.emplace_back(i, j);
            prices2.push_back(prices[i]);
            prices2.push_back(prices[j]);

            // wave down
            i = j;
            while (i + 1 < size && prices[i] >= prices[i + 1])
                ++i;

            if (i + 1 >= size)
            {
                assert(possible_transactions && profit > 0);
                if (possible_transactions <= allowed_transactions)
                    return;

                break;
            }
        }

        maximum_share_speculation_simple<int_t, int_t2>(prices2, allowed_transactions, profit, indexes);
    }

    // Slow time O(n*n*transactions).
    template<class int_t, class int_t2>
    void maximum_share_speculation_slow(const std::vector<int_t>& prices, const size_t allowed_transactions,
        int_t2& profit, std::vector<std::pair<size_t, size_t>>& indexes)
    {
        static_assert(sizeof(int_t) <= sizeof(int_t2));
        profit = 0;
        indexes.clear();

        const auto size = prices.size();
        if (size < 2 || !allowed_transactions)
            return;

        // todo: p2 indexes.
        using vt = std::vector<int_t2>;
        std::array<vt, 2> buf{ vt(size + 1), vt(size + 1) };
        auto pos = 0;

        for (size_t tran{}; tran < allowed_transactions; ++tran)
        {
            const auto& prev = buf[pos];
            auto& cur = buf[pos ^ 1];
            auto has_changed = false;

            for (size_t to = 1; to < size; ++to)
            {
                auto& val = cur[to + 1];
                val = std::max(prev[to + 1], cur[to]);

                for (size_t from{}; from < to; ++from)
                {
                    if (prices[to] <= prices[from])
                        continue;

                    const int_t2 cand = prev[from] - static_cast<int_t2>(prices[from])
                        // The above line can be optimized to the external cycle on 'to'.
                        + static_cast<int_t2>(prices[to]);
                    if (val < cand)
                    {
                        val = cand;
                        has_changed = true;
                    }
                }
            }

            pos ^= 1;
            if (!has_changed)
                break;
        }

        const auto& pr = buf[pos];
        profit = *std::max_element(pr.begin(), pr.end());
    }
}