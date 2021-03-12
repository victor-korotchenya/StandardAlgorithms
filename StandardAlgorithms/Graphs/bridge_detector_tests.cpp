#include "../test_utilities.h"
#include "../Utilities/PrintUtilities.h"
#include "../Utilities/ExceptionUtilities.h"
#include "bridge_detector.h"
#include "bridge_detector_tests.h"
#include "graph_helper.h"
#include "../Utilities/VectorUtilities.h"
#include "random_graph_utilities.h"

using namespace std;
using namespace Privet::Algorithms::Graphs;
using namespace Privet::Algorithms::Tests;
using namespace Privet::Algorithms;

namespace
{
    template<class t>
    void sort_pairs(vector<pair<t, t>>& pairs, const bool min_first = true)
    {
        if (min_first)
        {
            for (auto& p : pairs)
            {
                if (p.second < p.first)
                    swap(p.second, p.first);
            }
        }

        sort(pairs.begin(), pairs.end());
    }

    struct test_case final : base_test_case
    {
        vector<vector<int>> graph;
        vector<pair<int, int>> bridges;
        vector<int> cut_nodes;
        bool is_computed;

        test_case(string&& name,
            vector<vector<int>>&& graph,
            vector<pair<int, int>>&& bridges,
            vector<int>&& cut_nodes,
            bool is_computed = true)
            : base_test_case(forward<string>(name)),
            graph(forward<vector<vector<int>>>(graph)),
            bridges(forward<vector<pair<int, int>>>(bridges)),
            cut_nodes(forward<vector<int>>(cut_nodes)),
            is_computed(is_computed)
        {
            sort_pairs(this->bridges);
            VectorUtilities::remove_duplicates(this->bridges);
            VectorUtilities::sort_remove_duplicates(this->cut_nodes);
        }

        void Validate() const override
        {
            base_test_case::Validate();

            const auto size = static_cast<int>(graph.size());
            RequirePositive(size, "graph size.");

            require_simple_graph(graph);
            require_undirected_graph(graph);

            // There can be at most n-1 bridges in a simple graph.
            Assert::Greater(size, bridges.size(), "graph size vs. bridges size.");

            const string bridges_name("bridges");
            for (const auto& p : bridges)
            {
                Assert::NotEqual(p.first, p.second, "Self loops not allowed.");
                demand_edge_exists(graph, p.first, p.second, bridges_name);
            }

            if (cut_nodes.empty())
                return;

            Assert::GreaterOrEqual(cut_nodes[0], 0, "0-th cut node");
            Assert::Greater(size, cut_nodes.back(), "last cut node");
        }

        void Print(ostream& str) const override
        {
            base_test_case::Print(str);

            ::Print("graph", graph, str);
            ::Print("bridges", bridges, str);
            ::Print("cut_nodes", cut_nodes, str);
            PrintValue(str, "is_computed", is_computed);
        }
    };

    void generate_test_cases(vector<test_case>& tests)
    {
        tests.push_back({
            "K1",
            {
                {},
            },
            {},
            {}
            });

        tests.push_back({
            "K2",
            {
                { 1 },
                { 0 },
            },
            { { 0,1 } },
            {}
            });

        tests.push_back({
            "K1, two K2",
            {
                { 1 },//0
                { 0 },
                {},//2
                { 4 },
                { 3 },//4
            },
            { { 0,1 },{ 3,4 } },
            {}
            });

        tests.push_back({
            "0-1-2",
            {
                { 1 },
                { 0,2 },
                { 1 },
            },
            { { 0,1 },{ 1,2 }, },
            { 1 }
            });

        tests.push_back({
            "1-0-2",
            {
                { 1,2 },
                { 0 },
                { 0 },
            },
            { { 0,1 },{ 0,2 }, },
            { 0 }
            });

        tests.push_back({
            "K3",
            {
                { 1,2 },
                { 0,2 },
                { 1,0 },
            },
            {},
            {},
            });

        tests.push_back({
            "K4 - K1",
            { { 2, 3, 1 },
            { 2, 0 },
            { 0, 1, 3 },
            { 0, 2 } },
            {},
            {}
            });

        tests.push_back({
            "6-cycle",
            {//0-1-3-2-4-5-0
                { 1,5 },//0
                { 3,0 },
                { 4,3 },//2
                { 1,2 },
                { 5,2 },//4
                { 0,4 },
            },
            {},
            {},
            });

        tests.push_back({
            "edge + 5-cycle",
            {// 0 - 1-2-3-4-5-1
                { 1 },//0
                { 2,0,5 },
                { 3,1 },//2
                { 4,2 },
                { 5,3 },//4
                { 1,4 },
            },
            {{0,1}},
            {1}
            });

        tests.push_back({
            "7 tree",
            {// 0-2-1
             //   |
             // 4-5  3-2-6
                { 2 },//0
                { 2 },
                { 0,1,3,6,5 },//2
                { 2 },
                { 5 },//4
                { 2,4 },
                { 2 },//6
            },
            { { 0,2 },{ 1,2 },{ 2,5 },{ 4,5 },{ 2,3 },{ 2,6 }, },
            { 2,5 }
            });

        tests.push_back({
            "Two touching K3",
            {// 2-0-1-2
             // |
             // 2-3-4-2
                { 1,2 },//0
                { 0,2 },
                { 0,1,3,4 },//2
                { 2,4 },
                { 2,3 },//4
            },
            {},
            { 2 },
            });

        tests.push_back({
            "base1 undirected",
            {
                { },//0
                { 2,3 },
                { 1,3,4 },//2
                { 2,4,5,1 },
                { 2,3 },//4
                { 3},
                // 0 1-2-4-3-1
                //     |
                //     3-5
            },
            {{3,5}},
            {3}
            });

        for (auto att = 0, att_max = 1; att < att_max; ++att)
        {
            auto graph_edges = create_random_graph<int, 10>();
            tests.emplace_back("random" + to_string(att), move(graph_edges.first), vector<pair<int, int>>(), vector<int>(), false);
        }
    }

    void run_test_case(const test_case& test)
    {
        auto actual = bridge_detector(test.graph);
        auto& bridges = actual.first;
        sort_pairs(bridges);

        auto cut_nodes = actual.second;
        sort(cut_nodes.begin(), cut_nodes.end());

        if (test.is_computed)
        {
            Assert::AreEqual(test.bridges, bridges, "bridge_detector");
            Assert::AreEqual(test.cut_nodes, cut_nodes, "cut_node_detector");
        }

        {
            auto bridge_slow = bridge_detector_slow(test.graph);
            sort_pairs(bridge_slow);
            Assert::AreEqual(bridges, bridge_slow, "bridge_detector_slow");
        }

        auto cut_nodes_slow = cut_node_detector_slow(test.graph);
        sort(cut_nodes_slow.begin(), cut_nodes_slow.end());

        Assert::AreEqual(cut_nodes, cut_nodes_slow, "cut_node_detector_slow");
    }
}

void Privet::Algorithms::Graphs::Tests::bridge_detector_tests()
{
    test_utilities<test_case>::run_tests(run_test_case, generate_test_cases);
}