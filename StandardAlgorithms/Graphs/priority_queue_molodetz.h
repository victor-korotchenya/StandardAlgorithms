#pragma once
#include"priority_queue.h"
#include"priority_queue_limited_size.h"
#include"weighted_vertex.h"

namespace Standard::Algorithms::Heaps
{
    template<class edge_t>
    [[nodiscard]] inline constexpr auto vertex_index(const edge_t &edge) noexcept -> std::size_t
    {
        return static_cast<std::size_t>(edge.vertex);
    }

    template<class vertex_t, class weight_t, class non_negative_weighted_vertex_t = weighted_vertex<vertex_t, weight_t>,
        bool is_small_size = false>
    [[nodiscard]] constexpr auto priority_queue_molodetz(
        const std::size_t vertex_count, [[maybe_unused]] const weight_t &infinite_weight)
    {
        if constexpr (is_small_size)
        {
            return Standard::Algorithms::Heaps::priority_queue_limited_size<non_negative_weighted_vertex_t, weight_t>(
                vertex_count, infinite_weight);
        }
        else
        {
            using vertex_index_function_t = std::size_t (*)(const non_negative_weighted_vertex_t &);

            vertex_index_function_t func = vertex_index<non_negative_weighted_vertex_t>;

            return Standard::Algorithms::Heaps::priority_queue<non_negative_weighted_vertex_t, vertex_index_function_t,
                std::vector<non_negative_weighted_vertex_t>, non_negative_weighted_vertex_t>(func, vertex_count);
        }
    }
} // namespace Standard::Algorithms::Heaps
