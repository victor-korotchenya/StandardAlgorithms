#pragma once
#include"../Utilities/remove_duplicates.h"
#include"../Utilities/require_utilities.h"
#include"../Utilities/same_sign_leq_size.h"
#include"point.h"
#include<memory>
#include<unordered_map>

namespace Standard::Algorithms::Geometry::Inner
{
    // Whether smaller lies entirely inside larger.
    template<class rectangle_t>
    [[nodiscard]] constexpr auto is_entirely_inside(const rectangle_t &smaller, const rectangle_t &larger) noexcept
        -> bool
    {
        auto res = larger.first.x <= smaller.first.x && larger.first.y <= smaller.first.y &&
            smaller.second.x <= larger.second.x && smaller.second.y <= larger.second.y;

        return res;
    }

    // todo(p3): del rectangle_cross?
    // template<class rectangle_t>
    // [[nodiscard]] constexpr auto rectangle_cross(const rectangle_t &smaller,
    //     const rectangle_t &larger) noexcept -> bool
    //{
    //     {
    //         const auto ax = std::max(smaller.first.x, larger.first.x);
    //         const auto ix = std::min(smaller.second.x, larger.second.x);
    //         if (ix < ax)
    //             return false;
    //     }
    //     const auto ay = std::max(smaller.first.y, larger.first.y);
    //     const auto iy = std::min(smaller.second.y, larger.second.y);
    //     return ay <= iy;
    // }

    template<class rectangle_t>
    constexpr void remove_useless(std::vector<rectangle_t> &rectangles)
    {
        const auto size = static_cast<std::int32_t>(rectangles.size());
        assert(0 < size);

        std::sort(rectangles.begin(), rectangles.end());

        auto good = 1;
        auto changed = false;

        for (auto index = 1; index < size; ++index)
        {
            assert(0 < good);

            const auto &cur = rectangles[index];
            // Case 1. Current is inside previous - ignore current.
            // 1   12
            // 1   1
            {
                const auto &prev = rectangles[good - 1];
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
                assert(0 < good);

                const auto &prev = rectangles[good - 1];
                if (!is_entirely_inside(prev, cur))
                {
                    break;
                }

                changed = true;
            } while (0 < --good);

            if (changed)
            {
                assert(good < index);
                rectangles[good] = rectangles[index];
            }

            ++good;
        }

        assert(0 < good);
        rectangles.resize(good);

        if constexpr (::Standard::Algorithms::is_debug)
        {
            require_sorted(rectangles, "rectangles");
        }
    }

    // See also build_ranks.
    template<class v_t, class map_t, class int_t = std::int32_t>
    constexpr void to_map(std::vector<v_t> &source, map_t &destination)
    {
        Standard::Algorithms::sort_remove_duplicates(source);

        destination.clear();
        destination.reserve(source.size());

        constexpr auto minus_one = static_cast<int_t>(int_t{} - static_cast<int_t>(1));

        auto ide = minus_one;

        for (const auto &from : source)
        {
            destination[from] = ++ide;
        }

        assert(source.size() == destination.size());
    }

    // Compress or normalize coordinates. See also build_ranks.
    template<class coord_t, class rectangle_t, class coord_map_t>
    constexpr void compress_coordinates(const std::vector<rectangle_t> &rectangles, coord_map_t &x_to_id,
        coord_map_t &y_to_id, std::vector<coord_t> &id_to_x, std::vector<coord_t> &id_to_y)
    {
        const auto size = static_cast<std::int32_t>(rectangles.size());
        assert(0 < size);

        id_to_x.clear();
        id_to_x.reserve(size * 2LL);

        id_to_y.clear();
        id_to_y.reserve(size * 2LL);

        for (const auto &rectangle : rectangles)
        {
            id_to_x.push_back(rectangle.first.x);
            id_to_x.push_back(rectangle.second.x);

            id_to_y.push_back(rectangle.first.y);
            id_to_y.push_back(rectangle.second.y);
        }

        to_map<coord_t, coord_map_t>(id_to_x, x_to_id);
        to_map<coord_t, coord_map_t>(id_to_y, y_to_id);
    }

    template<class tree_t, class coord_t>
    [[nodiscard]] constexpr auto create_tree(const std::vector<coord_t> &id_to_y) -> std::unique_ptr<tree_t>
    {
        using long_int_t = typename tree_t::long_int_t;

        static_assert(sizeof(coord_t) <= sizeof(long_int_t));

        const auto y_size = static_cast<std::int32_t>(id_to_y.size());
        require_greater(y_size, 1, "y_size");

        std::vector<long_int_t> widths(y_size - 1);

        for (std::int32_t index{}; index < y_size - 1; ++index)
        {
            const auto &first = id_to_y[index];
            const auto &last = id_to_y[index + 1];

            auto &wid = widths[index];

            wid = static_cast<long_int_t>(static_cast<long_int_t>(last) - static_cast<long_int_t>(first));

            assert(first < last && long_int_t{} < wid);
        }

        auto tree = std::make_unique<tree_t>(std::move(widths));
        return tree;
    }

    template<class coord_t, class rectangle_t, class coord_map_t>
    constexpr void form_events(const std::vector<rectangle_t> &rectangles, coord_map_t &x_to_id,
        std::vector<std::vector<std::int32_t>> &add_events, std::vector<std::vector<std::int32_t>> &remove_events)
    {
        {
            const auto x_size = static_cast<std::int32_t>(x_to_id.size());
            assert(1 < x_size);

            add_events.clear();
            add_events.resize(x_size);

            remove_events.clear();
            remove_events.resize(x_size);
        }

        const auto size = static_cast<std::int32_t>(rectangles.size());
        assert(0 < size);

        for (std::int32_t index{}; index < size; ++index)
        {
            const auto &rectangle = rectangles[index];
            const auto &x_0 = rectangle.first.x;
            const auto &x_1 = rectangle.second.x;

            assert(x_to_id.contains(x_0) && x_to_id.contains(x_1));

            const auto id0 = x_to_id[x_0];
            const auto id1 = x_to_id[x_1];
            add_events[id0].push_back(index);
            remove_events[id1].push_back(index);
        }
    }

    template<class coord_t, class long_t>
    requires(sizeof(coord_t) <= sizeof(long_t))
    [[nodiscard]] constexpr auto process_area(long_t &result, const std::vector<coord_t> &id_to_x, const coord_t height,
        const std::int32_t ide, const coord_t prev_x) -> coord_t
    {
        const auto xxx = id_to_x[ide];
        assert(ide == 0 || prev_x < xxx);

        const auto width = static_cast<long_t>(xxx - prev_x);
        const auto are = static_cast<long_t>(static_cast<long_t>(height) * width);
        result += are;
        assert(!(result < long_t{}) && !(height < coord_t{}) && !(width < long_t{}) && !(are < long_t{}));

        assert(ide == 0 || long_t{} < result);

        return xxx;
    }

    template<class tree_t, class coord_t, class rectangle_t, class coord_map_t,
        class long_int_t = typename tree_t::long_int_t>
    constexpr void process_events(const std::vector<rectangle_t> &rectangles,
        const std::vector<std::int32_t> &add_events, const std::vector<std::int32_t> &remove_events,
        coord_map_t &y_to_id, tree_t &tree, coord_t &height)
    {
        assert(!add_events.empty() || !remove_events.empty());

        for (const auto &eve : add_events)
        {
            const auto &rectangle = rectangles[eve];
            const auto &yy0 = rectangle.first.y;
            const auto &yy1 = rectangle.second.y;
            assert(yy0 < yy1);

            const auto id0 = y_to_id[yy0];
            const auto id1 = y_to_id[yy1];
            const auto add = tree.add(id0, id1);
            height += add;

            assert(coord_t{} < height && !(add < long_int_t{}));
        }

        for (const auto &eve : remove_events)
        {
            const auto &rectangle = rectangles[eve];
            const auto &yy0 = rectangle.first.y;
            const auto &yy1 = rectangle.second.y;
            assert(yy0 < yy1);

            const auto id0 = y_to_id[yy0];
            const auto id1 = y_to_id[yy1];

            const auto rem = tree.remove(id0, id1);
            height -= rem;

            assert(!(height < coord_t{}) && !(rem < long_int_t{}));
        }
    }
} // namespace Standard::Algorithms::Geometry::Inner

namespace Standard::Algorithms::Geometry
{
    // Find the area, covered by rectangles, in O(n * log(n)).
    // Each rectangle consists of 2 points: {min x, min y}, {max x, max y},
    //  and has a positive area.
    // Points have non-negative x, y.
    template<class tree_t, // tree_t.add and remove are O(log(n)).
        std::integral coord_t, std::integral long_t, class point_t = point2d<coord_t>,
        class rectangle_t = std::pair<point_t, point_t>, class coord_map_t = std::unordered_map<coord_t, std::int32_t>>
    requires(same_sign_leq_size<coord_t, long_t>)
    [[nodiscard]] constexpr auto rectangle_common_area(
        // The rectangles might be edited.
        std::vector<rectangle_t> rectangles) -> long_t
    {
        require_positive(rectangles.size(), "rectangles.size");
        Inner::remove_useless<rectangle_t>(rectangles);

        std::vector<coord_t> id_to_x;
        coord_map_t y_to_id;

        std::vector<std::vector<std::int32_t>> add_events;
        std::vector<std::vector<std::int32_t>> remove_events;
        std::unique_ptr<tree_t> tree_p;
        {
            std::vector<coord_t> id_to_y;
            {
                coord_map_t x_to_id;
                Inner::compress_coordinates<coord_t, rectangle_t, coord_map_t>(
                    rectangles, x_to_id, y_to_id, id_to_x, id_to_y);

                Inner::form_events<coord_t, rectangle_t, coord_map_t>(rectangles, x_to_id, add_events, remove_events);
            }

            tree_p = Inner::create_tree<tree_t, coord_t>(id_to_y);
        }

        const auto x_size = static_cast<std::int32_t>(id_to_x.size());
        assert(0 < x_size && tree_p);

        auto &tree = *tree_p.get();
        long_t result{};
        coord_t height{};
        auto prev_x = rectangles[0].first.x;

        for (std::int32_t ide{};; ++ide)
        {
            const auto xxx = Inner::process_area<coord_t, long_t>(result, id_to_x, height, ide, prev_x);

            prev_x = xxx;
            if (ide == x_size - 1)
            {
                break;
            }

            Inner::process_events<tree_t, coord_t, rectangle_t, coord_map_t>(
                rectangles, add_events[ide], remove_events[ide], y_to_id, tree, height);
        }

        assert(long_t{} < result);

        return result;
    }

    // Slow time = max(x) * max(y).
    template<class long_t = std::int64_t, class point_t = point2d<std::int32_t>,
        class rectangle_t = std::pair<point_t, point_t>>
    [[nodiscard]] constexpr auto rectangle_common_area_slow(const std::vector<rectangle_t> &rectangles) -> long_t
    {
        const auto size = static_cast<std::int32_t>(rectangles.size());
        require_positive(size, "rectangles.size");

        using distance_t = typename point_t::distance_t;

        auto x_max = rectangles[0].second.x;
        auto y_max = rectangles[0].second.y;

        static_assert(sizeof(distance_t) == sizeof(decltype(x_max)) && sizeof(distance_t) == sizeof(decltype(y_max)));

        for (const auto &rec : rectangles)
        {
            x_max = std::max(x_max, rec.second.x);
            y_max = std::max(y_max, rec.second.y);
        }

        [[maybe_unused]] static constexpr distance_t zer{};

        assert(zer < x_max && zer < y_max && zer < x_max + 1 && zer < y_max + 1);

        std::vector<std::vector<bool>> buffers(x_max + 1LL, std::vector<bool>(y_max + 1LL));

        for (const auto &rec : rectangles)
        {
            const auto &fir = rec.first;
            const auto &sec = rec.second;

            const auto &xx0 = fir.x;
            const auto &xx1 = sec.x;
            const auto &yy0 = fir.y;
            const auto &yy1 = sec.y;
            assert(xx0 < xx1 && yy0 < yy1);

            for (auto xxx = xx0; xxx < xx1; ++xxx)
            {
                auto &buf = buffers[xxx];

                for (auto yyy = yy0; yyy < yy1; ++yyy)
                {
                    buf[yyy] = true;
                }
            }
        }

        long_t result{};

        for (distance_t xxx{}; xxx <= x_max; ++xxx)
        {
            const auto &buf = buffers[xxx];

            for (distance_t yyy{}; yyy <= y_max; ++yyy)
            {
                result += buf[yyy] ? 1 : 0;
            }
        }

        assert(long_t{} < result);
        return result;
    }
} // namespace Standard::Algorithms::Geometry
