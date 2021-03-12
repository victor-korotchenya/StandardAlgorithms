#pragma once
#include<algorithm>
#include<cassert>
#include<cstdint>
#include<utility>
#include<vector>

namespace Standard::Algorithms::Graphs::Inner
{
    template<class vertex_t>
    constexpr auto count_components(const std::vector<std::vector<vertex_t>> &graph, std::vector<bool> &visited,
        std::vector<std::int32_t> &sta) -> std::int32_t
    {
        const auto size = static_cast<std::int32_t>(graph.size());
        visited.assign(size, false);

        sta.clear();

        std::int32_t count{};

        for (std::int32_t index{}; index < size; ++index)
        {
            if (visited[index])
            {
                continue;
            }

            ++count;
            visited[index] = true;
            sta.push_back(index);
            do
            {
                const auto from = sta.back();
                sta.pop_back();

                const auto &edges = graph[from];
                for (const auto &edge : edges)
                {
                    const auto &tod = static_cast<std::int32_t>(edge);
                    if (visited[tod])
                    {
                        continue;
                    }

                    visited[tod] = true;
                    sta.push_back(tod);
                }
            } while (!sta.empty());
        }

        return count;
    }

    template<class vertex_t>
    struct cut_context final
    {
        const std::vector<std::vector<vertex_t>> &graph;

        std::vector<std::int32_t> times{};
        std::vector<std::int32_t> min_times{};

        std::vector<std::int32_t> cut_nodes{};
        std::vector<std::pair<std::int32_t, std::int32_t>> bridges{};
        std::int32_t time{};
    };

    template<class vertex_t>
    constexpr auto find_cuts(const std::int32_t from, cut_context<vertex_t> &context, const std::int32_t parent = -1)
        -> bool
    {
        auto &tim = context.times.at(from);
        auto &min_tim = context.min_times[from];
        assert(-1 <= parent && tim == 0 && min_tim == 0);

        tim = min_tim = ++context.time;

        const auto &edges = context.graph[from];

        constexpr auto one_branch_bit = 1U;
        constexpr auto two_branches_bit = 2U;
        constexpr auto cut_bit = 4U;

        std::uint32_t flags{};

        for (const auto &edge : edges)
        {
            const auto &tod = static_cast<std::int32_t>(edge);
            assert(!(tod < 0) && from != tod);

            const auto &min_tim_2 = context.min_times[tod];
            if (0 < min_tim_2)
            {
                if (parent == tod)
                {
                    continue;
                }

                // Using min_tim_2 would be an error for artic. points:
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
                const auto &tim_3 = context.times[tod];
                min_tim = std::min(min_tim, tim_3);

                continue;
            }

            if (find_cuts<vertex_t>(tod, context, from))
            {
                context.cut_nodes.push_back(tod);
            }

            if (min_tim_2 < min_tim)
            {
                min_tim = min_tim_2;
                continue;
            }

            if (min_tim_2 < tim)
            {
                continue;
            }

            // 'from' is a cut node when it isn't a root, and
            // there is no edge 'tod' -> some parent of 'from'.
            flags |= cut_bit;

            // No need to keep more than 2.
            flags |= (flags & one_branch_bit) != 0U ? two_branches_bit : one_branch_bit;

            if (tim < min_tim_2)
            {// 'tod' can reach 'from' in the only way.
                context.bridges.emplace_back(from, tod);
            }
        }

        if (const auto is_root = parent < 0 && (flags & two_branches_bit) != 0U; is_root)
        {
            context.cut_nodes.push_back(from);
        }

        return (flags & cut_bit) != 0U;
    }
} // namespace Standard::Algorithms::Graphs::Inner

namespace Standard::Algorithms::Graphs
{
    // Detect bridge edges (cut-edges, or cut arcs) and
    // articulation points (cut-nodes, or cut vertices) in an undirected graph
    // using Tarjan's idea.
    // Bridges belong to no cycle, or their deletion increases the connected component number.
    // Cut-node deletion with its edges increases the connected component number.
    // A leaf or a standalone node is never a cut node.
    // Vertices are [0 .. (n-1)].
    template<class vertex_t>
    constexpr auto bridge_detector(const std::vector<std::vector<vertex_t>> &graph)
        -> std::pair<std::vector<std::pair<std::int32_t, std::int32_t>>, std::vector<std::int32_t>>
    {
        const auto size = static_cast<std::int32_t>(graph.size());

        Inner::cut_context<vertex_t> context{ graph, std::vector<std::int32_t>(size, 0),
            std::vector<std::int32_t>(size, 0) };

        for (std::int32_t from{}; from < size; ++from)
        {
            if (context.times[from] == 0)
            {
                Inner::find_cuts<vertex_t>(from, context);
            }
        }

        return { context.bridges, context.cut_nodes };
    }

    // Slow: edge deletion increases the connected component number.
    template<class vertex_t>
    constexpr auto bridge_detector_slow(const std::vector<std::vector<vertex_t>> &source)
        -> std::vector<std::pair<std::int32_t, std::int32_t>>
    {
        std::vector<std::pair<std::int32_t, std::int32_t>> bridges;

        auto graph = source;
        const auto size = static_cast<std::int32_t>(graph.size());

        std::vector<bool> visited;
        std::vector<std::int32_t> sta;
        sta.reserve(size);

        const auto component_count_0 = Inner::count_components(graph, visited, sta);

        for (std::int32_t from{}; from < size; ++from)
        {
            auto &edges = graph[from];
            const auto edges_size = static_cast<std::int32_t>(edges.size());
            for (std::int32_t index{}; index < edges_size; ++index)
            {
                const auto tod = static_cast<std::int32_t>(edges[index]);
                if (tod < from)
                {
                    continue;
                }

                assert(from < tod);

                const auto edge1 = edges[index];
                // todo(p4): use an adjacency matrix.
                edges.erase(edges.begin() + index);

                // Temporarily delete the edge to->from.
                auto &edges_other = graph[tod];
                const auto it2 = std::find(edges_other.cbegin(), edges_other.cend(),
                    // todo(p3): lambda.
                    from);
                const auto pos = static_cast<std::int32_t>(it2 - edges_other.cbegin());
                assert(it2 != edges_other.cend() && !(pos < 0) && pos < static_cast<std::int32_t>(edges_other.size()));

                const auto edge2 = *it2;
                edges_other.erase(it2);

                const auto count = Inner::count_components(graph, visited, sta);
                edges.insert(edges.begin() + index, edge1);
                edges_other.insert(edges_other.begin() + pos, edge2);
                assert(edges == source[from]);
                assert(edges_other == source[tod]);

                if (count == component_count_0)
                {
                    continue;
                }

                bridges.emplace_back(from, tod);
                assert(count - 1 == component_count_0 && bridges.size() < source.size());
            }
        }

        return bridges;
    }

    // Slow: node deletion increases the connected component number.
    template<class vertex_t>
    constexpr auto cut_node_detector_slow(const std::vector<std::vector<vertex_t>> &source) -> std::vector<std::int32_t>
    {
        std::vector<std::int32_t> cut_nodes;

        auto graph = source;
        const auto size = static_cast<std::int32_t>(graph.size());

        // The actual position of an edge might change in 'graph'.
        std::vector<std::pair<vertex_t, std::int32_t>> buf;
        buf.reserve(size);

        std::vector<bool> visited;
        std::vector<std::int32_t> sta;
        sta.reserve(size);

        const auto component_count_0 = Inner::count_components(graph, visited, sta);

        for (std::int32_t from{}; from < size; ++from)
        {
            auto &orig_edges = graph[from];
            if (orig_edges.size() <= 1U)
            {
                continue;
            }

            auto edges = std::move(orig_edges);
            assert(graph[from].empty());

            buf.clear();

            for (const auto &edge : edges)
            {
                const auto tod = static_cast<std::int32_t>(edge);
                assert(from != tod);

                // Temporarily delete the edge tod->from.
                auto &edges_other = graph[tod];
                const auto it2 = std::find(edges_other.cbegin(), edges_other.cend(),
                    // todo(p3): lambda.
                    from);
                const auto pos = static_cast<std::int32_t>(it2 - edges_other.cbegin());

                assert(it2 != edges_other.end() && !(pos < 0) && pos < static_cast<std::int32_t>(edges_other.size()));

                buf.emplace_back(std::move(*it2), tod);
                std::swap(edges_other[pos], edges_other.back());
                edges_other.pop_back();
            }

            const auto count = Inner::count_components(graph, visited, sta) - 1;
            if (count != component_count_0)
            {
                cut_nodes.push_back(from);
                assert(component_count_0 < count && cut_nodes.size() <= source.size());
            }

            orig_edges = std::move(edges);

            for (auto &par : buf)
            {
                auto &edge = par.first;
                const auto &tod = par.second;
                assert(from != tod);

                // Insert back the deleted edge tod->from.
                auto &edges_other = graph[tod];
                edges_other.push_back(std::move(edge));
            }
        }

        return cut_nodes;
    }
} // namespace Standard::Algorithms::Graphs
