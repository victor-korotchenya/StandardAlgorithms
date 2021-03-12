#pragma once
#include"../Utilities/require_utilities.h"
#include"geometry_utilities.h"
#include"point.h"
#include<cmath>
#include<cstddef>
#include<vector>

namespace Standard::Algorithms::Geometry
{
    namespace Inner
    {
        template<class point_t, class floating_t, class cost_mid_t>
        constexpr void set_costs_single_edge(const std::vector<point_t> &polygon, const floating_t &inf,
            std::vector<std::vector<cost_mid_t>> &buffers, const std::size_t size)
        {
            for (std::size_t from{}; from < size; ++from)
            {
                const auto &one = polygon[from];
                auto &buf = buffers[from];

                for (std::size_t to{}; to < from; ++to)
                {
                    const auto &two = polygon[to];
                    auto &cost = buf[to].first;
                    cost = distance<floating_t>(two - one);

                    assert(!(cost < floating_t{}) && cost < inf);
                }

                if (const auto single_edge_to = from + 1U; single_edge_to < size)
                {
                    buf[single_edge_to].first = floating_t{};
                }
            }
        }

        template<class floating_t, class cost_mid_t>
        constexpr void set_costs_triangles(
            const floating_t &inf, std::vector<std::vector<cost_mid_t>> &buffers, const std::size_t size)
        {
            for (std::size_t index{}; index + 2U < size; ++index)
            {
                const auto &edge01_cost = buffers[index + 1U][index].first;
                const auto &edge02_cost = buffers[index + 2U][index].first;
                const auto &edge12_cost = buffers[index + 2U][index + 1U].first;

                auto &cost = buffers[index][index + 2U].first;
                cost = static_cast<floating_t>(edge01_cost + edge02_cost + edge12_cost);

                assert(!(std::min({ cost, edge01_cost, edge02_cost, edge12_cost }) < floating_t{}) &&
                    std::max({ cost, edge01_cost, edge02_cost, edge12_cost }) < inf);
            }
        }
    } // namespace Inner

    // Given a convex polygon, find minimum weight (cost) triangulation,
    // where the weight is the sum of all triangle edge lengths.
    template<class point_t, class floating_t>
    constexpr auto convex_min_weight_triangulation_slow(const std::vector<point_t> &polygon, const floating_t &inf,
        std::vector<std::pair<std::size_t, std::size_t>> &cross_edges) -> floating_t
    {
        assert(floating_t{} < inf);

        cross_edges.clear();

        const auto size = polygon.size();
        if (const auto no_cuts = size < 3U; no_cuts)
        {
            return {};
        }

        using cost_mid_t = std::pair<floating_t, std::size_t>;

        // from, to_incl -> cost_mid_t
        std::vector<std::vector<cost_mid_t>> buffers(size, std::vector<cost_mid_t>(size, cost_mid_t(inf, 0U)));

        // Store the single edge costs in the lower triangle
        // e.g. buffers[3][2] is the edge {2, 3} length.
        Inner::set_costs_single_edge<point_t, floating_t, cost_mid_t>(polygon, inf, buffers, size);

        Inner::set_costs_triangles<floating_t, cost_mid_t>(inf, buffers, size);

        //     b   -   e   -   mid   -   g   -   k
        //    /                                            \;
        // from                  -                      to
        //
        // The mid is in the range [(from + 1)..(to - 1)].
        // Subparts of a consecutive [from .. to] figure:
        // - triangle { from, mid, to }.
        // - consecutive p-angle { from - b .. e - mid }.
        // - consecutive q-angle { mid - g .. k - to }.
        //
        // For example, the cost of the polygon [10..20] with mid = 15 includes:
        // - triangle { 10, 15, 20 }.
        // - consecutive p-angle { 10..15 }.
        // - consecutive q-angle { 15..20 }.
        //
        // However, when mid = 11, the p-angle { 10, 11 } is not even a triangle.
        // To simplify the code (fewer ifs),
        // it is convenient to set the cost of an edge { index, index + 1 } to be 0
        // for all valid indexes in [0..(size - 2)] in the method set_costs_single_edge().

        for (std::size_t length = 4; length <= size; ++length)
        {
            for (std::size_t from{}; from <= size - length; ++from)
            {
                const auto to_incl = from + length - 1U;
                const auto edge02_cost = buffers[to_incl][from].first;

                auto &par = buffers[from][to_incl];
                auto &cost = par.first;

                assert(!(edge02_cost < floating_t{}) && edge02_cost < inf && cost == inf);

                for (auto mid = from + 1U; mid < to_incl; ++mid)
                {
                    const auto &edge01_cost = buffers[mid][from].first;
                    const auto &edge12_cost = buffers[to_incl][mid].first;
                    const auto &left = buffers[from][mid].first;
                    const auto &right = buffers[mid][to_incl].first;

                    const auto cand = static_cast<floating_t>(edge01_cost + edge02_cost + edge12_cost + left + right);

                    assert(!(std::min({ edge01_cost, edge12_cost, left, right, cand }) < floating_t{}) &&
                        std::max({ edge01_cost, edge12_cost, left, right, cand }) < inf);

                    if (cand < cost)
                    {
                        auto &best_mid = par.second;
                        cost = cand, best_mid = mid;
                    }
                }
            }
        }

        const auto &best = buffers[0][size - 1U];
        const auto &best_cost = best.first;

        assert(!(best_cost < floating_t{}) && best_cost < inf);

        // todo(p2): restore the edges using a queue.

        return best_cost;
    }
} // namespace Standard::Algorithms::Geometry
