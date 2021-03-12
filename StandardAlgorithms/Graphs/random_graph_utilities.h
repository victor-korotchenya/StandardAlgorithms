#pragma once
#include <algorithm>
#include <unordered_set>
#include <vector>
#include "../Utilities/RandomGenerator.h"
#include "../Numbers/hash_utilities.h"

namespace Privet::Algorithms::Graphs
{
    template<class int_t>
    constexpr int_t graph_edges(int64_t nodes, bool is_undirected = true, int64_t limit_edges = 0)
    {
        const auto complete_graph_edges = nodes * (nodes - 1) >> (is_undirected ? 1 : 0);

        const auto max_edges = limit_edges > 0 && limit_edges <= complete_graph_edges
            ? limit_edges : complete_graph_edges;

        const auto edges = static_cast<int_t>(max_edges);

        assert(nodes >= 0 && limit_edges >= 0 && complete_graph_edges >= 0 && max_edges >= 0 && edges >= 0);
        return edges;
    }

    // Nodes are [0..n-1].
    template<class add_edge_t, class edge_t, class int_t, int_t max_nodes, bool is_undirected = true, int_t min_nodes = 1, int_t limit_edges = 0>
    std::pair<std::vector<std::vector<edge_t>>, int_t> create_random_graph_base(add_edge_t& add_edge)
    {
        constexpr auto max_edges_total = graph_edges<int_t>(max_nodes, is_undirected, limit_edges);

        static_assert(min_nodes >= 0 && min_nodes <= max_nodes && max_edges_total >= 0);

        RandomGenerator<int_t> r;
        const int_t node_count = r(min_nodes, max_nodes),
            max_edges = graph_edges<int_t>(node_count, is_undirected, max_edges_total);

        assert(node_count >= 0 && max_edges >= 0);
        const int_t edge_count = r(int_t(), max_edges);

        std::vector<std::vector<edge_t>> result(node_count);

        using pair_t = std::pair<int_t, int_t>;
        std::unordered_set<pair_t, std::pair_hash> edges;

        while (static_cast<int_t>(edges.size()) < edge_count)
        {
            int_t from = r(0, node_count - 1),
                to;
            do
            {
                to = r(0, node_count - 1);
            } while (from == to);

            if (is_undirected && to < from)
                std::swap(to, from);

            if (!edges.insert(pair_t(from, to)).second)
                continue;

            add_edge(result, from, to);
        }

        return { result, edge_count };
    }

    // Nodes are [0..n-1].
    template<class int_t = int, int_t max_nodes = 10, bool is_undirected = true, int_t min_nodes = 1, int_t limit_edges = 0>
    std::pair<std::vector<std::vector<int_t>>, int_t> create_random_graph()
    {
        const auto add_edge = [](std::vector<std::vector<int_t>>& graph,
            const int_t from, const int_t to)
        {
            graph[from].push_back(to);
            if (is_undirected)
                graph[to].push_back(from);
        };

        const auto result = create_random_graph_base<decltype(add_edge), int_t, int_t, max_nodes, is_undirected, min_nodes, limit_edges>(add_edge);
        return result;
    }

    // Nodes are [0..n-1].
    template<class edge_t, class weight_t, class int_t = int, int_t max_nodes = 10, bool is_undirected = true, int_t min_nodes = 1, int_t limit_edges = 0>
    std::pair<std::vector<std::vector<edge_t>>, int_t> create_random_graph_weighted(const weight_t min_weight, const weight_t max_weight)
    {
        assert(min_weight <= max_weight);
        RandomGenerator<weight_t> rw;

        const auto add_edge = [&rw, min_weight, max_weight](std::vector<std::vector<edge_t>>& graph,
            const int_t from, const int_t to)
        {
            const weight_t w = rw(min_weight, max_weight);
            graph[from].emplace_back(to, w);
            if (is_undirected)
                graph[to].emplace_back(from, w);
        };

        const auto result = create_random_graph_base<decltype(add_edge), edge_t, int_t, max_nodes, is_undirected, min_nodes, limit_edges>(add_edge);
        return result;
    }
}