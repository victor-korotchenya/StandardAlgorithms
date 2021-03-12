#include "detect_many_cycles_tests.h"
#include "detect_many_cycles.h"
#include "../test_utilities.h"

using namespace std;

namespace
{
    using vertex_t = int;

    void add(vector<vector<vertex_t>>& graph, int a, int b)
    {
        assert(a != b);

        graph[a].push_back(b);
        graph[b].push_back(a);
    }

    vector<vector<vertex_t>> build_graph()
    {
        vector<vector<vertex_t>> graph(10);
        // 0-1-2-3-0 cycle
        // 2-4
        // 4-5-6-4 cycle
        // 6-7-8-9
        for (auto i = 0; i <= 3; ++i)
        {
            const auto ot = (i + 1) % 4;
            add(graph, i, ot);
        }
        add(graph, 2, 4);
        for (auto i = 0; i <= 2; ++i)
        {
            {
                const auto a = 4 + i;
                const auto b = (i + 1) % 3 + 4;
                add(graph, a, b);
            }

            const auto a = 6 + i;
            const auto b = a + 1;
            add(graph, a, b);
        }

        return graph;
    }
}

void Privet::Algorithms::Graphs::Tests::detect_many_cycles_tests()
{
    const auto graph = build_graph();

    vector<vector<int>> cycles;
    vector<int> depths, st;
    detect_many_cycles(graph, cycles, depths, st, 0);

    const vector<vector<int>> expected = {
        { 0 , 1 , 2 , 3 },
        { 4 , 5 , 6 },
    };
    Assert::AreEqual(expected, cycles, "cycles");
}