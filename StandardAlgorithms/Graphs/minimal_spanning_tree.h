#pragma once
#include"../Utilities/require_utilities.h"
#include"graph.h"
#include"priority_queue.h"
#include"weighted_vertex.h"
#include<queue>
#include<vector>

namespace Standard::Algorithms::Graphs
{
    // See also jarnik_minimal_spanning_tree.h
    //
    // Prim - grow a tree.
    // Kruskal - grow a forest of trees.
    template<std::signed_integral vertex_t, std::integral weight_t, std::integral long_weight_t,
        class edge_t = Standard::Algorithms::weighted_vertex<vertex_t, weight_t>,
        class greater_t = Standard::Algorithms::weighted_vertex_weight_greater<vertex_t, weight_t, edge_t>>
    requires(sizeof(weight_t) <= sizeof(long_weight_t))
    struct minimal_spanning_tree final
    {
        using priority_queue_t = std::priority_queue<edge_t, std::vector<edge_t>, greater_t>;

        using get_vertex_function_t = std::size_t (*)(const edge_t &edge);

        using priority_queue_decrease_key_t =
            Standard::Algorithms::Heaps::priority_queue<edge_t, get_vertex_function_t, std::vector<edge_t>, edge_t>;

        static constexpr vertex_t initial_vertex{};
        static constexpr vertex_t absent_parent = -static_cast<vertex_t>(1);
        static constexpr auto plus_infinity_w = std::numeric_limits<weight_t>::max();

        static_assert(absent_parent < vertex_t{} && 1 < plus_infinity_w);

        minimal_spanning_tree() = delete;

        // See also get_vertex_function_t.
        [[nodiscard]] inline static constexpr auto get_vertex(const edge_t &edge) noexcept -> std::size_t
        {
            return static_cast<std::size_t>(edge.vertex);
        }

        static constexpr void prim(const Standard::Algorithms::Graphs::graph<vertex_t, weight_t> &graf,
            std::vector<vertex_t> &parent_vertices, long_weight_t &total_weight,
            const bool shall_check_connectivity = true)
        {
            // Ahtung! If the std::vector is declared as value (not as reference), performance will be lost!
            // const auto BAD = graph2.adjacency_lists();
            const auto &adjacency_lists = graf.adjacency_lists();

            const auto vertex_count = require_greater(adjacency_lists.size(), 1U, "graph size");
            const auto vertex_count_int = static_cast<vertex_t>(vertex_count);

            if (const auto cas = static_cast<std::size_t>(vertex_count_int); cas != vertex_count) [[unlikely]]
            {
                throw std::runtime_error(std::string("Vertex type '") + typeid(vertex_t).name() +
                    "' is too small to hold cast " + std::to_string(cas) + " != " + std::to_string(vertex_count) +
                    " actual nodes.");
            }

            parent_vertices.assign(vertex_count, absent_parent);
            total_weight = {};

            // todo(p3): check. All indexes must be within [0..(vertexCount-1)]
            priority_queue_decrease_key_t que(get_vertex, vertex_count);
            add_vertices_to_queue(vertex_count_int, que);

            do
            {
                const auto top = que.top();
                que.pop();

                total_weight += top.weight;
                update_adjacents(adjacency_lists, top.vertex, parent_vertices, que);
            } while (!que.is_empty());

            if (shall_check_connectivity)
            {
                // todo(p3): CheckConnectivity(vertexCount, visitedVertices);
            }
        }

        // todo(p3): In the original, a queue by vertices must be used and
        // it must support decreasing the vertex weight.
        static constexpr void prim_by_edges(const Standard::Algorithms::Graphs::graph<vertex_t, weight_t> &graf,
            std::vector<vertex_t> &parent_vertices, long_weight_t &total_weight,
            const bool shall_check_connectivity = true)
        {
            const auto &adjacency_lists = graf.adjacency_lists();
            const auto vertex_count = require_greater(adjacency_lists.size(), 1U, "graph size");

            parent_vertices.assign(vertex_count, absent_parent);
            total_weight = {};

            std::vector<bool> visited_vertices(vertex_count);
            std::vector<weight_t> weights(vertex_count, plus_infinity_w - 1);
            weights[initial_vertex] = weight_t{};

            priority_queue_t que;
            que.emplace(initial_vertex, weight_t{});

            std::size_t visited_count{};
            do
            {
                const auto top = que.top();
                que.pop();

                const auto &from = top.vertex;
                if (visited_vertices[from])
                {
                    continue;
                }

                visited_vertices[from] = true;

                total_weight += top.weight;
                ++visited_count;

                if (const auto is_now_tree = visited_count == vertex_count; is_now_tree)
                {
                    break;
                }

                add_adjacent_edges(adjacency_lists, from, parent_vertices, visited_vertices, weights, que);
            } while (!que.empty());

            if (shall_check_connectivity)
            {
                check_connectivity(vertex_count, visited_vertices);
            }
        }

private:
        inline static constexpr void update_adjacents(const std::vector<std::vector<edge_t>> &adjacency_lists,
            const vertex_t from, std::vector<vertex_t> &parent_vertices, auto &que)
        {
            edge_t edge_in_queue{};

            const auto &adjacency_list = adjacency_lists[from];

            for (const auto &edge : adjacency_list)
            {
                if (que.try_get_item(edge, edge_in_queue) && edge.weight < edge_in_queue.weight)
                {
                    parent_vertices.at(edge.vertex) = from;
                    que.decrease_key(edge);
                }
            }
        }

        inline static constexpr void add_adjacent_edges(const std::vector<std::vector<edge_t>> &adjacency_lists,
            const vertex_t from, std::vector<vertex_t> &parent_vertices, const std::vector<bool> &visited_vertices,
            std::vector<weight_t> &weights, priority_queue_t &que)
        {
            if constexpr (::Standard::Algorithms::is_debug)
            {
                if (!visited_vertices[from]) [[unlikely]]
                {
                    throw std::runtime_error(
                        "Error2. The first vertex " + std::to_string(from) + " of an edge must have been visited.");
                }
            }

            const auto &adjacency_list = adjacency_lists[from];

            for (const auto &edge : adjacency_list)
            {
                const auto &tod = edge.vertex;

                if (visited_vertices[tod])
                {
                    continue;
                }

                const auto &weight_old = weights[tod];
                const auto &weight_new = edge.weight;

                if (!(weight_new < weight_old))
                {
                    continue;
                }

                que.emplace(tod, weight_new);
                parent_vertices[tod] = from;
                weights[tod] = weight_new;
            }
        }

        static constexpr void check_connectivity(
            const std::size_t vertex_count, const std::vector<bool> &visited_vertices)
        {
            for (std::size_t index{}; index < vertex_count; ++index)
            {
                if (!visited_vertices[index])
                {
                    auto str = ::Standard::Algorithms::Utilities::w_stream();
                    str << "The graph is not connected: the vertex (" << index << ") has not been visited.";
                    throw_exception(str);
                }
            }
        }

        inline static constexpr void add_vertices_to_queue(const vertex_t vertex_count, auto &que)
        {
            que.emplace(initial_vertex, weight_t{});

            constexpr vertex_t skip_initial_vertex{ 1 };

            for (auto vertex = skip_initial_vertex; vertex < vertex_count; ++vertex)
            {
                que.emplace(vertex, plus_infinity_w - 1);
            }
        }
    };
} // namespace Standard::Algorithms::Graphs
