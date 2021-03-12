#include"minimal_spanning_tree_tests.h"
#include"../Utilities/random.h"
#include"../Utilities/test_utilities.h"
#include"boruvka_mst.h"
#include"jarnik_minimal_spanning_tree.h"
#include"minimal_spanning_tree.h"
#include<array>
#include<functional>

namespace
{
    using vertex_t = std::int32_t;
    using weight_t = std::int32_t;
    using long_weight_t = std::int64_t;

    using edge_t = Standard::Algorithms::weighted_vertex<vertex_t, weight_t>;
    using graph_weighted_vertex_t = std::vector<std::vector<edge_t>>;

    using edge_t2 = std::pair<weight_t, vertex_t>;
    using pque_t2 = std::priority_queue<edge_t2, std::vector<edge_t2>, std::greater<>>;

    // todo(p3): del it, and use AddAdjacentEdges only when que.emplace(..) is impl.
    constexpr void add_adjacent_edges_to_del(const graph_weighted_vertex_t &graf, const vertex_t from,
        std::vector<vertex_t> &parent_vertices, const std::vector<bool> &visited_vertices,
        std::vector<weight_t> &weights, pque_t2 &que)
    {
        const auto &adjacency_list = graf[from];

        for (const auto &edge : adjacency_list)
        {
            const auto &tod = edge.vertex;
            if (visited_vertices[tod])
            {
                continue;
            }

            const auto &weight_old = weights[tod];
            const auto &weight_new = edge.weight;
            if (!(weight_new < weight_old))
            {
                continue;
            }

            que.emplace(weight_new, tod);

            parent_vertices[tod] = from;
            weights[tod] = weight_new;
        }
    }

    [[nodiscard]] auto prim(const graph_weighted_vertex_t &graf, std::vector<vertex_t> &parent_vertices)
        -> long_weight_t
    {
        const auto size = Standard::Algorithms::require_positive(graf.size(), "graph size");
        long_weight_t total_weight{};

        std::vector<bool> visited_vertices(size);
        pque_t2 que;
        que.emplace(weight_t{}, vertex_t{});

        std::vector<weight_t> weights(size, std::numeric_limits<weight_t>::max());
        weights[0] = 0;

        parent_vertices.assign(size, -1);

        do
        {
            const auto top = que.top();
            que.pop();

            const auto &from = top.second;
            if (visited_vertices[from])
            {
                continue;
            }

            visited_vertices[from] = true;

            const auto &weight = top.first;
            total_weight += weight;

            add_adjacent_edges_to_del(graf, from, parent_vertices, visited_vertices, weights, que);
        } while (!que.empty());

        return total_weight;
    }

    template<weight_t size>
    requires(1 < size)
    constexpr void perform_test_once(graph_weighted_vertex_t &initial_graph)
    {
        initial_graph = graph_weighted_vertex_t(size, std::vector<edge_t>{});

        Standard::Algorithms::Graphs::graph<vertex_t, weight_t> initial_graph_2(size);

        Standard::Algorithms::Utilities::random_t<std::int32_t> rnd(0, size - 1);

        for (vertex_t index{}; index < size - 1; ++index)
        {
            for (auto ind2 = index + 1; ind2 < size; ++ind2)
            {
                const auto weight = static_cast<weight_t>(rnd());

                initial_graph[index].emplace_back(ind2, weight);
                initial_graph[ind2].emplace_back(index, weight);

                initial_graph_2.add_edge(index, ind2, weight);
            }
        }

        // initial_graph_2._AdjacencyLists = initial_graph;

        // graf[0].emplace_back(1, 10);
        // graf[0].emplace_back(2, 5);

        // graf[1].emplace_back(0, 10);
        // graf[1].emplace_back(2, 10);

        // graf[2].emplace_back(1, 10);
        // graf[2].emplace_back(0, 5);

        std::vector<vertex_t> parent_vertices0;
        const auto res0 = prim(initial_graph, parent_vertices0);

        {
            std::vector<vertex_t> parent_vertices2;
            long_weight_t res2{};

            Standard::Algorithms::Graphs::minimal_spanning_tree<vertex_t, weight_t, long_weight_t>::prim_by_edges(
                initial_graph_2, parent_vertices2, res2, false);

            ::Standard::Algorithms::ert::are_equal(res0, res2, "prim by edges");
        }
        {
            std::vector<vertex_t> parent_vertices3;
            long_weight_t res3{};

            constexpr auto shall_check_connectivity = false;

            Standard::Algorithms::Graphs::minimal_spanning_tree<vertex_t, weight_t, long_weight_t>::prim(
                initial_graph_2, parent_vertices3, res3, shall_check_connectivity);

            ::Standard::Algorithms::ert::are_equal(res0, res3, "prim");
        }
    }
} // namespace

void Standard::Algorithms::Graphs::Tests::minimal_spanning_tree_tests()
{
    constexpr auto vertex_count = 5;
    Standard::Algorithms::Graphs::graph<vertex_t, weight_t> graf(vertex_count);

    // NOLINTNEXTLINE
    graf.add_edge(2, 4, 10);

    // NOLINTNEXTLINE
    graf.add_edge(0, 1, 5);
    // NOLINTNEXTLINE
    graf.add_edge(0, 2, 9);
    // NOLINTNEXTLINE
    graf.add_edge(0, 3, 10);
    // NOLINTNEXTLINE
    graf.add_edge(1, 2, 7);

    // NOLINTNEXTLINE
    graf.add_edge(3, 2, 4);
    // NOLINTNEXTLINE
    graf.add_edge(3, 4, 9);

    const std::vector<long_weight_t> expected_weights{// NOLINTNEXTLINE
        0, 5, 12, 16, 25
    };

    std::vector<vertex_t> prim_previous_vertices;
    long_weight_t total_weight{};

    Standard::Algorithms::Graphs::minimal_spanning_tree<vertex_t, weight_t, long_weight_t>::prim_by_edges(
        graf, prim_previous_vertices, total_weight);

    ::Standard::Algorithms::ert::are_equal(
        static_cast<std::size_t>(vertex_count), prim_previous_vertices.size(), "primPreviousVertices Size");

    constexpr vertex_t source_node{};

    for (auto index = source_node; index < vertex_count; ++index)
    {
        const auto expected = index - 1;
        const auto &actual_previous_vertex = prim_previous_vertices[index];
        ::Standard::Algorithms::ert::are_equal(
            expected, actual_previous_vertex, "Error in prim by edges algorithm previous vertex");
    }

    const auto graph = graf.to_plain_graph();
    const std::vector<bool> expected_visited(graf.adjacency_lists().size(), true);

    // temps
    std::vector<vertex_t> parents;
    std::vector<bool> visited;

    std::vector<long_weight_t> weights;
    std::priority_queue<std::pair<long_weight_t, vertex_t>> que;

    const auto min_w =
        Standard::Algorithms::Graphs::jarnik_minimal_spanning_tree<>(graph, source_node, parents, visited, weights, que);

    ::Standard::Algorithms::ert::are_equal(min_w, total_weight, "min_w");
    ::Standard::Algorithms::ert::are_equal(expected_visited, visited, "visited");
    ::Standard::Algorithms::ert::are_equal(parents, prim_previous_vertices, "parents");
    ::Standard::Algorithms::ert::are_equal(expected_weights, weights, "weights");
}

void Standard::Algorithms::Graphs::Tests::mst_performance_tests()
{
    // NOLINTNEXTLINE
    constexpr auto max_attempts = ::Standard::Algorithms::is_debug ? 1 : 3;

    // NOLINTNEXTLINE
    constexpr auto size = ::Standard::Algorithms::is_debug ? 5 : 16;

    graph_weighted_vertex_t graph(size);

    for (std::int32_t attempt{}; attempt < max_attempts; ++attempt)
    {
        try
        {
            perform_test_once<size>(graph);
        }
        catch (const std::exception &exc)
        {
            auto str = ::Standard::Algorithms::Utilities::w_stream();
            str << "Performance attempt " << attempt << ", error: " << exc.what() << "\n";
            ::Standard::Algorithms::print("Graph", graph, str);

            throw std::runtime_error(str.str());
        }
    }
}
