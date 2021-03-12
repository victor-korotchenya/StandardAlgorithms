#pragma once
#include <vector>
#include <queue>
#include "../Utilities/ExceptionUtilities.h"

namespace
{
    template<class int_t, int_t inf>
    void min_stick_cuts_rec_slow(const std::vector<int_t>& cuts,
        std::vector<std::vector<int_t>>& buf,
        const int_t from_pos, const int_t to_incl_pos)
    {
        static_assert(inf > 0);
        assert(from_pos >= 0 && from_pos < to_incl_pos&& from_pos + 1 < to_incl_pos && to_incl_pos < static_cast<int_t>(cuts.size()));

        auto& mi = buf[from_pos][to_incl_pos];
        assert(mi == inf);

        const auto& from = cuts[from_pos],
            & to_incl = cuts[to_incl_pos];
        assert(from >= 0 && from < to_incl&& from + 1 < to_incl && to_incl <= cuts.back());

        for (auto mid_pos = static_cast<int_t>(from_pos + 1); mid_pos < to_incl_pos; ++mid_pos)
        {
            const auto& prev = buf[from_pos][mid_pos], & next = buf[mid_pos][to_incl_pos];
            if (prev == inf)
                min_stick_cuts_rec_slow<int_t, inf>(cuts, buf, from_pos, mid_pos);
            assert(prev >= 0 && prev < inf);

            if (next == inf)
                min_stick_cuts_rec_slow<int_t, inf>(cuts, buf, mid_pos, to_incl_pos);

            const auto cand = static_cast<int_t>(prev + next + to_incl - from);
            assert(next >= 0 && next < inf&& cand < inf&& cand > 0);
            if (cand < mi)
                mi = cand;
        }

        assert(mi < inf);
    }
}

namespace Privet
{
    namespace Algorithms
    {
        namespace Numbers
        {
            // Given a stick/rod of length n>0, and sorted unique cut coordinates,
            // where cut_coordinates[i] is in [1..n-1],
            // compute the min sum of cuts and the cut sequence
            // using Knuth Yao speed up.
            // If a segment can be cut further, its sum is its length, else 0.
            //
            // Time, space O(n*n). todo: p2. do.

            // Slow time O(n**3).
            template<class int_t>
            std::pair<std::vector<int_t>, int_t> min_stick_cuts_slow(const std::vector<int_t>& cut_coordinates, const int_t stick_length)
            {
                RequirePositive(stick_length, "stick_length");

                const auto size = static_cast<int_t>(cut_coordinates.size());
                if (!size)
                    return {};

                assert(std::is_sorted(cut_coordinates.begin(), cut_coordinates.end()));
                RequirePositive(cut_coordinates[0], "cut_coordinates[0]");
                RequireGreater(stick_length, cut_coordinates.back(), "cut_coordinates.back");

                // Easy search.
                std::vector<int_t> cuts(size + 2);
                std::copy(cut_coordinates.begin(), cut_coordinates.end(), cuts.begin() + 1);
                cuts.back() = stick_length;

                constexpr auto inf = std::numeric_limits<int_t>::max() / 2;

                // from_pos,to_incl_pos -> total cost
                std::vector<std::vector<int_t>> buf(size + 1,
                    std::vector<int_t>(size + 2, inf));

                for (ino_t i{}; i <= size; ++i)
                    buf[i][i + 1] = 0;

                // todo: p3. The recursive version does not make sense as all triangle values are run.
                min_stick_cuts_rec_slow<int_t, inf>(cuts, buf, 0, static_cast<int_t>(size + 1));

                const auto& sum = buf[0].back();
                assert(sum > 0 && sum < inf);

                // Restore
                std::vector<int_t> sequence;
                sequence.reserve(size);

                // {from_pos, to_pos} in cuts.
                std::queue<std::pair<int_t, int_t>> q;
                q.push({ int_t(), static_cast<int_t>(size + 1) });
                do
                {
                    const auto from_pos = q.front().first, to_incl_pos = q.front().second;
                    assert(from_pos >= 0 && from_pos < to_incl_pos&& from_pos + 1 < to_incl_pos && to_incl_pos < static_cast<int_t>(cuts.size()));
                    q.pop();

                    const auto& from = cuts[from_pos], to_incl = cuts[to_incl_pos];
                    assert(from >= 0 && from < to_incl&& from + 1 < to_incl && to_incl <= cuts.back());

                    const auto& cost = buf[from_pos][to_incl_pos];
                    assert(cost >= 0 && cost < inf);
#if _DEBUG
                    auto has = false;
#endif

                    for (auto mid_pos = static_cast<int_t>(from_pos + 1); mid_pos < to_incl_pos; ++mid_pos)
                    {
                        const auto& prev = buf[from_pos][mid_pos], & next = buf[mid_pos][to_incl_pos];
                        assert(prev >= 0 && prev < inf&& next >= 0 && next < inf);

                        const auto cand = prev + next + to_incl - from;
                        assert(cand < inf&& cand > 0);
                        if (cand == cost)
                        {
#if _DEBUG
                            has = true;
#endif
                            sequence.push_back(cuts[mid_pos]);
                            if (from_pos + 1 < mid_pos)
                                q.push({ from_pos, mid_pos });

                            if (mid_pos + 1 < to_incl_pos)
                                q.push({ mid_pos, to_incl_pos });

                            break;
                        }
                    }

#if _DEBUG
                    assert(has);
#endif
                } while (q.size());

                assert(cuts.size() - 2 == sequence.size());
                return { sequence, sum };
            }
        }
    }
}