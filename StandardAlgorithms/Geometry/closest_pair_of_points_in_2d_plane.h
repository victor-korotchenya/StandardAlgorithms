#pragma once
#include"../Numbers/arithmetic.h"
#include"../Numbers/shift.h"
#include"../Utilities/require_utilities.h"
#include<set>
#include<vector>

namespace Standard::Algorithms::Geometry::Inner
{
    template<class point_t, class distance_t, class distance_t1, class best_t>
    struct closest_pair_context final
    {
        constexpr closest_pair_context(
            const std::vector<point_t> &points, distance_t &distance_func, distance_t1 &distance_func1)
            : Points(points)
            , Distance_func(distance_func)
            , Distance_func1(distance_func1)
            , Buffers(Standard::Algorithms::Numbers::most_significant_bit64(points.size() + 1LLU) + 2LL)
            , Best({ distance_func(points[0], points[1]), { points[0], points[1] } })
        {
        }

        [[nodiscard]] constexpr auto points() const &noexcept -> const std::vector<point_t> &
        {
            return Points;
        }

        [[nodiscard]] constexpr auto distance_func() &noexcept -> distance_t &
        {
            return Distance_func;
        }

        [[nodiscard]] constexpr auto distance_func1() &noexcept -> distance_t1 &
        {
            return Distance_func1;
        }

        [[nodiscard]] constexpr auto buffers() &noexcept -> std::vector<std::vector<point_t>> &
        {
            return Buffers;
        }

        [[nodiscard]] constexpr auto best() &noexcept -> best_t &
        {
            return Best;
        }

private:
        const std::vector<point_t> &Points;
        distance_t &Distance_func;
        distance_t1 &Distance_func1;

        std::vector<std::vector<point_t>> Buffers;
        best_t Best;
    };

    // Divide & conquer.
    template<class floating_t, class point_t, class distance_t, class context_t>
    constexpr void closest_pair_of_points_rec(context_t &context, const std::vector<point_t> &y_points,
        const std::int32_t beg, const std::int32_t end, const std::int16_t height = 0)
    {
        const auto size = end - beg;

        assert(!(beg < 0) && 1 < size && size == static_cast<std::int32_t>(y_points.size()) && height < 100);

        // if constexpr (::Standard::Algorithms::is_debug)
        // {
        //         for (auto index = 1U; index < y_points.size(); ++index)
        //         {
        //             const auto &prev = y_points[index - 1U], &cur = y_points[index];
        //             assert(prev.y < cur.y || prev.y == cur.y && prev.x < cur.x);
        //         }
        // }

        if (size <= 3)
        {
            closest_pair_of_points_in_2d_plane_slow<floating_t, point_t, distance_t>(
                context.points(), context.distance_func(), beg, end, context.best());
            return;
        }

        const auto mid = (size - 1) / 2 + beg;
        const auto &mid_point = context.points()[mid];

        if (static_cast<std::int32_t>(context.buffers().size()) <= height)
        {
            if constexpr (::Standard::Algorithms::is_debug)
            {
                assert(0);
            }

            context.buffers().resize(height + 1LL);
        }

        auto &buf = context.buffers()[height];
        buf.clear();

        std::copy_if(y_points.cbegin(), y_points.cend(), std::back_inserter(buf),
            [mid_point](const point_t &pnt)
            {
                const auto has = pnt.x < mid_point.x || (pnt.x == mid_point.x && pnt.y <= mid_point.y);
                return has;
            });

        closest_pair_of_points_rec<floating_t, point_t, distance_t, context_t>(context, buf, beg, mid + 1, height + 1);

        buf.clear();

        std::copy_if(y_points.cbegin(), y_points.cend(), std::back_inserter(buf),
            [mid_point](const point_t &pnt)
            {
                const auto has = !(pnt.x < mid_point.x || (pnt.x == mid_point.x && pnt.y <= mid_point.y));
                return has;
            });

        closest_pair_of_points_rec<floating_t, point_t, distance_t, context_t>(context, buf, mid + 1, end, height + 1);

        buf.clear();

        std::copy_if(y_points.cbegin(), y_points.cend(), std::back_inserter(buf),
            [&context, mid_point](const point_t &pnt)
            {
                return context.distance_func1()(mid_point.x - pnt.x) < context.best().first;
            });

        assert(!buf.empty());

        for (auto ite = buf.cbegin(); ite != buf.cend() - 1; ++ite)
        {
            for (auto jter2 = ite + 1;
                 jter2 != buf.cend() && context.distance_func1()(jter2->y - ite->y) < context.best().first; ++jter2)
            {
                const auto cand = context.distance_func()(*jter2, *ite);

                if (cand < context.best().first)
                {
                    context.best() = { cand, { *jter2, *ite } };
                }
            }
        }
    }
} // namespace Standard::Algorithms::Geometry::Inner

namespace Standard::Algorithms::Geometry
{
    // Sweep line - the code is simple and fast.
    //
    // Given n>= 2 distinct sorted points, find the closest/shortest/min/nearest neighbor distance between 2 points in
    // O(n*log(n)). Optim note. Distance might be computed as (x*x + y*y), and sqrt(minimum) taken in the last step. It
    // is 300 times faster than slow on n=100K.
    template<class floating_t, class point_t, class distance_t, class distance_t1, class int_t = decltype(point_t::y),
        class best_t = std::pair<floating_t, std::pair<point_t, point_t>>>
    constexpr auto closest_pair_sweep_line(
        const std::vector<point_t> &points, distance_t &distance_func, distance_t1 &distance_func1) -> best_t
    {
        assert(std::is_sorted(points.cbegin(), points.cend()));

        const auto size = points.size();
        require_greater(size, 1U, "points.size");

        best_t best = { distance_func(points[0], points[1]), { points[0], points[1] } };

        std::set<point_t> cands;
        int_t dif{};
        auto changed = true;

        for (std::size_t back{}, front{}; front < size; ++front)
        {
            const auto &cur = points[front];

            while (back < size && best.first <= distance_func1(cur.x - points[back].x))
            {
                const auto &prev = points[back];
                cands.erase(point_t{ prev.y, prev.x });
                ++back;
            }

            if (changed)
            {
                constexpr auto small_coef = 1.01;
                dif = static_cast<int_t>(std::sqrt(best.first) * small_coef);
                changed = false;
            }

            const auto high = cands.upper_bound({ static_cast<int_t>(cur.y + dif), cur.x });
            auto low = cands.lower_bound({ static_cast<int_t>(cur.y - dif), cur.x });

            const point_t rev{ cur.y, cur.x };

            while (low != high)
            {
                const auto dis = distance_func(rev, *low);
                if (dis < best.first)
                {
                    best = { dis, { cur, *low } };
                    changed = true;
                }

                ++low;
            }

            cands.insert(rev);
        }

        return best;
    }

    // Slow time O(n*n).
    template<class floating_t, class point_t, class distance_t,
        class best_t = std::pair<floating_t, std::pair<point_t, point_t>>>
    constexpr void closest_pair_of_points_in_2d_plane_slow(const std::vector<point_t> &points,
        distance_t &distance_func, const std::int32_t beg, const std::int32_t end, best_t &best)
    {
        assert(1U < points.size() && !(beg < 0) && 2 <= end - beg && end <= static_cast<std::int32_t>(points.size()));

        // todo(p2): best = ????

        for (auto index = beg; index + 1LL < end; ++index)
        {
            for (auto ind_2 = index + 1; ind_2 < end; ++ind_2)
            {
                const auto cand = distance_func(points[index], points[ind_2]);
                if (cand < best.first)
                {
                    best = { cand, { points[index], points[ind_2] } };
                }
            }
        }

        assert(best.first < std::numeric_limits<floating_t>::max());
    }

    // Not fastest, a lot of code, using divide and conquer.
    template<class floating_t, class point_t, class distance_t, class distance_t1,
        class best_t = std::pair<floating_t, std::pair<point_t, point_t>>>
    constexpr auto closest_pair_of_points_in_2d_plane_slow_still(
        const std::vector<point_t> &points, distance_t &distance_func, distance_t1 &distance_func1) -> best_t
    {
        assert(std::is_sorted(points.cbegin(), points.cend()));

        const auto size = static_cast<std::int32_t>(points.size());
        require_greater(size, 1, "points.size");

        auto y_points = points;
        std::sort(y_points.begin(), y_points.end(),
            [] [[nodiscard]] (const point_t &one, const point_t &two)
            {
                return one.y < two.y || (one.y == two.y && one.x < two.x);
            });

        using context_t = Inner::closest_pair_context<point_t, distance_t, distance_t1, best_t>;
        context_t context(points, distance_func, distance_func1);

        const auto &result = context.best();

        Inner::closest_pair_of_points_rec<floating_t, point_t, distance_t, context_t>(context, y_points, 0, size);

        assert(result.first < std::numeric_limits<floating_t>::max());
        return result;
    }
} // namespace Standard::Algorithms::Geometry
