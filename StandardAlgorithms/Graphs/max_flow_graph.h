#pragma once
#include"../Utilities/require_utilities.h"
#include"flow_edge.h"
#include<array>
#include<numeric>
#include<vector>

namespace Standard::Algorithms::Graphs
{
    // todo(p4): Min path cover in a DAG by splitting a vertex into 2: in- and out-.
    // It can be used to run a minimum number of tests to verify a program.
    // A path cover is placing each vertex to exactly 1 path, including empty,
    // such that every 2 neighbors in a path are joined if such an edge belong to the graph.
    // Min means minimum # of paths.
    // Min path cover is NP-hard in general.

    // todo(p3): bipartite graph min vertex cover MVC from max matching MM:
    // Let V = X + Y.
    // Orient the vertices in MM as X -> Y, not in MM as Y -> X.
    // Find the vertices U = Y \ MM, can be empty.
    // Visit each u in U that has not been visited.
    // MVC = {visited in X} + {unvisited in Y}.
    // assert(MVC is cover and |MVC| == |MM| by Konig's).

    // todo(p3): bipartite graph min edge cover MVC from max matching MM:
    // MM + choose an edge for uncovered vertices.


    // Dinic's (Dinitz's) max flow algorithm.
    // Might require a large stack size, see dfs_flow().
    template<std::signed_integral weight_t, std::int32_t max_size, class edge_t = flow_edge<weight_t>>
    requires(2 <= max_size)
    struct max_flow_graph
    {
        constexpr max_flow_graph(const std::int32_t source, const std::int32_t sink, const std::int32_t size)
            : Source(source)
            , Sink(sink)
            , Size(size)
        {
            assert(!(source < 0) && source < size && !(sink < 0) && sink < size && source != sink);
            assert(2 <= size && size <= max_size);

            const auto capacity = size - 1;
            Ques[0].reserve(capacity);
            Ques[1].reserve(capacity);
        }

        constexpr void source(std::int32_t source)
        {
            assert(0 < Size);
            require_between(0, source, Size - 1, "source");

            Source = source;
        }

        constexpr void sink(std::int32_t sink)
        {
            assert(0 < Size);
            require_between(0, sink, Size - 1, "sink");

            Sink = sink;
        }

        [[nodiscard]] constexpr auto size() const noexcept
        {
            return Size;
        }

        [[nodiscard]] constexpr auto graph() const &noexcept -> const std::array<std::vector<edge_t>, max_size> &
        {
            return Graph;
        }

        [[nodiscard]] constexpr auto graph() &noexcept -> std::array<std::vector<edge_t>, max_size> &
        {
            return Graph;
        }

        constexpr void clear()
        {
            for (std::int32_t index{}; index < Size; ++index)
            {
                Graph.at(index).clear();
            }
        }

        constexpr void zero_flow()
        {
            constexpr weight_t zerow{};

            for (std::int32_t from{}; from < Size; ++from)
            {
                auto &edges = Graph.at(from);

                for (auto &edge : edges)
                {
                    edge.flow = zerow;
                }
            }
        }

        constexpr void add_edge(const std::int32_t from, const std::int32_t tod, const weight_t &capacity)
        {
            assert(!(from < 0) && from < Size && !(tod < 0) && tod < Size && from != tod && weight_t{} < capacity);

            auto &edges_to = Graph[tod];
            const auto rev = static_cast<std::int32_t>(edges_to.size());

            auto &edges_from = Graph[from];
            const auto rev2 = static_cast<std::int32_t>(edges_from.size());

            edges_from.push_back({ tod, rev, capacity });
            edges_to.push_back({ from, rev2, weight_t{} });
        }

        [[nodiscard]] constexpr auto max_flow() -> weight_t
        {
            constexpr weight_t inf = std::numeric_limits<weight_t>::max() / 2;

            weight_t total_flow{};

            while (bfs_find_flow())
            {
                std::fill(Last_visited.begin(), Last_visited.begin() + Size, 0);

                for (;;)
                {
                    const auto flow = dfs_flow(Source, inf);

                    if (!(weight_t{} < flow))
                    {
                        break;
                    }

                    total_flow += flow;
                    assert(weight_t{} < total_flow);

                    if constexpr (::Standard::Algorithms::is_debug)
                    {
                        const auto lam = [] [[nodiscard]] (const weight_t tot, const edge_t &edge)
                        {
                            return static_cast<weight_t>(tot + edge.flow);
                        };

                        {
                            const auto &edges = Graph[Source];
                            const auto source_flow = std::accumulate(edges.cbegin(), edges.cend(), weight_t{}, lam);
                            assert(source_flow == total_flow);
                        }
                        {
                            const auto &edges2 = Graph[Sink];
                            const auto sink_flow = std::accumulate(edges2.cbegin(), edges2.cend(), weight_t{}, lam);
                            assert(-sink_flow == total_flow);
                        }
                    }
                }
            }

            return total_flow;
        }

private:
        [[nodiscard]] constexpr auto bfs_find_flow() -> bool
        {
            std::fill(Bfs_levels.begin(), Bfs_levels.begin() + Size, 0);
            std::int32_t next_level = 1;
            Bfs_levels[Source] = next_level;

            Ques[0].assign(1, Source);

            for (std::uint32_t parity{};;)
            {
                const auto &prev = Ques.at(parity);
                assert(!prev.empty());

                auto &cur = Ques.at(parity ^ 1U);
                cur.clear();
                ++next_level;

                for (const auto &from : prev)
                {
                    assert(next_level == Bfs_levels[from] + 1);

                    auto &edges = Graph[from];

                    for (auto &edge : edges)
                    {
                        if (!(edge.flow < edge.capacity))
                        {
                            continue;
                        }

                        if (auto &lev = Bfs_levels[edge.to]; lev == 0)
                        {
                            lev = next_level;
                            cur.push_back(edge.to);
                        }
                    }
                }

                if (cur.empty())
                {
                    auto has = 0 < Bfs_levels[Sink];
                    return has;
                }

                parity ^= 1U;
            }
        }

        [[nodiscard]] constexpr auto dfs_flow(const std::int32_t from, const weight_t &flow) -> weight_t
        {
            assert(!(from < 0) && from < Size && from != Sink && weight_t{} < flow);

            auto &edges = Graph[from];
            const auto size1 = static_cast<std::int32_t>(edges.size());
            const auto &lev0 = Bfs_levels[from];

            for (auto &index = Last_visited[from]; index < size1; ++index)
            {
                auto &edge = edges[index];
                const auto &tod = edge.to;

                if (!(edge.flow < edge.capacity) || !(lev0 < Bfs_levels[tod]))
                {
                    continue;
                }

                const auto left_flow = std::min(static_cast<weight_t>(edge.capacity - edge.flow), flow);

                auto subflow = tod == Sink ? left_flow : dfs_flow(tod, left_flow);
                if (!(weight_t{} < subflow))
                {
                    continue;
                }

                auto &edge2 = Graph[tod][edge.rev_edge_index];

                assert(edge.flow + subflow <= edge.capacity && edge2.flow - subflow <= edge2.capacity &&
                    edge2.flow == -edge.flow);

                edge.flow += subflow;
                edge2.flow -= subflow;

                return subflow;
            }

            return {};
        }

        std::array<std::vector<edge_t>, max_size> Graph{};
        std::array<std::int32_t, max_size> Bfs_levels{};
        std::array<std::int32_t, max_size> Last_visited{};
        std::array<std::vector<std::int32_t>, 2> Ques{};
        std::int32_t Source;
        std::int32_t Sink;
        std::int32_t Size;
    };
} // namespace Standard::Algorithms::Graphs
