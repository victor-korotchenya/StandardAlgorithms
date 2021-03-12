#pragma once
#include"../Utilities/require_utilities.h"
#include<vector>

namespace Standard::Algorithms::Geometry::Inner
{
    template<class int_t, class long_int_t, class pair_t>
    requires(sizeof(int_t) <= sizeof(long_int_t))
    [[nodiscard]] constexpr auto calc_cost_slow(
        const std::vector<pair_t> &rectangles, const std::vector<std::int32_t> &poss) -> long_int_t
    {
        constexpr auto mini = std::numeric_limits<int_t>::min();

        long_int_t cost{};
        std::int32_t prev{};

        for (const auto &cur : poss)
        {
            auto one = mini;
            auto two = mini;

            for (auto ind2 = prev; ind2 <= cur; ++ind2)
            {
                one = std::max<int_t>(one, rectangles[ind2].first);
                two = std::max<int_t>(two, rectangles[ind2].second);
            }

            cost += static_cast<long_int_t>(static_cast<long_int_t>(one) * two);
            prev = cur + 1;
        }

        return cost;
    }

    template<class int_t, class long_int_t, class pair_t>
    requires(sizeof(int_t) <= sizeof(long_int_t))
    [[nodiscard]] constexpr auto convex_hull_trick_slow_impl(
        const std::vector<pair_t> &rectangles, std::vector<std::int32_t> &pos, std::int32_t cnt = {}) -> long_int_t
    {
        if (cnt == static_cast<std::int32_t>(rectangles.size()))
        {
            const auto last_missing = pos.empty() || pos.back() != (cnt - 1);
            if (last_missing)
            {
                pos.push_back(cnt - 1);
            }

            auto cost1 = calc_cost_slow<int_t, long_int_t, pair_t>(rectangles, pos);

            if (last_missing)
            {
                pos.pop_back();
            }

            return cost1;
        }

        pos.push_back(cnt);

        const auto cost1 = convex_hull_trick_slow_impl<int_t, long_int_t, pair_t>(rectangles, pos, cnt + 1);
        pos.pop_back();

        const auto cost2 = convex_hull_trick_slow_impl<int_t, long_int_t, pair_t>(rectangles, pos, cnt + 1);

        auto cost = std::min(cost1, cost2);
        return cost;
    }

    // The "first" will be  strongly increasing, "second" - decreasing.
    template<class int_t, class pair_t>
    constexpr void leave_only_useful(const int_t &size, std::vector<pair_t> &rectangles)
    {
        std::sort(rectangles.begin(), rectangles.end());

        int_t index{ 1 };
        int_t good{ 1 };
        auto shall_write = false;

        while (index < size)
        {
            const auto &cur = rectangles[index];
            while (good && rectangles[good - 1].second <= cur.second)
            {// remove useless.
                --good;
                shall_write = true;
            }

            if (shall_write)
            {
                rectangles[good] = cur;
            }

            ++index, ++good;
        }

        rectangles.resize(good);
    }

    // Whether the line li3 is always better than line li2.
    // < = >
    // Whether the cross_point(li1, li3) is lower than cross_point(li1, li2).
    //
    //  x=first=width decreases, y=second=cost increases.
    //
    // See the file "convex_hull_trick.png".
    //
    //                p3'(stay)                p3 (min k of all prev; cheaper to delete p2)
    //
    //             p2 (middle k)
    //
    //
    //    p1 (max coef k in y=k*x+b)
    //
    //  /
    //
    //
    template<class long_int_t, class pos_t, class pair_t>
    [[nodiscard]] constexpr auto shall_delete(
        const pos_t &li1, const pos_t &li2, const pos_t &li3, const std::vector<pair_t> &slopes_y_intercepts) -> bool
    {
        // Given:  slopes1*x + intercept1 == slopes2*x + intercept2.
        // the intersection(li1, li2) has x-coordinate === y-intercept:
        // (intercept2 - intercept1)/(slopes1 - slopes2)
        //
        // (li1, li3).x = (intercept3 - intercept1)/(slopes1 - slopes3)
        //
        // Note that slopes3 < slopes2 < slopes1.
        // (li1, li3).x < (li1, li2).x when
        // (intercept3 - intercept1)/(slopes1 - slopes3) < (intercept2 - intercept1)/(slopes1 - slopes2)
        //
        // (intercept3 - intercept1)*(slopes1 - slopes2) < (intercept2 - intercept1)*(slopes1 - slopes3)

        const auto intercept31 = slopes_y_intercepts[li3].second - slopes_y_intercepts[li1].second;
        const auto intercept21 = slopes_y_intercepts[li2].second - slopes_y_intercepts[li1].second;

        const auto slope12 = slopes_y_intercepts[li1].first - slopes_y_intercepts[li2].first;
        const auto slope13 = slopes_y_intercepts[li1].first - slopes_y_intercepts[li3].first;

        auto prod3 = static_cast<long_int_t>(intercept31) * slope12;
        auto prod2 = static_cast<long_int_t>(intercept21) * slope13;

        auto result = prod3 < prod2;
        return result;
    }

    // Add a new line (with lowest slope).
    template<class long_int_t, class pos_t, class pair_t>
    constexpr void add_line(
        const long_int_t &slope, std::vector<pair_t> &slopes_y_intercepts, const long_int_t &intercept)
    {
        slopes_y_intercepts.emplace_back(slope, intercept);

        for (;;)
        {
            constexpr pos_t three{ 3 };

            const auto size = static_cast<pos_t>(slopes_y_intercepts.size());

            if (size < three ||
                !shall_delete<long_int_t, pos_t, pair_t>(size - three, size - 2, size - 1, slopes_y_intercepts))
            {
                return;
            }

            // antepenultimate == last - 2
            // penultimate == last - 1
            // ultimate == last

            // If the penultimate is now made irrelevant between the ante-penultimate
            // and the ultimate, remove it.
            slopes_y_intercepts.erase(slopes_y_intercepts.end() - 2);
        }
    }

    template<class int_t, class pair_t>
    [[nodiscard]] constexpr auto value_at_x(const pair_t &slope_y_intercept, const int_t &xxx) -> int_t
    {
        auto result = slope_y_intercept.first * xxx + slope_y_intercept.second;

        return result;
    }

    // Return the minimum y-coordinate of any intersection between a given vertical line and the lower envelope.
    template<class long_int_t, class pos_t, class pair_t>
    [[nodiscard]] constexpr auto min_y_coordinate(const long_int_t &xxx, const std::vector<pair_t> &slopes_y_intercepts,
        pos_t &best_line_from_previous_query) -> long_int_t
    {
        const auto size_minus1 = static_cast<pos_t>(slopes_y_intercepts.size() - 1ZU);

        // If we removed what was the best line for the previous query,
        // then the newly inserted line becomes the new best for that query.
        if (slopes_y_intercepts.size() <= best_line_from_previous_query)
        {
            best_line_from_previous_query = size_minus1;
        }
        else
        {// Any better line must be to the right, since query values are non-decreasing.
            constexpr pos_t one{ 1 };

            while (best_line_from_previous_query < size_minus1)
            {
                const auto next_val = value_at_x(slopes_y_intercepts[best_line_from_previous_query + one], xxx);
                const auto curr_val = value_at_x(slopes_y_intercepts[best_line_from_previous_query], xxx);

                if (!(next_val < curr_val))
                {
                    return curr_val;
                }

                ++best_line_from_previous_query;
            }
        }

        auto result = value_at_x(slopes_y_intercepts[best_line_from_previous_query], xxx);
        return result;
    }
} // namespace Standard::Algorithms::Geometry::Inner

namespace Standard::Algorithms::Geometry
{
    // A "rectangle", represented by (both positive) sides width by height, cannot be rotated.
    // The cost of a rectangle group is (max_width * max_height),
    // where max_width is the maximum width of some rectangle,
    // and max_height is the maximum height of the same or possibly another rectangle.
    // The aim is to group rectangles so that the total cost over all groups is minimized.
    //
    // Hints:
    // 1) A rectangle, not greater by both sides than some other rectangle,
    // can be safely removed.
    // 2) When sorted first by width, then by height,
    // groups must take contiguous subsets.
    //  Assume the sorted array is a0, a1, .., a(m-1).
    //   Then a0.width < a1.width < .. < a(m-1).width.
    //   Moreover, a(m-1).height < .. < a1.height < a0.height.
    // 3) Convexity. Lines are already sorted in descending order of slope,
    // so that each line is added "at the right"; this is because we already sorted them by width.
    template<class int_t,
        // To avoid an overflow, long_int_t must be big enough to hold a value of int_t*int_t.
        class long_int_t,
        // Use "std::uint32_t" for smaller sizes to save memory.
        class pos_t = std::size_t, class pair_t = std::pair<int_t, int_t>>
    requires(sizeof(int_t) <= sizeof(long_int_t))
    [[nodiscard]] constexpr auto convex_hull_trick_slow(
        // The input can be changed.
        std::vector<pair_t> rectangles) -> long_int_t
    {
        require_positive(rectangles.size(), "size of rectangles");

        std::sort(rectangles.begin(), rectangles.end());

        std::vector<std::int32_t> pos;
        auto cost = Inner::convex_hull_trick_slow_impl<int_t, long_int_t, pair_t>(rectangles, pos);
        return cost;
    }

    template<std::signed_integral int_t,
        // To avoid an overflow, long_int_t must be big enough to hold a value of int_t*int_t.
        std::signed_integral long_int_t,
        // Use "std::uint32_t" for smaller sizes.
        class pos_t = std::size_t, class pair_t = std::pair<int_t, int_t>,
        class long_pair_t = std::pair<long_int_t, long_int_t>>
    requires(sizeof(int_t) <= sizeof(long_int_t))
    [[nodiscard]] constexpr auto convex_hull_trick(
        // The input can be changed.
        std::vector<pair_t> rectangles) -> long_int_t
    {
        const auto raw_size = require_positive(rectangles.size(), "size of rectangles");

        if (std::numeric_limits<pos_t>::max() <= raw_size) [[unlikely]]
        {
            throw std::runtime_error("The input size is too large to hold pos_t values.");
        }

        Inner::leave_only_useful<pos_t, pair_t>(static_cast<pos_t>(raw_size), rectangles);

        const auto size = static_cast<pos_t>(rectangles.size());

        std::vector<long_pair_t> slopes_y_intercepts;

        // It can only increase as the slope decreases.
        pos_t best_line_from_previous_query{};

        [[maybe_unused]] long_int_t prev_cost{};
        long_int_t cost{};

        for (pos_t index{}; index < size; ++index)
        {
            Inner::add_line<long_int_t, pos_t, long_pair_t>(rectangles[index].second, slopes_y_intercepts, cost);

            cost = Inner::min_y_coordinate<long_int_t, pos_t, long_pair_t>(
                rectangles[index].first, slopes_y_intercepts, best_line_from_previous_query);

            if constexpr (::Standard::Algorithms::is_debug)
            {
                if (!(prev_cost < cost)) [[unlikely]]
                {
                    throw std::runtime_error("Error: the cost cannot decrease.");
                }

                prev_cost = cost;
            }
        }

        return cost;
    }
} // namespace Standard::Algorithms::Geometry
