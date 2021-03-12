#pragma once
#include"../Utilities/check_size.h"
#include"../Utilities/require_utilities.h"
#include<cmath>
#include<vector>

namespace Standard::Algorithms::Numbers
{
    // Compute min total cost to remove n>0 positive integers,
    // where item remove cost = abs(source[index] - time),
    // initial time = 1, and then it increases by 1 after each removal or non-removal.
    // E.g. {6, 7} can be removed at 0 cost - just skip the first 5 shots.
    // {1, 1} has minimum cost 1.
    // Time O(|source|*max(|source|, max{source})). Space O(max(|source|, max{source})).
    template<std::signed_integral long_int_t, std::signed_integral int_t,
        long_int_t inf = std::numeric_limits<long_int_t>::max() / 3>
    requires(sizeof(int_t) <= sizeof(long_int_t) && long_int_t{} < inf && std::convertible_to<int_t, std::size_t> &&
        std::convertible_to<std::size_t, long_int_t>)
    constexpr auto min_time_array_abs_dif_sum_still(const std::vector<int_t> &source) -> long_int_t
    {
        constexpr long_int_t one = 1;

        const auto size = Standard::Algorithms::Utilities::check_size<long_int_t>("array size", source.size());

        assert(size == long_int_t{} || int_t{} < source.at(0));

        if (size <= one)
        {
            return {};
        }

        auto arr = source;
        std::sort(arr.begin(), arr.end());

        require_positive(arr.front(), "front");
        require_less_equal(arr.front(), arr.back(), "front vs back");

        if (const auto iter = std::adjacent_find(arr.cbegin(), arr.cend()); iter == arr.cend())
        {// All the items are positive and unique - zero cost.
            return {};
        }

        require_less_equal(arr.back(), inf, "back");

        std::vector<long_int_t> curs;
        curs.reserve(static_cast<std::size_t>(arr[0]) + 1LLU);

        std::vector<long_int_t> prevs;

        constexpr auto has_unused = false;
        if constexpr (has_unused)
        {
            curs.push_back(long_int_t{});
        }

        for (long_int_t time{}, dif = one; long_int_t{} != dif;)
        {// When (arr[0] == 4), curs will be {3, 2, 1, 0}.
            dif = static_cast<long_int_t>(std::abs(++time - arr[0]));
            assert(long_int_t{} <= dif && dif < inf);

            curs.push_back(dif);
        }

#if _DEBUG
        std::vector<std::vector<long_int_t>> all_debug;
        all_debug.reserve(size);
        all_debug.push_back(curs);
#endif

        for (auto index = one; index < size; ++index)
        {
            std::swap(prevs, curs);

            if constexpr (has_unused)
            {
                curs.assign(index + 1LL, inf);
            }
            else
            {
                curs.clear();
            }

            // const auto extr = has_unused ? long_int_t{} : index;
            const auto &extr = index;
            const auto max_time = static_cast<long_int_t>(prevs.size()) + extr;
            const auto &item = arr[index];

            auto prev_min = inf;
            auto time = static_cast<long_int_t>(index + one);
            bool has_time{};

            do
            {
                has_time = time <= max_time;

                if (has_time)
                {
                    const auto ind = time - one - extr;
                    const auto &pre = prevs.at(ind);
                    prev_min = std::min(prev_min, pre);
                }

                auto cost = static_cast<long_int_t>(prev_min + std::abs(time - item));

                assert(long_int_t{} < time && long_int_t{} <= prev_min && prev_min < inf && long_int_t{} <= cost &&
                    cost < inf);

                curs.push_back(cost);
                ++time;
            } while (has_time || !(item < time));

            while (2U <= curs.size() && curs[curs.size() - 2U] + one == curs.back())
            {// Remove the useless data e.g. {6, 7} from {5, 6, 7} because 6 and greater won't produce a smaller cost.
                curs.pop_back();
            }

#if _DEBUG
            all_debug.push_back(curs);
#endif
        }

        throw_if_empty("curs", curs);

        auto result = *std::min_element(curs.cbegin(), curs.cend());
        assert(long_int_t{} <= result && result < inf);

        return result;
    }

    // Slow time, space O(|source|*max(|source|, max{source})).
    // Note. {1E6, 1E6 + 1}, just 2 items, will require 4E6 iterations - definitely not the fastest way, but reliable.
    template<std::signed_integral long_int_t, std::signed_integral int_t,
        long_int_t inf = std::numeric_limits<long_int_t>::max() / 3>
    requires(sizeof(int_t) <= sizeof(long_int_t) && long_int_t{} < inf &&
        // Cannot be the max() value to avoid an overflow.
        inf <= std::numeric_limits<long_int_t>::max() / 2)
    constexpr auto min_time_array_abs_dif_sum_slow(const std::vector<int_t> &source) -> long_int_t
    {
        const auto size = Standard::Algorithms::Utilities::check_size<long_int_t>("array size", source.size());

        assert(size == long_int_t{} || int_t{} < source.at(0));

        if (size <= 1)
        {
            return {};
        }

        auto arr = source;
        std::sort(arr.begin(), arr.end());

        require_positive(arr.front(), "array front");
        assert(!(arr.back() < arr.front()));

        require_less_equal(arr.back(), inf, "array back");

        const auto remove_time_max = static_cast<long_int_t>(std::max(static_cast<long_int_t>(arr.back()), size) * 2);

        // Min cost to remove elements [0..i] assuming the i-th element is removed at time j,
        // and all the previous items are already removed optimally.
        std::vector<std::vector<long_int_t>> costs(size + 1LL, std::vector<long_int_t>(remove_time_max + 1LL, inf));

        costs[0][0] = {};

        for (long_int_t index{}; index < size; ++index)
        {
            for (long_int_t pre = costs[index][0], time = 1; time <= remove_time_max; ++time)
            {
                const auto cand = static_cast<long_int_t>(pre + std::abs(time - arr[index]));

                // cand can be > inf.
                assert(long_int_t{} <= cand);

                costs[index + 1][time] = std::min(cand, inf);

                const auto &old_2 = costs[index][time];
                pre = std::min(pre, old_2);
            }
        }

        auto &lasts = costs.back();

        auto min_it = std::min_element(lasts.cbegin() + size, lasts.cend());
        assert(min_it != lasts.cend());

        auto &res = *min_it;
        assert(long_int_t{} <= res && res < inf);

        return std::move(res);
    }

    // todo(p5): try all possible times, complexity (max(arr, |arr|) * 2)!.
} // namespace Standard::Algorithms::Numbers
