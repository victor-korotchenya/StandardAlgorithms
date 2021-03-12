#pragma once
//#include <algorithm> //reverse
#include <stack>
#include <vector>

namespace
{
    template <typename vertex_t>
    struct dfs_context final
    {
        const std::vector<std::vector<vertex_t>>& digraph;
        std::vector<char> visited;
        std::stack<int> st;
        int cycle_first_node;

        explicit dfs_context(const std::vector<std::vector<vertex_t>>& digraph)
            : digraph(digraph),
            visited(digraph.size()),
            st(),
            cycle_first_node(0)
        {
        }
    };

    //Detect a cycle in a subgraph, reachable from the vertex root.
    template <typename vertex_t>
    bool has_cycle(dfs_context<vertex_t>& context, const int root)
    {
        context.visited[root] = 1;
        for (const auto& u : context.digraph[root])
        {
            const auto& color = context.visited[u];
            if (!color)
            {
                context.st.push(u);
                if (has_cycle(context, u))
                    return true;

                context.st.pop();
            }
            else if (1 == color)
            {
                context.cycle_first_node = u;
                return true;
            }
        }

        context.visited[root] = 2;
        return false;
    }

    template <typename vertex_t>
    std::vector<int> restore_cycle(dfs_context<vertex_t>& context)
    {
        assert(context.st.size());

        std::vector<int> vertex_indexes;
        while (!context.st.empty() && context.cycle_first_node != context.st.top())
        {
            vertex_indexes.push_back(context.st.top());
            context.st.pop();
        }

        vertex_indexes.push_back(context.cycle_first_node);
        std::reverse(vertex_indexes.begin(), vertex_indexes.end());
        return vertex_indexes;
    }
}

namespace Privet
{
    namespace Algorithms
    {
        namespace Graphs
        {
            //Whether the directed graph contains a cycle.
            //Vertices are [0 .. (n-1)].
            template <typename vertex_t>
            std::vector<int> digraph_cycle_detector(
                const std::vector<std::vector<vertex_t>>& digraph)
            {
                dfs_context<vertex_t> context(digraph);

                const auto size = static_cast<int>(digraph.size());
                for (auto v = 0; v < size; ++v)
                {
                    if (!context.visited[v] && has_cycle<vertex_t>(context, v))
                    {
                        const auto cycle = restore_cycle<vertex_t>(context);
                        return cycle;
                    }
                }

                return {};
            }
        }
    }
}