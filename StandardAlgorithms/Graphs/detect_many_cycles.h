#pragma once
#include"../Utilities/check_size.h"
#include"../Utilities/require_utilities.h"
#include<array>
#include<vector>

namespace Standard::Algorithms::Graphs::Inner
{
    template<class graph_t, std::signed_integral vertex_t>
    struct det_8_cyc_dfs_context final
    {
        const graph_t &graph;
        std::vector<std::vector<vertex_t>> &cycles;
        std::vector<vertex_t> &depths;
        std::vector<vertex_t> &sta;
    };

    template<class graph_t, std::signed_integral vertex_t>
    constexpr void find_cycle(const vertex_t par, det_8_cyc_dfs_context<graph_t, vertex_t> &context, const vertex_t cur)
    {
        const auto &depth = context.depths.at(cur);
        assert(!(depth < 0));

        context.sta[depth] = cur;

        const auto &nodes = context.graph[cur];

        for (const auto &child : nodes)
        {
            if (child == par)
            {
                continue;
            }

            auto &child_depth = context.depths[child];
            if (child_depth < 0)
            {
                child_depth = depth + 1;
                find_cycle(cur, context, child);
                continue;
            }

            if (depth < child_depth)
            {
                continue;
            }

            context.cycles.push_back({});
            auto &cycle = context.cycles.back();

            for (auto dep = child_depth; dep <= depth; ++dep)
            {
                cycle.push_back(context.sta[dep]);
            }
        }
    }

    template<std::integral vertex_t>
    [[nodiscard]] constexpr auto has_undir_cycle_impl(
        const auto &graph, auto &sta, std::vector<bool> &visited, vertex_t node) -> bool
    {
        const auto size = static_cast<vertex_t>(graph.size());

        assert(vertex_t{} <= node && node < size && !graph.at(node).empty() && !visited.at(node));

        sta.assign(1,
            std::make_pair(size, // no parent
                std::make_pair(node, vertex_t{})));
        visited.at(node) = true;

        for (vertex_t parent{}, position{};;)
        {
            assert(!sta.empty());

            parent = sta.back().first;
            node = sta.back().second.first;
            position = sta.back().second.second;

            const auto &edges = graph.at(node);
            const auto &tod = edges.at(position);

            assert(vertex_t{} <= parent && vertex_t{} <= node && node < size && vertex_t{} <= position &&
                position < static_cast<vertex_t>(edges.size()) &&
                // No self-edges.
                node != tod);

            sta.pop_back();

            {
                const auto &edges_to = graph.at(tod);

                throw_if_empty("Invalid graph: must be an outgoing edge from the vertex " +
                        ::Standard::Algorithms::Utilities::zu_string(tod),
                    edges_to);
            }

            const auto is_parent = parent == tod;

            if (const auto is_back_edge = !is_parent && visited.at(tod); is_back_edge)
            {// Got a cycle.
                return true;
            }

            visited.at(tod) = true; // A tree-edge (node, tod).
            ++position;

            assert(vertex_t{} < position);

            if (const auto shall_push = position < static_cast<vertex_t>(edges.size()); shall_push)
            {
                sta.emplace_back(parent, std::make_pair(node, position));
            }

            if (!is_parent)
            {
                sta.emplace_back(node, std::make_pair(tod, vertex_t{}));
                continue;
            }

            if (sta.empty())
            {
                return false;
            }

            if constexpr (!::Standard::Algorithms::is_debug)
            {
                continue;
            }

            node = sta.back().second.first;
            position = sta.back().second.second;

            [[maybe_unused]] const auto color = visited.at(node);
            [[maybe_unused]] const auto &edges_2 = graph.at(node);

            assert(color && position < static_cast<vertex_t>(edges_2.size()));
        }
    }
} // namespace Standard::Algorithms::Graphs::Inner

namespace Standard::Algorithms::Graphs
{
    // An undirected simple graph might have only simple cycles, each having >= 3 nodes.
    // Each simple cycle consists of distinct vertices.
    // Nodes can start from 0 or 1.
    //
    // See also digraph_cycle_detector.
    template<std::signed_integral vertex_t>
    constexpr void detect_many_cycles(const std::vector<std::vector<vertex_t>> &graph,
        std::vector<vertex_t> &depths, // temp
        std::vector<std::vector<vertex_t>> &cycles,
        std::vector<vertex_t> &sta, // temp
        const vertex_t first_node = 0)
    {
        const auto size = static_cast<vertex_t>(graph.size());
        assert(!(first_node < 0) && first_node <= 1 && 0 < size);

        cycles.clear();
        depths.assign(graph.size(), -1);

        if constexpr (::Standard::Algorithms::is_debug)
        {
            sta.assign(size, -2);
        }
        else
        {
            sta.resize(size);
        }

        Inner::det_8_cyc_dfs_context<std::vector<std::vector<vertex_t>>, vertex_t> context{ graph, cycles, depths,
            sta };

        for (auto node = first_node; node < size; ++node)
        {
            auto &depth = context.depths[node];
            if (depth < 0)
            {
                depth = {};

                constexpr auto parent = -1;
                Inner::find_cycle(parent, context, node);
            }
        }
    }

    // Since cross- or back-edges are impossible in an undirected graph,
    // a back-edge in DFS indicates there is a cycle.
    // Self- and multi-edges should not be in an undirected graph.
    // Time O(n), n is the vertices number.
    template<std::integral vertex_t>
    [[nodiscard]] constexpr auto undirected_has_cycle(const std::vector<std::vector<vertex_t>> &graph) -> bool
    {
        const auto size = Standard::Algorithms::Utilities::check_size<vertex_t>("graph size", graph.size());

        if (size <= 1)
        {
            return false;
        }

        using from_position_t = std::pair<vertex_t, vertex_t>;
        using parent_from_position_t = std::pair<vertex_t, from_position_t>;

        std::vector<parent_from_position_t> sta;
        sta.reserve(size);

        std::vector<bool> visited(size);

        for (vertex_t root{}; root < size; ++root)
        {
            if (visited[root] || graph.at(root).empty())
            {
                continue;
            }

            if (Inner::has_undir_cycle_impl<vertex_t>(graph, sta, visited, root))
            {
                return true;
            }
        }

        return false;
    }
} // namespace Standard::Algorithms::Graphs
