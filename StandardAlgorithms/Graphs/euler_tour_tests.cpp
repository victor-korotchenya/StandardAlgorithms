#include"euler_tour_tests.h"
#include"../Utilities/test_utilities.h"
#include"euler_tour.h"

namespace
{
    struct edge_t final
    {
        std::int32_t to{};
        std::int32_t edge_number{};
    };

    constexpr void add_edge(std::int32_t &edge_count, const std::int32_t from, std::vector<std::vector<edge_t>> &graph,
        const std::int32_t tod)
    {
        const auto size = static_cast<std::int32_t>(graph.size());
        assert(!(std::min(from, tod) < 0) && std::max(from, tod) < size);

        ++edge_count;
        graph[from].push_back({ tod, edge_count });
        graph[tod].push_back({ from, edge_count });
    }

    [[nodiscard]] constexpr auto build_graph() -> std::tuple<std::vector<std::vector<edge_t>>, std::int32_t>
    {
        const std::vector<std::vector<std::int32_t>> source_graph{
            { 1, 3, 4, 5 }, { 2 }, // 1
            { 3 }, {}, // 3
            {}, {}, // 5
        };

        const auto size = source_graph.size();
        std::vector<std::vector<edge_t>> graph(size);

        std::int32_t edge_count{};

        for (std::int32_t from{}; from < static_cast<std::int32_t>(size); ++from)
        {
            for (const auto &tods = source_graph[from]; const auto &tod : tods)
            {
                add_edge(edge_count, from, graph, tod);
            }
        }

        return { graph, edge_count };
    }
} // namespace

void Standard::Algorithms::Graphs::Tests::euler_tour_tests()
{
    const auto graph_count = build_graph();
    const auto &original_graph = std::get<0>(graph_count);
    {
        auto graf = original_graph;
        const auto actual_directed = Standard::Algorithms::Graphs::euler_tour_directed(graf);

        const std::vector<std::int32_t> expected_directed{// NOLINTNEXTLINE
            0, 5, 0, 4, 0, 3, 2, 3, 0, 1, 2, 1, 0
        };
        ::Standard::Algorithms::ert::are_equal(expected_directed, actual_directed, "euler_tour_directed");
    }
    {
        auto graf = original_graph;
        const auto actual = Standard::Algorithms::Graphs::euler_tour_undirected(graf);

        const std::vector<std::int32_t> expected{// NOLINTNEXTLINE
            5, 0, 3, 2, 1, 0, 4
        };
        ::Standard::Algorithms::ert::are_equal(expected, actual, "euler_tour_undirected");
    }
    {
        auto graf = original_graph;
        auto edge_count = std::get<1>(graph_count);

        // NOLINTNEXTLINE
        add_edge(edge_count, 4, graf, 5);

        const auto actual = Standard::Algorithms::Graphs::euler_tour_undirected(graf);

        const std::vector<std::int32_t> expected{// NOLINTNEXTLINE
            0, 5, 4, 0, 3, 2, 1, 0
        };
        ::Standard::Algorithms::ert::are_equal(expected, actual, "euler_tour_undirected 4_5");
    }
}
