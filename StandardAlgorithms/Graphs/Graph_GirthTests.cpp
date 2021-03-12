#include <vector>
#include "Graph_Girth.h"
#include "../Assert.h"

using namespace std;
using namespace Privet::Algorithms;

void FindGirthTests()
{
    const int vertexCount = 10;
    Graph<int, unsigned int> g(vertexCount);

    //Edges: 0-1-2-3-4-5-6-7-8-9-0, 0-4, 0-6.
    //Some cycles: 0-1-2-3-4-0, 0-1-2-3-4-5-6-0, 0-1-2-3-4-5-6-7-8-9-0,
    //  0-4-5-6-0, 0-4-5-6-7-8-9-0,
    //  0-6-7-8-9-0.
    //Girth path: 0-4-5-6-0.
    for (int i = 0; i < vertexCount; ++i)
    {
        g.AddEdge(i, (i + 1) % vertexCount);
    }
    g.AddEdge(0, 4);
    g.AddEdge(0, 6);

    vector< size_t > actualCycleVertices;
    const unsigned long long actualGirth = Graph_Girth::FindGirthUnweighted(g, actualCycleVertices);
    const unsigned long long expectedGirth = 4;
    Assert::AreEqual<unsigned long long>(expectedGirth, actualGirth, "Girth.");

    const size_t expectedCycleVertices[expectedGirth] = { 6,5,4,0 };

    Assert::AreEqual<size_t>(
        expectedCycleVertices, expectedGirth, actualCycleVertices,
        "Error in FindGirthUnweighted for actualCycleVertices.");
}