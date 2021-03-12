#pragma once
#include <cassert>
#include <stdexcept>
#include <string>
#include <vector>

namespace
{
    template <class edge_t>
    void count_edges(const std::vector<edge_t>& edges,
        std::vector<int>& in_degrees, int64_t& total_edges)
    {
        total_edges += static_cast<int64_t>(edges.size());

        for (const auto& e : edges)
        {
            const auto to = static_cast<int>(e);
            ++in_degrees[to];
        }
    }

    template <class edge_t>
    void check_in_out_degrees(const std::vector<std::vector<edge_t>>& graph,
        const std::vector<int>& in_degrees)
    {
        const auto size = static_cast<int>(graph.size());
        for (auto i = 0; i < size; ++i)
        {
            const auto out_deg = graph[i].size();
            if (out_deg != in_degrees[i])
            {
                const auto message = "In " + std::to_string(in_degrees[i]) +
                    " and out " + std::to_string(out_deg) +
                    " degrees must be the same at " + std::to_string(i) + ".";
                throw std::invalid_argument(message);
            }
        }
    }

    template <class edge_t>
    int64_t check_directed(const std::vector<std::vector<edge_t>>& directed_graph)
    {
        const auto size = static_cast<int>(directed_graph.size());
        std::vector<int> in_degrees((size));
        int64_t total_edges{};

        for (auto i = 0; i < size; ++i)
            count_edges<edge_t>(directed_graph[i], in_degrees, total_edges);

        check_in_out_degrees<edge_t>(directed_graph, in_degrees);
        return total_edges;
    }

    // The graph might have 2 vertices of an odd degree.
    template <class edge_t>
    std::pair<int64_t, int> edges_start_undirected(const std::vector<std::vector<edge_t>>& graph)
    {
        const auto size = static_cast<int>(graph.size());
        std::vector<int> in_degrees(size);
        int64_t total_edges{};
        auto odds = 0, start = 0;

        for (auto i = 0; i < size; ++i)
        {
            count_edges<edge_t>(graph[i], in_degrees, total_edges);

            const auto is_even = !(graph[i].size() & 1);
            if (is_even)
                continue;

            if (++odds > 2)
                throw std::invalid_argument("Eulerian graph has at least 3 odd degree vertices - error.");

            start = i;
        }

        check_in_out_degrees<edge_t>(graph, in_degrees);
        if (total_edges & 1)
            throw std::invalid_argument("Eulerian graph has odd total_edges " + std::to_string(total_edges));

        return { total_edges >> 1, start };
    }
}

namespace Privet::Algorithms::Trees
{
    //Hierholzer finds an Euler path(circuit)/cycle in a plain graph.
    //The graph must be connected, zero based.
    //The graph, having edges, is modified!
    template <class edge_t>
    std::vector<int> euler_path_directed(std::vector<std::vector<edge_t>>& directed_graph)
    {
        if (directed_graph.empty())
            return {};

        const auto total_edges = check_directed<edge_t>(directed_graph);
        std::vector<int> path, buf{ 0 };
        path.reserve(total_edges + 1ll);
        do
        {
            const auto& from = buf.back();
            auto& edges = directed_graph[from];
            if (edges.size())
            {
                const auto to = static_cast<int>(edges.back());
                buf.push_back(to);
                edges.pop_back();
                continue;
            }

            path.push_back(from);
            buf.pop_back();
        } while (buf.size());

        assert(total_edges + 1 == static_cast<int64_t>(path.size()));
        std::reverse(path.begin(), path.end());
        return path;
    }

    //Undirected graph - see also euler_path_directed.
    //The graph, having edges, is modified!
    template <class edge_t>
    std::vector<int> euler_path_undirected(std::vector<std::vector<edge_t>>& graph)
    {
        if (graph.empty())
            return {};

        const auto edges_start = edges_start_undirected<edge_t>(graph);

        std::vector<int> path, buf{ edges_start.second };
        path.reserve(edges_start.first + 1ll);

        // Might use a set<pair<int, int>> when no time.
        std::vector<bool> edge_visited(edges_start.first + 1ll);
        do
        {
            const auto& from = buf.back();
            auto& edges = graph[from];
            if (edges.size())
            {
                const auto& edge = edges.back();
                const auto& edge_number = edge.edge_number;
                if (!edge_visited[edge_number])
                {
                    edge_visited[edge_number] = true;
                    const auto to = static_cast<int>(edge);
                    buf.push_back(to);
                }

                edges.pop_back();
                continue;
            }

            path.push_back(from);
            buf.pop_back();
        } while (buf.size());

        assert(edges_start.first + 1ll == static_cast<int64_t>(path.size()));

        std::reverse(path.begin(), path.end());
        return path;
    }
}