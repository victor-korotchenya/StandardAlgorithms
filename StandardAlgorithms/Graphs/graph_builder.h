#pragma once
// "graph_builder.h"
#include<cassert>
#include<concepts>
#include<cstddef>
#include<numeric> // std::iota
#include<vector>

// See also "create_random_graph.h".
namespace Standard::Algorithms::Graphs
{
    [[nodiscard]] inline constexpr auto build_complete_graph_boolean(
        const std::size_t size, const bool let_self_loops = false) -> std::vector<std::vector<bool>>
    {
        constexpr auto default_value = true;

        std::vector<std::vector<bool>> graph(size, std::vector<bool>(size, default_value));

        if (let_self_loops)
        {
            return graph;
        }

        for (std::size_t index{}; index < size; ++index)
        {
            graph[index][index] = !default_value;
        }

        return graph;
    }

    template<std::integral int_t, class edges_t = std::vector<int_t>, class graph_t = std::vector<edges_t>>
    [[nodiscard]] constexpr auto build_complete_graph(const int_t &size, const bool let_self_loops = false) -> graph_t
    {
        if (int_t{} == size)
        {
            return {};
        }

        assert(int_t{} < size);

        if (let_self_loops)
        {
            edges_t edges(size, int_t{});
            std::iota(edges.begin(), edges.end(), int_t{});

            return graph_t(size, edges);
        }

        const auto neighbor_edge_count = size - static_cast<int_t>(1);

        graph_t graph(size, edges_t(neighbor_edge_count, int_t{}));

        for (int_t index{}; index < size; ++index)
        {
            // No const.
            auto &edges = graph[index];

            for (int_t ind_2{}; auto &edge : edges)
            {
                ind_2 += index == ind_2 ? 1 : 0;
                edge = ind_2;
                ++ind_2;
            }
        }

        return graph;
    }
} // namespace Standard::Algorithms::Graphs
