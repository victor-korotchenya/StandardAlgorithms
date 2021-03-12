#pragma once

#include <cassert>
#include <queue>
#include <utility>
#include <vector>

namespace Privet
{
    namespace Algorithms
    {
        namespace Graphs
        {
            // The graph must be connected.
            template<class weight_t, class weight_t2 = int64_t>
            weight_t2 jarnik_minimal_spanning_tree(
                const std::vector<std::vector<std::pair<int, weight_t>>>& graph,
                const int source_node,
                std::vector<int>& parents,
                // temp vars:
                std::vector<bool>& visited,
                std::vector<weight_t2>& weights,
                // max queue - use minus.
                std::priority_queue<std::pair<weight_t2, int>>& q)
            {
                static_assert(sizeof weight_t <= sizeof weight_t2);
                assert(graph.size() >= 2 && source_node >= 0 && source_node < static_cast<int>(graph.size()));
                assert(q.empty());

                constexpr auto no_par = -1;
                parents.assign(graph.size(), no_par);

                visited.assign(graph.size(), false);

                weights.assign(graph.size() << 1, std::numeric_limits<weight_t2>::max());
                weights[source_node] = {};

                q.push({ weight_t2{}, source_node });

                weight_t2 min_w{};
                do
                {
                    const auto top = q.top();
                    const auto& cur = top.second;
                    q.pop();

                    if (visited[cur])
                        continue;

                    min_w -= top.first;
                    visited[cur] = true;

                    for (const auto& p : graph[cur])
                    {
                        const auto& next = p.first;
                        if (visited[next])
                            continue;

                        auto& old1 = weights[next + graph.size()];
                        const auto& new1 = p.second;
                        if (p.second < old1)
                        {
                            old1 = new1;
                            weights[next] = weights[cur] + new1;
                            parents[next] = cur;
                            q.push({ -p.second, p.first });
                        }
                    }
                } while (!q.empty());

                weights.resize(graph.size());
                return min_w;
            }
        }
    }
}