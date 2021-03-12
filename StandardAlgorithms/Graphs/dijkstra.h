#pragma once
#include"../Utilities/is_debug.h"
#include"graph.h"
#include"priority_queue_molodetz.h"
#include<set>
#include<sstream>
#include<vector>

namespace Standard::Algorithms::Graphs::Inner
{
    // Time O(n).
    template<class weight_t>
    [[nodiscard]] constexpr auto dijkstra_dense_more(const std::int32_t size, const std::vector<weight_t> &distances,
        const weight_t &inf, const std::vector<bool> &processed) -> std::pair<std::int32_t, bool>
    {
        assert(0 < size && size < std::numeric_limits<std::int32_t>::max() && weight_t{} < inf);

        auto source = size;

        for (std::int32_t index{}; index < size; ++index)
        {
            if (!processed[index] && distances[index] < distances[source])
            {
                source = index;
            }
        }

        const auto has = source < size;
        return { source, has };
    }
} // namespace Standard::Algorithms::Graphs::Inner

namespace Standard::Algorithms::Graphs
{
    // Find one source shortest path tree for a non-negative cost not-empty graph.
    // todo(p3): Test on a graph without negative cost cycles but with negative cost edges.

    constexpr auto absent_parent = -1;

    // Time O(n+m*log(n)) - suitable for a sparse graph.
    template<class graph_t, class weight_t>
    constexpr void dijkstra_sparse(const graph_t &graph, const std::int32_t source, std::vector<weight_t> &distances,
        std::vector<std::int32_t> &parents, std::set<std::pair<weight_t, std::int32_t>> &dist_vertex_temp,
        const weight_t &inf = std::numeric_limits<weight_t>::max() / 2)
    {
        const auto size = graph.size();

        assert(!(source < 0) && static_cast<std::size_t>(source) < size && weight_t{} < inf);

        distances.assign(size, inf);
        distances.at(source) = weight_t{};
        parents.assign(size, -1);

        dist_vertex_temp.clear();
        dist_vertex_temp.emplace(weight_t{}, source);

        do
        {
            const auto from = dist_vertex_temp.begin()->second;
            dist_vertex_temp.erase(dist_vertex_temp.begin());

            const auto &edges = graph[from];
            const auto &dist_from = distances[from];

            for (const auto &edge : edges)
            {
                const auto &tod = edge.vertex;
                assert(from != tod);

                const auto &weight = edge.weight;

                const auto cand = static_cast<weight_t>(dist_from + weight);
                assert(!(weight < weight_t{}) && !(cand < weight_t{}));

                auto &cur = distances[tod];
                if (!(cand < cur))
                {
                    continue;
                }

                if (cur != inf)
                {
                    const auto ite = dist_vertex_temp.find(std::make_pair(cur, tod));
                    assert(ite != dist_vertex_temp.end());
                    dist_vertex_temp.erase(ite);
                }

                cur = cand;
                dist_vertex_temp.emplace(cur, tod);
                parents[tod] = from;
            }
        } while (!dist_vertex_temp.empty());
    }

    // Time O(n*n) - good for a dense graph.
    template<class graph_t, class weight_t>
    constexpr void dijkstra_dense(const graph_t &graph, std::vector<bool> &processed, std::int32_t source,
        std::vector<weight_t> &distances, std::vector<std::int32_t> &parents,
        const weight_t &inf = std::numeric_limits<weight_t>::max() / 2)
    {
        const auto size = static_cast<std::int32_t>(graph.size());

        assert(0 <= source && source < size && size < std::numeric_limits<std::int32_t>::max() && weight_t{} < inf);

        parents.assign(size, -1);

        // +1 to have fewer checks when no vertex is found.
        distances.assign(size + 1LL, inf);
        distances.at(source) = {};
        processed.assign(distances.size(), false);

        for (auto step = size - 1;;)
        {
            assert(!(source < 0) && source < size && !processed[source]);

            processed[source] = true;

            const auto &edges = graph[source];
            const auto dist_source = distances[source];
            assert(!(dist_source < weight_t{}) && dist_source < inf);

            for (const auto &edge : edges)
            {
                const auto &tod = edge.vertex;

                assert(source != tod && !(tod < 0) && static_cast<std::int32_t>(tod) < size);

                const auto &weight = edge.weight;

                const auto cand = static_cast<weight_t>(dist_source + weight);
                assert(!(weight < weight_t{}) && !(cand < weight_t{}));

                auto &cur = distances[tod];
                if (cand < cur)
                {
                    cur = cand;
                    parents[tod] = source;
                }
            }

            if (--step <= 0) // A tree has size-1 edges.
            {
                break;
            }

            if (const auto node_has =
                    Standard::Algorithms::Graphs::Inner::dijkstra_dense_more<weight_t>(size, distances, inf, processed);
                node_has.second)
            {
                source = node_has.first;
            }
            else
            {
                break;
            }
        }

        distances.pop_back();
    }

    template<class vertex_t, class weight_t, class non_negative_weighted_vertex = weighted_vertex<vertex_t, weight_t>>
    struct dijkstra_prior_que final
    {
        dijkstra_prior_que() = delete;

        template<bool is_small_size = false>
        static constexpr void run(const graph<vertex_t, weight_t> &graph,
            // The indicator that the parent is missing e.g. -1.
            const vertex_t &absent_parent,
            // The maximum value for the given type e.g. such as (std::numeric_limits<std::int64_t>::max()/2).
            const weight_t infinite_weight,
            // The source vertex.
            const vertex_t source_vertex,
            // Returned minimum distances from "sourceVertex" to given vertex.
            std::vector<weight_t> &distances, std::vector<vertex_t> &parent_vertices)
        {
            require_positive(infinite_weight, "infiniteWeight");
            require_positive(infinite_weight - 1, "infiniteWeight - 1");

            const auto &adjacency_lists = graph.adjacency_lists();
            const auto vertex_count = adjacency_lists.size();
            if (vertex_count < 2U) [[unlikely]]
            {
                throw std::runtime_error("The graph must have at least 2 vertices.");
            }

            if (source_vertex < vertex_t{} || vertex_count <= static_cast<std::size_t>(source_vertex)) [[unlikely]]
            {
                auto str = ::Standard::Algorithms::Utilities::w_stream();
                str << "The sourceVertex " << source_vertex << " must be in [0, " << (vertex_count - 1U) << "].";
                throw_exception(str);
            }

            distances.assign(vertex_count, infinite_weight - 1);
            parent_vertices.assign(vertex_count, absent_parent);

            distances[source_vertex] = {};

            auto que = ::Standard::Algorithms::Heaps::priority_queue_molodetz<vertex_t, weight_t,
                non_negative_weighted_vertex, is_small_size>(vertex_count, infinite_weight);

            for (vertex_t vertex{}; static_cast<std::size_t>(vertex) < vertex_count; ++vertex)
            {
                que.emplace(vertex, infinite_weight - 1);
            }

            que.decrease_key(non_negative_weighted_vertex{ vertex_t(source_vertex), weight_t{} });

            do
            {
                const auto top = que.top();
                que.pop();

                const auto &adjacency_list = adjacency_lists[top.vertex];
                const auto adjacency_list_size = adjacency_list.size();
                const auto top_distance = distances[top.vertex];

                for (std::size_t index{}; index < adjacency_list_size; ++index)
                {
                    const auto &vertex2 = adjacency_list[index];

                    const auto new_distance = static_cast<weight_t>(top_distance + vertex2.weight);

                    auto &dist = distances[vertex2.vertex];
                    if (!(new_distance < dist))
                    {
                        continue;
                    }

                    if constexpr (::Standard::Algorithms::is_debug)
                    {
                        {
                            non_negative_weighted_vertex edge_in_queue{};

                            if (!que.try_get_item(
                                    non_negative_weighted_vertex{ vertex2.vertex, vertex2.weight }, edge_in_queue))
                            {
                                auto str = ::Standard::Algorithms::Utilities::w_stream();
                                str << "Priority queue must have vertex " << vertex2.vertex << " with weight "
                                    << vertex2.weight << " from vertex " << top.vertex << ".";
                                throw_exception(str);
                            }
                        }
                    }

                    dist = new_distance;
                    parent_vertices[vertex2.vertex] = top.vertex;
                    que.decrease_key(non_negative_weighted_vertex{ vertex2.vertex, new_distance });
                }
            } while (!que.is_empty());
        }

        static constexpr void find_path(
            // The indicator that the parent is missing e.g. -1.
            const vertex_t &absent_parent, const std::vector<vertex_t> &parent_vertices, vertex_t target_vertex,
            std::vector<vertex_t> &path)
        {
            path.clear();

            while (absent_parent != target_vertex)
            {
                path.push_back(target_vertex);
                target_vertex = parent_vertices.at(target_vertex);
            }

            std::reverse(path.begin(), path.end());
        }
    };
} // namespace Standard::Algorithms::Graphs
