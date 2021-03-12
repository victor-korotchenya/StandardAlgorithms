#pragma once
#include<cassert>
#include<concepts>
#include<ostream>
#include<type_traits>

namespace Standard::Algorithms::Graphs
{
    template<std::signed_integral weight_t>
    struct flow_edge final
    {
        std::int32_t to{};
        std::int32_t rev_edge_index{};
        weight_t capacity{};
        weight_t flow{};
    };

    template<class weight_t>
    auto operator<< (std::ostream &str, const flow_edge<weight_t> &edge) -> std::ostream &
    {
        str << "(" << edge.to << ", " << edge.flow << " of " << edge.capacity << ")";
        return str;
    }

    template<std::signed_integral weight_t, std::signed_integral cost_t>
    requires(sizeof(weight_t) <= sizeof(cost_t))
    struct cost_flow_edge final
    {
        std::int32_t to{};
        std::int32_t rev_edge_index{};
        weight_t capacity{};
        weight_t flow{};
        cost_t unit_cost{};
    };

    template<class weight_t, class cost_t>
    auto operator<< (std::ostream &str, const cost_flow_edge<weight_t, cost_t> &edge) -> std::ostream &
    {
        str << "(" << edge.to;

        if (edge.unit_cost != cost_t{})
        {
            str << ", cost " << edge.unit_cost;
        }

        str << ", " << edge.flow << " of " << edge.capacity << ")";
        return str;
    }
} // namespace Standard::Algorithms::Graphs
