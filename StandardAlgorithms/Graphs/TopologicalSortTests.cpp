#include "../test_utilities.h"
#include "../Utilities/PrintUtilities.h"
#include "TopologicalSort.h"
#include "TopologicalSortTests.h"

using namespace std;
using namespace Privet::Algorithms::Graphs;
using namespace Privet::Algorithms::Tests;
using namespace Privet::Algorithms;

namespace
{
    using vertices_t = vector<int>;
    using graph_t = vector<vertices_t>;

    struct TestCase final : base_test_case
    {
        graph_t graph;
        vertices_t expected_indexes;

        TestCase(
            string&& name = "",
            graph_t&& graph = {},
            vertices_t&& expected_indexes = {})
            : base_test_case(forward<string>(name)),
            graph(forward<graph_t>(graph)),
            expected_indexes(forward<vertices_t>(expected_indexes))
        {
        }

        void Validate() const override
        {
            base_test_case::Validate();

            RequirePositive(graph.size(), "graph size.");

            Assert::AreEqual(graph.size(), expected_indexes.size(),
                "expected_indexes size.");
        }

        void Print(ostream& str) const override
        {
            base_test_case::Print(str);

            Privet::Algorithms::Print("graph", graph, str);
            Privet::Algorithms::Print("expected_indexes", expected_indexes, str);
        }
    };

    void GenerateTestCases(vector<TestCase>& testCases)
    {
        testCases.push_back({
          "simple2",
          {
            {1,2},
            {3},
            {},
            {},
            {},
            {},
          },
          {0,4,5,1,2,3},
            });
        testCases.push_back({
          "simple",
          {
            {1},
            {2},
            {},
          },
          {0,1,2},
            });
        testCases.push_back({
          "trivial graph",
          {
            {},
          },
          {0},
            });
        testCases.push_back({
          "one edge",
          {
            {1},
            {},
          },
          {0,1},
            });
        testCases.push_back({
          "one edge reverted",
          {
            {},
            {0},
          },
          {1,0},
            });
    }

    void RunTestCase(const TestCase& testCase)
    {
        const auto actual = topological_sort(testCase.graph, static_cast<int>(testCase.graph.size()));
        Assert::AreEqual(testCase.expected_indexes, actual, "cycle");
    }
}

namespace Privet
{
    namespace Algorithms
    {
        namespace Graphs
        {
            namespace Tests
            {
                void TopologicalSortTests()
                {
                    test_utilities<TestCase>::run_tests(RunTestCase, GenerateTestCases);
                }
            }
        }
    }
}