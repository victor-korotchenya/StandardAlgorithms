#include"bellman_ford_shortest_paths_tests.h"
#include"../Utilities/elapsed_time_ns.h"
#include"../Utilities/floating_point_type.h"
#include"../Utilities/test_utilities.h"
#include"bellman_ford_shortest_paths.h"
#include"create_random_graph.h"
#include"dijkstra_tests.h"
#include"weighted_vertex.h"
#include<iostream>

namespace
{
    using weight_t = std::int64_t;
    using int_t = std::int32_t;
    using edge_t = Standard::Algorithms::weighted_vertex<int_t, weight_t>;
    using graph_t = std::vector<std::vector<edge_t>>;
    using floating_point_type = Standard::Algorithms::floating_point_type;

    constexpr auto print_bellman_ford_test = false;
    constexpr floating_point_type million{ 1'000'000.0 };

    constexpr int_t max_nodes = 10;
    constexpr auto limit_edges = 10;

    constexpr weight_t min_weight = -5;
    constexpr auto max_weight = max_nodes * 2;
    constexpr auto inf = (max_weight + 10) * max_nodes;

    [[nodiscard]] auto run_bellman_ford_test(const graph_t &graph, std::vector<weight_t> &distances,
        std::vector<int_t> &parents, std::vector<weight_t> &distances2, std::vector<int_t> &parents2,
        std::deque<std::int32_t> &temp, std::vector<bool> &used, std::deque<std::int32_t> &temp2) -> bool
    {
        const Standard::Algorithms::elapsed_time_ns ti0;

        const auto fast = Standard::Algorithms::Graphs::bellman_ford_shortest_paths_fast<graph_t, weight_t>(
            graph, 0, distances, parents, temp, temp2, used, inf);

        [[maybe_unused]] const auto elapsed0 = ti0.elapsed();
        const Standard::Algorithms::elapsed_time_ns ti1;

        const auto actual1 = Standard::Algorithms::Graphs::bellman_ford_shortest_paths_slow<graph_t, weight_t>(
            graph, 0, distances2, parents2, inf);

        [[maybe_unused]] const auto elapsed1 = ti1.elapsed();

        ::Standard::Algorithms::ert::are_equal(fast, actual1, "bellman_ford_shortest_paths.");
        if (fast)
        {
            ::Standard::Algorithms::ert::are_equal(distances, distances2, "distances in bellman_ford_shortest_paths.");
            // parents might differ - skip comparing.
        }

        if constexpr (print_bellman_ford_test)
        {
            const auto ratio = ::Standard::Algorithms::ratio_compute(elapsed0, elapsed1);

            const auto rep = " n " + std::to_string(graph.size()) + ", has " + (fast ? "yes" : "no") + ", t0 " +
                std::to_string(static_cast<floating_point_type>(elapsed0) / million) + ",  t1 " +
                std::to_string(static_cast<floating_point_type>(elapsed1) / million) + ",  t1/t0 " +
                std::to_string(ratio) + "\n";

            std::cout << rep;
        }

        return fast;
    }

    template<class graph_t>
    constexpr auto run_bellman_ford_test_wrapper(const graph_t &graph, std::vector<weight_t> &distances,
        std::vector<int_t> &parents, std::vector<weight_t> &distances2, std::vector<int_t> &parents2,
        std::deque<std::int32_t> &temp, std::vector<bool> &used, std::deque<std::int32_t> &temp2) -> bool
    {
        try
        {
            const auto actual =
                run_bellman_ford_test(graph, distances, parents, distances2, parents2, temp, used, temp2);
            return actual;
        }
        catch (const std::exception &exc)
        {
            auto str = ::Standard::Algorithms::Utilities::w_stream();
            str << exc.what();
            ::Standard::Algorithms::print(" Graph ", graph, str);

            throw std::runtime_error(str.str());
        }
    }

    constexpr void bellman_ford_functional(std::vector<weight_t> &distances, std::vector<int_t> &parents,
        std::vector<weight_t> &distances2, std::vector<int_t> &parents2, std::deque<std::int32_t> &temp,
        std::vector<bool> &used, std::deque<std::int32_t> &temp2)
    {
        // todo(p3): more tests.
        const graph_t graph{
            { { 1, -1 }, { 2, -1 } },
            { { 0, -1 }, { 2, -1 } },
            { { 1, -1 }, { 0, -1 } },
        };

        const auto actual =
            run_bellman_ford_test_wrapper(graph, distances, parents, distances2, parents2, temp, used, temp2);
        ::Standard::Algorithms::ert::are_equal(false, actual, "all -1");
    }

    void bellman_ford_speed(std::vector<weight_t> &distances, std::vector<int_t> &parents,
        std::vector<weight_t> &distances2, std::vector<int_t> &parents2, std::deque<std::int32_t> &temp,
        std::vector<bool> &used, std::deque<std::int32_t> &temp2)
    {
        constexpr auto max_attempts = 1;

        for (std::int32_t att{}; att < max_attempts; ++att)
        {
            [[maybe_unused]] auto rep = "  gen graph attempt " + std::to_string(att) + "\n";

            if constexpr (print_bellman_ford_test)
            {
                std::cout << rep;
            }

            const Standard::Algorithms::elapsed_time_ns tim;

            const auto graph_edges = Standard::Algorithms::Graphs::create_random_graph_weighted<edge_t, weight_t, int_t,
                max_nodes, false, 1, limit_edges>(min_weight, max_weight);

            if constexpr (print_bellman_ford_test)
            {
                const auto elapsed = tim.elapsed();
                rep.pop_back();
                rep += " v " + std::to_string(graph_edges.first.size()) + " e " + std::to_string(graph_edges.second) +
                    " done in " + std::to_string(static_cast<floating_point_type>(elapsed) / million) + " ms.\n";
                std::cout << rep;
            }

            run_bellman_ford_test_wrapper(
                graph_edges.first, distances, parents, distances2, parents2, temp, used, temp2);
        }
    }
} // namespace

void Standard::Algorithms::Graphs::Tests::bellman_ford_shortest_paths_tests()
{
    std::vector<weight_t> distances;
    std::vector<weight_t> distances2;
    std::vector<int_t> parents;
    std::vector<int_t> parents2;

    std::deque<std::int32_t> temp;
    std::deque<std::int32_t> temp2;
    std::vector<bool> used;

    bellman_ford_functional(distances, parents, distances2, parents2, temp, used, temp2);

    bellman_ford_speed(distances, parents, distances2, parents2, temp, used, temp2);
}
