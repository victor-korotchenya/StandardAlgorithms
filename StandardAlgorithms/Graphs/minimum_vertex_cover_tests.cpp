#include"minimum_vertex_cover_tests.h"
#include"../Utilities/iota_vector.h"
#include"../Utilities/test_utilities.h"
#include"graph_builder.h"
#include"graph_helper.h"
#include"minimum_vertex_cover.h"

namespace
{
    using int_t = std::int32_t;
    using nodes_t = std::vector<int_t>;
    using graph_t = std::vector<nodes_t>;

    using floating_t = Standard::Algorithms::floating_point_type;
    using approx_func_t = void (*)(const graph_t &, nodes_t &);

    // Duplicates are not allowed in the cover.
    [[nodiscard]] constexpr auto is_vertex_cover(
        const graph_t &graph, const nodes_t &tentative_cover, std::vector<bool> &buffers) -> bool
    {
        const auto graph_size = graph.size();
        if (graph_size < tentative_cover.size())
        {
            return false;
        }

        buffers.assign(graph_size, false);

        for (const auto &node : tentative_cover)
        {
            if (!Standard::Algorithms::Graphs::is_valid_node(node, graph_size) || buffers[node])
            {
                return false;
            }

            buffers[node] = true;
        }

        for (int_t from = int_t{} - int_t{ 1 }; const auto &edges : graph)
        {
            ++from;

            if (!Standard::Algorithms::Graphs::is_valid_node(from, graph_size))
            {
                return false;
            }

            if (buffers[from])
            {
                continue;
            }

            if (const auto is_bad = std::any_of(edges.begin(), edges.end(),
                    [&buffers, graph_size](const auto &tod)
                    {
                        auto good1 = Standard::Algorithms::Graphs::is_valid_node(tod, graph_size) && buffers[tod];
                        return !good1;
                    });
                is_bad) [[unlikely]]
            {
                return false;
            }
        }

        return true;
    }

    [[nodiscard]] constexpr auto is_vertex_cover_slow(const graph_t &graph, const nodes_t &tentative_cover) -> bool
    {
        std::vector<bool> buffers;
        return is_vertex_cover(graph, tentative_cover, buffers);
    }

    struct test_case final : Standard::Algorithms::Tests::base_test_case
    {
        constexpr test_case(
            std::string &&name, graph_t &&graph, nodes_t &&expected, const std::int32_t edge_count) noexcept
            : base_test_case(std::move(name))
            , Graph(std::move(graph))
            , Expected(std::move(expected))
            , Edge_count(edge_count)
        {
        }

        [[nodiscard]] constexpr auto graph() const &noexcept -> const graph_t &
        {
            return Graph;
        }

        [[nodiscard]] constexpr auto expected() const &noexcept -> const nodes_t &
        {
            return Expected;
        }

        [[nodiscard]] constexpr auto edge_count() const noexcept -> std::int32_t
        {
            return Edge_count;
        }

        constexpr void validate() const override
        {
            base_test_case::validate();
            Standard::Algorithms::Graphs::require_simple_graph(Graph);
            Standard::Algorithms::Graphs::require_undirected_graph(Graph);

            {
                constexpr auto is_directed = false;

                const auto actual_edges = Standard::Algorithms::Graphs::count_edges(Graph, is_directed);
                ::Standard::Algorithms::ert::are_equal(static_cast<std::size_t>(Edge_count), actual_edges, "edges count");
            }
            {
                const auto graph_size = Graph.size();
                const auto expected_size = Expected.size();

                Standard::Algorithms::require_less_equal(expected_size, graph_size, "expected vs graph sizes");
            }
            {
                const auto isvc = is_vertex_cover_slow(Graph, Expected);
                ::Standard::Algorithms::ert::are_equal(true, isvc, "is vertex cover");
            }
        }

        void print(std::ostream &str) const override
        {
            ::Standard::Algorithms::print(", graph ", Graph, str);
            ::Standard::Algorithms::print(", expected ", Expected, str);
            str << " edge count " << Edge_count;
        }

private:
        graph_t Graph;
        nodes_t Expected;
        std::int32_t Edge_count;
    };

    constexpr void generate_test_cases(std::vector<test_case> &tests)
    {
        {
            constexpr auto edge_count = 1;
            tests.emplace_back("P2", graph_t{ { 1 }, { 0 } }, nodes_t{ 0 }, edge_count);
        }
        {
            constexpr auto edge_count = 2;
            // 1 - 2
            //     |
            //     3
            tests.emplace_back("1-2-3 path", graph_t{ {}, { 2 }, { 1, 3 }, { 2 } }, nodes_t{ 2 }, edge_count);
        }
        {
            constexpr auto edge_count = 5;
            // 0 - 1
            // | \ |
            // 2 - 3
            tests.emplace_back(
                "K4\\{1-2}", graph_t{ { 1, 2, 3 }, { 0, 3 }, { 0, 3 }, { 0, 1, 2 } }, nodes_t{ 0, 3 }, edge_count);
        }

        constexpr auto max_graph_size = 5;

        for (auto graph_size = 1; graph_size <= max_graph_size; ++graph_size)
        {
            const auto edge_count = graph_size * (graph_size - 1) / 2;

            auto graph = Standard::Algorithms::Graphs::build_complete_graph<int_t>(graph_size);
            auto expected = Standard::Algorithms::Utilities::iota_vector<int_t>(graph_size - 1);

            tests.emplace_back("Complete graph K" + ::Standard::Algorithms::Utilities::zu_string(graph_size),
                std::move(graph), std::move(expected), edge_count);
        }
    }

    constexpr void run_approx(const test_case &test, const std::string &name, nodes_t &actual, approx_func_t func)
    {
        assert(!name.empty());

        const auto &graph = test.graph();

        actual.assign(graph.size(), int_t{});
        func(graph, actual);

        {
            const auto approx_size = actual.size();
            ::Standard::Algorithms::ert::greater_or_equal(
                graph.size(), approx_size, name + " graph vs approximation sizes");

            const auto exact_mvc_size = test.expected().size();
            ::Standard::Algorithms::ert::greater_or_equal(approx_size, exact_mvc_size, name + " approximation size");

            ::Standard::Algorithms::ert::greater_or_equal(exact_mvc_size * Standard::Algorithms::Graphs::mvc_approx_ratio,
                approx_size, name + " approximation size quality");
        }
        {
            const auto isvc = is_vertex_cover_slow(graph, actual);
            ::Standard::Algorithms::ert::are_equal(true, isvc, name + " approximation is vertex cover");
        }
    }

    constexpr void run_test_case(const test_case &test)
    {
        const auto &graph = test.graph();

        nodes_t actual{};
        Standard::Algorithms::Graphs::minimum_vertex_cover<graph_t, std::uint32_t>(graph, test.edge_count(), actual);
        ::Standard::Algorithms::ert::are_equal(test.expected(), actual, "minimum exact vertex cover");

        actual.assign(graph.size(), int_t{});
        Standard::Algorithms::Graphs::minimum_vertex_cover_still<graph_t, std::uint32_t>(
            graph, test.edge_count(), actual);
        ::Standard::Algorithms::ert::are_equal(test.expected(), actual, "minimum exact vertex cover still");

        run_approx(test, "minimum_vertex_cover_2_approximation", actual,
            &Standard::Algorithms::Graphs::minimum_vertex_cover_2_approximation<graph_t, int_t>);

        run_approx(test, "minimum_vertex_cover_2_relax", actual,
            &Standard::Algorithms::Graphs::minimum_vertex_cover_2_relax<floating_t, graph_t, int_t>);
    }
} // namespace

void Standard::Algorithms::Graphs::Tests::minimum_vertex_cover_tests()
{
    // todo(p4): static_assert
    ::Standard::Algorithms::Tests::test_utilities<test_case>::run_tests(run_test_case, generate_test_cases);
}
