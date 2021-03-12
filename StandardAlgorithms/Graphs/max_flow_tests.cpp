#include"max_flow_tests.h"
#include"../Utilities/random.h"
#include"../Utilities/test_utilities.h"
#include"create_random_graph.h"
#include"dinic_flow_old.h"
#include"edmonds_karp_flow.h"
#include"flow_edge.h"
#include"max_flow_graph.h"
#include"min_cost_max_flow_graph.h"
#include"weighted_vertex.h"
#include<unordered_map>

// todo(p3): split into several tests.

namespace
{
    using weight_t = std::int32_t;
    using cost_t = std::int64_t;
    using test_edge_t = Standard::Algorithms::weighted_vertex<std::int32_t, weight_t>;

    // Only in tests, use cost_flow_edge instead of flow_edge
    using edge_t = Standard::Algorithms::Graphs::cost_flow_edge<weight_t, cost_t>;

    constexpr auto max_size = 20;

    // Add empty reverse edges if not present.
    constexpr auto to_flow_graph(const std::vector<std::vector<test_edge_t>> &graph) -> std::vector<std::vector<edge_t>>
    {
        std::vector<std::vector<edge_t>> result(graph.size());

        std::vector<std::unordered_map<std::int32_t, std::int32_t>> edge_map(graph.size());

        {
            std::int32_t index{};

            for (const auto &edges : graph)
            {
                std::int32_t ind2{};

                for (const auto &edge : edges)
                {
                    result[index].push_back({ edge.vertex, 0, edge.weight });

                    if (const auto ite = edge_map[index].emplace(edge.vertex, ind2); !ite.second) [[unlikely]]
                    {
                        throw std::runtime_error("The flow graph has multi-edges.");
                    }

                    ++ind2;
                }

                ++index;
            }
        }
        {
            std::int32_t index{};

            for (auto &edges : result)
            {
                std::int32_t ind2{};

                for (auto &edge : edges)
                {
                    auto &emap = edge_map[edge.to];
                    auto ite = emap.find(index);

                    if (emap.end() == ite)
                    {// Add the missing back edge.
                        edge.rev_edge_index = static_cast<std::int32_t>(result[edge.to].size());
                        result[edge.to].push_back({ index, ind2 });
                    }
                    else
                    {
                        edge.rev_edge_index = ite->second;
                    }

                    ++ind2;
                }

                ++index;
            }
        }

        return result;
    }

    template<class graph_t>
    constexpr void add_adges(const std::vector<std::vector<test_edge_t>> &source, graph_t &aim)
    {
        const auto size = static_cast<std::int32_t>(source.size());
        assert(0 < size);

        for (std::int32_t index{}; index < size; ++index)
        {
            for (const auto &edges = source[index]; const auto &edge : edges)
            {
                aim.add_edge(index, edge.vertex, edge.weight);
            }
        }
    }

    // It is needed to compare the results.
    template<class graph_t>
    constexpr void remove_empty_flow_edges(graph_t &graph, const std::int32_t size)
    {
        for (std::int32_t index{}; index < size; ++index)
        {
            auto &edges = graph.at(index);

            for (std::int32_t ind2{}; ind2 < static_cast<std::int32_t>(edges.size());)
            {
                if (edges[ind2].flow <= 0)
                {
                    std::swap(edges[ind2], edges[edges.size() - 1U]);
                    edges.pop_back();
                }
                else
                {
                    ++ind2;
                }
            }
        }
    }

    [[nodiscard]] inline constexpr auto compare_edges(const test_edge_t *const expected,
        // todo(p4): const edge_t&
        const edge_t *const actual) -> bool
    {
        assert(expected != nullptr && actual != nullptr);

        const auto equ = expected != nullptr && actual != nullptr && actual->to == expected->vertex &&
            actual->flow == expected->weight;
        return equ;
    }

    template<class graph_t>
    constexpr void check_edge_flows(const std::string &name,
        const std::vector<std::vector<test_edge_t>> &expected_flows, graph_t &graph, const std::int32_t size)
    {
        assert(0 < size);

        remove_empty_flow_edges(graph, size);

        using lambda_t = bool (*)(const test_edge_t *expected, const edge_t *actual);

        auto full_name = name + ", flow at row ";
        const auto full_name_size = full_name.size();

        for (std::int32_t index{}; index < static_cast<std::int32_t>(size); ++index)
        {
            full_name.resize(full_name_size);
            full_name += std::to_string(index);

            auto &actual = graph.at(index);

            std::sort(actual.begin(), actual.end(),
                [] [[nodiscard]] (const auto &ed1, const auto &ed2) -> bool
                {
                    return ed1.to < ed2.to;
                });

            const auto &expected = expected_flows[index];

            assert(std::is_sorted(expected.cbegin(), expected.cend(),
                [] [[nodiscard]] (const test_edge_t &one, const test_edge_t &two)
                {
                    return one.vertex < two.vertex || (one.vertex == two.vertex && one.weight < two.weight);
                }));

            ::Standard::Algorithms::ert::are_equal<test_edge_t, edge_t, lambda_t>(
                expected, actual, Standard::Algorithms::mess_c_str(full_name), compare_edges);
        }
    }

    template<class run_lambda>
    constexpr auto run_alg(const std::string &name, const std::vector<std::vector<test_edge_t>> &graph,
        const weight_t expected_flow, const std::vector<std::vector<test_edge_t>> &expected_flows, run_lambda &lam,
        const bool has_flow) -> weight_t
    {
        auto graph2 = to_flow_graph(graph);
        const auto size = static_cast<std::int32_t>(graph.size());

        const auto actual_flow = lam(graph2);
        if (has_flow)
        {
            ::Standard::Algorithms::ert::are_equal(expected_flow, actual_flow, name + " actual flow");

            // The actual flow might pass through different edges.
            check_edge_flows(name, expected_flows, graph2, size);
        }

        return actual_flow;
    }

    constexpr void test_flow_algs(const std::string &name, const std::int32_t source_index,
        const std::vector<std::vector<test_edge_t>> &graph, const std::int32_t sink, weight_t expected_flow,
        const std::vector<std::vector<test_edge_t>> &expected_flows)
    {
        const auto size = static_cast<std::int32_t>(graph.size());
        assert(0 < size);

        const auto has_flow = 0 < expected_flow;
        {
            // std::vector<std::vector<test_edge_t>> actualFlows;

            // const auto actual_flow = PushRelabelMaxFlow<weight_t, test_edge_t>::Compute(graph, sourceIndex,
            // actualFlows, sink);

            // if (has_flow)
            //{
            //     ::Standard::Algorithms::ert::are_equal(expectedFlow, actual_flow, name + " Push-Relabel Total flow");
            // todo(p3): finish. ::Standard::Algorithms::ert::are_equal(expected_flows, actualFlows, "Flows");
            // }
            // else
            //     expectedFlow = actual_flow;
        } {
            auto lam = [source_index, sink] [[nodiscard]] (std::vector<std::vector<edge_t>> & graph2)
            {
                const auto flo = edmonds_karp_flow<weight_t>(source_index, graph2, sink);
                return flo;
            };

            const auto actual_flow =
                run_alg(name + " Edmonds-Karp", graph, expected_flow, expected_flows, lam, has_flow);
            if (!has_flow)
            {
                expected_flow = actual_flow;
            }
        }
        {
            auto lam = [source_index, sink] [[nodiscard]] (std::vector<std::vector<edge_t>> & graph2)
            {
                const auto actual_flow = dinic_flow_old<weight_t>(source_index, graph2, sink);
                return actual_flow;
            };

            run_alg(name + " Dinic", graph, expected_flow, expected_flows, lam, has_flow);
        }
        {
            Standard::Algorithms::Graphs::max_flow_graph<weight_t, max_size, edge_t> graph2(source_index, sink, size);
            add_adges(graph, graph2);

            const auto fl4 = graph2.max_flow();
            const auto name2 = name + " max_flow_graph";
            ::Standard::Algorithms::ert::are_equal(expected_flow, fl4, name2 + " flow");

            if (has_flow)
            {
                check_edge_flows(name2, expected_flows, graph2.graph(), size);
            }
        }
        {
            Standard::Algorithms::Graphs::min_cost_max_flow_graph<weight_t, cost_t, max_size, edge_t> graph2;
            graph2.init(size, false);
            add_adges(graph, graph2);

            const auto name2 = name + " min_cost_max_flow_graph";
            const auto mfc = graph2.min_cost_max_flow(source_index, sink);
            ::Standard::Algorithms::ert::are_equal(expected_flow, mfc.first, name2 + " flow");

            if (has_flow)
            {
                check_edge_flows(name2, expected_flows, graph2.graph(), size);
            }
        }
    }

    constexpr void flow_test1()
    {
        const std::string name = "test1 ";

        //                 A0     (4 ->)      D3
        //      (10 />)                               (10) \;
        //                                    ^             v
        // B1           (2) |     (8) \   (6) |                   F5
        //                  v          v                    ^
        //                                                 /;
        //      (10 \>)                               (10)
        //                  C2    (9 ->)      E4
        //
        constexpr auto size = 6;
        std::vector<std::vector<test_edge_t>> graph(size);

        graph[0].push_back(test_edge_t(2, 2));
        graph[0].push_back(test_edge_t(3, 4));
        // NOLINTNEXTLINE
        graph[0].push_back(test_edge_t(4, 8));

        // NOLINTNEXTLINE
        graph[1].push_back(test_edge_t(0, 10));
        // NOLINTNEXTLINE
        graph[1].push_back(test_edge_t(2, 10));

        // NOLINTNEXTLINE
        graph[2].push_back(test_edge_t(4, 9));

        // NOLINTNEXTLINE
        graph[3].push_back(test_edge_t(5, 10));

        // NOLINTNEXTLINE
        graph[4].push_back(test_edge_t(3, 6));
        // NOLINTNEXTLINE
        graph[4].push_back(test_edge_t(5, 10));

        constexpr auto source_index = 1;
        constexpr auto sink = 5;
        constexpr weight_t expected_flow = 19;

        std::vector<std::vector<test_edge_t>> expected_flows(graph.size());

        expected_flows[0].push_back(test_edge_t(3, 4));
        // NOLINTNEXTLINE
        expected_flows[0].push_back(test_edge_t(4, 6));

        // NOLINTNEXTLINE
        expected_flows[1].push_back(test_edge_t(0, 10));
        // NOLINTNEXTLINE
        expected_flows[1].push_back(test_edge_t(2, 9));

        // NOLINTNEXTLINE
        expected_flows[2].push_back(test_edge_t(4, 9));

        // NOLINTNEXTLINE
        expected_flows[3].push_back(test_edge_t(5, 9));

        // NOLINTNEXTLINE
        expected_flows[4].push_back(test_edge_t(3, 5));
        // NOLINTNEXTLINE
        expected_flows[4].push_back(test_edge_t(5, 10));

        test_flow_algs(name, source_index, graph, sink, expected_flow, expected_flows);
    }

    constexpr void flow_test2()
    {
        const std::string name = "test2 ";

        constexpr auto size = 10;
        std::vector<std::vector<test_edge_t>> graph(size);
        // S..T map to 8..9
        // A..F map to 1..6
        constexpr auto source_index = 8;
        constexpr auto sink = 9;
        constexpr weight_t expected_flow = 3;

        graph[source_index].push_back(test_edge_t(1, 2));

        // too much.
        // NOLINTNEXTLINE
        graph[source_index].push_back(test_edge_t(3, 25));

        graph[1].push_back(test_edge_t(2, 1));
        // NOLINTNEXTLINE
        graph[1].push_back(test_edge_t(5, 1));
        graph[1].push_back(test_edge_t(sink, 1));

        graph[2].push_back(test_edge_t(sink, 1));
        graph[3].push_back(test_edge_t(4, 1));
        graph[4].push_back(test_edge_t(2, 1));

        // unused.
        // NOLINTNEXTLINE
        graph[4].push_back(test_edge_t(3, 50));
        // NOLINTNEXTLINE
        graph[5].push_back(test_edge_t(6, 1));
        // NOLINTNEXTLINE
        graph[6].push_back(test_edge_t(sink, 1));

        std::vector<std::vector<test_edge_t>> expected_flows(graph.size());
        expected_flows[source_index].push_back(test_edge_t(1, 2));
        expected_flows[source_index].push_back(test_edge_t(3, 1));

        // NOLINTNEXTLINE
        expected_flows[1].push_back(test_edge_t(5, 1));
        expected_flows[1].push_back(test_edge_t(sink, 1));

        expected_flows[2].push_back(test_edge_t(sink, 1));
        expected_flows[3].push_back(test_edge_t(4, 1));
        expected_flows[4].push_back(test_edge_t(2, 1));
        // NOLINTNEXTLINE
        expected_flows[5].push_back(test_edge_t(6, 1));
        // NOLINTNEXTLINE
        expected_flows[6].push_back(test_edge_t(sink, 1));

        test_flow_algs(name, source_index, graph, sink, expected_flow, expected_flows);
    }

    void flow_test_random()
    {
        const std::string name = "flow_test_random_";
        Standard::Algorithms::Utilities::random_t<std::int32_t> rnd{};
        std::vector<std::vector<test_edge_t>> graph;

        constexpr auto max_attempts = 1;

        for (std::int32_t att{}; att < max_attempts; ++att)
        {
            constexpr auto id1 = -10;
            constexpr auto id2 = -138;

            auto source = id1;
            auto sink = id2;

            try
            {
                graph.clear();

                constexpr auto n_min = 2;
                constexpr auto cnt = 10;

                graph = Standard::Algorithms::Graphs::create_random_graph_weighted<test_edge_t, weight_t, std::int32_t,
                    max_size, true, n_min>(1, cnt)
                            .first;

                const auto size = static_cast<std::int32_t>(graph.size());
                assert(n_min <= size);

                source = rnd(0, size - 1);

                do
                {
                    sink = rnd(0, size - 1);
                } while (source == sink);

                test_flow_algs(name, source, graph, sink, 0, {});
            }
            catch (const std::exception &exc)
            {
                auto str = ::Standard::Algorithms::Utilities::w_stream();
                str << exc.what() << " attempt " << att << ", source " << source << ", sink " << sink << "\n";
                ::Standard::Algorithms::print("Graph", graph, str);

                throw std::runtime_error(str.str());
            }
        }
    }

    constexpr void min_cost_test()
    {
        constexpr auto size = 5;

        Standard::Algorithms::Graphs::min_cost_max_flow_graph<weight_t, cost_t, max_size, edge_t> graph;

        graph.init(size, false);

        // NOLINTNEXTLINE
        graph.add_edge(1, 2, 10, 100);
        // NOLINTNEXTLINE
        graph.add_edge(1, 4, 7, 10);
        // NOLINTNEXTLINE
        graph.add_edge(2, 3, 20, 1'000);
        // NOLINTNEXTLINE
        graph.add_edge(3, 4, 30, 10'000);

        const std::string name = "min_cost_max_flow test";
        const auto mfc = graph.min_cost_max_flow(
            // NOLINTNEXTLINE
            1, 4, 9);

        ::Standard::Algorithms::ert::are_equal(
            // NOLINTNEXTLINE
            std::make_pair(9, cost_t(22270)), mfc, name + " flow_cost");

        const std::vector<std::vector<test_edge_t>> expected_flows{
            {},
            // NOLINTNEXTLINE
            {
                test_edge_t(2, 2),
                test_edge_t(4, 7),
            },
            {
                test_edge_t(3, 2),
            },
            {
                test_edge_t(4, 2),
            },
            {},
        };

        check_edge_flows(name, expected_flows, graph.graph(), size);
    }
} // namespace

void Standard::Algorithms::Graphs::Tests::max_flow_tests()
{
    flow_test1();
    flow_test2();
    flow_test_random();
    min_cost_test();
}
