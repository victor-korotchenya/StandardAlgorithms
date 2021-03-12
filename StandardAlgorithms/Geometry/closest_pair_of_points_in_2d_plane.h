#pragma once
#include <set>
#include <vector>
#include "../Utilities/ExceptionUtilities.h"
#include "../Numbers/Arithmetic.h"

namespace
{
    template<class float_t, class point_t, class distance_t, class distance_t1, class best_t>
    struct closest_pair_context final
    {
        const std::vector<point_t>& points;
        distance_t& distance_func;
        distance_t1& distance_func1;

        std::vector<std::vector<point_t>> bufs;
        best_t best;

        closest_pair_context(const std::vector<point_t>& points, distance_t& distance_func, distance_t1& distance_func1)
            : points(points), distance_func(distance_func), distance_func1(distance_func1),
            bufs(MostSignificantBit64(points.size() + 1llu) + 2llu),
            best({ distance_func(points[0], points[1]), { points[0], points[1] } })
        {
        }
    };

    template<class float_t, class point_t, class distance_t, class context_t>
    void closest_pair_of_points_rec(context_t& context, const std::vector<point_t>& y_points, const int be, const int en, const short height = 0)
    {
        const auto size = en - be;
        assert(be >= 0 && size >= 2 && size == static_cast<int>(y_points.size()) && height < 100);

        //#if _DEBUG
        //        for (auto i = 1u; i < y_points.size(); ++i)
        //        {
        //            const auto &prev = y_points[i - 1u], &cur = y_points[i];
        //            assert(prev.Y < cur.Y || prev.Y == cur.Y && prev.X < cur.X);
        //        }
        //#endif
        if (size <= 3)
        {
            closest_pair_of_points_in_2d_plane_slow<float_t, point_t, distance_t>(context.points, context.distance_func, be, en, context.best);
            return;
        }

        const auto mid = be + ((size - 1) >> 1);
        const auto& mid_point = context.points[mid];

        if (height >= static_cast<int>(context.bufs.size()))
        {
#if _DEBUG
            assert(0);
#endif
            context.bufs.resize(height + 1ll);
        }

        auto& buf = context.bufs[height];
        buf.clear();

        for (const auto& point : y_points)
        {
            if (point.X < mid_point.X ||
                point.X == mid_point.X && point.Y <= mid_point.Y)
                buf.push_back(point);
        }
        closest_pair_of_points_rec<float_t, point_t, distance_t, context_t>(context, buf, be, mid + 1, height + 1);

        buf.clear();
        for (const auto& point : y_points)
        {
            if (!(point.X < mid_point.X ||
                point.X == mid_point.X && point.Y <= mid_point.Y))
                buf.push_back(point);
        }
        closest_pair_of_points_rec<float_t, point_t, distance_t, context_t>(context, buf, mid + 1, en, height + 1);

        buf.clear();
        std::copy_if(y_points.begin(), y_points.end(), std::back_inserter(buf), [&context, mid_point](const point_t& p) {
            return context.distance_func1(mid_point.X - p.X) < context.best.first;
            });

        assert(buf.size());
        for (auto it = buf.begin(); it != buf.end() - 1; ++it)
        {
            for (auto jt = it + 1; jt != buf.end() && context.distance_func1(jt->Y - it->Y) < context.best.first; ++jt)
            {
                const auto cand = context.distance_func(*jt, *it);
                if (cand < context.best.first)
                    context.best = { cand, { *jt, *it } };
            }
        }
    }
}

namespace Privet::Algorithms::Geometry
{
    // Sweep line - the code is simple and fast.
    //
    // Given n>= 2 distinct sorted points, find the closest/shortest/min/nearest neighbor distance between 2 points in O(n*log(n)).
    // Opt note. Distance might be computed as (x*x + y*y), and sqrt(minimum) taken in the last step.
    // It is 300 times faster than slow on n=100K.
    template<class float_t, class point_t, class distance_t, class distance_t1,
        class int_t = decltype(point_t::Y),
        class best_t = std::pair<float_t, std::pair<point_t, point_t>>>
        best_t closest_pair_sweep_line(const std::vector<point_t>& points, distance_t& distance_func, distance_t1& distance_func1)
    {
        assert(std::is_sorted(points.begin(), points.end()));

        const auto size = points.size();
        RequireGreater(size, 1, "points.size");

        best_t best = { distance_func(points[0], points[1]), {points[0], points[1]} };

        std::set<point_t> cands;
        int_t d{};
        auto changed = true;

        for (size_t back{}, front{}; front < size; ++front)
        {
            const auto& cur = points[front];

            while (back < size && distance_func1(cur.X - points[back].X) >= best.first)
            {
                const auto& prev = points[back];
                cands.erase(point_t(prev.Y, prev.X));
                ++back;
            }

            if (changed)
            {
                changed = false;
                d = static_cast<int_t>(sqrt(best.first) * 1.01);
            }

            const auto hi = cands.upper_bound({ static_cast<int_t>(cur.Y + d), cur.X });
            auto low = cands.lower_bound({ static_cast<int_t>(cur.Y - d), cur.X });

            const point_t rev(cur.Y, cur.X);

            while (low != hi)
            {
                const auto di = distance_func(rev, *low);
                if (di < best.first)
                {
                    best = { di, {cur, *low} };
                    changed = true;
                }

                ++low;
            }

            cands.insert(rev);
        }

        return best;
    }

    // Slow O(n*n) time.
    template<class float_t, class point_t, class distance_t,
        class best_t = std::pair<float_t, std::pair<point_t, point_t>>>
        void closest_pair_of_points_in_2d_plane_slow(const std::vector<point_t>& points, distance_t& distance_func, const int be, const int en, best_t& best)
    {
        assert(points.size() >= 2 && be >= 0 && en - be >= 2 && en <= static_cast<int>(points.size()));

        for (auto i = be; i < en - 1; ++i)
        {
            for (auto j = i + 1; j < en; ++j)
            {
                const auto cand = distance_func(points[i], points[j]);
                if (cand < best.first)
                    best = { cand, {points[i], points[j]} };
            }
        }

        assert(best.first < std::numeric_limits<float_t>::max());
    }

    // Not fastest, a lot of code, using divide and conquer.
    template<class float_t, class point_t, class distance_t, class distance_t1,
        class best_t = std::pair<float_t, std::pair<point_t, point_t>>>
        best_t closest_pair_of_points_in_2d_plane_slow_still(const std::vector<point_t>& points, distance_t& distance_func, distance_t1& distance_func1)
    {
        assert(std::is_sorted(points.begin(), points.end()));

        const auto size = static_cast<int>(points.size());
        RequireGreater(size, 1, "points.size");

        auto y_points = points;
        std::sort(y_points.begin(), y_points.end(), [](const point_t& a, const point_t& b) {
            return a.Y < b.Y || a.Y == b.Y && a.X < b.X;
            });

        using context_t = closest_pair_context<float_t, point_t, distance_t, distance_t1, best_t>;
        context_t context(points, distance_func, distance_func1);
        const auto& result = context.best;

        closest_pair_of_points_rec<float_t, point_t, distance_t, context_t>(context, y_points, 0, size);

        assert(result.first < std::numeric_limits<float_t>::max());
        return result;
    }
}