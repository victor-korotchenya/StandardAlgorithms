#pragma once
#include <algorithm>
#include <limits>
#include <stdexcept>
#include <type_traits>
#include <vector>
#include "../Utilities/ExceptionUtilities.h"

namespace
{
    template <typename Number, typename LongNumber, typename Size, typename Pair>
    LongNumber calc_cost_slow(
        const std::vector<Pair>& rectangles,
        const std::vector<int>& pos)
    {
        LongNumber cost = 0;
        auto prev = 0;
        for (const auto& i : pos)
        {
            auto h = std::numeric_limits<Number>::min(), w = h;
            for (auto j = prev; j <= i; ++j)
            {
                h = std::max(h, rectangles[j].first);
                w = std::max(w, rectangles[j].second);
            }

            cost += static_cast<LongNumber>(h) * w;
            prev = i + 1;
        }

        return cost;
    }

    template<typename Number,
        typename LongNumber,
        typename Size,
        typename Pair>
        LongNumber ConvexHullTrick_slowImpl(
            const std::vector<Pair>& rectangles,
            std::vector<int>& pos,
            int cnt = 0)
    {
        if (cnt == static_cast<int>(rectangles.size()))
        {
            const auto last_missing = pos.empty() || pos.back() != cnt - 1;
            if (last_missing)
                pos.push_back(cnt - 1);

            const auto cost1 = calc_cost_slow<Number, LongNumber, Size, Pair>(rectangles, pos);
            if (last_missing)
                pos.pop_back();
            return cost1;
        }

        pos.push_back(cnt);
        const auto cost1 = ConvexHullTrick_slowImpl<Number, LongNumber, Size, Pair>(rectangles, pos, cnt + 1);
        pos.pop_back();

        const auto cost2 = ConvexHullTrick_slowImpl<Number, LongNumber, Size, Pair>(rectangles, pos, cnt + 1);
        const auto cost = std::min(cost1, cost2);
        return cost;
    }

    //The "first" will be  strongly increasing, "second" - decreasing.
    template<typename Size, typename Pair>
    void leave_only_useful(
        const Size size,
        std::vector<Pair>& rectangles)
    {
        std::sort(rectangles.begin(), rectangles.end());

        auto shall_write = false;
        Size i = 1, good = 1;
        while (i < size)
        {
            const auto& cur = rectangles[i];
            while (good &&
                rectangles[good - 1].second <= cur.second)
            {//remove useless.
                --good;
                shall_write = true;
            }

            if (shall_write)
                rectangles[good] = cur;

            ++i;
            ++good;
        }

        rectangles.resize(good);
    }

    //Whether the line l3 is always better than line l2.
    // <=>
    //Whether the cross_point(l1, l3) is lower than cross_point(l1, l2).
    //
    //  x=first=width decreases, y=second=cost increases.
    //
    // See the file "ConvexHullTrick.png".
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
    template<typename Number, typename LongNumber, typename Size, typename LongPair>
    bool shall_delete(const Size l1, const Size l2, const Size l3,
        const std::vector<LongPair>& slopes_y_intercepts)
    {
        // Given:  slopes1*x + intercept1 == slopes2*x + intercept2.
        // the intersection(l1, l2) has x-coordinate === y-intercept:
        // (intercept2 - intercept1)/(slopes1 - slopes2)
        //
        // (l1, l3).x = (intercept3 - intercept1)/(slopes1 - slopes3)
        //
        // Note that slopes1 > slopes2 > slopes3.
        // (l1, l3).x < (l1, l2).x when
        // (intercept3 - intercept1)/(slopes1 - slopes3) < (intercept2 - intercept1)/(slopes1 - slopes2)
        //
        // (intercept3 - intercept1)*(slopes1 - slopes2) < (intercept2 - intercept1)*(slopes1 - slopes3)

        const auto intercept31 = slopes_y_intercepts[l3].second - slopes_y_intercepts[l1].second;
        const auto intercept21 = slopes_y_intercepts[l2].second - slopes_y_intercepts[l1].second;

        const auto slope12 = slopes_y_intercepts[l1].first - slopes_y_intercepts[l2].first;
        const auto slope13 = slopes_y_intercepts[l1].first - slopes_y_intercepts[l3].first;

        auto prod3 = intercept31 * slope12;
        auto prod2 = intercept21 * slope13;

        const auto result = prod3 < prod2;
        return result;
    }

    //Add a new line (with lowest slope).
    template<typename Number, typename LongNumber, typename Size, typename LongPair>
    void add_line(const LongNumber slope, const LongNumber intercept,
        std::vector<LongPair>& slopes_y_intercepts)
    {
        slopes_y_intercepts.push_back(std::make_pair(slope, intercept));

        for (;;)
        {
            const auto size = static_cast<Size>(slopes_y_intercepts.size());
            if (size < 3 || !shall_delete<Number, LongNumber, Size, LongPair>(
                size - 3,
                size - 2,
                size - 1,
                slopes_y_intercepts))
            {
                break;
            }

            //antepenultimate == last - 2
            //penultimate == last - 1
            //ultimate == last

            //If the penultimate is now made irrelevant between the antepenultimate
            //and the ultimate, remove it.
            slopes_y_intercepts.erase(slopes_y_intercepts.end() - 2);
        }
    }

    template<typename LongNumber, typename LongPair>
    LongNumber value_at_x(const LongPair& slope_y_intercept, const LongNumber x)
    {
        const auto result = slope_y_intercept.first * x + slope_y_intercept.second;
        return result;
    }

    //Returns the minimum y-coordinate of any intersection between a given vertical
    //line and the lower envelope
    template<typename Number, typename LongNumber, typename Size, typename LongPair>
    LongNumber min_y_coordinate(
        const std::vector<LongPair>& slopes_y_intercepts,
        const LongNumber x,
        Size& bestLineFromPreviousQuery)
    {
        const auto size_minus1 = static_cast<Size>(slopes_y_intercepts.size() - 1);

        //If we removed what was the best line for the previous query, then the
        //newly inserted line is now the best for that query
        if (slopes_y_intercepts.size() <= bestLineFromPreviousQuery)
        {
            bestLineFromPreviousQuery = size_minus1;
        }
        else
        {
            //Any better line must be to the right, since query values are non-decreasing.
            while (bestLineFromPreviousQuery < size_minus1)
            {
                const auto next_val = value_at_x(slopes_y_intercepts[bestLineFromPreviousQuery + 1], x);
                const auto curr_val = value_at_x(slopes_y_intercepts[bestLineFromPreviousQuery], x);
                if (curr_val <= next_val)
                    return curr_val;
                ++bestLineFromPreviousQuery;
            }
        }

        const auto result = value_at_x(slopes_y_intercepts[bestLineFromPreviousQuery], x);
        return result;
    }
}

namespace Privet
{
    namespace Algorithms
    {
        namespace Geometry
        {
            //A "rectangle", represented by (both positive) sides width by height,
            // cannot be rotated.
            //The cost of a rectangle group is (max_width * max_height),
            // where max_width is the maximum width of some rectangle,
            // and max_height is the maximum height of the same or possibly another rectangle.
            //The aim is to group rectangles so that the total sum over all groups is minimized.
            //
            //Hints:
            //1) A rectangle, not greater by both sides than some other rectangle,
            // can be safely removed.
            //2) When sorted first by width, then by height,
            // groups must take contiguous subsets.
            //  Assume the sorted array is a0, a1, .., a(m-1).
            //   Then a0.width < a1.width < ... < a(m-1).width.
            //   Moreover, a(m-1).height < ... < a1.height < a0.height.
            //3) Convexity. Lines are already sorted in descending order of slope,
            // so that each line is added "at the right"; this is because we already sorted them by width.
            template<typename Number,
                //To avoid an overflow, LongNumber must be big enough to have Number*Number.
                typename LongNumber,
                //Use "unsigned" for smaller sizes.
                typename Size = size_t,
                typename Pair = std::pair<Number, Number>
            >
                LongNumber ConvexHullTrickSlow(
                    //Note that the input can be changed.
                    std::vector<Pair> rectangles)
            {
                RequirePositive(rectangles.size(), "Size of rectangles");

                std::sort(rectangles.begin(), rectangles.end());

                std::vector<int> pos;
                auto cost = ConvexHullTrick_slowImpl<Number, LongNumber, Size, Pair>(rectangles, pos);
                return cost;
            }

            template<typename Number,
                //To avoid an overflow, LongNumber must be big enough to have Number*Number.
                typename LongNumber,
                //Use "unsigned" for smaller sizes.
                typename Size = size_t,
                typename Pair = std::pair<Number, Number>
            >
                LongNumber ConvexHullTrick(
                    //Note that the input can be changed.
                    std::vector<Pair> rectangles)
            {
                static_assert(sizeof(Number) <= sizeof(LongNumber),
                    "sizeof(Number) <= sizeof(LongNumber)");

                static_assert(std::is_signed<Number>::value, "The Number must be signed.");
                static_assert(std::is_signed<LongNumber>::value, "The Long Number must be signed.");

                const size_t raw_size = rectangles.size();
                RequirePositive(raw_size, "Size of rectangles");
                if (std::numeric_limits<Size>::max() <= raw_size)
                {
                    throw std::runtime_error("The input size is too large to hold Size values.");
                }

                leave_only_useful<Size, Pair>(static_cast<Size>(raw_size), rectangles);

                using LongPair = std::pair<LongNumber, LongNumber>;
                std::vector<LongPair> slopes_y_intercepts;

                const auto size = static_cast<Size>(rectangles.size());
                // This can only increase as the slope decreases.
                Size bestLineFromPreviousQuery = 0;
#ifdef _DEBUG
                LongNumber prev_cost = 0;
#endif
                LongNumber cost = 0;
                for (Size i = 0; i < size; ++i)
                {
                    add_line<Number, LongNumber, Size, LongPair>(
                        rectangles[i].second, cost, slopes_y_intercepts);

                    cost = min_y_coordinate<Number, LongNumber, Size, LongPair>(
                        slopes_y_intercepts, rectangles[i].first, bestLineFromPreviousQuery);
#ifdef _DEBUG
                    if (cost <= prev_cost)
                        throw std::runtime_error("Error: cost <= prev_cost.");
                    prev_cost = cost;
#endif
                }

                return cost;
            }
        }
    }
}