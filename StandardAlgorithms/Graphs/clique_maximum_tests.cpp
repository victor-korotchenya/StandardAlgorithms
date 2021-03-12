#include"clique_maximum_tests.h"
#include"../Utilities/iota_vector.h"
#include"../Utilities/require_matrix.h"
#include"../Utilities/simple_test_case.h"
#include"../Utilities/test_utilities.h"
#include"clique_maximum.h"
#include"create_random_graph.h"
#include"graph_builder.h"
#include"graph_helper.h"
#include<optional>

namespace
{
    using edges_t = std::vector<bool>;
    using graph_t = std::vector<edges_t>;

    using int_t = std::size_t;
    using clique_t = std::vector<int_t>;
    using clique_vec_t = std::vector<clique_t>;

    constexpr auto shall_require_unique_clique = true;
    constexpr auto is_undirected1 = true;

    constexpr int_t skip_tiny_size = 2U;
    constexpr int_t min_nodes = skip_tiny_size + 1;
    constexpr int_t max_nodes = ::Standard::Algorithms::is_debug ? 8 : 11;

    static_assert(skip_tiny_size <= min_nodes && min_nodes < max_nodes);
    static_assert(max_nodes <= Standard::Algorithms::Graphs::slow_clique_max_size);

    struct unique_tag final
    {
    };

    using test_case = Standard::Algorithms::Tests::simple_test_case<unique_tag, graph_t, std::optional<clique_vec_t>>;

    inline constexpr void demand_no_self_loops(const graph_t &graph)
    {
        const auto graph_size = graph.size();

        for (int_t index{}; const auto &edges : graph)
        {
            ::Standard::Algorithms::ert::are_equal(graph_size, edges.size(), "edges size");

            {
                constexpr auto expected = false;

                ::Standard::Algorithms::ert::are_equal(
                    expected, edges[index], "self loop at " + ::Standard::Algorithms::Utilities::zu_string(index));
            }

            ++index;
        }
    }

    constexpr void require_clique(
        const std::string &name, const graph_t &graph, const int_t expected_clique_size, const clique_t &clique)
    {
        assert(!name.empty());

        Standard::Algorithms::require_positive(expected_clique_size, name + " expected clique size");
        ::Standard::Algorithms::ert::are_equal(expected_clique_size, clique.size(), name + " clique size");
        assert(!clique.empty());

        Standard::Algorithms::require_sorted(clique, name, shall_require_unique_clique); // Easier to test.

        const auto graph_size = graph.size();
        const auto &maxi = clique.back();
        Standard::Algorithms::require_greater(graph_size, maxi, name + " clique back");

        for (int_t from_ind{}; from_ind < expected_clique_size; ++from_ind)
        {
            const auto &from = clique[from_ind];
            Standard::Algorithms::require_greater(graph_size, from, name + " from");

            const auto &edges = graph[from];

            for (int_t tod_ind = from_ind + 1; tod_ind < expected_clique_size; ++tod_ind)
            {
                const auto &tod = clique[tod_ind];
                Standard::Algorithms::require_greater(graph_size, tod, name + " to");

                if (edges[tod]) [[likely]]
                {
                    continue;
                }

                auto err = "An edge " + ::Standard::Algorithms::Utilities::zu_string(from) + " - " +
                    ::Standard::Algorithms::Utilities::zu_string(tod) + " must exist. " + name;

                throw std::runtime_error(err);
            }
        }
    }

    constexpr void require_cliques(const graph_t &graph, const std::string &name, const clique_vec_t &cliques)
    {
        assert(!name.empty());

        const auto clique_size = cliques.empty() ? 0ZU : cliques[0].size();

        for (int_t index{}; const auto &clique : cliques)
        {
            const auto sub_name = name + " expected clique #" + ::Standard::Algorithms::Utilities::zu_string(index);
            require_clique(sub_name, graph, clique_size, clique);
            ++index;
        }

        Standard::Algorithms::require_sorted(cliques, name + " all cliques", shall_require_unique_clique);
    }

    constexpr void simple_validate(const test_case &test)
    {
        const auto &graph = test.input();
        {
            constexpr Standard::Algorithms::check_matrix_options options{ .square = true, .symmetric = true };

            Standard::Algorithms::require_matrix(graph, options);
        }

        demand_no_self_loops(graph);

        const auto graph_size = graph.size();

        const auto &output = test.output();
        const auto has_output = output.has_value();

        if (graph_size <= 1U)
        {
            ::Standard::Algorithms::ert::are_equal(true, has_output, "has output");

            if (has_output) [[likely]]
            {
                ::Standard::Algorithms::ert::are_equal(
                    clique_vec_t(graph_size, clique_t(graph_size)), output.value(), "output");
            }
        }

        if (!has_output)
        {
            return;
        }

        const auto &cliques = output.value();
        require_cliques(graph, "validate", cliques);
    }

    void generate_test_cases_random(std::vector<test_case> &test_cases)
    {
        constexpr auto max_attempts = 1;

        for (std::int32_t att{}; att < max_attempts; ++att)
        {
            const auto vec_graph =
                Standard::Algorithms::Graphs::create_random_graph<int_t, max_nodes, is_undirected1, min_nodes>().first;

            const auto size = vec_graph.size();
            std::vector<std::vector<bool>> adj_matr(size, std::vector<bool>(size, false));

            Standard::Algorithms::Graphs::graph_list_to_adjacency_matrix(vec_graph, adj_matr);

            test_cases.emplace_back("random" + std::to_string(att), std::move(adj_matr), std::nullopt);
        }
    }

    constexpr void run_test_case_impl(
        const graph_t &graph, const std::optional<clique_vec_t> &expected_opt, const bool is_komplett = false)
    {
        const auto [fast, fast_steps] = Standard::Algorithms::Graphs::clique_maximum_still(graph);

        if (is_komplett)
        {
            ::Standard::Algorithms::ert::are_equal(graph.size(), fast_steps, "still steps");
        }

        if (expected_opt.has_value())
        {
            ::Standard::Algorithms::ert::are_equal(expected_opt.value(), fast, "clique_maximum_still");
        }
        else
        {
            require_cliques(graph, "run", fast);
        }

        {
            const auto [slow, slow_steps] = Standard::Algorithms::Graphs::clique_maximum_slow(graph);
            ::Standard::Algorithms::ert::are_equal(fast, slow, "clique_maximum_slow");

            // Standard::Algorithms::require_less_equal(fast_steps, slow_steps, "fast vs slow steps");
            if (is_komplett)
            {
                ::Standard::Algorithms::ert::are_equal(graph.size(), slow_steps, "slow steps");
            }
        }
    }

    constexpr void run_test_case(const test_case &test)
    {
        run_test_case_impl(test.input(), test.output());
    }
} // namespace

namespace // generate_test_cases_xpr
{
    constexpr void generate_test_cases_xpr(std::vector<test_case> &test_cases)
    {
        {
            constexpr auto kotleta_size = 5U;

            for (std::size_t size1{}; size1 <= kotleta_size; ++size1)
            {
                auto graph1 = Standard::Algorithms::Graphs::build_complete_graph_boolean(size1);
                auto clique1 = Standard::Algorithms::Utilities::iota_vector<int_t>(size1);

                const auto all_cliques_size = 0U < size1 ? 1U : 0U;

                test_cases.emplace_back("Complete graph " + ::Standard::Algorithms::Utilities::zu_string(size1),
                    std::move(graph1), clique_vec_t(all_cliques_size, clique1));
            }
        }

        test_cases.emplace_back("K1 + K1", graph_t{ edges_t{ false, false }, edges_t{ false, false } },
            clique_vec_t{ clique_t{ 0 }, clique_t{ 1 } });

        test_cases.emplace_back("K1 + K1 + K1",
            graph_t{
                //
                edges_t{ false, false, false }, // 0
                edges_t{ false, false, false }, // 1
                edges_t{ false, false, false } // 2
            },
            clique_vec_t{ clique_t{ 0 }, clique_t{ 1 }, clique_t{ 2 } });

        test_cases.emplace_back("K1 + K1 + K1 + K1",
            graph_t{
                //
                edges_t{ false, false, false, false }, // 0
                edges_t{ false, false, false, false }, // 1
                edges_t{ false, false, false, false }, // 2
                edges_t{ false, false, false, false } // 3
            },
            clique_vec_t{ clique_t{ 0 }, clique_t{ 1 }, clique_t{ 2 }, clique_t{ 3 } });

        test_cases.emplace_back("K3 + K1*0", // 0 + 1*2*3
            graph_t{//
                edges_t{ false, false, false, false }, // 0
                edges_t{ false, false, true, true }, // 1
                edges_t{ false, true, false, true }, // 2
                edges_t{ false, true, true, false } }, // 3
            clique_vec_t{ clique_t{ 1, 2, 3 } });

        test_cases.emplace_back("K3 + K1*1", // 1 + 0*2*3
            graph_t{//
                edges_t{ false, false, true, true }, // 0
                edges_t{ false, false, false, false }, // 1
                edges_t{ true, false, false, true }, // 2
                edges_t{ true, false, true, false } }, // 3
            clique_vec_t{ clique_t{ 0, 2, 3 } });

        test_cases.emplace_back("K3 + K1*2", // 2 + 0*1*3
            graph_t{//
                edges_t{ false, true, false, true }, // 0
                edges_t{ true, false, false, true }, // 1
                edges_t{ false, false, false, false }, // 2
                edges_t{ true, true, false, false } }, // 3
            clique_vec_t{ clique_t{ 0, 1, 3 } });

        test_cases.emplace_back("K3 + K1*3", // 3 + 0*1*2
            graph_t{//
                edges_t{ false, true, true, false }, // 0
                edges_t{ true, false, true, false }, // 1
                edges_t{ true, true, false, false }, // 2
                edges_t{ false, false, false, false } }, // 3
            clique_vec_t{ clique_t{ 0, 1, 2 } });

        test_cases.emplace_back("K5 without 0-3",
            graph_t{//
                edges_t{ false, true, true, false, true }, // 0
                edges_t{ true, false, true, true, true }, // 1
                edges_t{ true, true, false, true, true }, // 2
                edges_t{ false, true, true, false, true }, // 3
                edges_t{ true, true, true, true, false } }, // 4
            clique_vec_t{ clique_t{ 0, 1, 2, 4 }, clique_t{ 1, 2, 3, 4 } });

        test_cases.emplace_back("K5 without 0-3, 0-4",
            graph_t{//
                edges_t{ false, true, true, false, false }, // 0
                edges_t{ true, false, true, true, true }, // 1
                edges_t{ true, true, false, true, true }, // 2
                edges_t{ false, true, true, false, true }, // 3
                edges_t{ false, true, true, true, false } }, // 4
            clique_vec_t{ clique_t{ 1, 2, 3, 4 } });

        test_cases.emplace_back("K5 without 0-1",
            graph_t{//
                edges_t{ false, false, true, true, true }, // 0
                edges_t{ false, false, true, true, true }, // 1
                edges_t{ true, true, false, true, true }, // 2
                edges_t{ true, true, true, false, true }, // 3
                edges_t{ true, true, true, true, false } }, // 4
            clique_vec_t{ clique_t{ 0, 2, 3, 4 }, clique_t{ 1, 2, 3, 4 } });

        test_cases.emplace_back("K5 without 1-3",
            graph_t{//
                edges_t{ false, true, true, true, true }, // 0
                edges_t{ true, false, true, false, true }, // 1
                edges_t{ true, true, false, true, true }, // 2
                edges_t{ true, false, true, false, true }, // 3
                edges_t{ true, true, true, true, false } }, // 4
            clique_vec_t{ clique_t{ 0, 1, 2, 4 }, clique_t{ 0, 2, 3, 4 } });

        test_cases.emplace_back("K5 without 1-4",
            graph_t{//
                edges_t{ false, true, true, true, true }, // 0
                edges_t{ true, false, true, true, false }, // 1
                edges_t{ true, true, false, true, true }, // 2
                edges_t{ true, true, true, false, true }, // 3
                edges_t{ true, false, true, true, false } }, // 4
            clique_vec_t{ clique_t{ 0, 1, 2, 3 }, clique_t{ 0, 2, 3, 4 } });

        test_cases.emplace_back("P2 + P2", // 0 - 1, 2 - 3
            graph_t{
                //
                edges_t{ false, true, false, false }, // 0
                edges_t{ true, false, false, false }, // 1
                edges_t{ false, false, false, true }, // 2
                edges_t{ false, false, true, false } // 3
            },
            clique_vec_t{ clique_t{ 0, 1 }, clique_t{ 2, 3 } });

        test_cases.emplace_back("P2 + P2 02", // 0 - 2, 1 - 3
            graph_t{
                //
                edges_t{ false, false, true, false }, // 0
                edges_t{ false, false, false, true }, // 1
                edges_t{ true, false, false, false }, // 2
                edges_t{ false, true, false, false } // 3
            },
            clique_vec_t{ clique_t{ 0, 2 }, clique_t{ 1, 3 } });

        test_cases.emplace_back("P3", // 0 - 1 - 2.
            graph_t{//
                edges_t{ false, true, false }, // 0
                edges_t{ true, false, true }, // 1
                edges_t{ false, true, false } }, // 2
            clique_vec_t{ clique_t{ 0, 1 }, clique_t{ 1, 2 } });

        test_cases.emplace_back("P4", // 0 - 1 - 2 - 3.
            graph_t{//
                edges_t{ false, true, false, false }, // 0
                edges_t{ true, false, true, false }, // 1
                edges_t{ false, true, false, true }, // 2
                edges_t{ false, false, true, false } }, // 3
            clique_vec_t{ clique_t{ 0, 1 }, clique_t{ 1, 2 }, clique_t{ 2, 3 } });
    }

    constexpr void generate_test_cases_large_sortir(std::vector<test_case> &test_cases)
    {
        test_cases.emplace_back("Sortir breaker", // todo(p5): Clang "constexpr evaluation hit maximum step limit".
            graph_t{//
                { false, true, false, true, false, true, true, true }, // 0
                { true, false, false, false, false, true, true, false }, // 1
                { false, false, false, false, false, true, true, false }, // 2
                { true, false, false, false, false, true, true, false }, // 3
                { false, false, false, false, false, true, false, false }, // 4
                { true, true, true, true, true, false, false, true }, // 5
                { true, true, true, true, false, false, false, false }, // 6
                { true, false, false, false, false, true, false, false } }, // 7
            clique_vec_t{// NOLINTNEXTLINE
                clique_t{ 0, 1, 5 }, clique_t{ 0, 1, 6 }, clique_t{ 0, 3, 5 }, // NOLINTNEXTLINE
                clique_t{ 0, 3, 6 }, clique_t{ 0, 5, 7 } });

        test_cases.emplace_back("K6 without 0-1",
            graph_t{//
                edges_t{ false, false, true, true, true, true }, // 0
                edges_t{ false, false, true, true, true, true }, // 1
                edges_t{ true, true, false, true, true, true }, // 2
                edges_t{ true, true, true, false, true, true }, // 3
                edges_t{ true, true, true, true, false, true }, // 4
                edges_t{ true, true, true, true, true, false } }, // 5 // NOLINTNEXTLINE
            clique_vec_t{ clique_t{ 0, 2, 3, 4, 5 }, clique_t{ 1, 2, 3, 4, 5 } });

        test_cases.emplace_back("K7 without 0-1",
            graph_t{//
                edges_t{ false, false, true, true, true, true, true }, // 0
                edges_t{ false, false, true, true, true, true, true }, // 1
                edges_t{ true, true, false, true, true, true, true }, // 2
                edges_t{ true, true, true, false, true, true, true }, // 3
                edges_t{ true, true, true, true, false, true, true }, // 4
                edges_t{ true, true, true, true, true, false, true }, // 5
                edges_t{ true, true, true, true, true, true, false } }, // 6 // NOLINTNEXTLINE
            clique_vec_t{ clique_t{ 0, 2, 3, 4, 5, 6 }, clique_t{ 1, 2, 3, 4, 5, 6 } });

        test_cases.emplace_back("K7 without 2-5",
            graph_t{//
                edges_t{ false, true, true, true, true, true, true }, // 0
                edges_t{ true, false, true, true, true, true, true }, // 1
                edges_t{ true, true, false, true, true, false, true }, // 2
                edges_t{ true, true, true, false, true, true, true }, // 3
                edges_t{ true, true, true, true, false, true, true }, // 4
                edges_t{ true, true, false, true, true, false, true }, // 5
                edges_t{ true, true, true, true, true, true, false } }, // 6 // NOLINTNEXTLINE
            clique_vec_t{ clique_t{ 0, 1, 2, 3, 4, 6 }, clique_t{ 0, 1, 3, 4, 5, 6 } });
    }
} // namespace

namespace // test_clique_steps
{
    constexpr void test_clique_steps_impl(const std::uint32_t size1)
    {
        constexpr auto is_komplett = true;

        const auto graph = Standard::Algorithms::Graphs::build_complete_graph_boolean(size1);
        auto clique1 = Standard::Algorithms::Utilities::iota_vector<int_t>(size1);

        run_test_case_impl(graph, clique_vec_t{ std::move(clique1) }, is_komplett);
    }

    [[nodiscard]] constexpr auto test_clique_steps() -> bool
    {
        constexpr auto max_clique_size = 10U;
        static_assert(skip_tiny_size < max_clique_size);

        for (auto size1 = skip_tiny_size; size1 <= max_clique_size; ++size1)
        {
            test_clique_steps_impl(size1);
        }

        return true;
    }
} // namespace

void Standard::Algorithms::Graphs::Tests::clique_maximum_tests()
{
    static_assert(test_clique_steps());

    static_assert(
        Standard::Algorithms::Tests::test_utilities<test_case>::run_tests(run_test_case, generate_test_cases_xpr));

    static_assert(Standard::Algorithms::Tests::test_utilities<test_case>::run_tests(
        run_test_case, generate_test_cases_large_sortir));

    Standard::Algorithms::Tests::test_utilities<test_case>::run_tests(run_test_case, generate_test_cases_random);
}
