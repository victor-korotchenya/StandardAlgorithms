#pragma once
#include <algorithm>
#include <cassert>
#include<memory>
#include<unordered_map>
#include <utility>
#include <vector>
#include "../Utilities/ExceptionUtilities.h"
#include "point.h"
#include "../Utilities/VectorUtilities.h"

namespace
{
    // Whether smaller lies entirely inside larger.
    template<class rectangle_t>
    bool is_entirely_inside(const rectangle_t& smaller, const rectangle_t& larger)
    {
        const auto r = larger.first.X <= smaller.first.X &&
            larger.first.Y <= smaller.first.Y &&
            smaller.second.X <= larger.second.X &&
            smaller.second.Y <= larger.second.Y;
        return r;
    }

    //todo: p3. del rectangle_cross?
    //template<class rectangle_t>
    //bool rectangle_cross(const rectangle_t &smaller, const rectangle_t &larger)
    //{
    //    {
    //        const auto ax = std::max(smaller.first.X, larger.first.X);
    //        const auto ix = std::min(smaller.second.X, larger.second.X);
    //        if (ix < ax)
    //            return false;
    //    }
    //    const auto ay = std::max(smaller.first.Y, larger.first.Y);
    //    const auto iy = std::min(smaller.second.Y, larger.second.Y);
    //    return ay <= iy;
    //}

    template<class rectangle_t>
    void remove_useless(std::vector<rectangle_t>& rectangles)
    {
        const auto size = static_cast<int>(rectangles.size());
        assert(size > 0);

        std::sort(rectangles.begin(), rectangles.end());

        auto good = 1;
        auto changed = false;
        for (auto i = 1; i < size; ++i)
        {
            assert(good > 0);
            const auto& cur = rectangles[i];
            // Case 1. Current is inside previous - ignore current.
            // 1   12
            // 1   1
            {
                const auto& prev = rectangles[good - 1];
                if (is_entirely_inside(cur, prev))
                {
                    changed = true;
                    continue;
                }
            }

            // Case 2. Previous is inside current - remove previous ones.
            // 13   3
            // 123  23
            do
            {
                assert(good > 0);
                const auto& prev = rectangles[good - 1];
                if (!is_entirely_inside(prev, cur))
                    break;

                changed = true;
            } while (--good);

            if (changed)
            {
                assert(good < i);
                rectangles[good] = rectangles[i];
            }

            ++good;
        }

        assert(good > 0);
        rectangles.resize(good);

#if _DEBUG
        Privet::Algorithms::VectorUtilities::RequireArrayIsSorted(rectangles, "rectangles");
#endif
    }

    template <class v_t, class map_t>
    void to_map(std::vector<v_t>& source, map_t& target)
    {
        assert(source.size() >= 2);
        Privet::Algorithms::VectorUtilities::sort_remove_duplicates(source);
        assert(source.size() >= 2);

        target.clear();
        target.reserve(source.size());

        auto id = -1;
        for (const auto& k : source)
        {
            target[k] = ++id;
        }

        assert(source.size() >= 2 && source.size() == target.size());
    }

    template<class coord_t, class rectangle_t, class coord_map_t>
    void compress_coordinates(const std::vector<rectangle_t>& rectangles,
        coord_map_t& x_to_id,
        coord_map_t& y_to_id,
        std::vector<coord_t>& id_to_x,
        std::vector<coord_t>& id_to_y)
    {
        const auto size = static_cast<int>(rectangles.size());
        assert(size > 0);

        id_to_x.clear();
        id_to_x.reserve(size << 1);

        id_to_y.clear();
        id_to_y.reserve(size << 1);

        for (const auto& rectangle : rectangles)
        {
            id_to_x.push_back(rectangle.first.X);
            id_to_x.push_back(rectangle.second.X);

            id_to_y.push_back(rectangle.first.Y);
            id_to_y.push_back(rectangle.second.Y);
        }

        to_map<coord_t, coord_map_t>(id_to_x, x_to_id);
        to_map<coord_t, coord_map_t>(id_to_y, y_to_id);
    }

    template <class tree_t, class coord_t>
    std::unique_ptr<tree_t> create_tree(const std::vector<coord_t>& id_to_y)
    {
        const auto y_size = static_cast<int>(id_to_y.size());
        assert(y_size >= 2);

        std::vector<coord_t> widths(y_size - 1);
        for (auto i = 0; i < y_size - 1; ++i)
        {
            widths[i] = id_to_y[i + 1] - id_to_y[i];
            assert(widths[i] > 0);
        }

        auto tree = std::make_unique<tree_t>(std::move(widths));
        return tree;
    }

    template <class coord_t, class rectangle_t, class coord_map_t>
    void form_events(const std::vector<rectangle_t>& rectangles,
        coord_map_t& x_to_id,
        std::vector<std::vector<int>>& add_events,
        std::vector<std::vector<int>>& remove_events)
    {
        {
            const auto x_size = static_cast<int>(x_to_id.size());
            assert(x_size >= 2);

            add_events.clear();
            add_events.resize(x_size);

            remove_events.clear();
            remove_events.resize(x_size);
        }

        const auto size = static_cast<int>(rectangles.size());
        assert(size > 0);

        for (auto i = 0; i < size; ++i)
        {
            const auto& rectangle = rectangles[i];
            const auto& x0 = rectangle.first.X,
                & x1 = rectangle.second.X;

            assert(x_to_id.count(x0) && x_to_id.count(x1));
            const auto id0 = x_to_id[x0],
                id1 = x_to_id[x1];

            add_events[id0].push_back(i);
            remove_events[id1].push_back(i);
        }
    }

    template <class coord_t, class long_int_t>
    coord_t process_area(long_int_t& result,
        const std::vector<coord_t>& id_to_x,
        const coord_t height,
        const coord_t prev_x, const int id)
    {
        static_assert(sizeof(coord_t) <= sizeof(long_int_t));
        const auto x = id_to_x[id];
        assert(!id || x > prev_x);

        const long_int_t width = x - prev_x,
            ar = static_cast<long_int_t>(height) * width;
        result += ar;
        assert(result >= 0 && height >= 0 && width >= 0 && ar >= 0);
        assert(!id || result > 0);
        return x;
    }

    template <class tree_t, class coord_t, class rectangle_t, class coord_map_t>
    void process_events(const std::vector<rectangle_t>& rectangles,
        const std::vector<int>& add_events,
        const std::vector<int>& remove_events,
        coord_map_t& y_to_id,
        tree_t& tree,
        coord_t& height)
    {
        assert(add_events.size() || remove_events.size());

        for (const auto& ev : add_events)
        {
            const auto& rectangle = rectangles[ev];
            const auto& y0 = rectangle.first.Y,
                & y1 = rectangle.second.Y;
            const auto i0 = y_to_id[y0],
                i1 = y_to_id[y1];
            assert(y0 < y1);

            const auto ad = tree.add(i0, i1);
            height += ad;
            assert(height > 0 && ad >= 0);
        }

        for (const auto& ev : remove_events)
        {
            const auto& rectangle = rectangles[ev];
            const auto& y0 = rectangle.first.Y,
                & y1 = rectangle.second.Y;
            const auto i0 = y_to_id[y0],
                i1 = y_to_id[y1];
            assert(y0 < y1);

            const auto r = tree.remove(i0, i1);
            height -= r;
            assert(height >= 0 && r >= 0);
        }
    }
}

namespace Privet
{
    namespace Algorithms
    {
        namespace Geometry
        {
            // Find the area, covered by rectangles, in O(n * log(n)).
            // Each rectangle consists of 2 points: {min x, min y}, {max x, max y},
            //  and has a positive area.
            // Points have non-negative x, y.
            template<class tree_t, // tree_t.add and remove are O(log(n)).
                class coord_t = int,
                class long_int_t = int64_t,
                class point_t = point2d<coord_t>,
                class rectangle_t = std::pair<point_t, point_t>,
                class coord_map_t = std::unordered_map<coord_t, int>>
                long_int_t rectangle_common_area(
                    // The rectangles might be edited.
                    std::vector<rectangle_t> rectangles)
            {
                static_assert(sizeof(coord_t) <= sizeof(long_int_t));
                RequirePositive(rectangles.size(), "rectangles.size");
                remove_useless<rectangle_t>(rectangles);

                std::vector<coord_t> id_to_x;
                coord_map_t y_to_id;
                std::vector<std::vector<int>> add_events, remove_events;
                std::unique_ptr<tree_t> tree_p;
                {
                    std::vector<coord_t> id_to_y;
                    {
                        coord_map_t x_to_id;
                        compress_coordinates<coord_t, rectangle_t, coord_map_t>(rectangles, x_to_id, y_to_id, id_to_x, id_to_y);

                        form_events<coord_t, rectangle_t, coord_map_t>(rectangles, x_to_id, add_events, remove_events);
                    }
                    tree_p = create_tree<tree_t, coord_t>(id_to_y);
                }

                const auto x_size = static_cast<int>(id_to_x.size());
                assert(x_size > 0 && tree_p);

                auto& tree = *tree_p.get();
                long_int_t result = 0;
                coord_t height = 0,
                    prev_x = rectangles[0].first.X;
                for (auto id = 0; ; ++id)
                {
                    const auto x = process_area<coord_t, long_int_t>(result, id_to_x, height, prev_x, id);
                    prev_x = x;
                    if (id == x_size - 1)
                        break;

                    process_events<tree_t, coord_t, rectangle_t, coord_map_t>(rectangles,
                        add_events[id], remove_events[id], y_to_id, tree, height);
                }

                assert(result > 0);
                return result;
            }

            // Slow time = max(x) * max(y).
            template<class long_int_t = int64_t, class point_t = point2d<int>,
                class rectangle_t = std::pair<point_t, point_t>>
                long_int_t rectangle_common_area_slow(const std::vector<rectangle_t>& rectangles)
            {
                const auto size = static_cast<int>(rectangles.size());
                RequirePositive(size, "rectangles.size");

                auto x_max = rectangles[0].second.X;
                auto y_max = rectangles[0].second.Y;
                for (const auto& r : rectangles)
                {
                    x_max = std::max(x_max, r.second.X);
                    y_max = std::max(y_max, r.second.Y);
                }

                assert(x_max > 0 && y_max > 0 && x_max + 1 > 0 && y_max + 1 > 0);
                std::vector<std::vector<bool>> buf(x_max + 1, std::vector<bool>(y_max + 1));

                for (const auto& r : rectangles)
                {
                    const auto& p0 = r.first, & p1 = r.second;
                    const auto& x0 = p0.X, & x1 = p1.X,
                        & y0 = p0.Y, & y1 = p1.Y;
                    assert(x0 < x1&& y0 < y1);

                    for (auto x = x0; x < x1; ++x)
                    {
                        auto& b = buf[x];
                        for (auto y = y0; y < y1; ++y)
                            b[y] = true;
                    }
                }

                long_int_t result = 0;
                for (auto x = x_max * 0; x <= x_max; ++x)
                {
                    const auto& b = buf[x];
                    for (auto y = y_max * 0; y <= y_max; ++y)
                        result += b[y];
                }

                assert(result > 0);
                return result;
            }
        }
    }
}