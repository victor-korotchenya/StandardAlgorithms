#include "page_rank.h"
#include "page_rank_tests.h"
#include "../test_utilities.h"
#include "../Utilities/PrintUtilities.h"

using namespace std;
using namespace Privet::Algorithms::Tests;
using namespace Privet::Algorithms::Graphs;
using namespace Privet::Algorithms;

namespace
{
    using edge_t = int;
    using float_number_t = double;
    using graph_t = vector<vector<edge_t>>;

    constexpr float_number_t epsilon = 1.0e-6;

    struct TestCase final : base_test_case
    {
        const graph_t graph;
        const float_number_t damping_factor;
        const vector<float_number_t> expected;

        TestCase(string&& name,
            graph_t&& graph,
            const float_number_t& damping_factor,
            vector<float_number_t>&& expected)
            : base_test_case(forward<string>(name)),
            graph(forward<graph_t>(graph)),
            damping_factor(damping_factor),
            expected(forward<vector<float_number_t>>(expected))
        {
        }

        void Print(ostream& str) const override
        {
            base_test_case::Print(str);
            AppendSeparator(str);
            PrintValue(str, "damping_factor", damping_factor);
            //todo:  ::PrintValue(str, "graph", graph.cbegin(), graph.cend());

            AppendSeparator(str);
            ::Print("expected", expected.cbegin(), expected.cend(), str);
        }

        void Validate() const override
        {
            base_test_case::Validate();
            ThrowIfEmpty(graph, "graph");
            ThrowIfEmpty(expected, "expected");
            Assert::AreEqual(graph.size(), expected.size(), "size");
        }
    };

    void GenerateTestCases(vector<TestCase>& testCases)
    {
        constexpr auto damping_factor = 0.8;
        constexpr auto nodes = 4;
        constexpr auto rest = (1 - damping_factor) / nodes;

        testCases.push_back({ "Simple damping_factor",
          { {}, {0, 2}, {0}, {0,1,2}, },
          damping_factor,
          { 1 - rest * 3, rest, rest, rest,} });
        // b -------> a,  d -> {a,b,c}
        // \          ^
        //  \--->c---/
        testCases.push_back({ "Simple1",
          { {}, {0, 2}, {0}, {0,1,2}, },
          1,
          { 1,0,0, 0,} });
    }

    void RunTestCase(const TestCase& testCase)
    {
        vector<float_number_t> buffer, actual;

        page_rank_simple(testCase.graph, buffer, actual, epsilon, testCase.damping_factor);
        Assert::AreEqualWithEpsilon(testCase.expected, actual, "ranks");
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
                void page_rank_tests()
                {
                    test_utilities<TestCase>::run_tests(RunTestCase, GenerateTestCases);
                }
            }
        }
    }
}