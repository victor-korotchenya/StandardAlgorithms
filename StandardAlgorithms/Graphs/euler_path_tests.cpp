#include "euler_path.h"
#include "euler_path_tests.h"
#include "../test_utilities.h"

using namespace std;
using namespace Privet::Algorithms::Tests;
using namespace Privet::Algorithms::Trees;
using namespace Privet::Algorithms;

namespace
{
    struct edge_t final
    {
        int to, edge_number;

        explicit edge_t(int to = 0, int edge_number = 0)
            : to(to), edge_number(edge_number)
        {
        }

        explicit operator int() const
        {
            return to;
        }
    };

    void add_edge(vector<vector<edge_t>>& graph, int& edge_count, const int from, const int to)
    {
        const auto size = static_cast<int>(graph.size());
        assert(from >= 0 && from < size&& to >= 0 && to < size);

        ++edge_count;
        graph[from].emplace_back(to, edge_count);
        graph[to].emplace_back(from, edge_count);
    }
}

tuple<vector<vector<edge_t>>, int> get_graph()
{
    const vector<vector<int>> source_graph{
        { 1,3,4,5 },
        { 2 },//1
        { 3 },
        {},//3
        {},
        {},//5
    };

    const auto size = source_graph.size();
    vector<vector<edge_t>> graph(size);

    auto edge_count = 0;
    for (auto from = 0; from < size; ++from)
    {
        for (const auto& to : source_graph[from])
            add_edge(graph, edge_count, from, to);
    }

    return { graph, edge_count };
}

void Privet::Algorithms::Trees::Tests::euler_path_tests()
{
    const auto graph_count = get_graph();
    const auto& graph = get<0>(graph_count);
    {
        auto g = graph;
        const auto actual_directed = euler_path_directed(g);
        const vector<int> expected_directed{ 0,5,0,4,0,3,2,3,0,1,2,1,0, };
        Assert::AreEqual(expected_directed, actual_directed, "euler_path_directed");
    }
    {
        auto g = graph;
        const auto actual = euler_path_undirected(g);
        const vector<int> expected{ 5,0,3,2,1,0,4, };
        Assert::AreEqual(expected, actual, "euler_path_undirected");
    }
    {
        auto g = graph;
        auto edge_count = get<1>(graph_count);
        add_edge(g, edge_count, 4, 5);

        const auto actual = euler_path_undirected(g);
        const vector<int> expected{ 0,5,4,0,3,2,1,0, };
        Assert::AreEqual(expected, actual, "euler_path_undirected 4_5");
    }
}