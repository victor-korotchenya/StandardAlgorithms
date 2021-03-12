#include <stdexcept>
#include <unordered_map>
#include <utility>
#include "max_flow_tests.h"
#include "flow_edge.h"
#include "Edmonds_Karp_flow.h"
#include "dinic_flow_old.h"
#include "PushRelabelMaxFlow.h"
#include "max_flow_graph.h"
#include "min_cost_max_flow_graph.h"
#include "random_graph_utilities.h"
#include "../Utilities/Random.h"
#include "../test_utilities.h"

using namespace std;
using namespace Privet::Algorithms::Graphs;
using namespace Privet::Algorithms;

namespace
{
    using weight_t = int;
    using cost_t = int64_t;
    using test_edge_t = WeightedVertex<int, weight_t>;

    // Only in tests, use cost_flow_edge instead of flow_edge
    using edge_t = cost_flow_edge<weight_t, cost_t>;
    constexpr auto n_max = 20;

    //Add empty reverse edges if not present.
    vector<vector<edge_t>> to_flow_graph(const vector<vector<test_edge_t>>& graph)
    {
        vector<vector<edge_t>> result(graph.size());
        vector<unordered_map<int, int>> edge_map(graph.size());

        {
            int i = 0;
            for (const auto& v : graph)
            {
                int j = 0;
                for (const auto& e : v)
                {
                    result[i].emplace_back(e.Vertex, 0, e.Weight);
                    const auto it = edge_map[i].insert(make_pair(e.Vertex, j));
                    if (!it.second)
                        throw std::runtime_error("The flow graph has multi-edges.");

                    ++j;
                }

                ++i;
            }
        }
        {
            int i = 0;
            for (auto& v : result)
            {
                int j = 0;
                for (auto& e : v)
                {
                    auto it = edge_map[e.to].find(i);
                    if (edge_map[e.to].end() == it)
                    {//Add the missing back edge.
                        e.rev_edge_index = static_cast<int>(result[e.to].size());
                        result[e.to].push_back({ i, j });
                    }
                    else
                    {
                        e.rev_edge_index = it->second;
                    }
                    ++j;
                }
                ++i;
            }
        }

        return result;
    }

    template<class graph_t>
    void add_adges(const vector<vector<test_edge_t>>& source, graph_t& aim)
    {
        const auto size = static_cast<int>(source.size());
        assert(size > 0);

        for (auto i = 0; i < size; ++i)
            for (const auto& e : source[i])
                aim.add_edge(i, e.Vertex, e.Weight);
    }

    //It is needed to compare the results.
    template<class graph_t>
    void remove_empty_flow_edges(graph_t& graph, const int size)
    {
        for (auto i = 0; i < size; ++i)
        {
            auto& edges = graph[i];
            for (auto j = 0; j < static_cast<int>(edges.size());)
            {
                if (edges[j].flow <= 0)
                {
                    std::swap(edges[j], edges[edges.size() - 1llu]);
                    edges.pop_back();
                }
                else
                    ++j;
            }
        }
    }

    inline bool CompareEdges(const test_edge_t* const expected, const edge_t* const actual)
    {
        const auto r = (actual->to == expected->Vertex) && (actual->flow == expected->Weight);
        return r;
    }

    template<class graph_t>
    void check_edge_flows(const string& name, const vector<vector<test_edge_t>>& expected_flows, graph_t& graph, const int size)
    {
        assert(size > 0);
        remove_empty_flow_edges(graph, size);

        using TLambda = bool(*)(const test_edge_t* const expected, const edge_t* const actual);

        auto fullName = name + ", flow at row ";
        const auto fullName_size = fullName.size();

        for (int i = 0; i < static_cast<int>(size); i++)
        {
            fullName.resize(fullName_size);
            fullName += to_string(i);

            auto& actual = graph[i];
            sort(actual.begin(), actual.end(),
                [](const edge_t& e1, const edge_t& e2) -> bool {
                    return e1.to < e2.to;
                });

            const auto& expected = expected_flows[i];
            assert(is_sorted(expected.begin(), expected.end(), [](const test_edge_t& a, const test_edge_t& b) {
                return a.Vertex < b.Vertex || a.Vertex == b.Vertex && a.Weight < b.Weight;
                }));

            Assert::AreEqual<test_edge_t, edge_t, TLambda>(expected, actual, fullName.c_str(), CompareEdges);
        }
    }

    template<class run_lambda>
    weight_t run_alg(const string& name,
        const vector<vector<test_edge_t>>& graph,
        const weight_t expectedFlow,
        const vector<vector<test_edge_t>>& expected_flows,
        run_lambda& lam,
        const bool has_flow)
    {
        auto graph2 = to_flow_graph(graph);
        const auto size = static_cast<int>(graph.size());

        const auto actual_flow = lam(graph2);
        if (has_flow)
        {
            Assert::AreEqual(expectedFlow, actual_flow, name + " actual flow");

            // The actual flow might pass through different edges.
            check_edge_flows(name, expected_flows, graph2, size);
        }

        return actual_flow;
    }

    void test_flow_algs(
        const string& name,
        const vector<vector<test_edge_t>>& graph,
        const int sourceIndex,
        const int sink,
        weight_t expectedFlow,
        const vector<vector<test_edge_t>>& expected_flows)
    {
        const auto size = static_cast<int>(graph.size());
        assert(size > 0);

        const auto has_flow = expectedFlow > 0;
        {
            //vector<vector<test_edge_t>> actualFlows;

            //const auto actual_flow = PushRelabelMaxFlow<weight_t, test_edge_t>
            //    ::Calc(graph, sourceIndex, sink, actualFlows);

            //if (has_flow)
            //{
            //    Assert::AreEqual(expectedFlow, actual_flow, name + " Push-Relabel Total flow");
            //TODO: p1. finish. Assert::AreEqual(expected_flows, actualFlows, "Flows");
            //}
            //else
            //    expectedFlow = actual_flow;
        }
        {
            auto lam = [sourceIndex, sink](vector<vector<edge_t>>& graph2) {
                const auto fl = Edmonds_Karp_flow<weight_t>(sourceIndex, sink, graph2);
                return fl;
            };

            const auto actual_flow = run_alg(name + " Edmonds-Karp", graph, expectedFlow, expected_flows, lam, has_flow);
            if (!has_flow)
                expectedFlow = actual_flow;
        }
        {
            auto lam = [sourceIndex, sink](vector<vector<edge_t>>& graph2) {
                const auto actual_flow = dinic_flow_old<weight_t>(sourceIndex, sink, graph2);
                return actual_flow;
            };

            run_alg(name + " Dinic", graph, expectedFlow, expected_flows, lam, has_flow);
        }
        {
            max_flow_graph<weight_t, n_max, edge_t> graph2(sourceIndex, sink, size);
            add_adges(graph, graph2);

            const auto f4 = graph2.max_flow();
            const auto name2 = name + " max_flow_graph";
            Assert::AreEqual(expectedFlow, f4, name2 + " flow");

            if (has_flow)
                check_edge_flows(name2, expected_flows, graph2.get_graph(), size);
        }
        {
            min_cost_max_flow_graph<weight_t, cost_t, n_max, edge_t> graph2;
            graph2.init(size, false);
            add_adges(graph, graph2);

            const auto name2 = name + " min_cost_max_flow_graph";
            const auto fc = graph2.min_cost_max_flow(sourceIndex, sink);
            Assert::AreEqual(expectedFlow, fc.first, name2 + " flow");

            if (has_flow)
                check_edge_flows(name2, expected_flows, graph2.get_graph(), size);
        }
    }

    void flow_test1()
    {
        const string name = "test1 ";

        //                 A0     (4 ->)      D3
        //      (10 />)                               (10) \
                //                                    ^             v
// B1           (2) |     (8) \   (6) |                   F5
//                  v          v                    ^
//                                                 /
//      (10 \>)                               (10)
//                  C2    (9 ->)      E4
//
        vector<vector<test_edge_t>> graph(6);
        graph[0].push_back(test_edge_t(2, 2));
        graph[0].push_back(test_edge_t(3, 4));
        graph[0].push_back(test_edge_t(4, 8));

        graph[1].push_back(test_edge_t(0, 10));
        graph[1].push_back(test_edge_t(2, 10));

        graph[2].push_back(test_edge_t(4, 9));

        graph[3].push_back(test_edge_t(5, 10));

        graph[4].push_back(test_edge_t(3, 6));
        graph[4].push_back(test_edge_t(5, 10));

        constexpr int sourceIndex = 1;
        constexpr int sink = 5;
        constexpr weight_t expectedFlow = 19;

        vector<vector<test_edge_t>> expected_flows(graph.size());
        expected_flows[0].push_back(test_edge_t(3, 4));
        expected_flows[0].push_back(test_edge_t(4, 6));

        expected_flows[1].push_back(test_edge_t(0, 10));
        expected_flows[1].push_back(test_edge_t(2, 9));

        expected_flows[2].push_back(test_edge_t(4, 9));

        expected_flows[3].push_back(test_edge_t(5, 9));

        expected_flows[4].push_back(test_edge_t(3, 5));
        expected_flows[4].push_back(test_edge_t(5, 10));

        test_flow_algs(name, graph,
            sourceIndex, sink, expectedFlow,
            expected_flows);
    }

    void flow_test2()
    {
        const string name = "test2 ";

        vector<vector<test_edge_t>> graph(10);
        //S..T map to 8..9
        //A..F map to 1..6
        constexpr int sourceIndex = 8;
        constexpr int sink = 9;
        constexpr weight_t expectedFlow = 3;

        graph[sourceIndex].push_back(test_edge_t(1, 2));
        graph[sourceIndex].push_back(test_edge_t(3, 25));//too much.

        graph[1].push_back(test_edge_t(2, 1));
        graph[1].push_back(test_edge_t(5, 1));
        graph[1].push_back(test_edge_t(sink, 1));

        graph[2].push_back(test_edge_t(sink, 1));
        graph[3].push_back(test_edge_t(4, 1));
        graph[4].push_back(test_edge_t(2, 1));
        graph[4].push_back(test_edge_t(3, 50));//unused.
        graph[5].push_back(test_edge_t(6, 1));
        graph[6].push_back(test_edge_t(sink, 1));

        vector<vector<test_edge_t>> expected_flows(graph.size());
        expected_flows[sourceIndex].push_back(test_edge_t(1, 2));
        expected_flows[sourceIndex].push_back(test_edge_t(3, 1));

        expected_flows[1].push_back(test_edge_t(5, 1));
        expected_flows[1].push_back(test_edge_t(sink, 1));

        expected_flows[2].push_back(test_edge_t(sink, 1));
        expected_flows[3].push_back(test_edge_t(4, 1));
        expected_flows[4].push_back(test_edge_t(2, 1));
        expected_flows[5].push_back(test_edge_t(6, 1));
        expected_flows[6].push_back(test_edge_t(sink, 1));

        test_flow_algs(name, graph,
            sourceIndex, sink, expectedFlow,
            expected_flows);
    }

    void flow_test_random()
    {
        const string name = "flow_test_random_";
        IntRandom r;
        vector<vector<test_edge_t>> graph;
        int source, sink;

        for (auto att = 0, att_max = 1; att < att_max; att++)
        {
            try
            {
                graph.clear();
                constexpr auto n_min = 2;
                graph = create_random_graph_weighted<test_edge_t, weight_t, int, n_max, true, n_min>(1, 10).first;

                const auto size = static_cast<int>(graph.size());
                assert(size >= n_min);

                source = r(0, size - 1);

                do sink = r(0, size - 1);
                while (source == sink);

                test_flow_algs(name, graph, source, sink, 0, {});
            }
            catch (const exception& e)
            {
                stringstream ss;
                ss << e.what() << " attempt " << att
                    << ", source " << source << ", sink " << sink << "\n";
                Print("Graph", graph, ss);
                throw runtime_error(ss.str());
            }
        }
    }

    void min_cost_test()
    {
        constexpr auto size = 5;

        min_cost_max_flow_graph<weight_t, cost_t, n_max, edge_t> graph;
        graph.init(size, false);
        graph.add_edge(1, 2, 10, 100);
        graph.add_edge(1, 4, 7, 10);
        graph.add_edge(2, 3, 20, 1000);
        graph.add_edge(3, 4, 30, 10000);

        const auto name = "min_cost_max_flow test"s;
        const auto fc = graph.min_cost_max_flow(1, 4, 9);
        Assert::AreEqual(make_pair(9, cost_t(22270)), fc, name + " flow_cost");

        const vector<vector<test_edge_t>> expected_flows{
             {},
             {test_edge_t(2, 2), test_edge_t(4, 7),},
             {test_edge_t(3, 2),},
             {test_edge_t(4, 2),},
             {},
        };
        check_edge_flows(name, expected_flows, graph.get_graph(), size);
    }
}

void Privet::Algorithms::Graphs::Tests::max_flow_tests()
{
    flow_test1();
    flow_test2();
    flow_test_random();
    min_cost_test();
}