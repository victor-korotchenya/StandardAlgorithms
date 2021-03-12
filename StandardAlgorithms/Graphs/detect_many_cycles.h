#pragma once
#include <cassert>
#include <vector>

namespace
{
    template <class graph_t>
    struct dfs_context final
    {
        const graph_t& graph;
        std::vector<std::vector<int>>& cycles;
        std::vector<int>& depths, & st;

        dfs_context(const graph_t& graph,
            std::vector<std::vector<int>>& cycles,
            std::vector<int>& depths,
            std::vector<int>& st)
            : graph(graph), cycles(cycles), depths(depths), st(st)
        {
        }
    };

    template <class graph_t>
    void find_cycle(dfs_context<graph_t>& context,
        const int cur, const int par)
    {
        const auto& depth = context.depths[cur];
        assert(depth >= 0);
        context.st[depth] = cur;

        const auto& nodes = context.graph[cur];
        for (const auto& child : nodes)
        {
            if (child == par)
                continue;

            if (context.depths[child] < 0)
            {
                context.depths[child] = depth + 1;
                find_cycle(context, child, cur);
                continue;
            }

            if (context.depths[child] > depth)
                continue;

            context.cycles.push_back({});
            auto& cycle = context.cycles.back();
            for (auto j = context.depths[child]; j <= depth; ++j)
                cycle.push_back(context.st[j]);
        }
    }
}

namespace Privet
{
    namespace Algorithms
    {
        namespace Graphs
        {
            // An undirected simple graph might have only simple cycles, each having >= 3 nodes.
            // So, each vertex belongs to max 1 simple cycle.
            // Nodes can start from 0 or 1.
            //
            // See also digraph_cycle_detector.
            template <typename vertex_t>
            void detect_many_cycles(const std::vector<std::vector<vertex_t>>& graph,
                std::vector<std::vector<int>>& cycles,
                // temp
                std::vector<int>& depths,
                std::vector<int>& st,
                // end of temp
                const int first_node)
            {
                const auto size = static_cast<int>(graph.size());
                assert(first_node >= 0 && first_node <= 1 && size > 0);

                cycles.clear();
                depths.assign(graph.size(), -1);
                st.resize(size);
#ifdef _DEBUG
                st.assign(size, -2);
#endif
                dfs_context<std::vector<std::vector<vertex_t>>> context(graph, cycles, depths, st);
                for (auto v = first_node; v < size; ++v)
                {
                    if (context.depths[v] < 0)
                    {
                        context.depths[v] = 0;
                        find_cycle(context, v, -1);
                    }
                }
            }
        }
    }
}