#include"dijkstra_tests.h"
#include"../Utilities/ert.h"
#include"bellman_ford_shortest_paths.h"
#include"dijkstra.h"
#include"shortest_path_linear_programming.h"
#include<array>

namespace
{
    using weight_t = std::int64_t;
    using simplex_result = Standard::Algorithms::Geometry::simplex_result;

    constexpr auto absent_parent = Standard::Algorithms::Graphs::absent_parent;
    constexpr auto vertex_count = 5;
    constexpr auto source_vertex = 2;
    constexpr weight_t infinite_weight{ 1'000 };

    static_assert(static_cast<weight_t>(vertex_count) < infinite_weight);

    // It is assumed that there is no negative weight cycle.
    constexpr void verify_good_gold_dj_iskra_result(const auto &graph, const std::size_t source_vertex,
        const auto &expected_min_weights, const auto &expected_parents)
    {
        const auto size = graph.size();
        Standard::Algorithms::require_greater(size, source_vertex, "source vertex");

        ::Standard::Algorithms::ert::are_equal(size, expected_min_weights.size(), "expected min weights size");

        ::Standard::Algorithms::ert::are_equal(size, expected_parents.size(), "parents size");

        ::Standard::Algorithms::ert::are_equal(
            weight_t{}, expected_min_weights.at(source_vertex), "source vertex distance");

        ::Standard::Algorithms::ert::are_equal(absent_parent, expected_parents.at(source_vertex), "source vertex parent");

        for (std::size_t tod{}; tod < size; ++tod)
        {
            if (tod == source_vertex)
            {
                continue;
            }

            auto name = "to node " + std::to_string(tod);
            const auto &parent = expected_parents.at(tod);
            const auto &weight = expected_min_weights.at(tod);

            if (absent_parent == parent)
            {
                ::Standard::Algorithms::ert::are_equal(infinite_weight - 1, weight, "no parent weight " + name);
                continue;
            }

            name += " from parent " + std::to_string(parent);
            Standard::Algorithms::require_less_equal(0, parent, name);
            Standard::Algorithms::require_greater(size, static_cast<std::size_t>(parent), name);

            const auto &par_weight = expected_min_weights.at(parent);
            const auto &edges = graph.at(parent);
            const auto iter = std::find_if(edges.cbegin(), edges.cend(),
                [tod](const auto &edge)
                {
                    return static_cast<std::size_t>(edge.vertex) == tod;
                });

            if (iter == edges.cend()) [[unlikely]]
            {
                auto err = "Not found an edge " + name;
                throw std::runtime_error(err);
            }

            name += " weight";
            Standard::Algorithms::require_greater(infinite_weight, par_weight, name);
            Standard::Algorithms::require_less_equal(weight_t{}, par_weight, name);
            {
                const auto actual = par_weight + iter->weight;
                ::Standard::Algorithms::ert::are_equal(weight, actual, name);
            }
        }

        if (Standard::Algorithms::Graphs::has_negative_cost_cycle(graph, expected_min_weights))
        {
            throw std::runtime_error("The graph has a negative cost cycle.");
        }
    }

    [[nodiscard]] constexpr auto build_graph() -> Standard::Algorithms::Graphs::graph<std::int32_t, weight_t>
    {
        // Each edge weight should not exceed infiniteWeight/(vertexCount - 1) - 1 = 249.
        Standard::Algorithms::Graphs::graph<std::int32_t, weight_t> graph(vertex_count);

        // NOLINTNEXTLINE
        graph.add_edge(0, 1, 5); // NOLINTNEXTLINE
        graph.add_edge(0, 2, 90); // NOLINTNEXTLINE
        graph.add_edge(0, 3, 6);

        // NOLINTNEXTLINE
        graph.add_edge(1, 2, 6); // NOLINTNEXTLINE
        graph.add_edge(2, 4, 10);

        // NOLINTNEXTLINE
        graph.add_edge(3, 2, 4); // NOLINTNEXTLINE
        graph.add_edge(3, 4, 5);
        // 0 -> 1(5) -> 2(6) -> 4(10)
        // 0 -> 2(90) -> 3(4) -> 4(5)
        // 0 -> 3(6)
        return graph;
    }

    constexpr void run_subtest(const auto &graph, const auto &expected_min_weights, const auto &expected_parents)
    {
        std::vector<weight_t> distances(vertex_count + 1);
        std::vector<std::int32_t> parents(vertex_count + 1);
        std::vector<std::int32_t> path(parents.size());
        std::vector<bool> used;

        Standard::Algorithms::Graphs::dijkstra_prior_que<std::int32_t, weight_t>::run(
            graph, absent_parent, infinite_weight, source_vertex, distances, parents);

        ::Standard::Algorithms::ert::are_equal(
            expected_parents.data(), vertex_count, parents, "parents in dijkstra_prior_que.");

        ::Standard::Algorithms::ert::are_equal(
            expected_min_weights.data(), vertex_count, distances, "distances in dijkstra_prior_que.");

        {
            std::set<std::pair<weight_t, std::int32_t>> dist_vertex_temp;

            Standard::Algorithms::Graphs::dijkstra_sparse(
                graph.adjacency_lists(), source_vertex, distances, parents, dist_vertex_temp, infinite_weight);

            ::Standard::Algorithms::ert::are_equal(
                expected_min_weights.data(), vertex_count, distances, "distances in dijkstra_sparse.");
            ::Standard::Algorithms::ert::are_equal(
                expected_parents.data(), vertex_count, parents, "parents in dijkstra_sparse.");
        }
        {
            Standard::Algorithms::Graphs::dijkstra_dense(
                graph.adjacency_lists(), used, source_vertex, distances, parents, infinite_weight);

            ::Standard::Algorithms::ert::are_equal(
                expected_min_weights.data(), vertex_count, distances, "distances in dijkstra_dense.");
            ::Standard::Algorithms::ert::are_equal(
                expected_parents.data(), vertex_count, parents, "parents in dijkstra_dense.");
        }
        {
            const auto sim_res = Standard::Algorithms::Graphs::single_source_shortest_path_linear_programming<weight_t>(
                source_vertex, graph.adjacency_lists(), distances);

            ::Standard::Algorithms::ert::are_equal(
                simplex_result::one_solution, sim_res, "single_source_shortest_path_linear_programming sim_res");

            ::Standard::Algorithms::ert::are_equal(expected_min_weights.data(), vertex_count, distances,
                "single_source_shortest_path_linear_programming distances");
        }
        {
            for (std::int32_t destination{}; destination < vertex_count; ++destination)
            {
                const auto actual = Standard::Algorithms::Graphs::pair_shortest_path_linear_programming<weight_t>(
                    source_vertex, graph.adjacency_lists(), destination, infinite_weight);

                const auto &weight = expected_min_weights.at(destination);
                const auto expected = std::make_pair(weight, simplex_result::one_solution);

                ::Standard::Algorithms::ert::are_equal(
                    expected, actual, "pair_shortest_path_linear_programming dest " + std::to_string(destination));
            }
        }
        {
            std::deque<std::int32_t> temp;
            std::deque<std::int32_t> temp2;

            const auto actual = Standard::Algorithms::Graphs::bellman_ford_shortest_paths_fast(
                graph.adjacency_lists(), source_vertex, distances, parents, temp, temp2, used, infinite_weight);

            ::Standard::Algorithms::ert::are_equal(true, actual, "bellman_ford_shortest_paths_fast.");

            ::Standard::Algorithms::ert::are_equal(
                expected_min_weights.data(), vertex_count, distances, "distances in bellman_ford_shortest_paths_fast.");
            ::Standard::Algorithms::ert::are_equal(
                expected_parents.data(), vertex_count, parents, "parents in bellman_ford_shortest_paths_fast.");
        }
        {
            const auto actual = Standard::Algorithms::Graphs::bellman_ford_shortest_paths_slow(
                graph.adjacency_lists(), source_vertex, distances, parents, infinite_weight);

            ::Standard::Algorithms::ert::are_equal(true, actual, "bellman_ford_shortest_paths_slow.");

            ::Standard::Algorithms::ert::are_equal(
                expected_min_weights.data(), vertex_count, distances, "distances in bellman_ford_shortest_paths_slow.");
            ::Standard::Algorithms::ert::are_equal(
                expected_parents.data(), vertex_count, parents, "parents in bellman_ford_shortest_paths_slow.");
        }
        {
            std::vector<std::vector<weight_t>> distances2d;
            std::vector<std::vector<std::int32_t>> nexts2d;

            {
                const auto actual = Standard::Algorithms::Graphs::floyd_warshall_shortest_paths(
                    graph.adjacency_lists(), distances2d, nexts2d, infinite_weight);

                ::Standard::Algorithms::ert::are_equal(true, actual, "floyd_warshall_shortest_paths.");
            }

            if constexpr (Standard::Algorithms::has_openmp())
            {
                std::vector<std::vector<weight_t>> parallel_distances2d;
                std::vector<std::vector<std::int32_t>> parallel_nexts2d;
                {
                    const auto actual = Standard::Algorithms::Graphs::parallel_floyd_warshall_shortest_paths(
                        graph.adjacency_lists(), parallel_distances2d, parallel_nexts2d, infinite_weight);

                    ::Standard::Algorithms::ert::are_equal(true, actual, "parallel_floyd_warshall_shortest_paths.");
                }

                ::Standard::Algorithms::ert::are_equal(
                    distances2d, parallel_distances2d, "parallel_floyd_warshall_shortest_paths distances.");

                ::Standard::Algorithms::ert::are_equal( // These might differ, but not this time.
                    nexts2d, parallel_nexts2d, "parallel_floyd_warshall_shortest_paths nexts.");
            }

            const auto &dis = distances2d[source_vertex];
            ::Standard::Algorithms::ert::are_equal(
                expected_min_weights.data(), vertex_count, dis, "distances in floyd_warshall_shortest_paths.");

            parents.resize(vertex_count);

            for (std::int32_t tod{}; tod < vertex_count; ++tod)
            {
                Standard::Algorithms::Graphs::restore_floyd_warshall_shortest_paths(nexts2d, source_vertex, tod, path);

                // todo(p3): Could be done faster.
                parents[tod] = 1U < path.size() ? path[path.size() - 2U] : absent_parent;
            }

            ::Standard::Algorithms::ert::are_equal(
                expected_parents.data(), vertex_count, parents, "parents in floyd_warshall_shortest_paths.");
        }

        // todo(p2): use matrix multiplication, divide n conquer 'mmdctc' to compute the transitive closure of a graph:
        // Let n = 2**p for some integer p; and assume no negative cost paths(?).
        // Let's use minimum when adding, and sum when multiplying matrices.
        //
        // if n <= 2, return naive alg.
        // else split:
        // - the vertices v into 2 halves: h0, h1;
        // - and the edges into 4 subsets: e00, e01, e10, e11 where e01 are the edges from h0 to h1.
        // Call cand0 = mmdctc(e00 on h0);
        // Compute m11 = e11 + e10 * cand0 * e01; // either stay in h1, or go h1 -> (move within h0) -> h1.
        // Compute tc11 = mmdctc(m11 on h1);
        // Set tc00 = cand0 + cand0 * e01 * tc11 * e10 * cand0; // stay in h0, or h0 -> (move in whole v) -> h0.
        // Set tc01 = cand0 * e01 * tc11; // start in h0 -> (move in whole v) -> h1.
        // Set tc10 = tc11 * e10 * cand0; // similar.
        // return { tc00, tc01, tc10, tc11 }
        //
        // {
        // }


        constexpr std::int32_t target_vertex{};

        Standard::Algorithms::Graphs::dijkstra_prior_que<std::int32_t, weight_t>::find_path(
            absent_parent, parents, target_vertex, path);

        const std::array expected_path = { source_vertex, 3, target_vertex };
        ::Standard::Algorithms::ert::are_equal(
            expected_path.data(), expected_path.size(), path, "path in dijkstra_prior_que find_path.");
    }
} // namespace

void Standard::Algorithms::Graphs::Tests::dijkstra_tests()
{
    const auto graph = build_graph();

    constexpr std::array<weight_t, vertex_count> expected_min_weights{// NOLINTNEXTLINE
        10, 6, 0, 4, 9
    };

    constexpr std::array expected_parents{// NOLINTNEXTLINE
        3, 2, absent_parent, 2, 3
    };

    verify_good_gold_dj_iskra_result(graph.adjacency_lists(), source_vertex, expected_min_weights, expected_parents);

    run_subtest(graph, expected_min_weights, expected_parents);
}
