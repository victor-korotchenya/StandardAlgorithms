#pragma once
#include <algorithm>
#include <limits>
#include <vector>
#include <type_traits>
#include "../Numbers/one_time_queue.h"
#include "Edmonds_Karp_flow.h"

namespace
{
    template<typename TWeight, typename TEdge>
    bool bfs_find_shortest_paths(
        const int source, const int target,
        const std::vector<std::vector<TEdge>>& graph,
        std::vector<int>& levels,
        Privet::Algorithms::Numbers::one_time_queue<int, int>& q)
    {
        q.reset(); q.push(source);

        levels.assign(graph.size(), -1);
        levels[source] = 0;
        do
        {
            const auto v = q.top();
            q.pop();

            for (const auto& e : graph[v])
            {
                if (e.flow < e.capacity && levels[e.to] < 0)
                {
                    q.push(e.to);
                    levels[e.to] = levels[v] + 1;
                }
            }
        } while (!q.empty() && levels[target] < 0);

        return 0 <= levels[target];
    }

    template<typename TWeight, typename TEdge>
    TWeight dfs_add_flow(
        const int source, const int target,
        std::vector<std::vector<TEdge>>& graph,
        const std::vector<int>& levels,
        std::vector<std::pair<int, TWeight>>& stack,
        std::vector<int>& visits)
    {
        auto top = stack[0] = std::make_pair(source, std::numeric_limits<TWeight>::max());
        auto stack_size = 1, size = 0;
        auto shall_delete = false;

        for (;;)
        {
        Label_1:
            top = stack[stack_size - 1ll];
            size = static_cast<int>(graph[top.first].size());
            visits[top.first] += shall_delete && visits[top.first] < size;

            while (visits[top.first] < size)
            {
                auto& e = graph[top.first][visits[top.first]];
                if (e.flow < e.capacity && levels[top.first] + 1 == levels[e.to])
                {
                    const auto result = std::min(top.second, e.capacity - e.flow);
                    if (target == e.to)
                    {
                        auto pe = &e;
                        for (;;)
                        {
                            pe->flow += result;
                            graph[pe->to][pe->rev_edge_index].flow -= result;
                            if (0 == --stack_size)
                                return result;

                            top = stack[stack_size - 1ll];
                            pe = &graph[top.first][visits[top.first]];
                        }
                    }

                    stack[stack_size++] = std::make_pair(e.to, result);
                    shall_delete = false;
                    goto Label_1;
                }

                ++(visits[top.first]);
            }

            if (0 == --stack_size)
                return 0;

            shall_delete = true;
        }
    }
}

namespace Privet::Algorithms::Graphs
{
    //Finds the max flow using Dinic algorithm.
    //The "graph" MUST have all the reverse edges!
    template <typename TWeight, typename TEdge>
    TWeight dinic_flow_old(
        const int source, const int target,
        std::vector<std::vector<TEdge>>& graph)
    {
        static_assert(std::is_signed_v<TWeight>);

        check_data_and_zero_flow<TWeight, TEdge>(
            source, target, graph);

        const auto size = graph.size();

        TWeight result{};
        std::vector<int> levels(size);
        std::vector<int> visits(size);
        std::vector<std::pair<int, TWeight>> stack(size);
        Privet::Algorithms::Numbers::one_time_queue<int, int> q(size);

        while (bfs_find_shortest_paths<TWeight, TEdge>(
            source, target, graph, levels, q))
        {
            visits.assign(graph.size(), 0);

            while (auto f = dfs_add_flow<TWeight, TEdge>(
                source, target, graph, levels, stack, visits))
            {
                result += f;
            }
        }

        return result;
    }
}