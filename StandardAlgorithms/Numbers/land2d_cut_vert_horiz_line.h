#pragma once
#include <cmath>
#include <tuple>
#include <vector>
#include "Arithmetic.h"
#include "bit_utilities.h"
#include "../Utilities/ExceptionUtilities.h"

namespace
{
    template<class int_t>
    [[nodiscard]]
    std::vector<std::pair<int_t, int_t>> cut_2d_areas(const int_t area)
    {
        RequirePositive(area, "area");
        std::vector<std::pair<int_t, int_t>> result;

        const auto q = static_cast<int_t>(sqrt(area));
        assert(q >= 1);

        for (int_t i = 1; i <= q; ++i)
        {
            if (area % i)
                continue;

            const auto d = static_cast<int_t>(area / i);
            assert(i * d == area && i <= d);
            result.emplace_back(i, d);
        }

        assert(result.size());
        return result;
    }

    // 3*5 cannot cut area 7,11,13,14.
    // 3*5 can 1,2,3,4,5,6=2*3,8=2*4,9=3*3,10=2*5,12=3*4,15=3*5.
    // 4*5 cannot cut area 18.
    template<class int_t>
    bool can_cut_area_from_2d(const std::vector<std::pair<int_t, int_t>>& area_cuts, const int_t request_area, int_t r, int_t c)
    {
        if (c < r)
            std::swap(r, c);

        assert(request_area > 0 && r > 0 && r <= c && area_cuts.size() && area_cuts[0].first * area_cuts[0].second == request_area);

        if (r == 1)
            return request_area <= c;

        const auto sq = r * c;
        assert(sq > 0 && r < sq&& c < sq);

        if (sq < request_area)
            return false;
        if (sq == request_area)
            return true;

        for (const auto& p : area_cuts)
        {
            assert(p.first <= p.second);
            if (p.first <= r && p.second <= c)
                return true;
        }

        return false;
    }

    //template<class int_t>
    //void land2d_cut_rec_slow(const std::vector<int_t> &pieces,
    //    const std::vector<std::vector<std::pair<int_t, int_t>>> &possible_cuts,
    //    // r,c,piece mask -> possible
    //    std::vector<std::vector<std::vector<char>>> &buf,
    //    int_t r, int_t c, const int mask)
    //{
    //    const auto id = MostSignificantBit(mask);
    //    assert(r > 0 && c > 0 && mask > 0 && id >= 0 && id < pieces.size() && (1 << id) <= mask && !buf[r - 1][c - 1][mask]);
    //
    //    if (c < r)
    //        std::swap(r, c);
    //    assert(r <= c);
    //
    //    constexpr auto possible = 2, imposs = 1;
    //    buf[r - 1][c - 1][mask] = imposs;
    //
    //    const auto &piece = pieces[id];
    //    assert(piece > 0 && piece <= 100 * 100);
    //
    //    const auto &area_cuts = possible_cuts[id];
    //    if (!can_cut_area_from_2d(area_cuts, piece, r, c))
    //        return;
    //    assert(piece <= r*c);
    //
    //    const auto mask2 = (1 << id) ^ mask;
    //    if (!mask2)
    //    { // 1 item
    //        buf[r - 1][c - 1][mask] = possible;
    //        return;
    //    }
    //
    //    if (r == 1)
    //    {
    //        auto sum = piece;
    //        auto mask3 = mask2;
    //        do
    //        {
    //            const auto id3 = MostSignificantBit(mask3);
    //            mask3 ^= 1 << id3;
    //            sum += pieces[id3];
    //        } while (mask3);
    //
    //        if (sum <= c)
    //            buf[r - 1][c - 1][mask] = possible;
    //        return;
    //    }
    //
    //    for (const auto &p : area_cuts)
    //    {
    //        assert(p.first <= p.second);
    //
    //        // r=10*20=c cut off area 6, p = {2,3}:
    //        // 2*20 + 8*20 -> 2*3 + 2*17 + 8*20
    //        // 3*20 + 7*20 -> 3*2 + 3*18 + 7*20
    //        //
    //        // 10*2 + 10*18 -> 3*2 + 7*2 + 10*18
    //        // 10*3 + 10*17 -> 2*3 + 8*3 + 10*17
    //
    //        //2*3 or 3*2 removed, got max 2 pieces left.
    //        if (p.first <= r && p.second <= c)
    //        {
    //            // 2*20 + 8*20 -> 2*3 + 2*17 + 8*20
    //            // cut row first
    //
    //            // 10*3 + 10*17 -> 2*3 + 8*3 + 10*17
    //            // cut col first
    //        }
    //
    //        if (p.first <= c && p.second <= r)
    //        {
    //            // 3*20 + 7*20 -> 3*2 + 3*18 + 7*20
    //            // 10*2 + 10*18 -> 3*2 + 7*2 + 10*18
    //        }
    //
    //        // cut by rows
    //        // cut by cols
    //    }
    //}

    template<class int_t>
    struct land2d_cut_context final
    {
        // Sum of chosen pieces
        std::vector<int_t> masked_sums;

        // row,mask -> processed before, whether possible.
        std::vector<std::vector<bool>> proced, results;

        const int full_mask;

        explicit land2d_cut_context(const std::vector<int_t>& pieces, const int_t row_col_max)
            : masked_sums(1 << RequirePositive(pieces.size(), "pieces.size") | 1),
            proced(row_col_max + 1, std::vector<bool>(masked_sums.size())),
            results(proced),
            full_mask((1u << pieces.size()) - 1u)
        {
            assert(pieces.size() < 20 && row_col_max > 0 && row_col_max <= 100 && full_mask);

            for (auto mask = 1; mask <= full_mask; ++mask)
            {
                int_t sum{};
                for (auto i = 0, m2 = mask; m2; ++i, m2 >>= 1)
                    if (m2 & 1)
                        sum += pieces[i];

                masked_sums[mask] = sum;
            }
        }

        // The sum of items can be formed only when
        // it is divisible by row (column)!
        bool can(const int_t row_or_col, const unsigned mask) const
        {
            assert(row_or_col > 0 && mask > 0);

            const auto rem = masked_sums[mask] % row_or_col;
            return !rem;
        }
    };

    template<class int_t>
    bool land2d_cut_rec(land2d_cut_context<int_t>& ctx,
        const int_t row,
        // Already processed pieces.
        const unsigned mask = {})
    {
        assert(row > 0);

        const auto left = ctx.full_mask ^ mask;
        if (!(left & (left - 1u)))
            return true; // 1 item, and checked it is possible before.

        if (ctx.proced[row][mask])
            return ctx.results[row][mask];

        ctx.proced[row][mask] = true;

        const auto col = ctx.masked_sums[left] / row;
        assert(col > 0 && 0 == ctx.masked_sums[left] % row);

        // 111111 -> 111110 -> 111100 -> 111000 -> 110000 -> 100000
        // Why not explore e.g. 101101 - why sequential 1s are enough?!
        // Maybe 101101 can be split in many ways?
        //
        // No - must explore ALL subsets!
        for (auto split = left & (left - 1u); ;
            // Enumerate all subsets.
            split = (split - 1u) & left)
        {
            const auto split2 = left ^ split;
            if (!split || // No pieces
                // Already explored
                split2 > split)
            {
                return false;
            }

            if (ctx.can(row, split) && ctx.can(row, split2))
            {// Split by rows.
                if (land2d_cut_rec<int_t>(ctx, row, mask | split) &&
                    land2d_cut_rec<int_t>(ctx, row, mask | split2))
                    break;
            }

            if (ctx.can(col, split) && ctx.can(col, split2))
            {// Split by columns.
                if (land2d_cut_rec<int_t>(ctx, col, mask | split) &&
                    land2d_cut_rec<int_t>(ctx, col, mask | split2))
                    break;
            }
        }

        return ctx.results[row][mask] = true;
    }
}

namespace Privet
{
    namespace Algorithms
    {
        namespace Numbers
        {
            // 2D land r*c <= 10K can be cut into pieces using horizontal or vertical lines.
            // Determine whether it is possible to get n <= 15 pieces with the given area?
            // If not, return an empty set.
            // Leftovers are not fine.
            // Time O(r*c * 2**n), space O(max(r,c) * 2**n).
            template<class int_t>
            std::vector<std::tuple<int_t, int_t, bool>> land2d_cut_vert_horiz_line(const std::vector<int_t>& pieces, const int_t r, const int_t c)
            {
                const auto size = static_cast<int>(pieces.size());
                assert(size <= 15 && r <= 100 && c <= 100);

                RequirePositive(size, "pieces.size");
                RequirePositive(r, "r");
                RequirePositive(c, "c");

                std::vector<
                    // Piece number (start from 0), position, is vertical.
                    std::tuple<int_t, int_t, bool>> result;
                {
                    const auto sum = std::accumulate(pieces.begin(), pieces.end(), int_t());
                    const auto pr = r * c;
                    //if (sum > pr)
                    if (sum != pr)
                        return {};

                    //if (size == 1)
                    //{
                    //    result.push_back({});
                    //    return result;
                    //}
                }

                land2d_cut_context<int_t> context(pieces, std::max(r, c));

                if (land2d_cut_rec<int_t>(context, r))
                    result.push_back({});

                return result;
            }

            //template<class int_t>
            // todo: p3. end. std::vector<std::tuple<int_t, int_t, bool>> land2d_cut_vert_horiz_line_slow(const std::vector<int_t> &pieces, int_t r, int_t c)
            //{
            //    const auto size = static_cast<int>(pieces.size());
            //    assert(size <= 15);
            //    RequirePositive(size, "pieces.size");
            //    RequireAllPositive(pieces, "pieces");
            //    RequirePositive(r, "r");
            //    RequirePositive(c, "c");
            //
            //    if (c < r)
            //        std::swap(r, c);
            //    assert(r > 0 && r <= c);
            //    {
            //        const auto sum = std::accumulate(pieces.begin(), pieces.end(), int_t());
            //        const auto pr = r*c;
            //        if (sum > pr)
            //            return {};
            //    }
            //
            //    std::vector<std::vector<std::pair<int_t, int_t>>> possible_cuts(size);
            //    for (auto i = 0; i < size; ++i)
            //        possible_cuts[i] = cut_2d_areas(pieces[i]);
            //
            //    std::vector<
            //        // Piece number (start from 0), position, is vertical.
            //        std::tuple<int_t, int_t, bool>> result;
            //
            //    const auto edge_mask = 1 << size;
            //    assert(edge_mask > 0);
            //
            //    // r,c,piece mask -> 2=possible, 1=imposs, 0=unknown.
            //    std::vector<std::vector<std::vector<char>>> buf(r, std::vector<std::vector<char>>(c, std::vector<char>(edge_mask)));
            //
            //    constexpr auto possible = 2;
            //    land2d_cut_rec_slow<int_t>(pieces, possible_cuts, buf, r, c, edge_mask - 1);
            //
            //    if (buf[r - 1][c - 1][edge_mask - 1] == possible)
            //        result.push_back({});
            //
            //    return result;
            //}
        }
    }
}