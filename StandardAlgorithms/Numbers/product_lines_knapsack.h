#pragma once
#include <vector>
#include "../Utilities/ExceptionUtilities.h"

namespace
{
    template<class int_t, class int_t2>
    int_t2 product_lines_knapsack_quick_check(const std::vector<std::vector<int_t>>& line_prices, const int_t2 max_total_price, std::vector<int_t>& chosen_prices)
    {
        static_assert(sizeof(int_t) <= sizeof(int_t2) && 2 <= sizeof(int_t2) && std::is_signed_v<int_t2>);

        const auto size = line_prices.size();
        RequirePositive(size, "line_prices.size");
        chosen_prices.resize(size);

        RequirePositive(max_total_price, "max_total_price");

        constexpr int_t2 upper_total_price = 1000;
        static_assert(upper_total_price == 1000);
        RequireNotGreater(max_total_price, upper_total_price, "max_total_price");

        int_t2 min_sum{}, max_sum{};
        for (auto i = 0u; i < size; ++i)
        {
            const auto& prices = line_prices[i];
            const auto name = "line_prices[" + std::to_string(i) + "]";
            RequirePositive(prices.size(), name);

            const auto mi_ma = std::minmax_element(prices.begin(), prices.end());
            const auto& mi = *mi_ma.first, & ma = *mi_ma.second;
            constexpr int_t upper_price = 100;

            assert(0 < mi && mi <= ma && ma <= upper_price);
            min_sum += mi, max_sum += ma;
        }

        if (max_total_price < min_sum)
            return int_t2(-1); // No solution.

        const auto is_min = max_total_price == min_sum;
        if (!is_min && max_total_price != max_sum)
            // There is a solution greater than sum(mins).
            return {};

        // Lucky case.
        for (auto i = 0u; i < size; ++i)
        {
            const auto& prices = line_prices[i];

            auto b = prices.begin();
            auto e = prices.end();
            const auto val = is_min ? std::min_element(b, e) : std::max_element(b, e);

            chosen_prices[i] = *val;
        }

        return max_total_price;
    }
}

namespace Privet
{
    namespace Algorithms
    {
        namespace Numbers
        {
            // Given n <= 20 product lines,
            // each having 1..20 distinct positive price <= 100,
            // select 1 price from each line to
            // make a knapsack of maximum total price <= TP, where 0 < TP <= 1000.
            // Time O(?), space O(?).
            template<class int_t, class int_t2>
            std::pair<int_t2, std::vector<int_t>> product_lines_knapsack(const std::vector<std::vector<int_t>>& line_prices, const int_t2 max_total_price)
            {
                static_assert(sizeof(int_t) <= sizeof(int_t2) && 2 <= sizeof(int_t2) && std::is_signed_v<int_t2>);

                const auto size = line_prices.size();
                RequirePositive(size, "line_prices.size");
                RequirePositive(line_prices[0].size(), "line_prices[0].size");

                constexpr int_t2 upper_total_price = 1000;
                RequireNotGreater(max_total_price, upper_total_price, "max_total_price");
                RequirePositive(max_total_price, "max_total_price");

                std::vector<int_t> chosen_prices(size);
                //{
                //    int_t2 min_sum{};
                //    for (auto i = 0u; i < size; ++i)
                //    {
                //        const auto &prices = line_prices[i];
                //        RequirePositive(prices.size(), "line_prices[" + std::to_string(i) + "]");
                //
                //        const auto &mi = *std::min_element(prices.begin(), prices.end());
                //        chosen_prices[i] = mi;
                //        min_sum += mi;
                //
                //        constexpr int_t upper_price = 100;
                //        assert(0 < min_sum && 0 < mi && mi <= upper_price);
                //    }
                //
                //    if (max_total_price < min_sum)
                //        // No solution.
                //        return { int_t2(-1), {} };
                //
                //    if (max_total_price == min_sum)
                //        return { max_total_price, chosen_prices };
                //}

                std::vector<std::vector<bool>> buf(size,
                    std::vector<bool>(max_total_price + 1));

                {
                    auto has = false;
                    for (const auto& price : line_prices[0])
                    {
                        auto& cur = buf[0];
                        assert(price > 0);
                        if (price <= max_total_price)
                            cur[price] = has = true;
                    }

                    if (!has)
                        return { int_t2(-1),{} };
                }

                for (int_t i = 1; i < size; ++i)
                {
                    const auto& prices = line_prices[i];
                    assert(prices.size());

                    const auto& prev = buf[i - 1];
                    auto& cur = buf[i];
                    auto has = false;

                    for (int_t2 old_sum = 1; old_sum < max_total_price; ++old_sum)
                    {
                        if (!prev[old_sum])
                            continue;

                        for (const auto& price : prices)
                        {
                            const auto cand = static_cast<int_t2>(old_sum + price);
                            assert(price > 0 && cand > 0);
                            if (cand <= max_total_price)
                                cur[cand] = has = true;
                        }
                    }

                    if (!has)
                        return { int_t2(-1), {} };
                }

                auto left = max_total_price;
                {
                    auto& cur = buf.back();
                    assert(!cur[0] && !buf[0][0]);
                    cur[0] = buf[0][0] = true; // fool proof.

                    while (!cur[left])
                    {
                        assert(left > 0);
                        --left;
                    }
                }

                const auto result = left;

                for (auto i = size - 1u;;)
                {
                    assert(left > 0 && left <= max_total_price && buf[i][left]);

                    const auto& prices = line_prices[i];
                    int_t choice;
                    if (i)
                    {
                        const auto& prev = buf[i - 1];
                        choice = 0;

                        for (const auto& price : prices)
                        {
                            assert(price > 0);
                            const auto cand = static_cast<int_t2>(left - static_cast<int_t2>(price));
                            if (cand > 0 && prev[cand])
                            {
                                assert(cand > 0);
                                choice = price;
                                break;
                            }
                        }
                    }
                    else
                    {
                        assert(left <= std::numeric_limits<int_t>::max());
                        choice = static_cast<int_t>(left);
                    }

                    assert(choice > 0);
                    chosen_prices[i] = choice;
                    left -= choice;
                    if (!left)
                        return { result, chosen_prices };

                    assert(left > 0 && i > 0 && i < size);
                    --i;
                }
            }

            template<class int_t, class int_t2>
            std::pair<int_t2, std::vector<int_t>> product_lines_knapsack_slow(const std::vector<std::vector<int_t>>& line_prices, const int_t2 max_total_price)
            {
                static_assert(sizeof(int_t) <= sizeof(int_t2) && 2 <= sizeof(int_t2) && std::is_signed_v<int_t2>);

                const auto size = line_prices.size();
                std::vector<int_t> chosen_prices(size);
                {
                    const auto test = product_lines_knapsack_quick_check<int_t, int_t2>(line_prices, max_total_price, chosen_prices);
                    if (test < 0)
                        // No solution.
                        return { int_t2(-1), {} };

                    if (test)
                        return { test, chosen_prices };
                }
                assert(size);

                // Store which price were chosen.
                std::vector<std::vector<int_t>> buf(size,
                    std::vector<int_t>(max_total_price + 1));

                for (const auto& price : line_prices[0])
                {
                    assert(price > 0);
                    auto& cur = buf[0];

                    if (price <= max_total_price)
                        cur[price] = price;
                }

                for (int_t i = 1; i < size; ++i)
                {
                    const auto& prices = line_prices[i];
                    const auto& prev = buf[i - 1];
                    auto& cur = buf[i];

                    for (int_t2 old_sum = 1; old_sum < max_total_price; ++old_sum)
                    {
                        if (!prev[old_sum])
                            continue;

                        for (const auto& price : prices)
                        {
                            const auto cand = static_cast<int_t2>(old_sum + price);
                            if (cand <= max_total_price)
                                cur[cand] = price;
                        }
                    }
                }

                // Restore.
                auto left = max_total_price;
                {
                    auto& cur = buf.back();
                    assert(!cur[0]);
                    cur[0] = 1;

                    while (!cur[left])
                    {
                        assert(left > 0);
                        --left;
                    }
                }

                assert(left > 0 && left <= max_total_price);
                const auto result = left;

                for (auto pos = size - 1u;;)
                {
                    assert(pos < size);
                    const auto& cur = buf[pos];
                    const auto& choice = cur[left];
                    assert(left > 0 && choice > 0);

                    chosen_prices[pos] = choice;
                    left -= choice;
                    if (!left)
                        return { result, chosen_prices };

                    assert(left > 0 && pos > 0);
                    --pos;
                }
            }
        }
    }
}