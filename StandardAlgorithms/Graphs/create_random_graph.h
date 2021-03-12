#pragma once
// "create_random_graph.h"
#include"../Numbers/hash_utilities.h"
#include"../Numbers/shift.h"
#include"../Utilities/random.h"
#include<unordered_set>
#include<vector>

// See also "graph_builder.h".
namespace Standard::Algorithms::Graphs
{
    constexpr auto defaut_max_nodes = 10;
    constexpr auto defaut_max_weight = 50;

    template<std::integral int_t>
    [[nodiscard]] constexpr auto count_graph_edges(
        std::int64_t nodes, bool is_undirected = true, std::int64_t limit_edges = 0) -> int_t
    {
        const auto complete_graph_edges =
            ::Standard::Algorithms::Numbers::shift_right(nodes * (nodes - 1), is_undirected ? 1U : 0U);

        const auto max_edges =
            0 < limit_edges && limit_edges <= complete_graph_edges ? limit_edges : complete_graph_edges;

        assert(!(nodes < 0) && !(limit_edges < 0) && !(complete_graph_edges < 0) && !(max_edges < 0));

        auto edges = static_cast<int_t>(max_edges);
        assert(!(edges < int_t{}));

        return edges;
    }

    // Nodes are [0..n-1].
    template<class add_edge_t, class edge_t, class int_t, int_t max_nodes, bool is_undirected = true,
        int_t min_nodes = 1, int_t limit_edges = 0>
    requires(!(min_nodes < 0) && min_nodes <= max_nodes)
    [[nodiscard]] constexpr auto create_random_graph_base(add_edge_t add_edge)
        -> std::pair<std::vector<std::vector<edge_t>>, int_t>
    {
        constexpr auto max_edges_total = count_graph_edges<int_t>(max_nodes, is_undirected, limit_edges);

        static_assert(!(max_edges_total < int_t{}));

        Standard::Algorithms::Utilities::random_t<int_t> rnd{};

        const auto node_count = rnd(min_nodes, max_nodes);
        const auto max_edges = count_graph_edges<int_t>(node_count, is_undirected, max_edges_total);

        assert(!(node_count < 0) && !(max_edges < int_t{}));

        const auto edge_count = rnd(int_t{}, max_edges);

        std::vector<std::vector<edge_t>> result(node_count);

        using pair_t = std::pair<int_t, int_t>;

        std::unordered_set<pair_t, Standard::Algorithms::Numbers::pair_hash> edges;

        while (static_cast<int_t>(edges.size()) < edge_count)
        {
            auto from = rnd(int_t{}, node_count - 1);
            int_t tod{};

            do
            {
                tod = rnd(int_t{}, node_count - 1);
            } while (from == tod);

            if (is_undirected && tod < from)
            {
                std::swap(tod, from);
            }

            if (!edges.insert(pair_t{ from, tod }).second)
            {
                continue;
            }

            add_edge(result, from, tod);
        }

        return { std::move(result), edge_count };
    }

    // Nodes are [0..n-1].
    template<class int_t = std::int32_t, int_t max_nodes = defaut_max_nodes, bool is_undirected = true,
        int_t min_nodes = 1, int_t limit_edges = 0>
    requires(!(min_nodes < 0) && min_nodes <= max_nodes)
    [[nodiscard]] constexpr auto create_random_graph() -> std::pair<std::vector<std::vector<int_t>>, int_t>
    {
        const auto add_edge = [](std::vector<std::vector<int_t>> &graph, const int_t from, const int_t tod)
        {
            graph[from].push_back(tod);

            if (is_undirected)
            {
                graph[tod].push_back(from);
            }
        };

        auto result = create_random_graph_base<decltype(add_edge), int_t, int_t, max_nodes, is_undirected, min_nodes,
            limit_edges>(add_edge);

        return result;
    }

    // Nodes are [0..n-1].
    template<class edge_t, class weight_t, class int_t = std::int32_t, int_t max_nodes = defaut_max_nodes,
        bool is_undirected = true, int_t min_nodes = 1, int_t limit_edges = 0>
    requires(!(min_nodes < 0) && min_nodes <= max_nodes)
    [[nodiscard]] constexpr auto create_random_graph_weighted(const weight_t min_weight = {},
        const weight_t max_weight = defaut_max_weight) -> std::pair<std::vector<std::vector<edge_t>>, int_t>
    {
        assert(min_weight <= max_weight);

        Standard::Algorithms::Utilities::random_t<weight_t> rwe(min_weight, max_weight);

        const auto add_edge = [&rwe](std::vector<std::vector<edge_t>> &graph, const int_t from, const int_t tod)
        {
            const auto weigh = rwe();
            graph[from].emplace_back(tod, weigh);

            if (is_undirected)
            {
                graph[tod].emplace_back(from, weigh);
            }
        };

        auto result = create_random_graph_base<decltype(add_edge), edge_t, int_t, max_nodes, is_undirected, min_nodes,
            limit_edges>(add_edge);

        return result;
    }

    // Nodes are [0..n-1].
    template<std::integral int_t, int_t min_nodes = 1, int_t max_nodes = defaut_max_nodes>
    requires(!(min_nodes < 0) && min_nodes <= max_nodes)
    [[nodiscard]] constexpr auto create_random_graph_weighted_symmetric(
        const int_t min_weight = {}, const int_t max_weight = defaut_max_weight) -> std::vector<std::vector<int_t>>
    {
        assert(min_weight <= max_weight);

        Standard::Algorithms::Utilities::random_t<int_t> rnd(min_weight, max_weight);

        const auto node_count = rnd(min_nodes, max_nodes);

        std::vector<std::vector<int_t>> graph(node_count, std::vector<int_t>(node_count, int_t{}));

        for (int_t from{}; from < node_count; ++from)
        {
            auto &edges = graph[from];

            for (int_t tod{}; tod < from; ++tod)
            {
                auto &edge = edges[tod];
                graph[tod][from] = edge = rnd();
            }

            assert(int_t{} == edges[from]); // Symmetry
        }

        return graph;
    }
} // namespace Standard::Algorithms::Graphs
