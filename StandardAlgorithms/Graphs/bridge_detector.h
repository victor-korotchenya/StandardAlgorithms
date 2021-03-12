#pragma once
#include <cassert>
#include <vector>

namespace
{
    template <typename vertex_t>
    int count_components(const std::vector<std::vector<vertex_t>>& graph,
        std::vector<bool>& visited,
        std::vector<int>& st)
    {
        const auto size = static_cast<int>(graph.size());
        visited.assign(size, false);
        st.clear();

        auto result = 0;
        for (auto i = 0; i < size; ++i)
        {
            if (visited[i])
                continue;

            ++result;
            visited[i] = true;
            st.push_back(i);
            do
            {
                const auto from = st.back();
                st.pop_back();

                const auto& edges = graph[from];
                for (const auto& edge : edges)
                {
                    const auto& to = static_cast<int>(edge);
                    if (visited[to])
                        continue;

                    visited[to] = true;
                    st.push_back(to);
                }
            } while (st.size());
        }

        return result;
    }

    template <typename vertex_t>
    struct cut_context final
    {
        const std::vector<std::vector<vertex_t>>& graph;
        std::vector<int> times, min_times, cut_nodes;
        std::vector<std::pair<int, int>> bridges;
        int time;

        explicit cut_context(const std::vector<std::vector<vertex_t>>& graph)
            : graph(graph),
            times(graph.size()),
            min_times(graph.size()),
            cut_nodes(),
            bridges(),
            time(0)
        {
        }
    };

    template <typename vertex_t>
    bool find_cuts(cut_context<vertex_t>& context, const int from, const int parent = -1)
    {
        auto& t = context.times[from];
        auto& mt = context.min_times[from];
        assert(parent >= -1 && !t && !mt);

        t = mt = ++context.time;

        const auto& edges = context.graph[from];

        constexpr auto one_branch_bit = 1,
            two_branches_bit = 2,
            cut_bit = 4;
        char flags = 0;

        for (const auto& edge : edges)
        {
            const auto& to = static_cast<int>(edge);
            assert(to >= 0 && from != to);

            auto& mt2 = context.min_times[to];
            if (mt2)
            {
                if (parent != to)
                {
                    // Using mt2 would be an error for artic. points:
                    // - first set min_time[2] = min_time[0] = 1;
                    // - second set min_time[4] = min_time[2] = 1;
                    // - then set min_time[3] = min_time[4] = 1;
                    // - last (time[2] <= min_time[3]) doesn't hold as time[2] = 3.
                    //
                    //  ---------
                    //  |       |
                    //  0 - 1 - 2 - 3 - 4
                    //          |       |
                    //          ---------
                    //
                    const auto& t2 = context.times[to];
                    if (mt > t2)
                        mt = t2;
                }

                continue;
            }

            if (find_cuts<vertex_t>(context, to, from))
                context.cut_nodes.push_back(to);

            if (mt > mt2)
                mt = mt2;
            else if (t <= mt2)
            {
                // 'from' is a cut node when it isn't a root, and
                // there is no edge 'to' -> some parent of 'from'.
                flags |= cut_bit;

                // No need to keep more than 2.
                flags |= flags & one_branch_bit ? two_branches_bit : one_branch_bit;

                if (t < mt2)
                    // 'to' can reach 'from' in the only way.
                    context.bridges.emplace_back(from, to);
            }
        }

        if (parent < 0 && flags & two_branches_bit)
            // The root node.
            context.cut_nodes.push_back(from);

        return flags & cut_bit;
    }
}

namespace Privet
{
    namespace Algorithms
    {
        namespace Graphs
        {
            // Detect bridge edges (cut-edges, or cut arcs) and
            // articulation points (cut-nodes, or cut vertices) in an undirected graph
            // using Tarjan's idea.
            // Bridges belong to no cycle, or their deletion increases the connected component number.
            // Cut-node deletion with its edges increases the connected component number.
            // A leaf or a standalone node is never a cut node.
            // Vertices are [0 .. (n-1)].
            template <typename vertex_t>
            std::pair<std::vector<std::pair<int, int>>, std::vector<int>> bridge_detector(
                const std::vector<std::vector<vertex_t>>& graph)
            {
                cut_context<vertex_t> context(graph);

                const auto size = static_cast<int>(graph.size());
                for (auto from = 0; from < size; ++from)
                {
                    if (!context.times[from])
                    {
                        find_cuts<vertex_t>(context, from);
                    }
                }

                return { context.bridges, context.cut_nodes };
            }

            // Slow: edge deletion increases the connected component number.
            template <typename vertex_t>
            std::vector<std::pair<int, int>> bridge_detector_slow(
                const std::vector<std::vector<vertex_t>>& source)
            {
                std::vector<std::pair<int, int>> bridges;

                auto graph = source;
                const auto size = static_cast<int>(graph.size());

                std::vector<bool> visited;
                std::vector<int> st;
                st.reserve(size);

                const auto component_count_0 = count_components(graph, visited, st);
                for (auto from = 0; from < size; ++from)
                {
                    auto& edges = graph[from];
                    const auto edges_size = static_cast<int>(edges.size());
                    for (auto i = 0; i < edges_size; ++i)
                    {
                        const auto to = static_cast<int>(edges[i]);
                        if (to < from)
                            continue;
                        assert(from < to);

                        const auto edge1 = edges[i];
                        // todo: p4. use adjacency matrix.
                        edges.erase(edges.begin() + i);

                        // Temporarily delete the edge to->from.
                        auto& edges_other = graph[to];
                        const auto it2 = std::find(edges_other.begin(), edges_other.end(),
                            // todo: p3. lambda.
                            from);
                        const auto j = static_cast<int>(it2 - edges_other.begin());
                        assert(it2 != edges_other.end() && j >= 0 && j < static_cast<int>(edges_other.size()));
                        const auto edge2 = *it2;
                        edges_other.erase(it2);

                        const auto count = count_components(graph, visited, st);
                        edges.insert(edges.begin() + i, edge1);
                        edges_other.insert(edges_other.begin() + j, edge2);
                        assert(edges == source[from]);
                        assert(edges_other == source[to]);

                        if (count == component_count_0)
                            continue;

                        bridges.emplace_back(from, to);
                        assert(count - 1 == component_count_0 && bridges.size() < source.size());
                    }
                }

                return bridges;
            }

            // Slow: node deletion increases the connected component number.
            template <typename vertex_t>
            std::vector<int> cut_node_detector_slow(
                const std::vector<std::vector<vertex_t>>& source)
            {
                std::vector<int> cut_nodes;

                auto graph = source;
                const auto size = static_cast<int>(graph.size());

                // The actual position of an edge might change in 'graph'.
                std::vector<std::pair<vertex_t, int>> buf;
                buf.reserve(size);

                std::vector<bool> visited;
                std::vector<int> st;
                st.reserve(size);

                const auto component_count_0 = count_components(graph, visited, st);
                for (auto from = 0; from < size; ++from)
                {
                    auto& orig_edges = graph[from];
                    if (orig_edges.size() <= 1)
                        continue;

                    auto edges = std::move(orig_edges);
                    assert(!graph[from].size());
                    buf.clear();

                    for (const auto& edge : edges)
                    {
                        const auto to = static_cast<int>(edge);
                        assert(from != to);

                        // Temporarily delete the edge to->from.
                        auto& edges_other = graph[to];
                        const auto it2 = std::find(edges_other.begin(), edges_other.end(),
                            // todo: p3. lambda.
                            from);
                        const auto j = static_cast<int>(it2 - edges_other.begin());
                        assert(it2 != edges_other.end() && j >= 0 && j < static_cast<int>(edges_other.size()));

                        buf.emplace_back(std::move(*it2), to);
                        std::swap(edges_other[j], edges_other.back());
                        edges_other.pop_back();
                    }

                    const auto count = count_components(graph, visited, st) - 1;
                    if (count != component_count_0)
                    {
                        cut_nodes.push_back(from);
                        assert(component_count_0 < count&& cut_nodes.size() <= source.size());
                    }

                    orig_edges = std::move(edges);

                    for (auto& p : buf)
                    {
                        auto& edge = p.first;
                        const auto& to = p.second;
                        assert(from != to);

                        // Insert back the deleted edge to->from.
                        auto& edges_other = graph[to];
                        edges_other.push_back(std::move(edge));
                    }
                }

                return cut_nodes;
            }
        }
    }
}