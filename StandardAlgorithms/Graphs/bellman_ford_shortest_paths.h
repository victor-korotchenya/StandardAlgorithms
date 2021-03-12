#pragma once
#include <deque>
#include <vector>
#include "../Utilities/ExceptionUtilities.h"

namespace Privet
{
    namespace Algorithms
    {
        namespace Graphs
        {
            template <class graph_t, class weight_t>
            bool has_negative_cost_cycle(
                const graph_t& graph,
                const std::vector<weight_t>& distances)
            {
                static_assert(std::is_signed_v<weight_t>);

                const auto size = static_cast<int>(graph.size());
                assert(size > 0);

                for (auto from = 0; from < size; ++from)
                {
                    const auto& edges = graph[from];
                    const auto& dist_from = distances[from];

                    for (const auto& edge : edges)
                    {
                        const auto& to = edge.Vertex;
                        assert(from != to && to >= 0 && to < size);
                        const auto& weight = edge.Weight;

                        const auto cand = dist_from + weight;
                        const auto& cur = distances[to];
                        if (cur > cand)
                            return true;
                    }
                }

                return false;
            }

            //Find 1 source shortest path tree for a negative cost graph.
            //Return false when a negative cycle exists - the computed distances and parents can be ignored.
            //Time O(v*e) is slower than Dijkstra's for non-negative costs.
            template <class graph_t, class weight_t>
            bool bellman_ford_shortest_paths_fast(
                const graph_t& graph,
                int source,
                std::vector<weight_t>& distances,
                std::vector<int>& parents,
                std::deque<int>& temp, std::deque<int>& temp2, std::vector<bool>& used,
                const weight_t inf = std::numeric_limits<weight_t>::max() / 2)
            {
                static_assert(std::is_signed_v<weight_t>);

                const auto size = static_cast<int>(graph.size());
                assert(source >= 0 && source < size&& size > 0 && inf > 0 && -inf < 0);

                distances.assign(size, inf);
                distances[source] = weight_t();
                parents.assign(size, -1);

                temp.clear();
                temp.push_back(source);

                for (auto att = 0, temp_ind = 0; ++att < size; temp_ind ^= 1)
                {
                    const auto& prev = temp_ind ? temp2 : temp;
                    auto& next = temp_ind ? temp : temp2;
                    next.clear();
                    used.assign(size, false);

                    for (const auto& from : prev)
                    {
                        const auto& edges = graph[from];
                        const auto& dist_from = distances[from];
                        assert(dist_from < inf&& dist_from > -inf);

                        for (const auto& edge : edges)
                        {
                            const auto& to = edge.Vertex;
                            const auto& weight = edge.Weight;
                            const auto cand = dist_from + weight;
                            assert(from != to && to >= 0 && to < size&& weight > -inf && weight < inf&& cand / 2 < inf);

                            if (cand <= -inf) // underflow.
                                return false;

                            auto& curd = distances[to];
                            if (curd <= cand)
                                continue;

                            curd = cand;
                            parents[to] = from;
                            if (used[to])
                                continue;

                            used[to] = true;
                            if (next.empty() || distances[next.front()] <= curd)
                                next.push_back(to);
                            else // Process shorter routes first.
                                next.push_front(to);
                        }
                    }

                    if (next.empty())
                        return true;
                }

                const auto has_cycle = has_negative_cost_cycle< graph_t, weight_t>(graph, distances);
                return !has_cycle;
            }

            // It is 1.5 times slower on n=5000.
            template <class graph_t, class weight_t>
            bool bellman_ford_shortest_paths_slow(
                const graph_t& graph,
                int source,
                std::vector<weight_t>& distances,
                std::vector<int>& parents,
                const weight_t inf = std::numeric_limits<weight_t>::max() / 2)
            {
                static_assert(std::is_signed_v<weight_t>);

                const auto size = static_cast<int>(graph.size());
                assert(source >= 0 && source < size&& size > 0 && inf > 0 && -inf < 0);

                distances.assign(size, inf);
                distances[source] = weight_t();
                parents.assign(size, -1);

                for (auto att = 0; ++att < size;)
                {
                    auto changed = false;

                    for (auto from = 0; from < size; ++from)
                    {
                        const auto& edges = graph[from];
                        const auto& dist_from = distances[from];
                        if (dist_from == inf)
                            continue;

                        for (const auto& edge : edges)
                        {
                            const auto& to = edge.Vertex;
                            const auto& weight = edge.Weight;
                            const auto cand = dist_from + weight;
                            assert(from != to && to >= 0 && to < size&& weight > -inf && weight < inf&& cand / 2 < inf);

                            if (cand <= -inf) // underflow.
                                return false;

                            auto& cur = distances[to];
                            if (cur <= cand)
                                continue;

                            cur = cand;
                            parents[to] = from;
                            changed = true;
                        }
                    }

                    if (!changed)
                        return true;
                }

                const auto has_cycle = has_negative_cost_cycle< graph_t, weight_t>(graph, distances);
                return !has_cycle;
            }

            // All vertex pairs shortest paths, weights can be negative.
             //Return false when a negative cycle exists - the computed distances and nexts can be ignored.
             // The  parents are 'nexts', not previous, to get rid of 'path.reverse'.
            //Time O(v**3).
            template <class graph_t, class weight_t>
            bool floyd_warshall_shortest_paths(
                const graph_t& graph,
                std::vector<std::vector<weight_t>>& distances,
                std::vector<std::vector<int>>& nexts,
                const weight_t inf = std::numeric_limits<weight_t>::max() / 2)
            {
                static_assert(std::is_signed_v<weight_t>);
                assert(inf > 0 && -inf < 0);

                const auto size = static_cast<int>(graph.size());
                RequirePositive(size, "size");

                distances.assign(size, std::vector<weight_t>(size, inf));
                nexts.assign(size, std::vector<int>(size, -1));

                for (auto from = 0; from < size; ++from)
                {
                    const auto& edges = graph[from];
                    auto& dist = distances[from];
                    auto& next = nexts[from];
                    for (const auto& edge : edges)
                    {
                        const auto& to = edge.Vertex;
                        const auto& weight = edge.Weight;

                        assert(from != to && to >= 0 && to < size&& weight > -inf && weight < inf);

                        dist[to] = weight;
                        next[to] = to;
                    }

                    dist[from] = 0;
                    next[from] = from;
                }

                for (auto mid = 0; mid < size; ++mid)
                {
                    const auto& m = distances[mid];

                    for (auto from = 0; from < size; ++from)
                    {
                        auto& f = distances[from];
                        auto& next = nexts[from];
                        for (auto to = 0; to < size; ++to)
                        {
                            auto cand = f[mid] + m[to];
                            if (cand < -inf)
                                // return false; Some paths might be good while others might include negative weight/cost cycles.
                                cand = -inf; // Avoid underflow.

                            auto& cur = f[to];
                            if (cur > cand)
                            {
                                cur = cand;

                                // from -> mid -> to
                                // Here 'from -> mid' might be not simple path.
                                next[to] = next[mid];
                            }
                        }
                    }
                }

                for (auto i = 0; i < size; ++i)
                    if (distances[i][i] < 0)
                        return false;

                return true;
            }

            // The 'path' will be empty when no path exists.
            void inline restore_floyd_warshall_shortest_paths(const std::vector<std::vector<int>>& nexts, int from, const int to, std::vector<int>& path)
            {
                assert(from >= 0 && to >= 0);
                path.clear();

                if (nexts[from][to] < 0)
                    return; // No path.

                for (;;)
                {
                    path.push_back(from);
                    if (to == from)
                        return;

                    const auto& next = nexts[from][to];
                    assert(next >= 0);
                    from = next;
                }
            }
        }
    }
}