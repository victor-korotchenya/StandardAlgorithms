#pragma once
#include"../Utilities/require_utilities.h"
#include<vector>

namespace Standard::Algorithms::Numbers
{
    constexpr auto prod_lin_max_size = 20U;

    constexpr std::int16_t prod_lin_up_price = 100;
    constexpr std::int16_t prod_lin_up_price_total = 1'000;

    static_assert(0 < prod_lin_up_price && prod_lin_up_price < prod_lin_up_price_total);

    [[maybe_unused]] inline constexpr auto check_prod_lin_size(const std::size_t size) -> std::size_t
    {
        require_between(1U, size, prod_lin_max_size, "prices size");
        return size;
    }

    template<std::signed_integral int_t>
    constexpr void check_prod_lin_price(const int_t &price)
    {
        constexpr int_t one = 1;

        require_between(one, price, prod_lin_up_price, "price");
    }

    template<std::signed_integral int_t>
    constexpr void check_prod_lin_prices(const std::vector<int_t> &prices)
    {
        check_prod_lin_size(prices.size());

        for (const auto &price : prices)
        {
            check_prod_lin_price<int_t>(price);
        }

        require_unique(prices, "prices");
    }

    template<std::signed_integral int_t>
    constexpr void check_prod_lin_prices_many(const std::vector<std::vector<int_t>> &line_prices)
    {
        check_prod_lin_size(line_prices.size());

        for (const auto &prices : line_prices)
        {
            check_prod_lin_prices<int_t>(prices);
        }
    }

    template<std::signed_integral long_int_t>
    requires(sizeof(std::int16_t) <= sizeof(long_int_t))
    constexpr void check_prod_lin_price_total(const long_int_t &max_total_price)
    {
        constexpr long_int_t one = 1;

        require_between(one, max_total_price, prod_lin_up_price_total, "max total price");
    }
} // namespace Standard::Algorithms::Numbers

namespace Standard::Algorithms::Numbers::Inner
{
    template<std::signed_integral int_t, std::signed_integral long_int_t>
    requires(sizeof(int_t) <= sizeof(long_int_t) && sizeof(std::int16_t) <= sizeof(long_int_t))
    constexpr auto product_lines_knapsack_quick_check_slow(const std::vector<std::vector<int_t>> &line_prices,
        const long_int_t &max_total_price, std::vector<int_t> &chosen_prices) -> long_int_t
    {
        check_prod_lin_price_total(max_total_price);
        check_prod_lin_prices_many(line_prices);

        const auto size = line_prices.size();

        long_int_t min_sum{};
        long_int_t max_sum{};

        for (std::size_t index{}; index < size; ++index)
        {
            const auto name = "line prices at " + std::to_string(index);
            const auto &prices = line_prices[index];
            require_positive(prices.size(), name + " prices");

            const auto [min_it, max_it] = std::minmax_element(prices.cbegin(), prices.cend());
            const auto &mini = *min_it;
            check_prod_lin_price(mini);

            const auto &maxi = *max_it;
            check_prod_lin_price(maxi);
            require_less_equal(mini, maxi, name + " price");

            min_sum += mini, max_sum += maxi;
            require_positive(min_sum, name + " min sum");
            require_positive(max_sum, name + " max sum");
        }

        if (const auto no_solution = max_total_price < min_sum; no_solution)
        {
            return -static_cast<long_int_t>(1);
        }

        const auto is_min = max_total_price == min_sum;

        if (!is_min && !(max_total_price == max_sum))
        {// There is a solution other than min or max sum.
            return {};
        }

        // Fast case.
        chosen_prices.resize(size);

        for (std::size_t index{}; index < size; ++index)
        {
            const auto &prices = line_prices[index];
            assert(!prices.empty());

            auto beg = prices.cbegin();
            auto centos = prices.cend();

            const auto val = is_min ? std::min_element(beg, centos) : std::max_element(beg, centos);

            chosen_prices[index] = *val;
        }

        return max_total_price;
    }

    template<std::signed_integral int_t, std::signed_integral long_int_t>
    requires(sizeof(int_t) <= sizeof(long_int_t) && sizeof(std::int16_t) <= sizeof(long_int_t))
    [[nodiscard]] constexpr auto prod_lin_kompot(const std::vector<std::vector<int_t>> &line_prices,
        const long_int_t &max_total_price, std::vector<int_t> &chosen_prices, std::vector<std::vector<bool>> &buffers)
        -> bool
    {
        constexpr long_int_t zero{};

        const auto size = line_prices.size();

        assert(0U < size && size <= prod_lin_max_size && size == chosen_prices.size() && size == buffers.size());

        {
            const auto &prices = line_prices.at(0);
            check_prod_lin_prices<int_t>(prices);

            auto has = false;

            for (const auto &price : prices)
            {
                assert(zero < price);

                if (!(max_total_price < price))
                {
                    buffers[0][price] = has = true;
                }
            }

            if (!has)
            {
                return false;
            }
        }

        for (std::size_t index = 1U; index < size; ++index)
        {
            const auto &prices = line_prices[index];
            check_prod_lin_prices<int_t>(prices);

            const auto &prev = buffers[index - 1U];
            auto &cur = buffers[index];
            auto has = false;

            for (long_int_t old_sum = 1; old_sum < max_total_price; ++old_sum)
            {
                if (!prev[old_sum])
                {
                    continue;
                }

                for (const auto &price : prices)
                {
                    const auto cand = static_cast<long_int_t>(old_sum + price);
                    assert(zero < price && !(cand < price));

                    if (!(max_total_price < cand))
                    {
                        cur[cand] = has = true;
                    }
                }
            }

            if (!has)
            {
                return false;
            }
        }

        return true;
    }

    template<std::signed_integral int_t, std::signed_integral long_int_t>
    requires(sizeof(int_t) <= sizeof(long_int_t) && sizeof(std::int16_t) <= sizeof(long_int_t))
    constexpr void prod_lin_kompot_slow(const std::vector<std::vector<int_t>> &line_prices,
        const long_int_t &max_total_price, std::vector<int_t> &chosen_prices,
        std::vector<std::vector<long_int_t>> &buffers)
    {
        const auto size = line_prices.size();
        assert(0U < size && size == chosen_prices.size() && size == buffers.size());

        constexpr long_int_t zero{};

        for (const auto &prices = line_prices.at(0); const auto &price : prices)
        {
            assert(int_t{} < price && !(prod_lin_up_price < price));

            if (!(max_total_price < price))
            {
                assert(zero == buffers[0][price]); // Unique prices.

                buffers[0][price] = price;
            }
        }

        for (std::size_t index = 1U; index < size; ++index)
        {
            const auto &prices = line_prices[index];
            const auto &prev = buffers[index - 1U];
            auto &cur = buffers[index];

            for (long_int_t old_sum = 1; old_sum < max_total_price; ++old_sum)
            {
                if (zero == prev[old_sum])
                {
                    continue;
                }

                for (const auto &price : prices)
                {
                    const auto cand = static_cast<long_int_t>(old_sum + price);
                    assert(zero < cand);

                    if (max_total_price < cand)
                    {
                        continue;
                    }

                    // Here could be several solutions - save the latest one.
                    cur[cand] = price;
                }
            }
        }
    }
} // namespace Standard::Algorithms::Numbers::Inner

namespace Standard::Algorithms::Numbers
{
    // Given 0 < n <= 20 product lines,
    // each having 1..20 distinct positive prices, price <= 100,
    // select 1 price from each line to
    // make a knapsack of maximum total price <= TP, where 0 < TP <= 1000.
    // Return sum = -1 when there is no solution.
    // Time O(n*m*max_total_price), space O(n*max_total_price).
    // todo(p4): verify: should be faster than slow due to smaller RAM use: 1 bit vs long_int_t?
    template<std::signed_integral int_t, std::signed_integral long_int_t>
    requires(sizeof(int_t) <= sizeof(long_int_t) && sizeof(std::int16_t) <= sizeof(long_int_t))
    [[nodiscard]] constexpr auto product_lines_knapsack(const std::vector<std::vector<int_t>> &line_prices,
        const long_int_t &max_total_price) -> std::pair<long_int_t, std::vector<int_t>>
    {
        check_prod_lin_price_total(max_total_price);

        const auto size = check_prod_lin_size(line_prices.size());
        std::vector<int_t> chosen_prices(size);

        std::vector<std::vector<bool>> buffers(size, std::vector<bool>(max_total_price + 1LL, false));

        if (!Inner::prod_lin_kompot<int_t, long_int_t>(line_prices, max_total_price, chosen_prices, buffers))
        {
            return { -static_cast<long_int_t>(1), {} };
        }

        // Restore
        constexpr long_int_t zero{};

        auto left = max_total_price;
        {
            auto &cur = buffers.back();
            assert(!cur[0] && !buffers[0][0]);

            cur[0] = buffers[0][0] = true; // fool proof.

            while (!cur[left])
            {
                assert(zero < left);
                --left;
            }
        }

        const auto result = require_positive(left, "final result");

        for (auto index = size - 1U;;)
        {
            assert(zero < left && left <= max_total_price && buffers.at(index).at(left));

            require_positive(left, "left");
            require_less_equal(left, max_total_price, "left");

            const auto &prices = line_prices.at(index);
            int_t choice{};

            if (0U < index)
            {
                const auto &prev = buffers[index - 1U];

                for (const auto &price : prices)
                {
                    assert(zero < price);

                    const auto cand = static_cast<long_int_t>(left - static_cast<long_int_t>(price));

                    if (zero < cand && prev.at(cand))
                    {
                        choice = price;
                        break;
                    }
                }
            }
            else
            {
                require_less_equal(left, std::numeric_limits<int_t>::max(), "left overflow");

                choice = static_cast<int_t>(left);
            }

            chosen_prices[index] = require_positive(choice, "choice");
            left -= choice;

            if (zero == left)
            {
                return { result, std::move(chosen_prices) };
            }

            assert(zero < left && 0U < index && index < size);

            require_positive(left, "left");
            require_positive(index, "index");
            --index;
        }
    }

    // Slow time O(n*m*max_total_price), space O(n*max_total_price).
    template<std::signed_integral int_t, std::signed_integral long_int_t>
    requires(sizeof(int_t) <= sizeof(long_int_t) && sizeof(std::int16_t) <= sizeof(long_int_t))
    [[nodiscard]] constexpr auto product_lines_knapsack_slow(const std::vector<std::vector<int_t>> &line_prices,
        const long_int_t &max_total_price) -> std::pair<long_int_t, std::vector<int_t>>
    {
        constexpr long_int_t zero{};

        const auto size = line_prices.size();
        std::vector<int_t> chosen_prices(size);
        {
            auto summa = Inner::product_lines_knapsack_quick_check_slow<int_t, long_int_t>(
                line_prices, max_total_price, chosen_prices);

            if (const auto no_solution = summa < zero; no_solution)
            {
                return { -static_cast<long_int_t>(1), {} };
            }

            if (zero < summa)
            {
                return { summa, std::move(chosen_prices) };
            }
        }

        assert(0U < size);

        // Which price was chosen - cook the same 2D kompot as in the standard knapsack.
        std::vector<std::vector<long_int_t>> buffers(size, std::vector<long_int_t>(max_total_price + 1LL, zero));

        Inner::prod_lin_kompot_slow<int_t, long_int_t>(line_prices, max_total_price, chosen_prices, buffers);

        // Restore.
        auto left = max_total_price;
        {
            const auto &cur = buffers.back();

            while (zero == cur[left])
            {
                require_positive(left, "left");
                --left;
            }

            assert(zero < left && left <= max_total_price);
        }

        const auto result = require_positive(left, "Slow final result");

        for (auto pos = size - 1U;;)
        {
            assert(zero < left && pos < size);

            const auto &cur = buffers.at(pos);
            const auto &choice = cur.at(left);
            require_positive(choice, "restore choice");

            chosen_prices[pos] = choice;
            left -= choice;

            if (zero == left)
            {
                return { result, std::move(chosen_prices) };
            }

            require_positive(left, "restore left");
            require_positive(pos, "restore position");
            --pos;
        }
    }
} // namespace Standard::Algorithms::Numbers
