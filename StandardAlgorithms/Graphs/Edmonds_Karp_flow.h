#pragma once
#include <algorithm>
#include <queue>
#include <vector>
#include <type_traits>
#include <utility>
#include "../Utilities/ExceptionUtilities.h"

namespace
{
    template<typename TWeight, typename TEdge>
    inline TWeight backtrack_max_capacity(
        const int source, const int target,
        const std::vector<std::pair<int, TEdge*>>& previous)
    {
        auto t = previous[target];
        auto result = t.second->capacity - t.second->flow;
        for (;;)
        {
            if (source == t.first)
            {
                return result;
            }

            t = previous[t.first];
            result = std::min(result, t.second->capacity - t.second->flow);
        }
    }

    template<typename TWeight, typename TEdge>
    inline TWeight bfs_return_residual_capacity(
        const int source, const int target,
        std::vector<std::vector<TEdge>>& graph,
        std::vector<std::pair<int, TEdge*>>& previous)
    {
        const auto size = static_cast<int>(graph.size());
        previous.assign(size, {});

        TEdge fake_edge(source);
        previous[source] = std::make_pair(source, &fake_edge);

        std::queue<int> q;
        q.push(source);
        do
        {
            const auto u = q.front();
            q.pop();
            for (auto& e : graph[u])
            {
                if (e.flow < e.capacity && nullptr == previous[e.to].second)
                {
                    previous[e.to] = std::make_pair(u, &e);
                    if (target == e.to)
                    {
                        auto result = backtrack_max_capacity<TWeight, TEdge>(
                            source, target, previous);
                        return result;
                    }

                    q.push(e.to);
                }
            }
        } while (!q.empty());

        return {};
    }

    template<typename TWeight, typename TEdge>
    inline void add_flow(
        const int source, const int target,
        const std::vector<std::pair<int, TEdge*>>& previous,
        const TWeight w,
        std::vector<std::vector<TEdge>>& graph)
    {
        auto t = previous[target];
        for (;;)
        {
            t.second->flow += w;
            graph[t.second->to][t.second->rev_edge_index].flow -= w;
            if (source == t.first)
                return;

            t = previous[t.first];
        }
    }
}

namespace Privet::Algorithms::Graphs
{
    template<typename TWeight, typename TEdge>
    void check_data_and_zero_flow(
        const int source, const int target,
        std::vector<std::vector<TEdge>>& graph)
    {
        const auto size = static_cast<int>(graph.size());
        RequirePositive(size, "graph size");

        if (size <= source)
        {
            std::ostringstream ss;
            ss << "The source(" << source <<
                ") must be smaller than size(" << size << ").";
            throw StreamUtilities::throw_exception(ss);
        }
        if (size <= target)
        {
            std::ostringstream ss;
            ss << "The target(" << target <<
                ") must be smaller than size(" << size << ").";
            throw StreamUtilities::throw_exception(ss);
        }
        if (source == target)
        {
            std::ostringstream ss;
            ss << "The source(" << source <<
                ") must differ from the target(" << target << ").";
            throw StreamUtilities::throw_exception(ss);
        }

        int i{};
        for (auto& v : graph)
        {
            for (auto& e : v)
            {
                if (e.to < 0 || size <= e.to)
                {
                    std::ostringstream ss;
                    ss << "The edge from " << i
                        << " to (" << e.to <<
                        ") must be between 0 and " << (size - 1) << ".";
                    throw StreamUtilities::throw_exception(ss);
                }

                if (e.capacity < 0)
                {
                    std::ostringstream ss;
                    ss << "The Capacity(" << e.capacity <<
                        ") from " << i
                        << " to " << e.to << " must be positive.";
                    throw StreamUtilities::throw_exception(ss);
                }

                e.flow = 0;
            }

            ++i;
        }
    }

    //Finds the max flow using Edmonds-Karp algorithm.
    //The "graph" MUST have all the reverse edges!
    template <typename TWeight, typename TEdge>
    TWeight Edmonds_Karp_flow(
        const int source, const int target,
        std::vector<std::vector<TEdge>>& graph)
    {
        static_assert(std::is_signed<TWeight>::value, "TWeight must be signed.");

        check_data_and_zero_flow<TWeight, TEdge>(
            source, target, graph);
        if (graph[source].empty())
        {
            return{};
        }

        const auto size = static_cast<int>(graph.size());

        TWeight result{};
        std::vector<std::pair<int, TEdge*>> previous(size);
        for (;;)
        {
            const auto w = bfs_return_residual_capacity<TWeight, TEdge>(
                source, target, graph, previous);
            if (0 == w)
                return result;

            add_flow<TWeight, TEdge>(source, target, previous, w, graph);
            result += w;
        }
    }
}