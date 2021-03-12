#pragma once
#include <vector>
#include "../Utilities/ExceptionUtilities.h"

namespace
{
    template<class float_t, class point_t, class distance_t>
    float_t min_length_2d_bitonic_tour_rec3(const std::vector<point_t>& points, distance_t& distance_func, std::vector<std::vector<std::vector<float_t>>>& buf,
        // Ending vertex of the left->right path
        const int left_end = {},
        // Starting vertex of the right->left path.
        const int right_start = {},
        // Current vertex to be added to either left->right, or back path.
        const int cur = 1)
    {
        auto& best = buf[left_end][right_start][cur];
        if (best != std::numeric_limits<float_t>::max() / 3)
            return best;

        const auto lc = distance_func(points[left_end], points[cur]),
            rc = distance_func(points[right_start], points[cur]);

        if (cur + 1 == static_cast<int>(points.size()))
            return best = static_cast<float_t>(lc + rc);

        // LR = left_end + cur. RL stays right_start.
        const auto left_full = lc + min_length_2d_bitonic_tour_rec3<float_t, point_t, distance_t>(points, distance_func, buf, cur, right_start, cur + 1);

        // LR stays left_end. RL = cur + right_start.
        const auto right_full = rc + min_length_2d_bitonic_tour_rec3<float_t, point_t, distance_t>(points, distance_func, buf, left_end, cur, cur + 1);

        best = std::min<float_t>(left_full, right_full);
        assert(best < std::numeric_limits<float_t>::max() / 3);
        return best;
    }

    template<class float_t, class point_t, class distance_t>
    float_t min_length_2d_bitonic_tour_rec(const std::vector<point_t>& points, distance_t& distance_func, std::vector<std::vector<float_t>>& buf,
        const int left_end = {}, const int right_start = {})
    {
        assert(left_end < static_cast<int>(points.size()) - 1 && right_start < static_cast<int>(points.size()) - 1);

        auto& best = buf[left_end][right_start];
        if (best != std::numeric_limits<float_t>::max() / 3)
            return best;

        // n times speed up.
        const auto cur = 1 + std::max(left_end, right_start);

        const auto lc = distance_func(points[left_end], points[cur]),
            rc = distance_func(points[right_start], points[cur]);

        if (cur + 1 == static_cast<int>(points.size()))
            return best = static_cast<float_t>(lc + rc);

        // LR = left_end + cur. RL stays right_start.
        const auto left = lc + min_length_2d_bitonic_tour_rec<float_t, point_t, distance_t>(points, distance_func, buf, cur, right_start);

        // LR stays left_end. RL = cur + right_start.
        const auto right = rc + min_length_2d_bitonic_tour_rec<float_t, point_t, distance_t>(points, distance_func, buf, left_end, cur);

        best = std::min<float_t>(left, right);
        assert(best < std::numeric_limits<float_t>::max() / 3);
        return best;
    }
}

namespace Privet::Algorithms::Geometry
{
    // Find a shortest bitonic tour. Unlike TSP traveling salesperson, time is O(n*n).
    // The points must be sorted.
    // A bitonic tour:
    // - starts from (ends at) the leftmost bottom point,
    // - goes to the rightmost top point never stepping back,
    // - then gets back to the beginning visiting those points, that have not been visited previously, never moving forward.
    template<class float_t, class point_t, class distance_t>
    std::pair<float_t, std::vector<int>> // Min distance, point indexes
        min_length_2d_bitonic_tour(const std::vector<point_t>& points, distance_t& distance_func)
    {
        // static_assert(std::is_floating_point_v<float_t>);
        assert(std::is_sorted(points.begin(), points.end()));

        const auto size = static_cast<int>(points.size());
        RequirePositive(size, "points.size");

        std::vector<int> indexes(size);
        if (size == 1)
            return { float_t(), indexes };

        //if (size == 2)
        //{
        //    indexes[1] = 1;
        //    const auto di = distance_func(points[0], points[1]) * 2;
        //    return { di, indexes };
        //}

        // {left->right end point, right->left end point} -> distance
        std::vector<std::vector<float_t>> buf(size,
            std::vector<float_t>(size, std::numeric_limits<float_t>::max() / 3));

        const auto min_distance = min_length_2d_bitonic_tour_rec<float_t, point_t, distance_t>(points, distance_func, buf);

        // todo: p3. do indexes.
        assert(min_distance < std::numeric_limits<float_t>::max() / 3);
        return { min_distance, indexes };
    }

    // Slow O(n**3) time, O(n*n) space.
    template<class float_t, class point_t, class distance_t>
    std::pair<float_t, std::vector<int>> // Min distance, point indexes
        min_length_2d_bitonic_tour_slow_they2(const std::vector<point_t>& points, distance_t& distance_func)
    {
        // static_assert(std::is_floating_point_v<float_t>);
        assert(std::is_sorted(points.begin(), points.end()));

        const auto size = static_cast<int>(points.size());
        RequirePositive(size, "points.size");

        std::vector<int> indexes(size);
        if (size == 1)
            return { float_t(), indexes };

        // left_right index, right_left index -> distance
        std::vector<std::vector<float_t>> buf(size,
            std::vector<float_t>(size, std::numeric_limits<float_t>::max() / 3));
        {
            auto& b = buf[0];
            b[0] = float_t();

            for (auto i = 1; i < size; ++i)
                b[i] = b[i - 1] + distance_func(points[i - 1], points[i]);
        }

        for (auto left = 1; left < size; ++left)
        {
            for (auto right = left; right < size; ++right)
            {
                float_t mi;
                if (left < right - 1)
                {
                    const auto& pr = buf[left][right - 1];
                    const auto d = distance_func(points[right - 1], points[right]);
                    mi = static_cast<float_t>(pr + d);
                }
                else
                {
                    mi = std::numeric_limits<float_t>::max();
                    for (auto k = 0; k < left; ++k)
                    {
                        const auto& pr = buf[k][left];
                        const auto d = distance_func(points[k], points[right]);
                        const auto di = static_cast<float_t>(pr + d);
                        mi = std::min(di, mi);
                    }
                }

                buf[left][right] = mi;
            }
        }

        const auto& min_distance = buf.back().back();
        assert(min_distance < std::numeric_limits<float_t>::max() / 3);
        // todo: p3. do indexes.
        return { min_distance, indexes };
    }

    // Slow O(n**3) time,space.
    template<class float_t, class point_t, class distance_t>
    std::pair<float_t, std::vector<int>> // Min distance, point indexes
        min_length_2d_bitonic_tour_slow3(const std::vector<point_t>& points, distance_t& distance_func)
    {
        // static_assert(std::is_floating_point_v<float_t>);
        assert(std::is_sorted(points.begin(), points.end()));

        const auto size = static_cast<int>(points.size());
        RequirePositive(size, "points.size");

        std::vector<int> indexes(size);
        if (size == 1)
            return { float_t(), indexes };

        // left, right,cur -> distance
        std::vector<std::vector<std::vector<float_t>>> buf(size,
            std::vector<std::vector<float_t>>(size,
                std::vector<float_t>(size, std::numeric_limits<float_t>::max() / 3)));

        const auto min_distance = min_length_2d_bitonic_tour_rec3<float_t, point_t, distance_t>(points, distance_func, buf);

        // todo: p3. do indexes.
        assert(min_distance < std::numeric_limits<float_t>::max() / 3);
        return { min_distance, indexes };
    }

    // Slow O(2**n * n) time.
    template<class float_t, class point_t, class distance_t>
    std::pair<float_t, std::vector<int>>
        min_length_2d_bitonic_tour_slow(const std::vector<point_t>& points, distance_t& distance_func)
    {
        // static_assert(std::is_floating_point_v<float_t>);
        assert(std::is_sorted(points.begin(), points.end()));

        const auto size = static_cast<int>(points.size());
        assert(size <= 20);
        RequirePositive(size, "points.size");

        std::vector<int> indexes(size);
        if (size == 1)
            return { float_t(), indexes };

        const auto mask_edge = 1u << size;
        assert(mask_edge > 0u);

        float_t min_distance = std::numeric_limits<float_t>::max();
        auto excluded = indexes;

        for (auto mask = 1u; mask < mask_edge; ++mask) // Choose some points.
        {
            float_t cand{};
            excluded.resize(1);

            // Always choose 1st/last points in left->right and back.
            const auto mask2 = mask | (1 << (size - 1));

            for (auto i = 1, prev = 0; i < size; ++i)
            {
                if ((mask2 >> i) & 1u)
                {
                    const auto di = distance_func(points[prev], points[i]);
                    cand += di;
                    prev = i;
                }
                else
                    excluded.push_back(i);
            }

            excluded.push_back(size - 1);
            for (auto i = 0; i < static_cast<int>(excluded.size()) - 1; ++i)
            {
                const auto& prev = excluded[i], & cur = excluded[i + 1];
                const auto di = distance_func(points[prev], points[cur]);
                cand += di;
            }

            if (min_distance <= cand)
                continue;

            min_distance = cand;
            indexes.resize(1);

            for (auto i = 1; i < size - 1; ++i)
                if ((mask >> i) & 1u)
                    indexes.push_back(i);

            indexes.push_back(size - 1);
        }

        assert(min_distance < std::numeric_limits<float_t>::max());
        return { min_distance, indexes };
    }
}