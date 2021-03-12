#pragma once
#include"../Utilities/require_utilities.h"
#include<queue>
#include<vector>

namespace Standard::Algorithms::Graphs::Inner
{
    template<class weight_t, class edge_t>
    constexpr auto backtrack_max_capacity(const std::int32_t source,
        const std::vector<std::pair<std::int32_t, edge_t *>> &previous, const std::int32_t destination) -> weight_t
    {
        assert(!previous.empty());

        auto cur = previous[destination];
        auto result = static_cast<weight_t>(cur.second->capacity - cur.second->flow);
        for (;;)
        {
            if (source == cur.first)
            {
                return result;
            }

            cur = previous[cur.first];
            result = std::min(result, static_cast<weight_t>(cur.second->capacity - cur.second->flow));
        }
    }

    template<class weight_t, class edge_t>
    constexpr auto bfs_return_residual_capacity(const std::int32_t source, std::vector<std::vector<edge_t>> &graph,
        const std::int32_t destination, std::vector<std::pair<std::int32_t, edge_t *>> &previous) -> weight_t
    {
        const auto size = static_cast<std::int32_t>(graph.size());
        previous.assign(size, {});

        edge_t fake_edge{ source };
        previous[source] = std::make_pair(source, &fake_edge);

        std::queue<std::int32_t> que;
        que.push(source);

        do
        {
            const auto from = que.front();
            que.pop();

            for (auto &edge : graph[from])
            {
                if (!(edge.flow < edge.capacity) || nullptr != previous[edge.to].second)
                {
                    continue;
                }

                previous[edge.to] = std::make_pair(from, &edge);
                if (destination == edge.to)
                {
                    auto result = backtrack_max_capacity<weight_t, edge_t>(source, previous, destination);

                    return result;
                }

                que.push(edge.to);
            }
        } while (!que.empty());

        return {};
    }

    template<class weight_t, class edge_t>
    constexpr void add_flow(const std::int32_t source, const std::vector<std::pair<std::int32_t, edge_t *>> &previous,
        const std::int32_t destination, const weight_t &weigh, std::vector<std::vector<edge_t>> &graph)
    {
        assert(!previous.empty());

        auto cur = previous[destination];
        for (;;)
        {
            cur.second->flow += weigh;

            auto &edge = graph[cur.second->to][cur.second->rev_edge_index];
            edge.flow -= weigh;

            if (source == cur.first)
            {
                return;
            }

            cur = previous[cur.first];
        }
    }
} // namespace Standard::Algorithms::Graphs::Inner

namespace Standard::Algorithms::Graphs
{
    template<std::signed_integral weight_t, class edge_t>
    constexpr void check_data_and_zero_flow(
        const std::int32_t source, std::vector<std::vector<edge_t>> &graph, const std::int32_t destination)
    {
        const auto size = require_positive(static_cast<std::int32_t>(graph.size()), "graph size");

        if (size <= source)
        {
            auto str = ::Standard::Algorithms::Utilities::w_stream();
            str << "The source(" << source << ") must be smaller than size(" << size << ").";
            throw_exception(str);
        }

        if (size <= destination)
        {
            auto str = ::Standard::Algorithms::Utilities::w_stream();
            str << "The destination(" << destination << ") must be smaller than size(" << size << ").";
            throw_exception(str);
        }

        if (source == destination)
        {
            auto str = ::Standard::Algorithms::Utilities::w_stream();
            str << "The source(" << source << ") must differ from the destination(" << destination << ").";
            throw_exception(str);
        }

        std::size_t from{};

        for (auto &edges : graph)
        {
            for (auto &edge : edges)
            {
                if (edge.to < 0 || size <= edge.to)
                {
                    auto str = ::Standard::Algorithms::Utilities::w_stream();
                    str << "The edge from " << from << " to (" << edge.to << ") must be between 0 and " << (size - 1)
                        << ".";
                    throw_exception(str);
                }

                if (edge.capacity < weight_t())
                {
                    auto str = ::Standard::Algorithms::Utilities::w_stream();
                    str << "The Capacity(" << edge.capacity << ") from " << from << " to " << edge.to
                        << " must be positive.";
                    throw_exception(str);
                }

                edge.flow = {};
            }

            ++from;
        }
    }

    // Find the max flow using Edmonds-Karp algorithm.
    // The "graph" MUST have all the reverse edges!
    template<std::signed_integral weight_t, class edge_t>
    constexpr auto edmonds_karp_flow(
        const std::int32_t source, std::vector<std::vector<edge_t>> &graph, const std::int32_t destination) -> weight_t
    {
        check_data_and_zero_flow<weight_t, edge_t>(source, graph, destination);

        if (graph[source].empty())
        {
            return {};
        }

        const auto size = static_cast<std::int32_t>(graph.size());

        weight_t result{};
        std::vector<std::pair<std::int32_t, edge_t *>> previous(size);

        for (;;)
        {
            const auto weigh =
                Inner::bfs_return_residual_capacity<weight_t, edge_t>(source, graph, destination, previous);
            if (weight_t() == weigh)
            {
                return result;
            }

            Inner::add_flow<weight_t, edge_t>(source, previous, destination, weigh, graph);

            result += weigh;
        }
    }
} // namespace Standard::Algorithms::Graphs
