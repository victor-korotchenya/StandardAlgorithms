#pragma once
#include <cassert>
#include <ostream>
#include <type_traits>

namespace Privet::Algorithms::Graphs
{
    template <class weight_t>
    struct flow_edge final
    {
        static_assert(std::is_signed_v<weight_t>);

        int to;
        int rev_edge_index;
        weight_t capacity;
        weight_t flow;

        flow_edge(int to = {}, int rev_edge_index = {}, weight_t capacity = {}, weight_t flow = {})
            : to(to), rev_edge_index(rev_edge_index), capacity(capacity), flow(flow)
        {
            assert(to >= 0 && rev_edge_index >= 0 && capacity >= 0);
        }

        friend std::ostream& operator <<
            (std::ostream& str, const flow_edge& b)
        {
            str
                << "(" << b.to
                << ", " << b.flow
                << " of " << b.capacity << ")";
            return str;
        }
    };

    template<class weight_t, class cost_t>
    struct cost_flow_edge final
    {
        static_assert(sizeof(weight_t) <= sizeof(cost_t) && std::is_signed_v<weight_t> && std::is_signed_v<cost_t>);

        int to;
        int rev_edge_index;
        weight_t capacity;
        weight_t flow;
        cost_t unit_cost;

        cost_flow_edge(int to = {}, int rev_edge_index = {}, weight_t capacity = {}, weight_t flow = {}, const cost_t unit_cost = {})
            : to(to), rev_edge_index(rev_edge_index), capacity(capacity), flow(flow), unit_cost(unit_cost)
        {
            assert(to >= 0 && rev_edge_index >= 0 && capacity >= 0);
        }

        friend std::ostream& operator <<
            (std::ostream& str, const cost_flow_edge& b)
        {
            str << "(" << b.to;
            if (b.unit_cost)
                str << ", cost " << b.unit_cost;

            str << ", " << b.flow
                << " of " << b.capacity << ")";
            return str;
        }
    };
}