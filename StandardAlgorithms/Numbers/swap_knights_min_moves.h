#pragma once
#include <unordered_set>
#include <vector>
#include "../Utilities/ExceptionUtilities.h"

namespace
{
    int64_t calls_g = 0;

    template <class int_t, class int_t2>
    void check_coordinate(const int_t& n, const int_t2& x, const char* const name)
    {
        if (x < 0 || x >= n)
            throw std::invalid_argument(std::string(name) +
                " (" + std::to_string(x) + ") must be in the range [0, " + std::to_string(x - 1) + "]");
    }

    template <class int_t>
    void check_point(const int_t& n, const int_t x, const int_t y)
    {
        check_coordinate(n, x, "first coordinate");
        check_coordinate(n, y, "second coordinate");
    }

    template <class int_t>
    void check_knights(const int_t n,
        const int_t x0, const int_t y0,
        const int_t x1, const int_t y1)
    {
        RequireGreater(n, 2);
        check_point(n, x0, y0);
        check_point(n, x1, y1);

        if (n == 3 && (x0 == y0 && x0 == 1 ||
            x1 == y1 && x1 == 1))
            throw std::invalid_argument("Point cannot be (1, 1) when n=3.");

        if (x0 == x1 && y0 == y1)
            throw std::invalid_argument("Points must be distinct: " + std::to_string(x0) + ", " + std::to_string(y0));
    }

    template <class int_t2, class int_t, class pair_t,
        class set_t,
        class f_t = typename pair_t::first_type,
        class s_t = typename pair_t::second_type>
        std::vector<pair_t> possible_knight_moves_slow(const set_t& moves, const int_t n, const pair_t& cur, const pair_t& cur_d, const pair_t& other)
    {
        static_assert(sizeof(int_t) <= sizeof(int_t2));
        static_assert(std::is_signed_v<f_t> && std::is_signed_v<s_t>);

        assert(cur != cur_d);
        std::vector<pair_t> cands;

        for (const auto& coef : { std::make_pair(1, 2), std::make_pair(2, 1) })
        {
            for (const auto& si0 : { -1, 1 })
                for (const auto& si1 : { -1, 1 })
                {
                    const auto x = static_cast<f_t>(cur.first + coef.first * si0);
                    if (x < 0 || x >= n)
                        continue;

                    const auto y = static_cast<s_t>(cur.second + coef.second * si1);
                    if (y < 0 || y >= n || other.first == x && other.second == y)
                        continue;

                    const pair_t cand(x, y);
                    if (!moves.count(cand))
                        cands.push_back(cand);
                }
        }

        // Prefer paths, closer to the destination.
        const auto lam = [&cur_d](const pair_t& a, const pair_t& b) ->bool
        {
            const int_t2 a_x = static_cast<int_t2>(a.first) - cur_d.first,
                a_y = static_cast<int_t2>(a.second) - cur_d.second,
                b_x = static_cast<int_t2>(b.first) - cur_d.first,
                b_y = static_cast<int_t2>(b.second) - cur_d.second;

            const auto dist_a = a_x * a_x + a_y * a_y,
                dist_b = b_x * b_x + b_y * b_y;
            return dist_a < dist_b;
        };

        std::sort(cands.begin(), cands.end(), lam);

        return cands;
    }

    template <class int_t2, class int_t, class pair_t, class set_t>
    void swap_knights_min_moves_rec_slow(set_t& moves0, set_t& moves1,
        const int_t n,
        const pair_t p0_cur, const pair_t p0_des,
        const pair_t p1_cur, const pair_t p1_des,
        int_t& best,
        int64_t& calls,
        const int_t count = 0)
    {
        static_assert(sizeof(int_t) <= sizeof(int_t2));

        assert(count >= 0 && best > 0 && p0_des != p1_des);
        if (count >= best)
            return;

        if (p0_cur == p0_des && p1_cur == p1_des)
        {
            assert(count > 0 && count < best);
            best = count;
            return;
        }

        for (int_t i = 0; i < 2; ++i)
        {
            const auto avoid_infinite_recursion = (count & 1) ^ i;
            const auto& cur = avoid_infinite_recursion ? p0_cur : p1_cur;
            const auto& cur_d = avoid_infinite_recursion ? p0_des : p1_des;
            if (cur == cur_d)
                // Once the dest is reached, stop.
                continue;

            const auto& ot = !avoid_infinite_recursion ? p0_cur : p1_cur;
            const auto& ot_d = !avoid_infinite_recursion ? p0_des : p1_des;

            auto& moves = avoid_infinite_recursion ? moves0 : moves1;
            auto& ot_moves = !avoid_infinite_recursion ? moves0 : moves1;

            const auto cands = possible_knight_moves_slow<int_t2, int_t, pair_t>(moves, n, cur, cur_d, ot);
            for (const auto& cand : cands)
            {
#if _DEBUG
                auto ins =
#endif
                    moves.insert(cand);

#if _DEBUG
                assert(ins.second);
#endif
                ++calls;

                swap_knights_min_moves_rec_slow<int_t2, int_t, pair_t, set_t>(ot_moves, moves, n,
                    // Swap current and other to make steps by both parties.
                    ot, ot_d,
                    cand, cur_d,
                    best,
                    calls,
                    static_cast<int_t>(count + 1));

#if _DEBUG
                const auto del =
#endif
                    moves.erase(cand);

#if _DEBUG
                assert(del == 1);
#endif
            }
        }
    }

    template <class int_t>
    int_t pos_to_index(const int_t& n, const int_t& x, const int_t& y)
    {
        assert(n >= 3 && x >= 0 && x < n&& y >= 0 && y < n);

        const int_t index = n * x + y;
        assert(index >= 0 && index < n* n);
        return index;
    }

    template <class int_t2, class int_t>
    int_t2 pos_to_index2(const int_t2& nn, const int_t& n,
        const int_t& x0, const int_t& y0,
        const int_t& x1, const int_t& y1)
    {
        static_assert(sizeof(int_t) <= sizeof(int_t2));

        assert(n >= 3 && x0 >= 0 && x0 < n&& y0 >= 0 && y0 < n&&
            x1 >= 0 && x1 < n&& y1 >= 0 && y1 < n);
        assert(x0 != x1 || y0 != y1);

        const int_t2 row = pos_to_index<int_t2>(n, x0, y0),
            col = pos_to_index<int_t2>(n, x1, y1),
            ind = row * nn + col;

        assert(ind >= 0 && ind < nn* nn);
        return ind;
    }

    template <class int_t2, class int_t>
    void possible_knight_moves(const int_t2& nn, const int_t& n,
        const int_t& x0, const int_t& y0,
        const int_t& x1, const int_t& y1,
        std::vector<int_t2>& cands)
    {
        static_assert(std::is_signed_v<int_t2> && sizeof(int_t) <= sizeof(int_t2));
        assert(n < nn);
        assert(x0 != x1 || y0 != y1);

        cands.clear();

        for (const auto& coef : {
            std::make_pair(1, 2),
            std::make_pair(1, -2),
            std::make_pair(-1, 2),
            std::make_pair(-1, -2),
            //
            std::make_pair(2, 1),
            std::make_pair(2, -1),
            std::make_pair(-2, 1),
            std::make_pair(-2, -1),
            })
        {
            {//0 move
                const auto x = static_cast<int_t>(x0 + coef.first);
                if (x >= 0 && x < n)
                {
                    const auto y = static_cast<int_t>(y0 + coef.second);
                    if (y >= 0 && y < n && (x1 != x || y1 != y))
                    {
                        const int_t2 i = pos_to_index2<int_t2, int_t>(nn, n, x, y, x1, y1);
                        cands.push_back(i);
                    }
                }
            }
            {//1 move
                const auto x = static_cast<int_t>(x1 + coef.first);
                if (x >= 0 && x < n)
                {
                    const auto y = static_cast<int_t>(y1 + coef.second);
                    if (y >= 0 && y < n && (x0 != x || y0 != y))
                    {
                        const int_t2 i = pos_to_index2<int_t2, int_t>(nn, n, x0, y0, x, y);
                        cands.push_back(i);
                    }
                }
            }
        }

        assert(cands.size());
    }

    template <class int_t2, class int_t>
    std::vector<std::vector<int_t2>> build_knight_moves(const int_t n)
    {
        static_assert(sizeof(int_t) <= sizeof(int_t2));

        const int_t2 nn = int_t2(n) * int_t2(n);
        assert(nn > n && nn * nn > nn);

        std::vector<int_t2> cands;
        std::vector<std::vector<int_t2>> moves(1ll * nn * nn);

        for (int_t x0 = 0; x0 < n; ++x0)
        {
            for (int_t y0 = 0; y0 < n; ++y0)
            {
                const auto index0 = pos_to_index<int_t2>(n, x0, y0) * nn;

                for (int_t x1 = 0; x1 < n; ++x1)
                {
                    for (int_t y1 = 0; y1 < n; ++y1)
                    {
                        if (x0 == x1 && y0 == y1)
                            continue;

                        possible_knight_moves<int_t2, int_t>(nn, n, x0, y0, x1, y1, cands);

                        const auto index_from = index0 + pos_to_index<int_t2>(n, x1, y1);
                        assert(index_from >= 0 && index_from < nn* nn);

                        auto& ms = moves[index_from];
                        for (const auto& index_to : cands)
                        {
                            ms.push_back(index_to);

                            constexpr auto knight_moves_max = 8 << 1;
                            assert(ms.size() <= knight_moves_max);
                        }

                        ms.shrink_to_fit();
                    }
                }
            }
        }

        return moves;
    }

    template <class int_t>
    int_t bfs_min_knights_moves(const std::vector<std::vector<int_t>>& moves, const int_t start, const int_t stop)
    {
        assert(start != stop && moves.size());

        std::vector<int_t> q, q2;
        q.push_back(start);

        std::vector<bool> visited(moves.size());
        visited[start] = true;

        int_t count = 0;
        do
        {
            ++count;
            assert(count < std::numeric_limits<int_t>::max());
            do
            {
                const auto& ms = moves[q.back()];
                for (const auto& to : ms)
                {
                    if (visited[to])
                        continue;

                    if (to == stop)
                        return count;

                    visited[to] = true;
                    q2.push_back(to);
                }

                q.pop_back();
            } while (q.size());

            std::swap(q, q2);
        } while (q.size());

        assert(0);
        throw std::runtime_error("bfs_min_knights_moves impossible error.");
    }
}

namespace Privet
{
    namespace Algorithms
    {
        namespace Numbers
        {
            // Compute min moves to swap 2 knights between 2 distinct 2D points, where x,y are in [0..n-1], n > 1.
            // In a move, a knight can move to a free point, where another knight is not standing.
            template <class int_t2, class int_t>
            int_t2 swap_knights_min_moves(const int_t n,
                const int_t x0, const int_t y0,
                const int_t x1, const int_t y1)
            {
                static_assert(sizeof(int_t) <= sizeof(int_t2));

                check_knights(n, x0, y0, x1, y1);

                const int_t2 nn = int_t2(n) * int_t2(n);
                assert(nn > n && nn * nn > nn);

                const auto moves = build_knight_moves<int_t2, int_t>(n);

                const auto start = pos_to_index2<int_t2, int_t>(nn, n, x0, y0, x1, y1);

                const auto stop = pos_to_index2<int_t2, int_t>(nn, n, x1, y1, x0, y0);

                const auto count = bfs_min_knights_moves<int_t2>(moves, start, stop);
                return count;
            }

            // Slow time about O(n*n)!
            template <class int_t2, class int_t, class pair_t, class pair_hash_t>
            int_t swap_knights_min_moves_slow(const int_t n, const pair_t p0, const pair_t p1)
            {
                static_assert(sizeof(int_t) <= sizeof(int_t2));

                check_knights(n, p0.first, p0.second, p1.first, p1.second);

                std::unordered_set<pair_t, pair_hash_t> moves0, moves1;
                moves0.insert(p0);
                moves1.insert(p1);

                int64_t calls{};
                auto best = std::numeric_limits<int_t>::max();

                swap_knights_min_moves_rec_slow<int_t2, int_t, pair_t>(moves0, moves1, n, p0, p1, p1, p0, best, calls);
                if (calls_g < calls)
                    calls_g = calls;

                assert(best < std::numeric_limits<int_t>::max());
                return best;
            }
        }
    }
}