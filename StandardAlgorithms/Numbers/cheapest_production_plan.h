#pragma once
#include"../Utilities/require_utilities.h"
#include"../Utilities/same_sign_leq_size.h"
#include<numeric>
#include<vector>

namespace Standard::Algorithms::Numbers::Inner
{
    template<std::integral long_int_t, std::integral int_t>
    requires(same_sign_leq_size<int_t, long_int_t>)
    [[nodiscard]] constexpr auto is_cheapest_production_plan(
        const std::vector<int_t> &sell_plan, const long_int_t &daily_max_wagons) -> bool
    {
        static constexpr int_t zero{};

        require_positive(daily_max_wagons, "daily max wagons");
        {
            constexpr int_t maxw = std::numeric_limits<int_t>::max() - 1;
            static_assert(zero < maxw);

            require_greater(maxw, daily_max_wagons, "daily max wagons");
        }

        if constexpr (std::is_signed_v<int_t>)
        {
            require_all_non_negative(sell_plan, "sell plan");
        }

        if (std::all_of(sell_plan.cbegin(), sell_plan.cend(),
                [] [[nodiscard]] (const auto &daily) noexcept
                {
                    return zero == daily;
                })) [[unlikely]]
        {
            throw std::runtime_error("The sell plan must have at least one positive value.");
        }

        auto is_cheapest = std::all_of(sell_plan.cbegin(), sell_plan.cend(),
            [&daily_max_wagons](const auto &daily) noexcept
            {
                auto within_limits = !(daily_max_wagons < daily);
                return within_limits;
            });

        return is_cheapest;
    }

    // Time O(central plan**2).
    template<std::integral long_int_t, class extra_t, std::integral int_t, long_int_t inf>
    requires(same_sign_leq_size<int_t, long_int_t> && long_int_t{} < inf)
    constexpr void cpp_optimize_day(const std::vector<int_t> &sell_plan, extra_t extra_costs,
        const long_int_t &daily_max_wagons, extra_t store_costs, const long_int_t &central_plan_sum,
        const std::size_t day, auto &optims, long_int_t &wag_sum)
    {
        constexpr long_int_t zero{};

        auto before_wagons = wag_sum;
        {
            const auto &plan = sell_plan[day];
            wag_sum += plan;

            assert(zero <= plan && plan <= wag_sum && before_wagons <= wag_sum && wag_sum < inf);
        }

        const auto &old_opts = optims[day];
        auto &opts = optims[day + 1LLU];

        for (; before_wagons <= central_plan_sum; ++before_wagons)
        {
            const auto &old_cost = old_opts[before_wagons].first;
            if (old_cost == inf)
            {
                continue;
            }

            assert(old_cost < inf);

            for (auto after_wagons = std::max(before_wagons, wag_sum); after_wagons <= central_plan_sum; ++after_wagons)
            {
                const auto today_wagons = static_cast<long_int_t>(after_wagons - before_wagons);

                const auto prod_cost =
                    daily_max_wagons < today_wagons ? static_cast<long_int_t>(extra_costs(today_wagons)) : zero;

                const auto store_cost =
                    wag_sum < after_wagons ? static_cast<long_int_t>(store_costs(after_wagons - wag_sum)) : zero;

                auto cand = static_cast<long_int_t>(old_cost + prod_cost + store_cost);
                assert(!(cand < zero)); // cand could be greater than inf.

                auto &cur = opts[after_wagons];
                if (cand < cur.first)
                {
                    cur.first = cand, cur.second = today_wagons;
                }
            }
        }
    }

    template<std::integral long_int_t, class extra_t, std::integral int_t, long_int_t inf>
    requires(same_sign_leq_size<int_t, long_int_t> && long_int_t{} < inf)
    [[nodiscard]] constexpr auto cpp_optims(const std::vector<int_t> &sell_plan, extra_t extra_costs,
        const long_int_t &daily_max_wagons, extra_t store_costs, const long_int_t &central_plan_sum)
    {
        constexpr long_int_t zero{};

        const auto days = sell_plan.size();
        assert(0U < days && zero < central_plan_sum);

        using cost_todaywag_t = std::pair<long_int_t, long_int_t>;

        std::vector<std::vector<cost_todaywag_t>> optims(days + 1LLU,
            // [0:days][produced wagons #] -> {min cost, today wagons produced }
            std::vector<cost_todaywag_t>(central_plan_sum + 1LL, cost_todaywag_t{ inf, zero }));
        optims[0][0] = {};

        long_int_t wag_sum{};

        for (std::size_t day{}; day < days; ++day)
        {
            cpp_optimize_day<long_int_t, extra_t, int_t, inf>(
                sell_plan, extra_costs, daily_max_wagons, store_costs, central_plan_sum, day, optims, wag_sum);

            assert(!(central_plan_sum < wag_sum));
        }

        assert(wag_sum == central_plan_sum);

        return optims;
    }

    template<std::integral long_int_t>
    constexpr void restore_produce_plan(
        const auto &optims, long_int_t decr_wagons, std::vector<long_int_t> &produce_plan)
    {
        constexpr long_int_t zero{};

        assert(zero < decr_wagons && 2U < optims.size());

        const auto days = optims.size() - 1LLU;

        for (auto day = days;;)
        {
            assert(0U < day);

            const auto &today_wagons = optims.at(day).at(decr_wagons).second;

            assert(!(decr_wagons < today_wagons) && !(today_wagons < zero));

            produce_plan.at(day - 1U) = today_wagons;
            decr_wagons -= today_wagons;

            if (--day == 0U)
            {
                assert(zero == decr_wagons);
                return;
            }
        }
    }
} // namespace Standard::Algorithms::Numbers::Inner

namespace Standard::Algorithms::Numbers
{
    // Given a sell plan e.g. for 3650 days,
    // and a factory producing daily up to max wagons,
    // it extra costs(w) yans to make w wagons after the max is reached,
    // and it takes store costs(w) yans to keep w assembled wagons a day,
    // all numbers are non-negative,
    // compute the cheapest production plan.
    // Note. The produced wagons cannot be less than the sell plan sum to date e.g. given a sell plan {10, 20, 40, 80},
    // the first day must have 10 or more wagons made, min 10+20 == 30 by the second day, at least 70 by 3rd.
    // And the total number of made wagons must equal the sell plan sum, 150 in the sample.
    template<std::integral long_int_t, class extra_t, std::integral int_t>
    requires(same_sign_leq_size<int_t, long_int_t> && sizeof(long_int_t) <= sizeof(std::size_t))
    [[nodiscard]] constexpr auto cheapest_production_plan(const std::vector<int_t> &sell_plan, extra_t extra_costs,
        const long_int_t &daily_max_wagons, extra_t store_costs, std::vector<long_int_t> &produce_plan) -> long_int_t
    {
        constexpr long_int_t zero{};

        throw_if_null_skip_non_pointer("extra costs function", extra_costs);
        assert(zero == extra_costs(zero));

        throw_if_null_skip_non_pointer("store costs function", store_costs);
        assert(zero == store_costs(zero));

        const auto days = require_positive(sell_plan.size(), "sell plan days");
        produce_plan.resize(days);

        if (const auto is_cheapest_possible =
                Inner::is_cheapest_production_plan<long_int_t, int_t>(sell_plan, daily_max_wagons),
            is_one_day = days == 1U;
            is_one_day || is_cheapest_possible)
        {
            std::copy(sell_plan.cbegin(), sell_plan.cend(), produce_plan.begin());

            if (is_cheapest_possible)
            {
                return {};
            }

            const auto &sell_0 = sell_plan.at(0);
            auto cost = extra_costs(sell_0);
            require_positive(cost, "day-0 extra costs");

            return cost;
        }

        // todo(p3): join 5 array scans into 1?
        const auto central_plan_sum = std::accumulate(sell_plan.cbegin(), sell_plan.cend(), zero);
        require_positive(central_plan_sum, "central sell plan possibly overflown");
        require_positive(central_plan_sum + 1LL, "central sell plan + 1 possibly overflown");

        constexpr auto inf = std::numeric_limits<long_int_t>::max() / 2;
        static_assert(zero < inf);

        const auto optims = Inner::cpp_optims<long_int_t, extra_t, int_t, inf>(
            sell_plan, extra_costs, daily_max_wagons, store_costs, central_plan_sum);

        const auto &min_cost = optims.back().back().first;
        {
            const auto *const name = "computed total min cost";
            require_positive(min_cost, name);
            require_greater(inf, min_cost, name);
        }

        Inner::restore_produce_plan<long_int_t>(optims, central_plan_sum, produce_plan);

        return min_cost;
    }
} // namespace Standard::Algorithms::Numbers
