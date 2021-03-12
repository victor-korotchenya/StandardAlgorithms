#pragma once
#include <algorithm>
#include <array>
#include <limits>
#include <numeric>
#include <vector>
#include "flow_edge.h"

namespace Privet::Algorithms::Graphs
{
    // Dinic's max flow algorithm.
    template <class weight_t, int n_max, class edge_t = flow_edge<weight_t>>
    class max_flow_graph
    {
        static_assert(std::is_signed_v<weight_t>&& n_max >= 2);

        std::array<std::vector<edge_t>, n_max> graph = {};
        std::array<int, n_max> bfs_levels = {}, last_visited = {};
        std::array<std::vector<int>, 2> qs = {};

        const int source, sink, n;

    public:
        max_flow_graph(const int source, const int sink, const int n)
            : source(source), sink(sink), n(n)
        {
            assert(source >= 0 && source < n&& sink >= 0 && sink < n&& source != sink && n >= 2 && n <= n_max);

            const auto capacity = n - 1;
            qs[0].reserve(capacity), qs[1].reserve(capacity);
        }

        auto size() const noexcept
        {
            return n;
        }

        auto& get_graph() noexcept
        {
            return graph;
        }

        void clear()
        {
            for (auto i = 0; i < n; ++i)
                graph[i].clear();
        }

        void add_edge(const int from, const int to, const weight_t capacity)
        {
            assert(from >= 0 && from < n&& to >= 0 && to < n&& from != to && capacity > 0);

            const auto s = static_cast<int>(graph[to].size()),
                s2 = static_cast<int>(graph[from].size());

            graph[from].emplace_back(to, s, capacity);
            graph[to].emplace_back(from, s2);
        }

        weight_t max_flow()
        {
            constexpr weight_t inf = std::numeric_limits<weight_t>::max() / 2;
            weight_t flow{};

            while (find_flow())
            {
                std::fill(last_visited.begin(), last_visited.begin() + n, 0);

                while (const auto f = get_flow(source, inf))
                {
                    flow += f;
                    assert(f > 0 && flow > 0);
#ifdef _DEBUG
                    const auto  lam = [](const weight_t tot, const edge_t& edge) {
                        return tot + edge.flow;
                    };
                    const auto source_flow = std::accumulate(graph[source].begin(), graph[source].end(), weight_t(), lam);
                    assert(source_flow == flow);

                    const auto sink_flow = std::accumulate(graph[sink].begin(), graph[sink].end(), weight_t(), lam);
                    assert(-sink_flow == flow);
#endif
                }
            }

            return flow;
        }

    private:
        bool find_flow()
        {
            std::fill(bfs_levels.begin(), bfs_levels.begin() + n, 0);
            bfs_levels[source] = 1;

            qs[0].resize(1);
            qs[0][0] = source;

            auto parity = false;
            for (;;)
            {
                const auto& prev = qs[parity];
                assert(prev.size());

                auto& cur = qs[!parity];
                cur.clear();

                for (const auto& from : prev)
                {
                    if (from == sink)
                        return true;

                    auto& edges = graph[from];
                    for (auto& edge : edges)
                    {
                        if (edge.flow < edge.capacity && !bfs_levels[edge.to])
                        {
                            bfs_levels[edge.to] = bfs_levels[from] + 1;
                            cur.push_back(edge.to);
                        }
                    }
                }

                if (cur.empty())
                    return false;

                parity = !parity;
            }
        }

        weight_t get_flow(const int from, const weight_t flow)
        {
            assert(from >= 0 && from < n&& from != sink && flow > 0);

            auto& edges = graph[from];
            const auto size = static_cast<int>(edges.size());
            const auto& lev0 = bfs_levels[from];

            for (auto& i = last_visited[from]; i < size; ++i)
            {
                auto& edge = edges[i];
                const auto& to = edge.to;
                if (edge.capacity == edge.flow || bfs_levels[to] <= lev0)
                    continue;

                const auto left = std::min(edge.capacity - edge.flow, flow);
                assert(left > 0);

                const auto f = to == sink ? left : get_flow(to, left);
                if (f > 0)
                {
                    auto& edge2 = graph[to][edge.rev_edge_index];
                    assert(edge.flow + f <= edge.capacity && edge2.flow - f <= edge2.capacity && edge2.flow == -edge.flow);

                    edge.flow += f;
                    edge2.flow -= f;

                    return f;
                }
            }

            return {};
        }
    };
}