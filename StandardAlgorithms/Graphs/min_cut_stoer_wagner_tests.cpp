#include"min_cut_stoer_wagner_tests.h"
#include"../Utilities/require_matrix.h"
#include"../Utilities/simple_test_case.h"
#include"../Utilities/test_utilities.h"
#include"create_random_graph.h"
#include"max_flow_graph.h"
#include"min_cut_stoer_wagner.h"
#include<optional>

namespace
{
    using int_t = std::int32_t;
    using edges_t = std::vector<int_t>;
    using full_cost_graph_t = std::vector<edges_t>;

    constexpr int_t min_nodes = 2;
    constexpr int_t max_nodes = ::Standard::Algorithms::is_debug ? 10 : 20;

    static_assert(1 < min_nodes && min_nodes <= max_nodes);

    using max_flow_graph_t = Standard::Algorithms::Graphs::max_flow_graph<int_t, max_nodes>;

    using expected_vertices_t = std::vector<std::size_t>;
    using int_t = int_t;
    using expected_t = std::pair<expected_vertices_t, int_t>;

    struct unique_tag final
    {
    };

    using test_case =
        Standard::Algorithms::Tests::simple_test_case<unique_tag, full_cost_graph_t, std::optional<expected_t>>;

    [[nodiscard]] constexpr auto to_flags(const std::size_t size, const auto &vertices) -> std::vector<bool>
    {
        assert(0ZU < size && 0ZU < vertices.size());

        std::vector<bool> flags(size);

        for (const auto &from : vertices)
        {
            assert(from < size);
            flags.at(from) = true;
        }

        return flags;
    }

    [[nodiscard]] constexpr auto compute_cut_cost(
        const full_cost_graph_t &graph, const expected_vertices_t &expected_vertices) -> int_t
    {
        const auto size = graph.size();
        assert(!graph.empty() && !expected_vertices.empty());

        const auto set1_flags = to_flags(size, expected_vertices);
        constexpr int_t zero{};

        auto cut_cost = zero;

        for (const auto &from_set_1 : expected_vertices)
        {
            const auto &edges = graph.at(from_set_1);

            for (std::size_t tod_set_2{}; tod_set_2 < size; ++tod_set_2)
            {
                if (set1_flags.at(tod_set_2))
                {
                    continue;
                }

                const auto &cost = edges.at(tod_set_2);
                cut_cost += cost;

                Standard::Algorithms::require_less_equal(zero, cut_cost, "temporal cut cost");
            }
        }

        return cut_cost;
    }

    // The input graph is assumed to be valid.
    constexpr void validate_min_cut(const std::string &name, const full_cost_graph_t &graph,
        const expected_vertices_t &set1_vertices, const int_t &min_cut_cost)
    {
        const auto size = graph.size();
        assert(!name.empty() && 1 < size);

        Standard::Algorithms::throw_if_empty(name + " vertices", set1_vertices);
        Standard::Algorithms::require_greater(size, set1_vertices.size(), name + " vertices size");

        Standard::Algorithms::require_unique(set1_vertices, name + " vertices");

        {
            const auto iter = std::max_element(set1_vertices.cbegin(), set1_vertices.cend());

            Standard::Algorithms::require_greater(size, *iter, name + " vertices max");
        }

        constexpr int_t zero{};

        Standard::Algorithms::require_less_equal(zero, min_cut_cost, name + " cost");

        {
            const auto actual_cost = compute_cut_cost(graph, set1_vertices);

            ::Standard::Algorithms::ert::are_equal(min_cut_cost, actual_cost, name + " computed cut cost");
        }
    }

    constexpr void simple_validate(const test_case &test)
    {
        const auto &input = test.input();
        const auto size = input.size();
        Standard::Algorithms::require_greater(size, 1ZU, "graph  size");

        for (std::size_t row{}; const auto &costs : input)
        {
            Standard::Algorithms::require_all_non_negative(costs, "graph costs at " + std::to_string(row));
            ++row;
        }

        {
            constexpr auto all = true;
            constexpr Standard::Algorithms::check_matrix_options options{ all, all, all, all, all };

            Standard::Algorithms::require_matrix(input, options);
        }

        const auto &output = test.output();
        if (!output.has_value())
        {
            return;
        }

        const auto &[expected_vertices, expected_cost] = output.value();

        validate_min_cut("validate test output", input, expected_vertices, expected_cost);
    }

    [[nodiscard]] constexpr auto build_input_1() -> full_cost_graph_t
    {
        // v=4, e=6.
        // 0a -  200 - 1b
        //  |  \        /  |;
        //  |   62  /    |;
        // 45    x    79
        //  |   52  \    |;
        //  |  /       \   |;
        // 3d  -   7   - 2c
        return full_cost_graph_t{// NOLINTNEXTLINE
            { 0, 200, 62, 45 }, // NOLINTNEXTLINE
            { 200, 0, 79, 52 }, // NOLINTNEXTLINE
            { 62, 79, 0, 7 }, // NOLINTNEXTLINE
            { 45, 52, 7, 0 }
        };
    }

    [[nodiscard]] constexpr auto build_output_1() -> expected_t
    {
        expected_vertices_t expected_vertices // NOLINTNEXTLINE
            { 3 };

        constexpr int_t expected_cost = // NOLINTNEXTLINE
            45 + 52 + 7;

        return { std::move(expected_vertices), expected_cost };
    }

    [[nodiscard]] constexpr auto build_input_2() -> full_cost_graph_t
    {
        // 1-3 is the only missing edge.
        return full_cost_graph_t{// NOLINTNEXTLINE
            { 0, 1, 6, 9 }, // NOLINTNEXTLINE
            { 1, 0, 4, 0 }, // NOLINTNEXTLINE
            { 6, 4, 0, 8 }, // NOLINTNEXTLINE
            { 9, 0, 8, 0 }
        };
    }

    [[nodiscard]] constexpr auto build_output_2() -> expected_t
    {
        expected_vertices_t expected_vertices // NOLINTNEXTLINE
            { 1 };

        constexpr int_t expected_cost = // NOLINTNEXTLINE
            1 + 4;

        return { std::move(expected_vertices), expected_cost };
    }

    [[nodiscard]] constexpr auto build_input_3() -> full_cost_graph_t
    {
        // 1-3 is the only missing edge.
        return full_cost_graph_t{// NOLINTNEXTLINE
            { 0, 1, 1, 1 }, // NOLINTNEXTLINE
            { 1, 0, 1, 0 }, // NOLINTNEXTLINE
            { 1, 1, 0, 1 }, // NOLINTNEXTLINE
            { 1, 0, 1, 0 }
        };
    }

    [[nodiscard]] constexpr auto build_input_5() -> full_cost_graph_t
    {
        return full_cost_graph_t{// NOLINTNEXTLINE
            { 0, 14, 28, 157, 29 }, // NOLINTNEXTLINE
            { 14, 0, 106, 19, 98 }, // NOLINTNEXTLINE
            { 28, 106, 0, 109, 89 }, // NOLINTNEXTLINE
            { 157, 19, 109, 0, 11 }, // NOLINTNEXTLINE
            { 29, 98, 89, 11, 0 }
        };
    }

    [[nodiscard]] constexpr auto build_output_5() -> expected_t
    {
        expected_vertices_t expected_vertices // NOLINTNEXTLINE
            { 0, 3 };

        constexpr int_t expected_cost = // NOLINTNEXTLINE
            14 + 28 + 29 + // 0   // NOLINTNEXTLINE
            19 + 109 + 11; // 3

        // NOLINTNEXTLINE
        static_assert(expected_cost == 210);

        return { std::move(expected_vertices), expected_cost };
    }

    [[nodiscard]] constexpr auto build_input_isolated() -> full_cost_graph_t
    {
        return full_cost_graph_t{// NOLINTNEXTLINE
            { 0, 0, 0, 1 }, // NOLINTNEXTLINE
            { 0, 0, 0, 0 }, // NOLINTNEXTLINE
            { 0, 0, 0, 1 }, // NOLINTNEXTLINE
            { 1, 0, 1, 0 }
        };
    }

    [[nodiscard]] constexpr auto build_output_isolated() -> expected_t
    {
        expected_vertices_t expected_vertices // NOLINTNEXTLINE
            { 1 };

        constexpr int_t expected_cost{};

        return { std::move(expected_vertices), expected_cost };
    }

    constexpr void generate_test_cases(std::vector<test_case> &test_cases)
    {
        constexpr auto could_be_several_results = std::nullopt;

        test_cases.emplace_back("test one", build_input_1(), build_output_1());

        test_cases.emplace_back("test two", build_input_2(), build_output_2());

        test_cases.emplace_back("test three", build_input_3(), could_be_several_results); // {1}, {3}.

        test_cases.emplace_back("test five", build_input_5(), build_output_5());

        test_cases.emplace_back("isolated vertex", build_input_isolated(), build_output_isolated());

        constexpr auto max_attempts = 1;

        for (std::int32_t att{}; att < max_attempts; ++att)
        {
            constexpr int_t min_weight{};
            constexpr int_t max_weight = 200;

            auto graph =
                Standard::Algorithms::Graphs::create_random_graph_weighted_symmetric<int_t, min_nodes, max_nodes>(
                    min_weight, max_weight);

            test_cases.emplace_back("random test" + std::to_string(att), std::move(graph), could_be_several_results);
        }
    }

    constexpr void verify_test_result(const std::string &name, const std::size_t expected_set1_size,
        const int_t &expected_cost, const test_case &test, const expected_t &actual)
    {
        assert(!name.empty());

        ::Standard::Algorithms::ert::are_equal(expected_cost, actual.second, name + " expected cost");

        const auto &graph = test.input();
        {
            const auto size = graph.size();
            const auto name_size = name + " expected set1 size";

            Standard::Algorithms::require_positive(expected_set1_size, name_size);
            Standard::Algorithms::require_greater(size, expected_set1_size, name_size);

            ::Standard::Algorithms::ert::are_equal(expected_set1_size, actual.first.size(), name_size);
        }

        const auto &output = test.output();
        const auto has_output = output.has_value();

        if (has_output)
        {
            ::Standard::Algorithms::ert::are_equal(output.value().first, actual.first, name + " set 1 vertices");

            ::Standard::Algorithms::ert::are_equal(output.value().second, actual.second, name + " cost");
            return;
        }

        validate_min_cut(name, graph, actual.first, actual.second);
    }

    constexpr void run_test_case(const test_case &test)
    {
        const auto &graph = test.input();
        const auto expected = Standard::Algorithms::Graphs::min_cut_slow<int_t, int_t>(graph);

        verify_test_result("min_cut_slow", expected.first.size(), expected.second, test, expected);

        {
            auto cop = graph;
            const auto actual = Standard::Algorithms::Graphs::min_cut_stoer_wagner<int_t>(cop);

            verify_test_result("min_cut_stoer_wagner", expected.first.size(), expected.second, test, actual);
        }
        {
            const auto size = static_cast<std::int32_t>(graph.size());
            max_flow_graph_t flow_graph(0, 1, size);

            const auto actual = Standard::Algorithms::Graphs::min_cut_via_flow<int_t>(graph, flow_graph);

            verify_test_result("min_cut_via_flow", expected.first.size(), expected.second, test, actual);
        }
    }
} // namespace

void Standard::Algorithms::Graphs::Tests::min_cut_stoer_wagner_tests()
{
    Standard::Algorithms::Tests::test_utilities<test_case>::run_tests(run_test_case, generate_test_cases);
}
