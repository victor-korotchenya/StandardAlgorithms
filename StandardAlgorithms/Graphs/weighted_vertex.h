#pragma once
#include"../Utilities/throw_exception.h"

namespace Standard::Algorithms
{
    template<class vertex_t, class weight_t>
    struct weighted_vertex final
    {
        // The weight must come first for the default comparison to work correctly.
        // NOLINTNEXTLINE
        weight_t weight{};

        // NOLINTNEXTLINE
        vertex_t vertex{};

        weighted_vertex() = default;

        constexpr weighted_vertex(const vertex_t &vertex1, const weight_t &weight2)
            : weight(weight2)
            , vertex(vertex1)
        {
        }

        [[nodiscard]] constexpr explicit operator vertex_t () const
        {
            return vertex;
        }

        // less or equal by weight. todo(p3): <, not <= ?
        [[nodiscard]] constexpr auto operator() (const weighted_vertex &one, const weighted_vertex &two) const -> bool
        {
            auto result = one <= two;
            return result;
        }

        [[nodiscard]] constexpr auto operator<=> (const weighted_vertex &) const noexcept = default;
    };

    template<class vertex_t, class weight_t>
    auto operator<< (std::ostream &str, const weighted_vertex<vertex_t, weight_t> &node) -> std::ostream &
    {
        str << "Vertex " << node.vertex << ", weight " << node.weight;

        return str;
    }

    template<class vertex_t, class weight_t, class node_t = weighted_vertex<vertex_t, weight_t>>
    struct weighted_vertex_weight_greater final
    {
        // Maximum weight.
        [[nodiscard]] constexpr auto operator() (const node_t &one, const node_t &two) const -> bool
        {
            auto result = two < one;
            return result;
        }
    };
} // namespace Standard::Algorithms
