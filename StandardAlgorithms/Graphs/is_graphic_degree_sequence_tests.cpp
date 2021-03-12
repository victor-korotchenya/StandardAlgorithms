#include <algorithm>
#include <unordered_set>
#include "../test_utilities.h"
#include "../Utilities/PrintUtilities.h"
#include "is_graphic_degree_sequence.h"
#include "is_graphic_degree_sequence_tests.h"
#include "graph_helper.h"

using namespace std;
using namespace Privet::Algorithms::Graphs;
using namespace Privet::Algorithms::Tests;
using namespace Privet::Algorithms;

namespace
{
    struct TestCase final : base_test_case
    {
        vector<int> degree_sorted_desc;
        vector<vector<int>> graph;
        bool expected;

        TestCase(string&& name = {},
            vector<int>&& degree_sorted_desc = {},
            vector<vector<int>>&& graph = {}, bool expected = false)
            : base_test_case(forward<string>(name)),
            degree_sorted_desc(forward<vector<int>>(degree_sorted_desc)),
            graph(forward<vector<vector<int>>>(graph)),
            expected(expected)
        {
        }

        void Validate() const override
        {
            base_test_case::Validate();
            if (!expected) return;

            const auto size = static_cast<int>(graph.size());
            if (!size) return;

            RequirePositive(size, "graph size.");
            require_simple_graph(graph);

            vector<int> degrees(size);
            transform(this->graph.begin(),
                this->graph.end(),
                degrees.begin(),
                [](const vector<int>& v) -> int
                {
                    return static_cast<int>(v.size());
                });

            sort(degrees.begin(), degrees.end(), greater<int>());
            Assert::AreEqual(degree_sorted_desc, degrees, "degree_sorted_desc");
        }

        void Print(ostream& str) const override
        {
            base_test_case::Print(str);

            Privet::Algorithms::Print("degree_sorted_desc", degree_sorted_desc, str);
            Privet::Algorithms::Print("graph", graph, str);
            PrintValue(str, "expected", expected);
        }
    };

    void GenerateTestCases(vector<TestCase>& testCases)
    {
        testCases.push_back({ "Book ex2",{3, 3, 3, 3, 3, 3},{{1,4,3},{0,2,3},{5,1,4},{5,1,0},{5,0,2},{4,3,2}}, true });
        testCases.push_back({ "Book ex1",{6, 6, 5, 4, 3, 3, 1},{}, false });
        testCases.push_back({ "P2-4",{4,1,1},{}, false });
        testCases.push_back({ "P2",{2,1,1},{{2,1},{0},{0}}, true });
        testCases.push_back({ "pair",{1,1},{{1},{0}}, true });
        testCases.push_back({ "three",{1,1,1},{},false });
        testCases.push_back({ "empty", {}, {}, true });
        testCases.push_back({ "trivial", {0}, {{}}, true });
        testCases.push_back({ "not trivial", {1}, {}, false });
        testCases.push_back({ "not trivial 2", {2}, {}, false });
    }

    void RunTestCase(const TestCase& testCase)
    {
        vector<vector<int>> graph;

        const auto actual = is_graphic_degree_sequence(testCase.degree_sorted_desc, graph);

        Assert::AreEqual(testCase.expected, actual, "result");
        if (!actual) return;
        Assert::AreEqual(testCase.graph, graph, "graph");
    }
}

void Privet::Algorithms::Graphs::Tests::is_graphic_degree_sequence_tests()
{
    test_utilities<TestCase>::run_tests(RunTestCase, GenerateTestCases);
}