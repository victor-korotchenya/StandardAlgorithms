#include <array>
#include <vector>
#include <sstream>
#include <queue>
#include <functional>
#include <random>

#include "MinimalSpanningTree.h"
#include "jarnik_minimal_spanning_tree.h"
#include "MinimalSpanningTreeTests.h"
#include "../Assert.h"

using namespace std;
using namespace Privet::Algorithms::Graphs;
using namespace Privet::Algorithms;

void MinimalSpanningTreeTests()
{
    const auto vertexCount = 5;
    Graph<int, int> g(vertexCount);

    g.AddEdge(2, 4, 10);

    g.AddEdge(0, 1, 5);
    g.AddEdge(0, 2, 9);
    g.AddEdge(0, 3, 10);
    g.AddEdge(1, 2, 7);

    g.AddEdge(3, 2, 4);
    g.AddEdge(3, 4, 9);

    vector<int> primPreviousVertices;
    long long totalWeight = 0;

    MinimalSpanningTree<int, int>::PrimByEdges(g, primPreviousVertices, totalWeight);

    const auto primPreviousVerticesSize = primPreviousVertices.size();
    Assert::AreEqual(vertexCount, primPreviousVerticesSize, "primPreviousVerticesSize");

    for (auto i = 0; i < vertexCount; ++i)
    {
        const auto expected = i - 1;
        const auto actualPreviousVertex = primPreviousVertices[i];
        if (expected != actualPreviousVertex)
        {
            ostringstream ss;
            ss << "Error in Prim's algorithm: vertex (" << i <<
                ") has previous " << actualPreviousVertex
                << " whereas " << expected << " is expected.";

            StreamUtilities::throw_exception(ss);
        }
    }

    using weight_t2 = int64_t;
    using weight_t = int64_t;
    const auto graph = g.to_plain_graph();

    // temps
    vector<int> parents;
    vector<bool> visited, visited2(g.AdjacencyLists().size(), true);
    vector<weight_t2> weights, expected_weights{ 0,5,12,16,25, };
    priority_queue<pair<weight_t2, int>> q;

    const auto min_w = jarnik_minimal_spanning_tree<>(graph, 0, parents, visited, weights, q);
    Assert::AreEqual(min_w, totalWeight, "min_w");
    Assert::AreEqual(visited, visited2, "visited");
    Assert::AreEqual(parents, primPreviousVertices, "parents");
    Assert::AreEqual(weights, expected_weights, "weights");
}

using intpair = pair<int, int>;
using Graph_WeightedVertex = vector< vector< WeightedVertex <int, int> > >;
using intpair_PQ = priority_queue< intpair, vector< intpair >, greater< intpair > >;

inline void AddAdjacents2(
    const Graph_WeightedVertex& g,
    const int vertex0,
    const vector< bool >& visitedVertices,
    vector< int >& weights,
    vector< int >& parentVertices,
    intpair_PQ& q)
{
    const vector< WeightedVertex <int, int> >& adjacencyList = g[vertex0];
    const size_t size = adjacencyList.size();
    for (size_t i = 0; i < size; ++i)
    {
        const WeightedVertex<int, int> p = adjacencyList[i];
        const int vertex2 = p.Vertex;
        if (!visitedVertices[vertex2])
        {
            const int weightOld = weights[vertex2];
            const int weightNew = p.Weight;
            if (weightNew < weightOld)
            {
                weights[vertex2] = weightNew;
                parentVertices[vertex2] = vertex0;
                q.push(make_pair(weightNew, vertex2));
            }
        }
    }
}

long long prim(const Graph_WeightedVertex& g, vector< int >& parentVertices)
{
    const size_t size = g.size();

    long long totalWeight = 0;

    vector< bool > visitedVertices(size);
    intpair_PQ q;

    vector< int > weights(size, INT_MAX);
    weights[0] = 0;

    parentVertices.assign(size, -1);

    q.push(make_pair(0, 0));

    while (!q.empty())
    {
        const auto top = q.top();
        q.pop();

        const int vertex0 = top.second;
        if (!visitedVertices[vertex0])
        {
            visitedVertices[vertex0] = true;

            const int weight0 = top.first;
            totalWeight += weight0;

            AddAdjacents2(g, vertex0, visitedVertices, weights, parentVertices, q);
        }
    }

    return totalWeight;
}

void MST_PerformanceTests()
{
#ifdef SHALL_PRINT_TEST_RESULTS
    LARGE_INTEGER Frequency;
    QueryPerformanceFrequency(&Frequency);
#endif

#ifdef _DEBUG
    const int attempts = 3;
#else
    const int attempts = 5;
#endif

#ifdef SHALL_PRINT_TEST_RESULTS
    double totalTime0ByTime1 = 0, totalTime1ByTime2 = 0;
#endif

    for (int attempt = 0; attempt < attempts; ++attempt)
    {
#ifdef _DEBUG
        const int size = 5;
#else
        const int size = 16
            //* 32
            ;
#endif
        Graph_WeightedVertex g0(size);
        Graph<int, int> g2(size);

        random_device rd;

        for (int i = 0; i < size - 1; ++i)
        {
            for (int j = i + 1; j < size; ++j)
            {
#ifdef _DEBUG
                const int weight = rd() % size;
#else
                const int weight = rd();
#endif
                g0[i].push_back(WeightedVertex<int, int>(j, weight));
                g0[j].push_back(WeightedVertex<int, int>(i, weight));

                g2.AddEdge(i, j, weight);
            }
        }

        //g2._AdjacencyLists = g0;

        //g[0].push_back(make_pair(1, 10));
        //g[0].push_back(make_pair(2, 5));

        //g[1].push_back(make_pair(0, 10));
        //g[1].push_back(make_pair(2, 10));

        //g[2].push_back(make_pair(1, 10));
        //g[2].push_back(make_pair(0, 5));

        vector< int > parentVertices0;

#ifdef SHALL_PRINT_TEST_RESULTS
        LARGE_INTEGER time0, time1, elapsedMicroseconds0;
        LARGE_INTEGER time2, time3, elapsedMicroseconds1;
        LARGE_INTEGER time4, time5, elapsedMicroseconds2;
        QueryPerformanceCounter(&time0);
#endif

        long long res0 = prim(g0, parentVertices0);

#ifdef SHALL_PRINT_TEST_RESULTS
        QueryPerformanceCounter(&time1);
#endif

        vector< int > parentVertices2;
        long long res2 = 0;

#ifdef SHALL_PRINT_TEST_RESULTS
        QueryPerformanceCounter(&time2);
#endif
        MinimalSpanningTree<int, int>::PrimByEdges(g2, parentVertices2, res2, false);
#ifdef SHALL_PRINT_TEST_RESULTS
        QueryPerformanceCounter(&time3);
#endif

        vector< int > parentVertices3;
        long long res3 = 0;

#ifdef SHALL_PRINT_TEST_RESULTS
        QueryPerformanceCounter(&time4);
#endif
        MinimalSpanningTree<int, int>::Prim(g2, parentVertices3, res3, false);

#ifdef SHALL_PRINT_TEST_RESULTS
        QueryPerformanceCounter(&time5);

        elapsedMicroseconds0.QuadPart = time1.QuadPart - time0.QuadPart;
        elapsedMicroseconds0.QuadPart *= 1000000;
        elapsedMicroseconds0.QuadPart /= Frequency.QuadPart;

        elapsedMicroseconds1.QuadPart = time3.QuadPart - time2.QuadPart;
        elapsedMicroseconds1.QuadPart *= 1000000;
        elapsedMicroseconds1.QuadPart /= Frequency.QuadPart;

        elapsedMicroseconds2.QuadPart = time5.QuadPart - time4.QuadPart;
        elapsedMicroseconds2.QuadPart *= 1000000;
        elapsedMicroseconds2.QuadPart /= Frequency.QuadPart;

        const double time0ByTime1 =
            (0 == elapsedMicroseconds1.QuadPart) ? 0
            : ((double)elapsedMicroseconds0.QuadPart) / elapsedMicroseconds1.QuadPart;

        const double time1ByTime2 =
            (0 == elapsedMicroseconds2.QuadPart) ? 0
            : ((double)elapsedMicroseconds1.QuadPart) / elapsedMicroseconds2.QuadPart;

        totalTime0ByTime1 += time0ByTime1;
        totalTime1ByTime2 += time1ByTime2;
#endif

        if (res0 != res2)
        {
            ostringstream ss;
            ss << "res0 (" << res0 << ") != res2 (" << res2 << ") in Prim by edges.";
            StreamUtilities::throw_exception(ss);
        }

        if (res0 != res3)
        {
            ostringstream ss;
            ss << "res0 (" << res0 << ") != res3 (" << res3 << ") in Prim.";
            StreamUtilities::throw_exception(ss);
        }

#ifdef SHALL_PRINT_TEST_RESULTS
        cout //<< "IS_OK = "<< (res0 == res2)
            << "Attempt = " << attempt
            << ", time0/t1 = " << time0ByTime1
            << ", t1/t2 = " << time1ByTime2
            << ", time0= " << elapsedMicroseconds0.QuadPart
            << ", t1 = " << elapsedMicroseconds1.QuadPart
            << ", t2 = " << elapsedMicroseconds2.QuadPart
            << ", weight0 = " << res0
            << ", w2 = " << res2
            << ", w3 = " << res3
            << '\n';
#endif
    }

#ifdef SHALL_PRINT_TEST_RESULTS
    totalTime0ByTime1 /= attempts;
    totalTime1ByTime2 /= attempts;
    cout << "totalTime0ByTime1 = " << totalTime0ByTime1
        << ", totalTime1ByTime2 = " << totalTime1ByTime2
        << '\n';
#endif
}