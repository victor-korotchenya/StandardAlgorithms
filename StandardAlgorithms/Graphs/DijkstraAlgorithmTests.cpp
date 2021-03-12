#include <array>
#include "DijkstraAlgorithm.h"
#include "DijkstraAlgorithmTests.h"
#include "bellman_ford_shortest_paths.h"
#include "../Assert.h"

using namespace std;
using namespace Privet::Algorithms::Graphs;
using namespace Privet::Algorithms;

namespace
{
    using weight_t = int64_t;

    constexpr auto vertexCount = 5;
    constexpr auto absentParent = -1, sourceVertex = 2;
    constexpr weight_t infiniteWeigth = numeric_limits<weight_t>::max() / 3;

    auto build_graph()
    {
        Graph<int, weight_t> graph(vertexCount);

        graph.AddEdge(0, 1, 5);
        graph.AddEdge(0, 2, 90);
        graph.AddEdge(0, 3, 6);

        graph.AddEdge(1, 2, 6);
        graph.AddEdge(2, 4, 10);

        graph.AddEdge(3, 2, 4);
        graph.AddEdge(3, 4, 5);
        // 0 -> 1(5) -> 2(6) -> 4(10)
        // 0 -> 2(90) -> 3(4) -> 4(5)
        //               3 -> 0(6)
        return graph;
    }
}

void Privet::Algorithms::Trees::Tests::DijkstraAlgorithmTests()
{
    const auto graph = build_graph();

    vector<weight_t> distances(vertexCount + 1);
    vector<int> parents(vertexCount + 1), path(parents.size());

    DijkstraAlgorithm<int, weight_t>::Run(graph, absentParent, infiniteWeigth, sourceVertex, distances, parents);

    const array expected_parents = { 3, 2, absentParent, 2, 3 };
    Assert::AreEqual<int>(expected_parents.data(), vertexCount, parents, "parents in DijkstraAlgorithm.");

    const array<weight_t, vertexCount> expected_min_weights = { 10, 6, 0, 4, 9 };
    Assert::AreEqual<weight_t>(expected_min_weights.data(), vertexCount, distances,
        "distances in DijkstraAlgorithm.");

    {
        set<pair<weight_t, int>> dist_vertex_temp;

        dijkstra_sparse(graph.AdjacencyLists(), sourceVertex, distances, parents, dist_vertex_temp, infiniteWeigth);

        Assert::AreEqual<weight_t>(expected_min_weights.data(), vertexCount, distances, "distances in dijkstra_sparse.");
        Assert::AreEqual<int>(expected_parents.data(), vertexCount, parents, "parents in dijkstra_sparse.");
    }
    {
        dijkstra_dense(graph.AdjacencyLists(), sourceVertex, distances, parents, infiniteWeigth);

        Assert::AreEqual<weight_t>(expected_min_weights.data(), vertexCount, distances, "distances in dijkstra_dense.");
        Assert::AreEqual<int>(expected_parents.data(), vertexCount, parents, "parents in dijkstra_dense.");
    }
    {
        deque<int> temp, temp2;
        std::vector<bool> used;
        const auto actual = bellman_ford_shortest_paths_fast(graph.AdjacencyLists(), sourceVertex,
            distances, parents, temp, temp2, used, infiniteWeigth);

        Assert::AreEqual(true, actual, "bellman_ford_shortest_paths_fast.");

        Assert::AreEqual<weight_t>(expected_min_weights.data(), vertexCount, distances, "distances in bellman_ford_shortest_paths_fast.");
        Assert::AreEqual<int>(expected_parents.data(), vertexCount, parents, "parents in bellman_ford_shortest_paths_fast.");
    }
    {
        const auto actual = bellman_ford_shortest_paths_slow(graph.AdjacencyLists(), sourceVertex, distances, parents, infiniteWeigth);

        Assert::AreEqual(true, actual, "bellman_ford_shortest_paths_slow.");

        Assert::AreEqual<weight_t>(expected_min_weights.data(), vertexCount, distances, "distances in bellman_ford_shortest_paths_slow.");
        Assert::AreEqual<int>(expected_parents.data(), vertexCount, parents, "parents in bellman_ford_shortest_paths_slow.");
    }
    {
        vector<vector<weight_t>> distances2d;
        vector<vector<int>> nexts2d;

        const auto actual = floyd_warshall_shortest_paths(graph.AdjacencyLists(), distances2d, nexts2d, infiniteWeigth);
        Assert::AreEqual(true, actual, "floyd_warshall_shortest_paths.");

        const auto& di = distances2d[sourceVertex];
        Assert::AreEqual<weight_t>(expected_min_weights.data(), vertexCount, di, "distances in floyd_warshall_shortest_paths.");

        parents.resize(vertexCount);
        for (auto to = 0; to < vertexCount; ++to)
        {
            restore_floyd_warshall_shortest_paths(nexts2d, sourceVertex, to, path);
            // Could be done faster.
            parents[to] = path.size() >= 2 ? path[path.size() - 2] : absentParent;
        }

        Assert::AreEqual<int>(expected_parents.data(), vertexCount, parents, "parents in floyd_warshall_shortest_paths.");
    }

    const int targetVertex = 0;

    DijkstraAlgorithm<int, weight_t>::FindPath(absentParent, targetVertex, parents, path);

    const array expectedPath = { sourceVertex, 3, targetVertex };
    Assert::AreEqual<int>(expectedPath.data(), expectedPath.size(), path,
        "path in DijkstraAlgorithm.");
}