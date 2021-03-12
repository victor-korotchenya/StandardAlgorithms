#pragma once
#include"../Utilities/has_openmp.h"
#include"../Utilities/require_utilities.h"
#include<deque>
#include<vector>

namespace Standard::Algorithms::Graphs
{
    // todo(p3): if dist[x][x] < 0, return 1;
    [[nodiscard]] constexpr auto has_negative_cost_cycle(const auto &graph, const auto &min_distances) -> bool
    {
        {
            using distance_t = std::remove_cvref_t<decltype(min_distances[0])>;
            static_assert(std::is_signed_v<distance_t>);
        }

        const auto size = static_cast<std::int32_t>(graph.size());
        require_positive(size, "graph size");

        for (std::int32_t from{}; from < size; ++from)
        {
            const auto &edges = graph.at(from);
            const auto &dist_from = min_distances.at(from);

            for (const auto &edge : edges)
            {
                const auto &sto = edge.vertex;
                assert(from != sto && !(sto < 0) && sto < size);

                const auto &weight = edge.weight;

                const auto cand = dist_from + weight;
                const auto &cur = min_distances.at(sto);
                if (cand < cur)
                {
                    return true;
                }
            }
        }

        return false;
    }

    namespace Inner
    {
        template<class graph_t, std::signed_integral weight_t>
        [[nodiscard]] constexpr auto bf_cycle_underflow(const graph_t &graph, std::vector<weight_t> &distances,
            std::vector<std::int32_t> &parents, const std::deque<std::int32_t> &prev, std::deque<std::int32_t> &next,
            std::vector<bool> &used, const weight_t inf) -> bool
        {
            const auto size = static_cast<std::int32_t>(graph.size());
            assert(0 < size && 0 < inf && -inf < 0);

            next.clear();
            used.assign(size, false);

            for (const auto &from : prev)
            {
                const auto &edges = graph[from];
                const auto &dist_from = distances[from];
                assert(-inf < dist_from && dist_from < inf);

                for (const auto &edge : edges)
                {
                    const auto &sto = edge.vertex;
                    const auto &weight = edge.weight;
                    const auto cand = dist_from + weight;

                    assert(from != sto && !(sto < 0) && sto < size && -inf < weight && weight < inf && cand / 2 < inf);

                    if (const auto is_underflow = cand <= -inf; is_underflow)
                    {
                        return true;
                    }

                    auto &curd = distances[sto];
                    if (curd <= cand)
                    {
                        continue;
                    }

                    curd = cand;
                    parents[sto] = from;
                    if (used[sto])
                    {
                        continue;
                    }

                    used[sto] = true;

                    if (next.empty() || distances[next.front()] <= curd)
                    {
                        next.push_back(sto);
                    }
                    else // Process shorter routes first.
                    {
                        next.push_front(sto);
                    }
                }
            }

            return false;
        }
    } // namespace Inner

    // Find 1 source shortest path tree for a negative cost graph.
    // Return false when a negative cycle exists - the computed distances and parents can be ignored.
    // Time O(v*e) is slower than Dijkstra's for non-negative costs.
    template<class graph_t, std::signed_integral weight_t>
    [[nodiscard]] constexpr auto bellman_ford_shortest_paths_fast(const graph_t &graph, std::int32_t source,
        std::vector<weight_t> &distances, std::vector<std::int32_t> &parents, std::deque<std::int32_t> &temp,
        std::deque<std::int32_t> &temp2, std::vector<bool> &used,
        const weight_t inf = std::numeric_limits<weight_t>::max() / 2) -> bool
    {
        const auto size = static_cast<std::int32_t>(graph.size());

        assert(!(source < 0) && source < size && 0 < size && 0 < inf && -inf < 0);

        distances.assign(size, inf);
        distances[source] = weight_t{};
        parents.assign(size, -1);

        temp.assign(1, source);

        std::uint32_t temp_ind{};

        for (std::int32_t att{}; ++att < size; temp_ind ^= 1U)
        {
            const auto accetto = temp_ind != 0U;
            const auto &prev = accetto ? temp2 : temp;
            auto &next = !accetto ? temp2 : temp;

            if (Inner::bf_cycle_underflow<graph_t, weight_t>(graph, distances, parents, prev, next, used, inf))
            {
                return false;
            }

            if (next.empty())
            {
                return true;
            }
        }

        const auto has_cycle = has_negative_cost_cycle(graph, distances);
        return !has_cycle;
    }

    // It is 1.5 times slower on n=5000.
    template<class graph_t, std::signed_integral weight_t>
    [[nodiscard]] constexpr auto bellman_ford_shortest_paths_slow(const graph_t &graph, std::int32_t source,
        std::vector<weight_t> &distances, std::vector<std::int32_t> &parents,
        const weight_t inf = std::numeric_limits<weight_t>::max() / 2) -> bool
    {
        const auto size = static_cast<std::int32_t>(graph.size());
        assert(!(source < 0) && source < size && 0 < size && 0 < inf && -inf < 0);

        distances.assign(size, inf);
        distances[source] = weight_t{};
        parents.assign(size, -1);

        for (std::int32_t att{}; ++att < size;)
        {
            auto changed = false;

            for (std::int32_t from{}; from < size; ++from)
            {
                const auto &edges = graph[from];
                const auto &dist_from = distances[from];
                if (dist_from == inf)
                {
                    continue;
                }

                for (const auto &edge : edges)
                {
                    const auto &sto = edge.vertex;
                    const auto &weight = edge.weight;
                    const auto cand = dist_from + weight;

                    assert(from != sto && !(sto < 0) && sto < size && -inf < weight && weight < inf && cand / 2 < inf);

                    if (const auto is_underflow = cand <= -inf; is_underflow)
                    {
                        return false;
                    }

                    auto &cur = distances[sto];
                    if (cur <= cand)
                    {
                        continue;
                    }

                    cur = cand;
                    parents[sto] = from;
                    changed = true;
                }
            }

            if (!changed)
            {
                return true;
            }
        }

        const auto has_cycle = has_negative_cost_cycle(graph, distances);
        return !has_cycle;
    }

    namespace Inner
    {
        [[nodiscard]] constexpr auto all_non_negative_distances(const auto &distances, const std::int32_t size) noexcept
            -> bool
        {
            for (std::int32_t ind{}; ind < size; ++ind)
            {
                if (distances[ind][ind] < 0)
                {
                    return false;
                }
            }

            return true;
        }
    } // namespace Inner

    // All vertex pairs shortest paths, weights can be negative.
    // Return false when a negative cycle exists - the computed distances and nexts can be ignored.
    // The  parents are 'nexts', not previous, to get rid of 'path.reverse'.
    // Time O(v**3), faster than repeated matrix squaring.
    template<class graph_t, std::signed_integral weight_t>
    [[nodiscard]] constexpr auto floyd_warshall_shortest_paths(const graph_t &graph,
        std::vector<std::vector<weight_t>> &distances, std::vector<std::vector<std::int32_t>> &nexts,
        const weight_t inf = std::numeric_limits<weight_t>::max() / 2) -> bool
    {
        assert(0 < inf && -inf < 0);

        // todo(p3): mov to a file.
        const auto size = require_positive(static_cast<std::int32_t>(graph.size()), "graph size");

        distances.assign(size, std::vector<weight_t>(size, inf));
        nexts.assign(size, std::vector<std::int32_t>(size, -1));

        for (std::int32_t from = 0; from < size; ++from)
        {
            const auto &edges = graph[from];
            auto &dist = distances[from];
            auto &next = nexts[from];

            for (const auto &edge : edges)
            {
                const auto &sto = edge.vertex;
                const auto &weight = edge.weight;

                assert(from != sto && !(sto < 0) && sto < size && -inf < weight && weight < inf);

                dist[sto] = weight;
                next[sto] = sto;
            }

            dist[from] = 0;
            next[from] = from;
        }

        for (std::int32_t mid{}; mid < size; ++mid)
        {
            const auto &mid_dis = distances[mid];

            for (std::int32_t from = 0; from < size; ++from)
            {
                auto &from_dis = distances[from];
                auto &next = nexts[from];

                for (std::int32_t sto{}; sto < size; ++sto)
                {
                    auto cand = from_dis[mid] + mid_dis[sto];

                    // Some paths might be good while others might include negative weight/cost cycles.
                    cand = std::max(cand, -inf); // Avoid underflow.

                    auto &cur = from_dis[sto];
                    if (cand < cur)
                    {
                        cur = cand;

                        // from -> mid -> sto
                        // Here 'from -> mid' might be not simple path.
                        next[sto] = next[mid];
                    }
                }
            }
        }

        auto good = Inner::all_non_negative_distances(distances, size);
        return good;
    }

    // OPENMP version.
    // todo(tormoz): is it faster?
    template<class graph_t, std::signed_integral weight_t, bool enabled = has_openmp()>
    requires(sizeof(weight_t) <= sizeof(std::size_t)) // Must fit in a register.
    [[nodiscard]] auto parallel_floyd_warshall_shortest_paths(const graph_t &graph,
        std::vector<std::vector<weight_t>> &distances, std::vector<std::vector<std::int32_t>> &nexts,
        const weight_t inf = std::numeric_limits<weight_t>::max() / 2) -> typename std::enable_if_t<enabled, bool>
    {
        assert(0 < inf && -inf < 0);

        const auto size = require_positive(static_cast<std::int32_t>(graph.size()), "graph size");

        distances.assign(size, std::vector<weight_t>(size, inf));
        nexts.assign(size, std::vector<std::int32_t>(size, -1));

#pragma omp parallel for default(none) shared(graph, size, distances, nexts, inf)
        for (std::int32_t from = 0; from < size; ++from)
        {
            const auto &edges = graph[from];
            auto &dist = distances[from];
            auto &next = nexts[from];

            for (const auto &edge : edges)
            {
                const auto &sto = edge.vertex;
                const auto &weight = edge.weight;

                assert(from != sto && !(sto < 0) && sto < size && -inf < weight && weight < inf);

                dist[sto] = weight;
                next[sto] = sto;
            }

            dist[from] = 0;
            next[from] = from;
        }

        for (std::int32_t mid{}; mid < size; ++mid)
        {
            const auto &mid_dis = distances[mid];

#pragma omp parallel for default(none) shared(size, distances, nexts, inf, mid, mid_dis)
            for (std::int32_t from = 0; from < size; ++from)
            {
                auto &from_dis = distances[from];
                auto &next = nexts[from];

                for (std::int32_t sto{}; sto < size; ++sto)
                {
                    auto cand = std::max<weight_t>(-inf, // Avoid underflow.
                        static_cast<weight_t>( // Let friendly (benign) data races be.
                            from_dis[mid] + mid_dis[sto]));

                    auto &cur = from_dis[sto];
                    if (cand < cur)
                    {// Use no lock provided the loads and stores are 1-op.
                        cur = cand;

                        // from -> mid -> sto
                        // Here 'from -> mid' might be not simple path.
                        next[sto] = next[mid];
                    }
                }
            }
        }

        auto good = Inner::all_non_negative_distances(distances, size);
        return good;
    }

    // The 'path' will be empty when no path exists.
    inline constexpr void restore_floyd_warshall_shortest_paths(const std::vector<std::vector<std::int32_t>> &nexts,
        std::int32_t from, const std::int32_t sto, std::vector<std::int32_t> &path)
    {
        assert(!(from < 0) && !(sto < 0));

        path.clear();

        if (const auto no_path = nexts[from][sto] < 0; no_path)
        {
            return;
        }

        for (;;)
        {
            path.push_back(from);
            if (sto == from)
            {
                return;
            }

            const auto &next = nexts[from][sto];
            assert(!(next < 0));

            from = next;
        }
    }

    // todo(p3): impl Johnson's all pairs shortest paths on a sparse graph in O(v*(e + v*log(v))).
    // Assume there are negative-weight edges, so re-weighting is needed. If not, skip a fake vertex and potentials.
    // - Add a fake vertex, connected to all other vertices by 0-cost edges.
    // - Run Bellman-Ford to compute the vertex potentials Pt. Exit when a negative-weight cycle is encountered.
    // - To get a non-negative number, to each a->b edge weight add: Pt(a) - Pt(b).
    // - Play DJ Iskra music for each vertex.
    // - To each a->b final min distance, if not infinity, add: Pt(b) - Pt(a).


    // Transitive closure TC can be computed by replacing each edge with a boolean in Floyd+Warshall.
    // Also, to get TC, run BFS on the boolean graph from each vertex in parallel.
    // Alternatively, divide the boolean graph into strong components, run reachability on DAG, get TC in time O(v*v).
    // todo(p2): Measure how much Cuda TC is slower on 1E6 cores?
} // namespace Standard::Algorithms::Graphs
