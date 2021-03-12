#include <limits>
#include <exception>
#include <string>
#include <sstream>
#include <vector>
#include <queue>
#include "Graph_Girth.h"
#include "../Utilities/QueueUtilities.h"

using namespace std;
using namespace Privet::Algorithms;

inline void Graph_Girth::FillParents(
    const vector< size_t >& parents,
    const size_t startVertex,
    vector< size_t >& data)
{
    data.clear();
    size_t vertex = startVertex;
    do
    {
        data.push_back(vertex);
        vertex = parents[vertex];
    } while (NoParent != vertex);
}

inline void Graph_Girth::AppendReversedButLast(
    const vector< size_t >& parentsTemp,
    vector< size_t >& cycleVertices)
{
    const auto parentsTempReversedEnd = parentsTemp.crend();
    auto it = parentsTemp.crbegin();
    if (it != parentsTempReversedEnd)
    {
        while ((++it) != parentsTempReversedEnd)
        {
            const size_t vertex = *it;
            cycleVertices.push_back(vertex);
        }
    }
}

unsigned long long Graph_Girth::FindGirthUnweighted(
    const Graph<int, unsigned int>& g,
    vector< size_t >& cycleVertices)
{
    const vector< vector< WeightedVertex <int, unsigned int> > >& adjacencyLists = g.AdjacencyLists();
    const size_t vertexCount = adjacencyLists.size();
    if (vertexCount < 3)
    {
        throw exception("The graph must have at least 3 vertices.");
    }

    cycleVertices.clear();

    unsigned long long result = (ULLONG_MAX);

    //To bound the search.
    unsigned long long semiDistance = (ULLONG_MAX);

    vector< bool > visited;
    vector< size_t > parents;
    vector< size_t > parentsTemp;
    vector< size_t > distances;

    const size_t plusInfinity = 0 - size_t(1);
    const size_t skipOneVertex = 1;

    for (size_t sourceVertex = skipOneVertex; sourceVertex < vertexCount; ++sourceVertex)
    {
        //TODO: p1. Get rid of distances by using 2 queues:
        // q1 - current level.
        // q2 - next level.
        visited.assign(vertexCount, false);
        parents.assign(vertexCount, NoParent);
        distances.assign(vertexCount, plusInfinity);
        const size_t distance0 = 0;
        distances[sourceVertex] = distance0;

        //Run BFS
        queue<size_t> q;
        q.push(sourceVertex);
        do
        {
            const size_t currentVertex = q.front();
            q.pop();

            if (semiDistance < distances[currentVertex])
                //Next level won't produce a smaller size cycle.
                break;

            visited[currentVertex] = true;
            const size_t currentVertexParent = parents[currentVertex];
            const size_t nextWeight = distances[currentVertex] + 1;

            const vector< WeightedVertex <int, unsigned int> >& edges = adjacencyLists[currentVertex];
            const size_t edgesSize = edges.size();
            for (size_t toIndex = 0; toIndex < edgesSize; ++toIndex)
            {
                const WeightedVertex<int, unsigned int> edge = edges[toIndex];
                const int to = edge.Vertex;
                if (currentVertexParent == to)
                { //Skip the edge (parents[currentVertex], currentVertex) which becomes
                  //(currentVertex, parents[currentVertex]).
                    continue;
                }

#ifdef _DEBUG
                const auto distance2 = distances[to];
                const long long d = nextWeight - static_cast<long long>(1) - distance2;
                const auto absDelta = abs(d);

                const bool isOkDistance2 = plusInfinity == distance2
                    || absDelta <= 2;
                if (!isOkDistance2)
                {
                    ostringstream ss;
                    ss << "Bad absDelta=" << absDelta << " in " << (__FUNCDNAME__)
                        << ", sourceVertex=" << sourceVertex
                        << ", to=" << to << ", currentVertex=" << currentVertex
                        << ", distance2=" << distance2
                        << ", (nextWeight - 1)=" << (nextWeight - 1) << ".";
                    StreamUtilities::throw_exception(ss);
                }
#endif
                if (visited[to])
                {//A cycle is found.
                    const unsigned long long currentDistance = nextWeight + distances[to];
                    if (currentDistance < result)
                    {
                        if ((ULLONG_MAX) == result)
                        {
                            cycleVertices.reserve(currentDistance);
                            const unsigned long long estimatedSize = (currentDistance >> 1) + 1u;
                            parentsTemp.reserve(estimatedSize);
                        }

                        FillParents(parents, currentVertex, cycleVertices);
                        FillParents(parents, to, parentsTemp);
                        AppendReversedButLast(parentsTemp, cycleVertices);

                        const unsigned long long minGirthForPlainGraph = 3;
                        if (minGirthForPlainGraph == currentDistance)
                        {//The possible smallest value is found - no sense to go on.
                            return minGirthForPlainGraph;
                        }

                        result = currentDistance;
                        semiDistance = (currentDistance - 1) >> 1;
                    }
                }
                else if (nextWeight < distances[to])
                {//Push only once as there can be several arcs to a vertex.
                    q.push(to);
                    parents[to] = currentVertex;
                    distances[to] = nextWeight;
                }
            }
        } while (!q.empty());
    }

    if ((ULLONG_MAX) == result)
    {//No cycle
        result = 0;
    }

    return result;
}