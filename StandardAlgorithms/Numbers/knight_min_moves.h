#pragma once
#include"knight_chess.h"
#include<queue>
#include<vector>

namespace Standard::Algorithms::Numbers
{
    // Compute min moves to ride a noble knight between 2 points.
    // todo(p3): Time, space O(1).
    // knight_min_moves

    // Slow time, space O(n*n).
    // todo(p4): meet 2 pointers in the middle could be slightly faster.
    template<std::signed_integral long_int_t, std::signed_integral int_t>
    requires(sizeof(int_t) <= sizeof(long_int_t))
    [[nodiscard]] constexpr auto knight_min_moves_slow(const std::pair<int_t, int_t> &from, const int_t &size,
        const std::pair<int_t, int_t> &tod, const std::pair<int_t, int_t> &skip = { -1, -1 }) -> long_int_t
    {
        Inner::check_knights<int_t>(from, size, tod);

        if (from == tod)
        {
            return {};
        }

        constexpr long_int_t zero{};
        constexpr auto inf = std::numeric_limits<long_int_t>::max();
        static_assert(zero < inf);

        const auto has_skip = !(skip.first < zero) && !(skip.second < zero) && skip.first < size &&
            skip.second < size && from != skip && tod != skip;

        constexpr auto cand_moves = Inner::knight_moves<long_int_t>();

        std::queue<std::pair<int_t, int_t>> que;
        que.push(from);

        std::vector<std::vector<long_int_t>> costs(size, std::vector<long_int_t>(size, inf));
        costs.at(from.first).at(from.second) = zero;

        do
        {
            const auto [xxx, yyy] = que.front();
            const auto &prev_cost = costs.at(xxx).at(yyy);

            assert(!(prev_cost < zero) && prev_cost < inf);

            if (que.front() == tod)
            {
                assert(zero < prev_cost);
                return prev_cost;
            }

            que.pop();

            const auto new_cost = static_cast<long_int_t>(prev_cost + 1);
            assert(prev_cost < new_cost);

            for (const auto &[vert, horiz] : cand_moves)
            {
                const auto cand_xxx = xxx + vert;
                const auto cand_yyy = yyy + horiz;

                if (cand_xxx < zero || cand_yyy < zero || !(cand_xxx < size) || !(cand_yyy < size))
                {// todo(p5): this check can be omitted - add 2*4 extras.
                    continue;
                }

                if (has_skip && cand_xxx == skip.first && cand_yyy == skip.second)
                {
                    continue;
                }

                auto &cur_cost = costs.at(cand_xxx).at(cand_yyy);

                if (!(new_cost < cur_cost))
                {
                    continue;
                }

                cur_cost = new_cost;
                que.emplace(static_cast<int_t>(cand_xxx), static_cast<int_t>(cand_yyy));
            }
        } while (!que.empty());

        assert(0);

        throw std::runtime_error("move_knights_min_moves impossible error.");
    }
} // namespace Standard::Algorithms::Numbers
