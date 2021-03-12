#pragma once
#include"../Utilities/require_utilities.h"
#include<limits>
#include<vector>

namespace Standard::Algorithms::Geometry::Inner
{
    template<class floating_t>
    [[nodiscard]] constexpr auto unknown_cost() -> floating_t
    {
        return static_cast<floating_t>(std::numeric_limits<floating_t>::max() / 3);
    }

    template<class floating_t, class point_t, class distance_t>
    [[nodiscard]] constexpr auto min_length_2d_bitonic_tour_rec3(const std::vector<point_t> &points,
        distance_t &distance_func, std::vector<std::vector<std::vector<floating_t>>> &buf,
        // Ending vertex of the left->right path
        const std::uint32_t left_end = {},
        // Starting vertex of the right->left path.
        const std::uint32_t right_start = {},
        // Current vertex to be added to either left->right, or back path.
        const std::uint32_t cur = 1) -> floating_t
    {
        constexpr auto unknown = unknown_cost<floating_t>();
        static_assert(floating_t{} < unknown);

        auto &best = buf[left_end][right_start][cur];

        if (best != unknown)
        {// todo(p3): avoid func call.
            return best;
        }

        const auto left = distance_func(points[left_end], points[cur]);
        const auto right = distance_func(points[right_start], points[cur]);

        if (cur + 1U == points.size())
        {
            return best = static_cast<floating_t>(left + right);
        }

        // LR = left_end + cur. RL stays right_start.
        const auto left_full = left +
            min_length_2d_bitonic_tour_rec3<floating_t, point_t, distance_t>(
                points, distance_func, buf, cur, right_start, cur + 1);

        // LR stays left_end. RL = cur + right_start.
        const auto right_full = right +
            min_length_2d_bitonic_tour_rec3<floating_t, point_t, distance_t>(
                points, distance_func, buf, left_end, cur, cur + 1U);

        best = std::min<floating_t>(left_full, right_full);
        assert(best < unknown);
        return best;
    }

    template<class floating_t, class point_t, class distance_t>
    [[nodiscard]] constexpr auto min_length_2d_bitonic_tour_rec(const std::vector<point_t> &points,
        distance_t &distance_func, std::vector<std::vector<floating_t>> &buf, const std::uint32_t left_end = {},
        const std::uint32_t right_start = {}) -> floating_t
    {
        constexpr auto unknown = unknown_cost<floating_t>();
        static_assert(floating_t{} < unknown);

        assert(left_end < points.size() - 1U && right_start < points.size() - 1U);

        auto &best = buf[left_end][right_start];
        if (best != unknown)
        {
            return best;
        }

        // n times speed up.
        const auto cur = 1U + std::max(left_end, right_start);

        const auto lco = distance_func(points[left_end], points[cur]);
        const auto rco = distance_func(points[right_start], points[cur]);

        if (cur + 1U == points.size())
        {
            return best = static_cast<floating_t>(lco + rco);
        }

        // LR = left_end + cur. RL stays right_start.
        const auto left = lco +
            min_length_2d_bitonic_tour_rec<floating_t, point_t, distance_t>(
                points, distance_func, buf, cur, right_start);

        // LR stays left_end. RL = cur + right_start.
        const auto right = rco +
            min_length_2d_bitonic_tour_rec<floating_t, point_t, distance_t>(points, distance_func, buf, left_end, cur);

        best = std::min<floating_t>(left, right);
        assert(best < unknown);
        return best;
    }
} // namespace Standard::Algorithms::Geometry::Inner

namespace Standard::Algorithms::Geometry
{
    // Find a shortest bitonic tour. Unlike TSP traveling salesperson, time is O(n*n).
    // The points must be sorted.
    // A bitonic tour:
    // - starts from (ends at) the leftmost bottom point,
    // - goes to the rightmost top point never stepping back,
    // - then gets back to the beginning visiting those points, that have not been visited previously,
    // never moving forward.
    template<class floating_t, class point_t, class distance_t>
    [[nodiscard]] constexpr auto min_length_2d_bitonic_tour(const std::vector<point_t> &points,
        distance_t &distance_func) -> std::pair<floating_t, std::vector<std::int32_t>> // Min distance, point indexes
    {
        assert(std::is_sorted(points.cbegin(), points.cend()));

        const auto size = points.size();
        require_positive(size, "points.size");

        std::vector<std::int32_t> indexes(size);
        if (size == 1U)
        {
            return { floating_t{}, std::move(indexes) };
        }

        // if (size == 2U)
        //{
        //     indexes[1] = 1;
        //     const auto dis = distance_func(points[0], points[1]) * 2;
        //     return { dis, std::move(indexes) };
        // }

        constexpr auto unknown = Inner::unknown_cost<floating_t>();

        // {left->right end point, right->left end point} -> distance
        std::vector<std::vector<floating_t>> buf(size, std::vector<floating_t>(size, unknown));

        const auto min_distance =
            Inner::min_length_2d_bitonic_tour_rec<floating_t, point_t, distance_t>(points, distance_func, buf);

        // todo(p3): do indexes.
        assert(min_distance < unknown);
        return { min_distance, std::move(indexes) };
    }

    // Slow time O(n**3), O(n*n) space.
    template<class floating_t, class point_t, class distance_t>
    [[nodiscard]] constexpr auto min_length_2d_bitonic_tour_slow2(const std::vector<point_t> &points,
        distance_t &distance_func) -> std::pair<floating_t, std::vector<std::int32_t>> // Min distance, point indexes
    {
        assert(std::is_sorted(points.cbegin(), points.cend()));

        const auto size = points.size();
        require_positive(size, "points.size");

        std::vector<std::int32_t> indexes(size);
        if (size == 1U)
        {
            return { floating_t{}, std::move(indexes) };
        }

        constexpr auto unknown = Inner::unknown_cost<floating_t>();

        // left_right index, right_left index -> distance
        std::vector<std::vector<floating_t>> buf(size, std::vector<floating_t>(size, unknown));
        {
            auto &bests = buf[0];
            bests[0] = floating_t{};

            for (auto index = 1U; index < size; ++index)
            {
                bests[index] = bests[index - 1U] + distance_func(points[index - 1U], points[index]);
            }
        }

        for (auto left = 1U; left < size; ++left)
        {
            for (auto right = left; right < size; ++right)
            {
                floating_t min1;
                if (left < right - 1U)
                {
                    const auto &pre = buf[left][right - 1U];
                    const auto dis = distance_func(points[right - 1U], points[right]);
                    min1 = static_cast<floating_t>(pre + dis);
                }
                else
                {
                    min1 = std::numeric_limits<floating_t>::max();

                    for (std::uint32_t ind3{}; ind3 < left; ++ind3)
                    {
                        const auto &pre = buf[ind3][left];
                        const auto dis1 = distance_func(points[ind3], points[right]);
                        const auto dis2 = static_cast<floating_t>(pre + dis1);
                        min1 = std::min(dis2, min1);
                    }
                }

                buf[left][right] = min1;
            }
        }

        const auto &min_distance = buf.back().back();
        assert(min_distance < unknown);
        // todo(p3): do indexes.
        return { min_distance, std::move(indexes) };
    }

    // Slow time O(n**3), space.
    template<class floating_t, class point_t, class distance_t>
    [[nodiscard]] constexpr auto min_length_2d_bitonic_tour_slow3(const std::vector<point_t> &points,
        distance_t &distance_func) -> std::pair<floating_t, std::vector<std::int32_t>> // Min distance, point indexes
    {
        assert(std::is_sorted(points.cbegin(), points.cend()));

        const auto size = points.size();
        require_positive(size, "points.size");

        std::vector<std::int32_t> indexes(size);
        if (size == 1U)
        {
            return { floating_t{}, std::move(indexes) };
        }

        constexpr auto unknown = Inner::unknown_cost<floating_t>();

        // left, right, cur -> distance
        std::vector<std::vector<std::vector<floating_t>>> buf(
            size, std::vector<std::vector<floating_t>>(size, std::vector<floating_t>(size, unknown)));

        const auto min_distance =
            Inner::min_length_2d_bitonic_tour_rec3<floating_t, point_t, distance_t>(points, distance_func, buf);

        // todo(p3): do indexes.
        assert(min_distance < unknown);
        return { min_distance, std::move(indexes) };
    }

    // Slow time O(2**n * n).
    template<class floating_t, class point_t, class distance_t>
    [[nodiscard]] constexpr auto min_length_2d_bitonic_tour_slow(const std::vector<point_t> &points,
        distance_t &distance_func) -> std::pair<floating_t, std::vector<std::int32_t>>
    {
        assert(std::is_sorted(points.cbegin(), points.cend()));

        const auto size = points.size();
        require_positive(size, "points size");
        assert(size <= 20U);

        std::vector<std::int32_t> indexes(size);
        if (size == 1U)
        {
            return { floating_t{}, std::move(indexes) };
        }

        const auto mask_edge = 1U << size;
        assert(0U < mask_edge);

        auto min_distance = std::numeric_limits<floating_t>::max();
        auto excluded = indexes;

        for (auto mask = 1U; mask < mask_edge; ++mask) // Choose some points.
        {
            floating_t cand{};
            excluded.resize(1);

            // Always choose 1st/last points in left->right and back.
            const auto mask2 = mask | (1U << (size - 1U));

            for (auto index = 1U, prev = 0U; index < size; ++index)
            {
                if ((mask2 >> index) & 1U)
                {
                    const auto dis = distance_func(points[prev], points[index]);
                    cand += dis;
                    prev = index;
                }
                else
                {
                    excluded.push_back(static_cast<std::int32_t>(index));
                }
            }

            excluded.push_back(static_cast<std::int32_t>(size - 1U));

            for (std::uint32_t index{}; index < excluded.size() - 1U; ++index)
            {
                const auto &prev = excluded[index];
                const auto &cur = excluded[index + 1LLU];
                const auto dis = distance_func(points[prev], points[cur]);
                cand += dis;
            }

            if (min_distance <= cand)
            {
                continue;
            }

            min_distance = cand;
            indexes.resize(1);

            for (std::uint32_t index = 1; index < size - 1U; ++index)
            {
                if ((mask >> index) & 1U)
                {
                    indexes.push_back(static_cast<std::int32_t>(index));
                }
            }

            indexes.push_back(static_cast<std::int32_t>(size - 1U));
        }

        assert(min_distance < std::numeric_limits<floating_t>::max());
        return { min_distance, std::move(indexes) };
    }
} // namespace Standard::Algorithms::Geometry
