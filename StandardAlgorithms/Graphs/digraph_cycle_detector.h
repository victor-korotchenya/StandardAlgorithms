#pragma once
#include"../Numbers/color_2bit.h"
#include<algorithm>
#include<concepts>
#include<stack>
#include<vector>

namespace Standard::Algorithms::Graphs::Inner
{
    template<class vertex_t>
    struct dig_cyc_detec_dfs_context final
    {
        const std::vector<std::vector<vertex_t>> &digraph;

        Standard::Algorithms::Numbers::color_2bit colors{};
        std::stack<vertex_t> sta{};
        vertex_t cycle_first_node{};
    };

    // Detect a cycle in a subgraph, reachable from the vertex root.
    template<class vertex_t>
    constexpr auto has_cycle(dig_cyc_detec_dfs_context<vertex_t> &context, const vertex_t root) -> bool
    {
        context.colors.specific({ root, Standard::Algorithms::Numbers::grey });

        for (const auto &tods = context.digraph[root]; const auto &tod : tods)
        {
            const auto color = context.colors.color(tod);
            if (color == Standard::Algorithms::Numbers::white)
            {
                context.sta.push(tod);

                if (has_cycle(context, tod))
                {
                    return true;
                }

                context.sta.pop();
            }
            else if (color < Standard::Algorithms::Numbers::white)
            {
                assert(Standard::Algorithms::Numbers::grey == color);
                context.cycle_first_node = tod;
                return true;
            }
        }

        context.colors.specific({ root, Standard::Algorithms::Numbers::black });
        return false;
    }

    template<class vertex_t>
    constexpr auto restore_cycle(dig_cyc_detec_dfs_context<vertex_t> &context) -> std::vector<vertex_t>
    {
        assert(!context.sta.empty());

        std::vector<vertex_t> vertex_indexes;

        while (!context.sta.empty() && context.cycle_first_node != context.sta.top())
        {
            vertex_indexes.push_back(context.sta.top());
            context.sta.pop();
        }

        vertex_indexes.push_back(context.cycle_first_node);

        std::reverse(vertex_indexes.begin(), vertex_indexes.end());

        return vertex_indexes;
    }
} // namespace Standard::Algorithms::Graphs::Inner

namespace Standard::Algorithms::Graphs
{
    // Whether the directed graph contains a cycle.
    // Vertices are [0 .. (n-1)].
    // See also detect_many_cycles.
    template<class vertex_t>
    constexpr auto digraph_cycle_detector(const std::vector<std::vector<vertex_t>> &digraph) -> std::vector<vertex_t>
    {
        Inner::dig_cyc_detec_dfs_context<vertex_t> context{ digraph };
        context.colors.white(digraph.size());

        const auto size = static_cast<vertex_t>(digraph.size());

        for (vertex_t from{}; from < size; ++from)
        {
            if (context.colors.color(from) == Standard::Algorithms::Numbers::white &&
                Inner::has_cycle<vertex_t>(context, from))
            {
                auto cycle = Inner::restore_cycle<vertex_t>(context);
                return cycle;
            }
        }

        return {};
    }
} // namespace Standard::Algorithms::Graphs
