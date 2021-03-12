#include"is_singly_connected_graph_tests.h"
#include"../Utilities/simple_test_case.h"
#include"../Utilities/test_utilities.h"
#include"create_random_graph.h"
#include"is_singly_connected_graph.h"
#include<optional>

namespace
{
    using edges_t = std::vector<std::size_t>;
    using graph_t = std::vector<edges_t>;

    struct unique_tag final
    {
    };

    using test_case = Standard::Algorithms::Tests::simple_test_case<unique_tag, graph_t, std::optional<bool>>;

    void add_random_tests(std::vector<test_case> &test_cases)
    {
        constexpr auto max_attempts = 1;

        for (std::int32_t att{}; att < max_attempts; ++att)
        {
            constexpr auto max_nodes = 10U;
            constexpr auto is_undirected = false;

            auto graph = Standard::Algorithms::Graphs::create_random_graph<std::size_t, max_nodes, is_undirected>().first;

            test_cases.emplace_back("random" + std::to_string(att), std::move(graph), std::nullopt);
        }
    }

    void generate_test_cases(std::vector<test_case> &test_cases)
    {
        test_cases.emplace_back("C2 + C2 vertex-sharing", graph_t{ { 1 }, { 0, 2 }, { 1 } }, true);

        test_cases.emplace_back("C3 + C2 vertex-sharing", graph_t{ { 1 }, { 2 }, { 0, 3 }, { 2 } }, true);

        test_cases.emplace_back("C3 + C3 vertex-sharing", graph_t{ { 1 }, { 2 }, { 0, 3 }, { 4 }, { 2 } }, true);

        test_cases.emplace_back("103 + 123 aga", graph_t{ { 3 }, { 0, 2 }, { 3 }, {}, {} }, false);

        test_cases.emplace_back("103 + 123", graph_t{ { 3 }, { 0, 2 }, { 4, 3 }, { 1 }, { 2 } }, false);

        test_cases.emplace_back("013 + 0243", graph_t{ { 1, 2 }, { 3 }, { 4 }, {}, { 3 } }, false);

        test_cases.emplace_back("full 2", graph_t{ { 0, 1 }, { 0, 1 } }, true);

        test_cases.emplace_back("full 3", graph_t{ { 0, 1, 2 }, { 0, 1, 2 }, { 0, 1, 2 } }, false);

        test_cases.emplace_back(
            "full 4", graph_t{ { 0, 1, 2, 3 }, { 0, 1, 2, 3 }, { 0, 1, 2, 3 }, { 0, 1, 2, 3 } }, false);

        test_cases.emplace_back("01 + 021", graph_t{ { 1, 2 }, {}, { 1 } }, false);

        test_cases.emplace_back("02 + 012", graph_t{ { 1, 2 }, { 2 }, {} }, false);

        test_cases.emplace_back("multi-edges", graph_t{ { 1, 1 }, {} }, false);
        test_cases.emplace_back("multi-edges with self-loops", graph_t{ { 1, 0, 1 }, {} }, false);

        test_cases.emplace_back("more multi-edges", graph_t{ { 1, 1 }, { 0, 0 } }, false);
        test_cases.emplace_back("more multi-edges with self-loops",
            graph_t{ { 1, 0, 1, 1, 1, 0, 1, 0, 1, 0, 0, 0 }, { 0, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0 } }, false);

        test_cases.emplace_back("multi-edges 5", graph_t{ {}, {}, {}, {}, {}, { 0, 0 }, {} }, false);

        test_cases.emplace_back("empty graph", graph_t{}, true);
        test_cases.emplace_back("trivial graph", graph_t{ {} }, true);

        test_cases.emplace_back("one edge", graph_t{ { 1 }, {} }, true);
        test_cases.emplace_back("one edge rev", graph_t{ {}, { 0 } }, true);

        test_cases.emplace_back("fork", graph_t{ { 1, 2 }, {}, {} }, true);

        test_cases.emplace_back("2 K1", graph_t{ {}, {} }, true);
        test_cases.emplace_back("3 K1", graph_t{ {}, {}, {} }, true);

        test_cases.emplace_back("4 K1", graph_t{ {}, {}, {}, {} }, true);

        {
            constexpr auto max_cycles = 6;

            for (std::size_t cycles = 2; cycles <= max_cycles; ++cycles)
            {
                graph_t graph(cycles);
                graph_t with_loops(cycles);

                for (std::size_t from = 1; from < cycles; ++from)
                {
                    graph.at(from - 1U).push_back(from);

                    auto &edges = with_loops.at(from - 1U);
                    edges.push_back(from - 1U);
                    edges.push_back(from);
                }

                graph.back().push_back(0Z);
                with_loops.back().push_back(0Z);

                test_cases.emplace_back("C" + std::to_string(cycles), std::move(graph), true);

                test_cases.emplace_back("C" + std::to_string(cycles) + " with self-loops", std::move(with_loops), true);
            }
        }

        add_random_tests(test_cases);
    }

    [[nodiscard]] constexpr auto run_slow(const test_case &test) -> bool
    {
        auto input = test.input();
        auto slow = Standard::Algorithms::Graphs::is_singly_connected_graph_slow(input);

        if (const auto &expected = test.output(); expected.has_value())
        {
            ::Standard::Algorithms::ert::are_equal(expected.value(), slow, "is_singly_connected_graph_slow");
        }

        return slow;
    }

    constexpr void run_test_case(const test_case &test)
    {
        const auto slow = run_slow(test);
        {
            auto input = test.input();
            const auto actual = Standard::Algorithms::Graphs::is_singly_connected_graph(input);

            ::Standard::Algorithms::ert::are_equal(slow, actual, "is_singly_connected_graph");
        }
    }
} // namespace

void Standard::Algorithms::Graphs::Tests::is_singly_connected_graph_tests()
{
    Standard::Algorithms::Tests::test_utilities<test_case>::run_tests(run_test_case, generate_test_cases);
}
