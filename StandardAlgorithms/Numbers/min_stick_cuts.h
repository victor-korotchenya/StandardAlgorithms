#pragma once
#include"../Utilities/project_constants.h"
#include"../Utilities/require_utilities.h"
#include"../Utilities/same_sign_leq_size.h"
#include"prefix_sum.h" // calculate_substring_sum
#include<queue>
#include<vector>

namespace Standard::Algorithms::Numbers::Inner
{
    // Add 0 and stick length to the cuts to make the search easier.
    // todo(p4): can be done beforehand.
    template<std::integral int_t>
    [[nodiscard]] constexpr auto pend_zero_and_length(const std::vector<int_t> &source_cuts, const int_t &stick_length)
        -> std::vector<int_t>
    {
        const auto size = require_positive(source_cuts.size(), "source cuts size");

        require_positive(source_cuts.at(0), "first cut");

        if (!std::is_sorted(source_cuts.cbegin(), source_cuts.cend())) [[unlikely]]
        {
            throw std::invalid_argument("The cuts should have been sorted.");
        }

        require_greater(stick_length, source_cuts.back(), "stick length vs last cut");

        std::vector<int_t> cuts(size + 2U);

        std::copy(source_cuts.cbegin(), source_cuts.cend(), cuts.begin() + 1U);

        cuts.back() = stick_length;

        return cuts;
    }

    // Can be done without recursion.
    template<std::integral long_int_t, std::integral int_t, long_int_t inf>
    requires(sizeof(int_t) <= sizeof(long_int_t) && long_int_t{} < inf)
    constexpr void min_stick_cuts_rec_slow(const std::vector<int_t> &cuts, const std::size_t from_pos,
        std::vector<std::vector<long_int_t>> &buffers, const std::size_t to_incl_pos)
    {
        assert(from_pos < to_incl_pos && from_pos + 1U < to_incl_pos && to_incl_pos < cuts.size());

        const auto &from = cuts[from_pos];
        const auto &to_incl = cuts[to_incl_pos];
        const auto cut_length = static_cast<long_int_t>(to_incl - from);
        auto &mini = buffers[from_pos][to_incl_pos];

        assert(mini == inf && from < to_incl && from + static_cast<int_t>(1) < to_incl && to_incl <= cuts.back());

        for (auto mid_pos = from_pos + 1U; mid_pos < to_incl_pos; ++mid_pos)
        {
            const auto &prev = buffers[from_pos][mid_pos];
            if (prev == inf)
            {
                min_stick_cuts_rec_slow<long_int_t, int_t, inf>(cuts, from_pos, buffers, mid_pos);
            }

            if (!(prev < mini))
            {// Avoid unnecessary computations.
                continue;
            }

            const auto &next = buffers[mid_pos][to_incl_pos];
            if (next == inf)
            {
                min_stick_cuts_rec_slow<long_int_t, int_t, inf>(cuts, mid_pos, buffers, to_incl_pos);
            }

            const auto cand = static_cast<long_int_t>(prev + cut_length + next); // todo(p4): might overflow.

            assert(long_int_t{} <= std::min(prev, next) && long_int_t{} < cand);
            assert(std::max({ prev, next, cand }) < inf);

            mini = std::min(mini, cand);
        }

        assert(long_int_t{} < mini && mini < inf);
    }

    template<std::integral long_int_t, std::integral int_t, long_int_t inf>
    requires(same_sign_leq_size<int_t, long_int_t> && long_int_t{} < inf)
    [[nodiscard]] constexpr auto restore_min_stick_cuts_slow(
        const std::vector<int_t> &cuts, const std::vector<std::vector<long_int_t>> &buffers) -> std::vector<int_t>
    {
        assert(2U < cuts.size());

        const auto size = cuts.size() - 2U;

        std::vector<int_t> sequence;
        sequence.reserve(size);

        // {from_pos, to_pos} in cuts.
        std::queue<std::pair<std::size_t, std::size_t>> que;
        que.emplace(std::size_t{}, size + 1U);

        do
        {
            const auto [from_pos, to_incl_pos] = que.front();

            assert(from_pos < to_incl_pos && from_pos + 1U < to_incl_pos && to_incl_pos < cuts.size());

            que.pop();

            const auto &from = cuts[from_pos];
            const auto &to_incl = cuts[to_incl_pos];

            assert(int_t{} <= from && from < to_incl && from + static_cast<int_t>(1) < to_incl);
            assert(to_incl <= cuts.back());

            const auto diff = static_cast<long_int_t>(to_incl - from);

            const auto &cost = buffers[from_pos][to_incl_pos];
            assert(long_int_t{} < diff && long_int_t{} <= cost && cost < inf);

            [[maybe_unused]] auto has = false;

            for (auto mid_pos = from_pos + 1U; mid_pos < to_incl_pos; ++mid_pos)
            {
                const auto &prev = buffers[from_pos][mid_pos];
                const auto &next = buffers[mid_pos][to_incl_pos];

                const auto cand = static_cast<long_int_t>(prev + next + diff);

                assert(long_int_t{} <= std::min(prev, next) && long_int_t{} < cand &&
                    std::max({ prev, next, cand }) < inf);

                if (cand != cost)
                {
                    continue;
                }

                if constexpr (::Standard::Algorithms::is_debug)
                {
                    has = true;
                }

                sequence.push_back(cuts[mid_pos]);

                if (from_pos + 1U < mid_pos)
                {
                    que.emplace(from_pos, mid_pos);
                }

                if (mid_pos + 1U < to_incl_pos)
                {
                    que.emplace(mid_pos, to_incl_pos);
                }

                break;
            }

            if constexpr (::Standard::Algorithms::is_debug)
            {
                assert(has);
            }
        } while (!que.empty());

        assert(cuts.size() - 2U == sequence.size());

        return sequence;
    }

#if _DEBUG
    template<std::integral long_int_t, std::integral int_t, long_int_t inf>
    requires(same_sign_leq_size<int_t, long_int_t> && long_int_t{} < inf)
#else
    template<std::integral int_t>
#endif
    [[nodiscard]] constexpr auto restore_min_stick_cuts(const std::vector<int_t> &cuts,
#if _DEBUG
        const std::vector<std::vector<long_int_t>> &buffers,
#endif
        const std::vector<std::vector<std::size_t>> &mids) -> std::vector<int_t>
    {
        assert(2U < cuts.size() && 2U <= mids.size());

        const auto size = cuts.size() - 2U;

        std::vector<int_t> sequence;
        sequence.reserve(size);

        // {from_pos, to_pos} in cuts.
        std::queue<std::pair<std::size_t, std::size_t>> que;
        que.emplace(0U, size + 1U);

        do
        {
            const auto [from_pos, to_incl_pos] = que.front();

            assert(from_pos < to_incl_pos && from_pos + 1U < to_incl_pos && to_incl_pos < cuts.size());

            que.pop();

            const auto &mid_pos = mids[from_pos][to_incl_pos - 1U];
            assert(mid_pos <= size);

            sequence.push_back(cuts[mid_pos]);

            if (from_pos + 1U < mid_pos)
            {
                que.emplace(from_pos, mid_pos);
            }

            if (mid_pos + 1U < to_incl_pos)
            {
                que.emplace(mid_pos, to_incl_pos);
            }

#if _DEBUG
            {
                const auto &from = cuts[from_pos];
                const auto &to_incl = cuts[to_incl_pos];

                assert(int_t{} <= from && from < to_incl && from + static_cast<int_t>(1) < to_incl &&
                    to_incl <= cuts.back());

                const auto &cost = buffers[from_pos][to_incl_pos];
                assert(long_int_t{} <= cost && cost < inf);

                const auto &prev = buffers[from_pos][mid_pos];
                const auto &next = buffers[mid_pos][to_incl_pos];

                const auto cand = static_cast<long_int_t>(prev + next + static_cast<long_int_t>(to_incl) - from);

                assert(long_int_t{} <= std::min(prev, next) && long_int_t{} < cand &&
                    std::max({ prev, next, cand }) < inf);

                assert(cand == cost);
            }
#endif
        } while (!que.empty());

        assert(cuts.size() - 2U == sequence.size());

        return sequence;
    }
} // namespace Standard::Algorithms::Numbers::Inner

namespace Standard::Algorithms::Numbers
{
    // Given a log/plank/rod/stick of length stick_length>0, and n>=0 unique sorted cut coordinates,
    // where source_cuts[i] is in [1..stick_length-1] for i in [0, n-1],
    // compute the min sum of cuts and the cut sequence.
    // If a segment can be cut further, it contributes its length to the resulting cost, else 0.
    //
    // Time, space O(n*n).
    template<std::integral long_int_t, std::integral int_t, long_int_t inf = std::numeric_limits<long_int_t>::max() / 2>
    requires(same_sign_leq_size<int_t, long_int_t> && long_int_t{} < inf)
    [[nodiscard]] constexpr auto min_stick_cuts(const std::vector<int_t> &source_cuts, const int_t &stick_length)
        -> std::pair<std::vector<int_t>, long_int_t>
    {
        require_positive(stick_length, "stick length");

        const auto size = source_cuts.size();
        if (size == 0U)
        {
            return {};
        }

        {
            constexpr auto max_size = std::numeric_limits<std::uint32_t>::max() / 2U;

            require_less_equal(size, max_size, "size");
        }

        // todo(p4): del cuts - use source_cuts.
        const auto cuts = Inner::pend_zero_and_length<int_t>(source_cuts, stick_length);

        // from, to -> total cost
        std::vector<std::vector<long_int_t>> buffers(size + 1U, std::vector<long_int_t>(size + 2U, long_int_t{}));

        // from, to_inclusive -> cut position
        std::vector<std::vector<std::size_t>> mids(size + 1U, std::vector<std::size_t>(size + 1U, 0U));

        for (std::size_t index = 1; index <= size; ++index)
        {
            mids[index][index] = index;
        }

#if _DEBUG
        const auto max_steps = static_cast<std::uint64_t>(size) * static_cast<std::uint64_t>(size) * 3LLU;

        std::uint64_t steps{};
#endif

        for (std::size_t length = 2; length <= size + 1U; ++length)
        {
            for (std::size_t from{}; from <= size + 1U - length; ++from)
            {
                const auto tod = from + length;
                const auto sub_cost = static_cast<long_int_t>(calculate_substring_sum<int_t>(from, cuts, tod));

                auto &best_cost = buffers[from][tod];
                best_cost = inf;

                // After the computation, due to Professor Knuth optimization, the monotonicity conditions:
                // roots[from][to - 1] <= roots[from][to]
                // roots[from][to] <= roots[from + 1][to]

                const auto tod_inclusive = tod - 1U;

                //     |   max_mid |
                // |    mid        |
                // |    best_mid    |
                const auto &min_mid_raw = mids[from][tod_inclusive - 1U];
                auto mid = std::max(from + 1U, min_mid_raw);

                const auto &max_mid_raw = mids[from + 1U][tod_inclusive];
                const auto max_mid = std::min(tod_inclusive, max_mid_raw);

                assert(long_int_t{} < sub_cost && sub_cost < inf && mid <= max_mid && max_mid <= size);

                auto &best_mid = mids[from][tod_inclusive];

                do
                {
#if _DEBUG
                    ++steps;
                    require_less_equal(steps, max_steps, "steps in min_stick_cuts should be about size squared.");
#endif

                    const auto &left = buffers[from][mid];
                    const auto &right = buffers[mid][tod];
                    const auto cand = static_cast<long_int_t>(left + sub_cost + right);

                    assert(long_int_t{} <= left && long_int_t{} <= right &&
                        std::max({ left, sub_cost, right }) <= cand && cand < inf);

                    if (cand < best_cost)
                    {
                        best_cost = cand, best_mid = mid;
                    }
                } while (++mid <= max_mid);
            }
        }

        const auto &sum = buffers[0].back();
        assert(long_int_t{} < sum && sum < inf);

        auto sequence =
#if _DEBUG
            Inner::restore_min_stick_cuts<long_int_t, int_t, inf>(cuts, buffers, mids);
#else
            Inner::restore_min_stick_cuts<int_t>(cuts, mids);
#endif

        return { std::move(sequence), sum };
    }

    // Slow time O(n**3).
    template<std::integral long_int_t, std::integral int_t, long_int_t inf = std::numeric_limits<long_int_t>::max() / 2>
    requires(same_sign_leq_size<int_t, long_int_t> && long_int_t{} < inf)
    [[nodiscard]] constexpr auto min_stick_cuts_slow(const std::vector<int_t> &source_cuts, const int_t &stick_length)
        -> std::pair<std::vector<int_t>, long_int_t>
    {
        require_positive(stick_length, "stick length");

        const auto size = source_cuts.size();
        if (size == 0U)
        {
            return {};
        }

        require_less_equal(size, static_cast<std::size_t>(Standard::Algorithms::Utilities::stack_max_size), "stack size");

        const auto cuts = Inner::pend_zero_and_length<int_t>(source_cuts, stick_length);

        // from_pos, to_incl_pos -> total cost
        std::vector<std::vector<long_int_t>> buffers(size + 1U, std::vector<long_int_t>(size + 2U, inf));

        for (std::size_t index{}; index <= size; ++index)
        {
            buffers[index][index + 1U] = {};
        }

        Inner::min_stick_cuts_rec_slow<long_int_t, int_t, inf>(cuts, std::size_t{}, buffers,
            // Having added 2 more items, the maximum index is (size + 2 - 1).
            size + 1U);

        const auto &sum = buffers[0].back();
        assert(long_int_t{} < sum && sum < inf);

        auto sequence = Inner::restore_min_stick_cuts_slow<long_int_t, int_t, inf>(cuts, buffers);

        return { std::move(sequence), sum };
    }
} // namespace Standard::Algorithms::Numbers
