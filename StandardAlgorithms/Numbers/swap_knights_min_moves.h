#pragma once
#include"knight_min_moves.h"
#include<unordered_set>

namespace Standard::Algorithms::Numbers::Inner
{
    template<std::signed_integral long_int_t, std::signed_integral int_t, class set_t>
    requires(sizeof(int_t) <= sizeof(long_int_t))
    [[nodiscard]] constexpr auto possible_knight_moves_slow(const std::pair<int_t, int_t> &cur, const set_t &moves,
        const std::pair<int_t, int_t> &destination, const int_t &size,
        std::array<std::pair<int_t, int_t>, knights_max_moves> &cands, const std::pair<int_t, int_t> &skip)
        -> std::uint32_t
    {
        assert(cur != destination);

        std::uint32_t count{};

        for (const auto &coef : {
                 std::make_pair(-2, -1),
                 std::make_pair(-2, 1),
                 std::make_pair(-1, -2),
                 std::make_pair(-1, 2),
                 std::make_pair(1, -2),
                 std::make_pair(1, 2),
                 std::make_pair(2, -1),
                 std::make_pair(2, 1),
             })
        {
            const auto xxx = static_cast<long_int_t>(cur.first) + coef.first;
            if (xxx < 0 || !(xxx < size))
            {
                continue;
            }

            const auto yyy = static_cast<long_int_t>(cur.second) + coef.second;

            if (yyy < 0 || !(yyy < size) || (skip.first == xxx && skip.second == yyy))
            {
                continue;
            }

            if (const std::pair<int_t, int_t> cand{ static_cast<int_t>(xxx), static_cast<int_t>(yyy) };
                !moves.contains(cand))
            {
                assert(count < cands.size());

                cands.at(count++) = cand;
            }
        }

        const auto prefer_paths_closer_to_destination = [&destination](const std::pair<int_t, int_t> &aaa,
                                                            const std::pair<int_t, int_t> &bbb) -> bool
        {
            const auto a_x = static_cast<long_int_t>(aaa.first) - destination.first;
            const auto a_y = static_cast<long_int_t>(aaa.second) - destination.second;

            const auto b_x = static_cast<long_int_t>(bbb.first) - destination.first;
            const auto b_y = static_cast<long_int_t>(bbb.second) - destination.second;

            const auto dist_a = a_x * a_x + a_y * a_y;
            const auto dist_b = b_x * b_x + b_y * b_y;

            return dist_a < dist_b;
        };

        std::sort(cands.begin(), cands.begin() + count, prefer_paths_closer_to_destination);

        return count;
    }

    template<std::signed_integral long_int_t, std::signed_integral int_t, class set_t>
    requires(sizeof(int_t) <= sizeof(long_int_t))
    constexpr void swap_knights_min_moves_rec_slow(set_t &moves0, const int_t &size,
        const std::pair<int_t, int_t> &p0_cur, set_t &moves1, const std::pair<int_t, int_t> &p0_des, long_int_t &best,
        const std::pair<int_t, int_t> &p1_cur, std::int64_t &calls, const std::pair<int_t, int_t> &p1_des,
        const long_int_t &count = 0)
    {
        assert(!(count < 0) && 0 < best && p0_des != p1_des);

        if (!(count < best))
        {// todo(p4): evaluate the distance to cut off unneeded calls.
            return;
        }

        if (p0_cur == p0_des && p1_cur == p1_des)
        {
            assert(0 < count);

            best = count;
            return;
        }

        for (int_t pos{}; pos < 2; ++pos)
        {
            const auto avoid_infinite_recursion = (count & 1) ^ pos;
            const auto &cur = avoid_infinite_recursion ? p0_cur : p1_cur;
            const auto &destination = avoid_infinite_recursion ? p0_des : p1_des;

            if (cur == destination)
            {// Stop when the destination is reached to have min moves.
                continue;
            }

            const auto &ott = !avoid_infinite_recursion ? p0_cur : p1_cur;
            const auto &ott_d = !avoid_infinite_recursion ? p0_des : p1_des;

            auto &moves = avoid_infinite_recursion ? moves0 : moves1;
            auto &ot_moves = !avoid_infinite_recursion ? moves0 : moves1;

            // No initialization.
            // NOLINTNEXTLINE
            std::array<std::pair<int_t, int_t>, knights_max_moves> cands;

            const auto cands_size =
                possible_knight_moves_slow<long_int_t, int_t>(cur, moves, destination, size, cands, ott);

            for (std::uint32_t cind{}; cind < cands_size; ++cind)
            {
                const auto &cand = cands.at(cind);

                [[maybe_unused]] const auto ins = moves.insert(cand);

                assert(ins.second);

                ++calls;

                swap_knights_min_moves_rec_slow<long_int_t, int_t, set_t>(ot_moves, size,
                    // Swap current and other to make progress by both parties.
                    ott, moves, ott_d, best, cand, calls, destination, static_cast<long_int_t>(count + 1));

                [[maybe_unused]] const auto del = moves.erase(cand);

                assert(del == 1);
            }
        }
    }

    template<std::signed_integral long_int_t, std::signed_integral int_t>
    requires(sizeof(int_t) <= sizeof(long_int_t))
    [[nodiscard]] constexpr auto point_index(const int_t &size, const std::pair<int_t, int_t> &point) -> long_int_t
    {
        assert(knights_min_size <= size && !(point.first < 0) && point.first < size && !(point.second < 0) &&
            point.second < size);

        auto index = static_cast<long_int_t>(static_cast<long_int_t>(size) * point.first + point.second);

        assert(!(index < 0) && index < static_cast<long_int_t>(size) * size);

        return index;
    }

    template<std::signed_integral long_int_t, std::signed_integral int_t>
    requires(sizeof(int_t) <= sizeof(long_int_t))
    [[nodiscard]] constexpr auto two_points_index(const std::pair<int_t, int_t> &from, const int_t &size,
        const long_int_t &squared, const std::pair<int_t, int_t> &tod) -> long_int_t
    {
        assert(knights_min_size <= size && from != tod);
        assert(0 <= std::min({ from.first, from.second, tod.first, tod.second }));
        assert(std::max({ from.first, from.second, tod.first, tod.second }) < size);

        const auto row = point_index<long_int_t>(size, from);
        const auto col = point_index<long_int_t>(size, tod);
        auto ind = static_cast<long_int_t>(row * squared + col);

        assert(!(ind < 0) && ind < 1LL * squared * squared);

        return ind;
    }

    template<std::signed_integral long_int_t, std::signed_integral int_t>
    requires(sizeof(int_t) <= sizeof(long_int_t))
    [[nodiscard]] constexpr auto possible_knight_moves(const std::pair<int_t, int_t> &from, const int_t &size,
        const long_int_t &squared, const std::pair<int_t, int_t> &tod,
        std::array<long_int_t, knights_max_moves * 2LLU> &cands) noexcept -> std::uint32_t
    {
        assert(knights_min_size <= size && size < squared);
        assert(from.first != tod.first || from.second != tod.second);

        std::uint32_t count{};

        for (const auto &coef : {
                 std::make_pair(-2, -1),
                 std::make_pair(-2, 1),
                 std::make_pair(-1, -2),
                 std::make_pair(-1, 2),
                 std::make_pair(1, -2),
                 std::make_pair(1, 2),
                 std::make_pair(2, -1),
                 std::make_pair(2, 1),
             })
        {
            {// "from" move
                const auto xxx = static_cast<long_int_t>(from.first) + coef.first;
                const auto yyy = static_cast<long_int_t>(from.second) + coef.second;

                if (!(xxx < 0) && xxx < size && !(yyy < 0) && yyy < size && (tod.first != xxx || tod.second != yyy))
                {
                    assert(count < cands.size());

                    cands.at(count) =
                        two_points_index<long_int_t, int_t>(std::pair<int_t, int_t>{ xxx, yyy }, size, squared, tod);

                    ++count;
                }
            }
            {// "to" move
                const auto xxx = static_cast<long_int_t>(tod.first) + coef.first;
                const auto yyy = static_cast<long_int_t>(tod.second) + coef.second;

                if (!(xxx < 0) && xxx < size && !(yyy < 0) && yyy < size && (from.first != xxx || from.second != yyy))
                {
                    assert(count < cands.size());

                    cands.at(count) =
                        two_points_index<long_int_t, int_t>(from, size, squared, std::pair<int_t, int_t>{ xxx, yyy });

                    ++count;
                }
            }
        }

        assert(0U < count);

        return count;
    }

    template<std::signed_integral int_t>
    [[nodiscard]] constexpr auto bfs_min_knights_moves(
        const int_t &start, const int_t &size, const int_t &squared, const int_t &stop) -> int_t
    {
        assert(start != stop && int_t{} < size && size < squared);

        // No init.
        // NOLINTNEXTLINE
        std::array<int_t, knights_max_moves * 2U> cands;

        std::vector<int_t> que;
        std::vector<int_t> que_2;
        que.push_back(start);

        std::vector<bool> visited(1LL * squared * squared);
        visited.at(start) = true;

        int_t count{};

        do
        {
            assert(count < std::numeric_limits<int_t>::max() - 1);
            ++count;

            do
            {
                const auto &cur = que.back();

                // todo(p3): no long division.
                const int_t row = cur / squared;
                const int_t col = cur % squared;

                const int_t from_x = row / size;
                const int_t from_y = row % size;
                const std::pair<int_t, int_t> from{ from_x, from_y };

                const int_t tod_x = col / size;
                const int_t tod_y = col % size;
                const std::pair<int_t, int_t> tod{ tod_x, tod_y };

                const auto cands_size = possible_knight_moves<int_t, int_t>(from, size, squared, tod, cands);

                for (std::uint32_t pos{}; pos < cands_size; ++pos)
                {
                    const auto &cand = cands.at(pos);

                    if (visited.at(cand))
                    {
                        continue;
                    }

                    if (cand == stop)
                    {
                        return count;
                    }

                    visited.at(cand) = true;
                    que_2.push_back(cand);
                }

                que.pop_back();
            } while (!que.empty());

            std::swap(que, que_2);
        } while (!que.empty());

        assert(0);

        throw std::runtime_error("bfs_min_knights_moves impossible error.");
    }
} // namespace Standard::Algorithms::Numbers::Inner

namespace Standard::Algorithms::Numbers
{
    // Compute min moves to swap 2 noble knights between 2 distinct 2D points, where both x, y are in [0..n-1], n >= 3.
    // In a move, a knight can move to a free point, where another knight is not standing.
    // todo(p5): time, space O(1).

    // Slow time O(n*n).
    template<std::signed_integral long_int_t, std::signed_integral int_t>
    requires(sizeof(int_t) <= sizeof(long_int_t))
    [[nodiscard]] constexpr auto swap_knights_min_moves(
        const std::pair<int_t, int_t> &from, const int_t &size, const std::pair<int_t, int_t> &tod) -> long_int_t
    {
        Inner::check_knights<int_t>(from, size, tod);

        assert(from != tod);

        constexpr long_int_t zero{};
        constexpr long_int_t one = 1;

        auto best = std::numeric_limits<long_int_t>::max();

        auto compa = [&size, &best](const std::pair<int_t, int_t> &from_2, const auto &vert,
                         const std::pair<int_t, int_t> &tod_2, const auto &horiz)
        {
            const auto cand_xxx = static_cast<long_int_t>(from_2.first) + vert;
            const auto cand_yyy = static_cast<long_int_t>(from_2.second) + horiz;

            if ((cand_xxx < zero) || (cand_yyy < zero) || !(cand_xxx < size) || !(cand_yyy < size) ||
                (cand_xxx == tod_2.first && cand_yyy == tod_2.second))
            {
                return;
            }

            const std::pair<int_t, int_t> skip{ static_cast<int_t>(cand_xxx), static_cast<int_t>(cand_yyy) };

            // from -> skip, +1 move
            // tod -> from and omitting the skip
            // skip -> tod and omitting the from
            const auto cost =
                static_cast<long_int_t>(one + knight_min_moves_slow<long_int_t, int_t>(tod_2, size, from_2, skip));

            assert(one < cost);

            const auto full_cost =
                static_cast<long_int_t>(cost + knight_min_moves_slow<long_int_t, int_t>(skip, size, tod_2, from_2));

            assert(cost < full_cost);

            best = std::min(best, full_cost);
        };

        constexpr auto cand_moves = Inner::knight_moves<long_int_t>();

        for (const auto &[vert, horiz] : cand_moves)
        {
            // size 5, (0, 0), (3, 4).
            // Using only the direct computation, the best is 8 whereas it should have been 6!
            // A x x x x // Begin
            // x x x x x
            // x x x x x
            // x x x x B
            // x x x x x
            // (0, 0) moves to (2, 1) which is the only best move possible.
            // x x x x x // move 1
            // x x x ? x
            // x A x x x
            // x x x x B
            // x x ? x x
            // Now A(2, 1) is 2 moves away from (3, 4) via (1, 3) or (4, 2).
            // But B(3, 4) cannot move to (0, 0) in 3 steps with (2, 1) taken - it needs 5 steps, summing to 8.
            compa(from, vert, tod, horiz);

            // A x x x x // Begin
            // x x x x x
            // x x x x x
            // x x x x B
            // x x x x x
            //
            // A x x x x // move 1
            // x x x B x
            // x ? x x x
            // x x x x *
            // x x ! x x
            //
            // Here B(1, 3) sticks whereas A moves to (3, 4) following "?!*".
            // Then, B goes to "?" and finally reaches (0, 0), 6 moves in total.
            compa(tod, vert, from, horiz);
        }

        assert(zero < best && best < std::numeric_limits<long_int_t>::max());

        return best;
    }

    // Move from the initial to the desired position.
    // Slow time, space O(n**4).
    template<std::signed_integral long_int_t, std::signed_integral int_t>
    requires(sizeof(int_t) <= sizeof(long_int_t))
    [[nodiscard]] constexpr auto swap_knights_min_moves_slow_still(
        const std::pair<int_t, int_t> &from, const int_t &size, const std::pair<int_t, int_t> &tod) -> long_int_t
    {
        Inner::check_knights<int_t>(from, size, tod);

        assert(from != tod);

        const auto squared = static_cast<long_int_t>(static_cast<long_int_t>(size) * size);
        assert(size < squared && squared < 1LL * squared * squared);

        const auto start = Inner::two_points_index<long_int_t, int_t>(from, size, squared, tod);

        const auto stop = Inner::two_points_index<long_int_t, int_t>(tod, size, squared, from);

        auto count = Inner::bfs_min_knights_moves<long_int_t>(start, size, squared, stop);

        return count;
    }

    // Very slow time O(64**ans).
    template<std::signed_integral long_int_t, std::signed_integral int_t, class pair_hash_t>
    requires(sizeof(int_t) <= sizeof(long_int_t))
    [[nodiscard]] constexpr auto swap_knights_min_moves_slow(const std::pair<int_t, int_t> &from, const int_t &size,
        const std::pair<int_t, int_t> &tod) -> std::pair<long_int_t, std::int64_t>
    {
        Inner::check_knights<int_t>(from, size, tod);

        assert(from != tod);

        std::unordered_set<std::pair<int_t, int_t>, pair_hash_t> moves0;
        moves0.insert(from);

        std::unordered_set<std::pair<int_t, int_t>, pair_hash_t> moves1;
        moves1.insert(tod);

        std::int64_t calls{};
        auto best = std::numeric_limits<long_int_t>::max();

        Inner::swap_knights_min_moves_rec_slow<long_int_t, int_t>(
            moves0, size, from, moves1, tod, best, tod, calls, from);

        assert(best < std::numeric_limits<long_int_t>::max());

        return { best, calls };
    }
} // namespace Standard::Algorithms::Numbers
