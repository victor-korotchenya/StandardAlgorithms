#include"detect_many_cycles_tests.h"
#include"../Utilities/simple_test_case.h"
#include"../Utilities/test_utilities.h"
#include"detect_many_cycles.h"

namespace
{
    using vertex_t = std::int32_t;
    using edges_t = std::vector<vertex_t>;
    using graph_t = std::vector<edges_t>;
    using cycle_list_t = std::vector<edges_t>;

    struct unique_tag final
    {
    };

    using test_case = Standard::Algorithms::Tests::simple_test_case<unique_tag, graph_t, cycle_list_t>;

    constexpr void simple_validate(const test_case &test)
    {
        const auto &input = test.input();
        const auto size = Standard::Algorithms::require_positive(static_cast<vertex_t>(input.size()), "graph size.");

        const auto &output = test.output();

        for (std::size_t index{}; const auto &cycle : output)
        {
            const auto name = "expected cycle " + std::to_string(index);
            ++index;

            Standard::Algorithms::require_positive(cycle.size(), name + " size");
            Standard::Algorithms::require_unique(cycle, name);

            const auto [min_it, max_it] = std::minmax_element(cycle.cbegin(), cycle.cend());
            Standard::Algorithms::require_less_equal(vertex_t{}, *min_it, name + " min");
            Standard::Algorithms::require_greater(size, *max_it, name + " max");
        }
    }

    constexpr void add_edge(graph_t &graph, vertex_t one, vertex_t two)
    {
        assert(one != two);

        graph[one].push_back(two);
        graph[two].push_back(one);
    }

    [[nodiscard]] constexpr auto build_graph_1() -> graph_t
    {
        constexpr auto size = 10;

        graph_t graph(size);
        // 0-1-2-3-0 cycle
        // 2-4
        // 4-5-6-4 cycle
        // 6-7-8-9
        for (vertex_t index{}; index <= 3; ++index)
        {
            const auto ott = (index + 1) % 4;
            add_edge(graph, index, ott);
        }

        add_edge(graph, 2, 4);

        for (vertex_t index{}; index <= 2; ++index)
        {
            {
                const auto one = 4 + index;
                const auto two = (index + 1) % 3 + 4;
                add_edge(graph, one, two);
            }

            // NOLINTNEXTLINE
            const auto one = 6 + index;
            const auto two = one + 1;
            add_edge(graph, one, two);
        }

        return graph;
    }

    [[nodiscard]] constexpr auto build_expected_1()
    {
        return cycle_list_t{// NOLINTNEXTLINE
            { 0, 1, 2, 3 }, { 4, 5, 6 }
        };
    }

    [[nodiscard]] constexpr auto build_graph_2() -> graph_t
    {
        constexpr auto size = 4;

        graph_t graph(size);
        add_edge(graph, 0, 1);
        add_edge(graph, 1, 2);
        add_edge(graph, 1, 3);
        add_edge(graph, 2, 0);
        add_edge(graph, 3, 0);
        // 0-1-2-0 cycle
        // 0-1-3-0 cycle

        return graph;
    }

    [[nodiscard]] constexpr auto build_expected_2()
    {
        return cycle_list_t{// NOLINTNEXTLINE
            { 0, 1, 2 }, { 0, 1, 3 }
        };
    }

    constexpr void generate_test_cases(std::vector<test_case> &test_cases)
    {
        test_cases.emplace_back("test one", build_graph_1(), build_expected_1());

        test_cases.emplace_back("test two", build_graph_2(), build_expected_2());

        test_cases.emplace_back("C2 P2", graph_t{ { 1 }, { 0 } }, cycle_list_t{});

        test_cases.emplace_back("P3", graph_t{ { 1 }, { 0, 2 }, { 1 } }, cycle_list_t{});

        test_cases.emplace_back("P4", graph_t{ { 1 }, { 0, 2 }, { 1, 3 }, { 2 } }, cycle_list_t{});

        test_cases.emplace_back("P5", graph_t{ { 1 }, { 0, 2 }, { 1, 3 }, { 2, 4 }, { 3 } }, // 0 - 1 - 2 - 3 - 4
            cycle_list_t{});

        test_cases.emplace_back("C3 K3", graph_t{ { 1, 2 }, { 0, 2 }, { 0, 1 } }, cycle_list_t{ { 0, 1, 2 } });

        test_cases.emplace_back("C4", graph_t{ { 3, 2 }, { 2, 3 }, { 0, 1 }, { 1, 0 } },
            // 0 - 3
            // |      |
            // 2 - 1
            cycle_list_t{ { 0, 3, 1, 2 } });
    }

    constexpr void run_test_case(const test_case &test)
    {
        {
            cycle_list_t cycles;
            std::vector<vertex_t> depths;
            std::vector<vertex_t> sta;
            Standard::Algorithms::Graphs::detect_many_cycles(test.input(), depths, cycles, sta);

            ::Standard::Algorithms::ert::are_equal(test.output(), cycles, "detect_many_cycles");
        }
        {
            const auto has_expected = !test.output().empty();
            const auto has_actual = Standard::Algorithms::Graphs::undirected_has_cycle(test.input());

            ::Standard::Algorithms::ert::are_equal(has_expected, has_actual, "undirected_has_cycle");
        }
    }
} // namespace

void Standard::Algorithms::Graphs::Tests::detect_many_cycles_tests()
{
    Standard::Algorithms::Tests::test_utilities<test_case>::run_tests(run_test_case, generate_test_cases);
}
