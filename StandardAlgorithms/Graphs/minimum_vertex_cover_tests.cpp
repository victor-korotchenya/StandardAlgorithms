#include "minimum_vertex_cover.h"
#include "minimum_vertex_cover_tests.h"
#include "../test_utilities.h"
#include "../Utilities/PrintUtilities.h"
#include "graph_helper.h"

using namespace std;
using namespace Privet::Algorithms::Graphs::Tests;
using namespace Privet::Algorithms::Graphs;
using namespace Privet::Algorithms::Tests;
using namespace Privet::Algorithms;

namespace
{
    using result_t = vector<int>;
    using graph_t = vector<result_t>;

    struct test_case final : base_test_case
    {
        const graph_t graph;
        const result_t expected;
        const int edge_count;

        test_case(string&& name,
            graph_t&& graph,
            result_t&& expected,
            const int edge_count)
            : base_test_case(forward<string>(name)),
            graph(forward<graph_t>(graph)), expected(forward<result_t>(expected)), edge_count(edge_count)
        {
            require_simple_graph(graph);
        }

        void Print(ostream& str) const override
        {
            base_test_case::Print(str);
            ::Print(", graph=", graph, str);
            ::Print(", expected=", expected, str);
            str << " edge_count=" << edge_count;
        }
    };

    void generate_test_cases(vector<test_case>& testCases)
    {
        {
            constexpr auto edge_count = 2;
            //1 - 2
            //    |
            //    3
            testCases.emplace_back("1-2-3 path", graph_t{
                {},
                { 2 },
                { 1, 3 },
                { 2 },
                }, result_t{ 2 }, edge_count);
        }
        {
            constexpr auto edge_count = 5;
            //0 - 1
            //| \ |
            //2 - 3
            testCases.emplace_back("K4\\{1-2}", graph_t{
                { 1, 2, 3 },
                { 0, 3 },
                { 0, 3 },
                { 1, 2 },
                }, result_t{ 0, 3, }, edge_count);
        }

        for (auto node_count = 2; node_count <= 5; ++node_count)
        {
            const auto edge_count = node_count * (node_count - 1) >> 1;

            graph_t graph(node_count);
            result_t expected;
            for (auto i = 0; i < node_count - 1; ++i)
            {
                expected.push_back(i);
                for (auto j = i + 1; j < node_count; ++j)
                {
                    graph[i].push_back(j),
                        graph[j].push_back(i);
                }
            }

            assert(expected.size() + 1 == node_count);
            testCases.emplace_back("K" + to_string(node_count), move(graph), move(expected), edge_count);
        }
    }

    void run_test_case(const test_case& test)
    {
        result_t actual;
        vector<vector<bool>> vis;
        minimum_vertex_cover<graph_t, unsigned>(test.graph, test.edge_count, actual, vis);

        Assert::AreEqual(test.expected, actual, "minimum_vertex_cover");
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
                void minimum_vertex_cover_tests()
                {
                    test_utilities<test_case>::run_tests<0>(run_test_case, generate_test_cases);
                }
            }
        }
    }
}