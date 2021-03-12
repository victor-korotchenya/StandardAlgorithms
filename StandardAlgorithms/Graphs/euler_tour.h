#pragma once
#include"../Utilities/require_utilities.h"
#include<vector>

namespace Standard::Algorithms::Graphs::Inner
{
    template<class edge_t>
    constexpr void count_edges_slow(
        const std::vector<edge_t> &edges, std::vector<std::int32_t> &in_degrees, std::uint64_t &total_edges)
    {
        total_edges += edges.size();

        for (const auto &edge : edges)
        {
            ++in_degrees[edge.to];
        }
    }

    template<class edge_t>
    constexpr void check_in_out_degrees(
        const std::vector<std::vector<edge_t>> &graph, const std::vector<std::int32_t> &in_degrees)
    {
        const auto size = static_cast<std::int32_t>(graph.size());

        for (std::int32_t index{}; index < size; ++index)
        {
            const auto out_deg = graph[index].size();

            if (out_deg != static_cast<std::size_t>(in_degrees[index])) [[unlikely]]
            {
                const auto message = "In " + std::to_string(in_degrees[index]) + " and out " + std::to_string(out_deg) +
                    " degrees must be the same at " + std::to_string(index) + ".";
                throw std::invalid_argument(message);
            }
        }
    }

    template<class edge_t>
    constexpr auto check_directed(const std::vector<std::vector<edge_t>> &directed_graph) -> std::uint64_t
    {
        const auto size = static_cast<std::int32_t>(directed_graph.size());
        std::vector<std::int32_t> in_degrees(size);
        std::uint64_t total_edges{};

        for (std::int32_t index{}; index < size; ++index)
        {
            count_edges_slow<edge_t>(directed_graph[index], in_degrees, total_edges);
        }

        check_in_out_degrees<edge_t>(directed_graph, in_degrees);

        return total_edges;
    }

    // The graph might have 2 vertices of an odd degree.
    template<class edge_t>
    constexpr auto edges_start_undirected(const std::vector<std::vector<edge_t>> &graph)
        -> std::pair<std::uint64_t, std::int32_t>
    {
        const auto size = static_cast<std::int32_t>(graph.size());
        std::vector<std::int32_t> in_degrees(size);
        std::uint64_t total_edges{};
        std::int32_t odds{};
        std::int32_t start{};

        for (std::int32_t index{}; index < size; ++index)
        {
            count_edges_slow<edge_t>(graph[index], in_degrees, total_edges);

            if (const auto is_even = (graph[index].size() & 1U) == 0U; is_even)
            {
                continue;
            }

            if (2 < ++odds)
            {
                throw std::invalid_argument("Eulerian graph has at least 3 odd degree vertices - error.");
            }

            start = index;
        }

        check_in_out_degrees<edge_t>(graph, in_degrees);

        if (const auto is_odd = (total_edges & 1U) != 0U; is_odd)
        {
            throw std::invalid_argument("Eulerian graph has odd total edges " + std::to_string(total_edges));
        }

        return { total_edges >> 1U, start };
    }

    template<class edge_t, class first_time_t, class tour_t>
    struct et_context final
    {
        const std::vector<std::vector<edge_t>> &tree;
        first_time_t &first_time;
        tour_t &tour;
        std::int32_t time{};
    };

    template<class edge_t, class first_time_t, class tour_t>
    constexpr void dfs_et(
        const std::int32_t node, et_context<edge_t, first_time_t, tour_t> &context, const std::int32_t parent = -1)
    {
        context.tour[context.time] = node;
        context.first_time[node] = context.time++;

        const auto &children = context.tree[node];

        for (const auto &child : children)
        {
            if (parent == child)
            {
                continue;
            }

            dfs_et(child, context, node);
            context.tour[context.time++] = node;
        }
    }
} // namespace Standard::Algorithms::Graphs::Inner

namespace Standard::Algorithms::Graphs
{
    // Hierholzer finds an Euler tour in a plain graph.
    // In a trail, edges are unique.
    // A circuit is a closed trail with the same initial/terminating node.
    // An Euler tour includes all edges once.
    // The graph must be connected, zero based.
    // Important. The graph, having edges, is modified!
    template<class edge_t>
    constexpr auto euler_tour_directed(std::vector<std::vector<edge_t>> &directed_graph) -> std::vector<std::int32_t>
    {
        if (directed_graph.empty())
        {
            return {};
        }

        const auto total_edges = Inner::check_directed<edge_t>(directed_graph);
        std::vector<std::int32_t> path;
        path.reserve(total_edges + 1LLU);

        std::vector<std::int32_t> buf{ 0 };

        do
        {
            const auto &from = buf.back();
            auto &edges = directed_graph[from];
            if (!edges.empty())
            {
                const auto &tod = edges.back().to;
                buf.push_back(tod);
                edges.pop_back();
                continue;
            }

            path.push_back(from);
            buf.pop_back();
        } while (!buf.empty());

        assert(total_edges + 1LLU == path.size());

        std::reverse(path.begin(), path.end());

        return path;
    }

    // Undirected graph - see also euler_tour_directed.
    // The graph, having edges, is modified!
    template<class edge_t>
    constexpr auto euler_tour_undirected(std::vector<std::vector<edge_t>> &graph) -> std::vector<std::int32_t>
    {
        if (graph.empty())
        {
            return {};
        }

        const auto [edge_count, start] = Inner::edges_start_undirected<edge_t>(graph);

        // std::set<std::pair<std::int32_t, std::int32_t>> can be used when no time.
        std::vector<bool> edge_visited(edge_count + 1LLU);

        std::vector<std::int32_t> path;
        path.reserve(edge_visited.size());

        std::vector<std::int32_t> buf{ start };

        do
        {
            const auto &from = buf.back();
            auto &edges = graph[from];

            if (!edges.empty())
            {
                const auto &edge = edges.back();
                const auto &edge_number = edge.edge_number;

                if (!edge_visited[edge_number])
                {
                    edge_visited[edge_number] = true;
                    buf.push_back(edge.to);
                }

                edges.pop_back();
                continue;
            }

            path.push_back(from);
            buf.pop_back();
        } while (!buf.empty());

        assert(edge_count + 1LLU == path.size());

        std::reverse(path.begin(), path.end());

        return path;
    }

    // Euler tour for a valid and undirected graph. todo(p3): verify.
    // zero based index.
    template<class edge_t, class first_time_t, class tour_t>
    constexpr void to_check_euler_tour(
        const std::vector<std::vector<edge_t>> &tree, first_time_t &first_time, tour_t &tour)
    {
        require_positive(tree.size(), "tree size");

        Inner::et_context<edge_t, first_time_t, tour_t> context{ tree, first_time, tour };

        constexpr std::int32_t root{};
        Inner::dfs_et(root, context);
    }
} // namespace Standard::Algorithms::Graphs
