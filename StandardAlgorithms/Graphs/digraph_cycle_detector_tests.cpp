#include <unordered_set>
#include "../test_utilities.h"
#include "../Utilities/PrintUtilities.h"
#include "../Utilities/ExceptionUtilities.h"
#include "digraph_cycle_detector.h"
#include "digraph_cycle_detector_tests.h"
#include "graph_helper.h"

using namespace std;
using namespace Privet::Algorithms::Graphs;
using namespace Privet::Algorithms::Tests;
using namespace Privet::Algorithms;

namespace
{
    void demand_cycle_exists(const vector<vector<int>>& graph,
        const vector<int>& cycle)
    {
        const auto size = static_cast<int>(cycle.size());
        if (!size)
            return;

        RequireGreater(cycle.size(), 1u, "The cycle size cannot be 1.");

        for (auto i = 1; i < size; ++i)
            demand_edge_exists(graph, cycle[i - 1], cycle[i]);

        demand_edge_exists(graph, cycle.back(), cycle.front());
    }

    struct test_case final : base_test_case
    {
        vector<vector<int>> graph;
        vector<int> cycle;

        test_case(string&& name = {},
            vector<vector<int>>&& graph = {},
            vector<int>&& cycle = {})
            : base_test_case(forward<string>(name)),
            graph(forward<vector<vector<int>>>(graph)),
            cycle(forward<vector<int>>(cycle))
        {
        }

        void Validate() const override
        {
            base_test_case::Validate();

            const auto size = static_cast<int>(graph.size());
            RequirePositive(size, "graph size.");
            Assert::GreaterOrEqual(size, cycle.size(), "graph size vs. cycle size.");

            require_simple_graph(graph);
            demand_cycle_exists(graph, cycle);

            unordered_set<int> uniques;
            check_vertices(size, cycle, "cycle", uniques);
        }

        void Print(ostream& str) const override
        {
            base_test_case::Print(str);

            Privet::Algorithms::Print("graph", graph, str);
            Privet::Algorithms::Print("cycle", cycle, str);
        }
    };

    void generate_test_cases(vector<test_case>& tests)
    {
        tests.push_back({
            "5-cycle",
            {
                { 1 },
                { 2 },
                { 3 },
                { 4 },
                { 5 },
                { 1 },
            },
            { 1,2,3,4,5 },
            });

        tests.push_back({
            "no-cycle",
            {
                { 1 },
                { 2,3 },
                { 4 },
                { 2,4 },
                {},
                {},
            },
            {},
            });

        tests.push_back({
            "2-cycle on 4 nodes",
            {
                { 1 },
                {},
                { 0,3 },
                { 2 },
                { 1 }
            },
            { 2,3 },
            });
    }

    void run_test_case(const test_case& test)
    {
        const auto actual = digraph_cycle_detector(test.graph);
        Assert::AreEqual(test.cycle, actual, "digraph_cycle_detector");
    }
}

void Privet::Algorithms::Graphs::Tests::digraph_cycle_detector_tests()
{
    test_utilities<test_case>::run_tests(run_test_case, generate_test_cases);
}