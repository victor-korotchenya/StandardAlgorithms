#pragma once
#include <sstream>
#include <vector>
#include <queue>
#include "../disable_constructors.h"
#include "Graph.h"
#include "PriorityQueue.h"

// See also jarnik_minimal_spanning_tree.h

//Prim - grow a tree.
//Kruskal - grow a forest of trees.

template <typename TVertex = int, typename TWeight = int>
class MinimalSpanningTree final
{
    static const int initialVertex = 0;

    using IntPriorityQueue = std::priority_queue<
        Privet::Algorithms::WeightedVertex<TVertex, TWeight>,
        std::vector<Privet::Algorithms::WeightedVertex < TVertex, TWeight > >,
        Privet::Algorithms::WeightedVertex_WeightGreater<TVertex, TWeight>
    >;

    //TODO: p2. Implement it in C++.
    using GetVertex_Function = size_t(*)(const Privet::Algorithms::WeightedVertex<TVertex, TWeight>& edge);

    //See also GetVertex_Function.
    static inline size_t GetVertex(const Privet::Algorithms::WeightedVertex<TVertex, TWeight>& edge)
    {
        return edge.Vertex;
    }

    using PriorityQueue_DecreaseKey = Privet::Algorithms::Heaps::PriorityQueue<
        Privet::Algorithms::WeightedVertex<TVertex, TWeight>,
        //Function pointer.
        GetVertex_Function,
        std::vector<Privet::Algorithms::WeightedVertex < TVertex, TWeight > >,
        Privet::Algorithms::WeightedVertex<TVertex, TWeight>
    >;

    MinimalSpanningTree() = delete;

    static const int PlusInfinity = (INT_MAX);

public:

    static const int AbsentParent = -1;

    static void Prim(
        const Privet::Algorithms::Graph<TVertex, TWeight>& g,
        std::vector< int >& parentVertices,
        long long& totalWeight,
        const bool shallCheckConnectivity = true)
    {
        //Ahtung! If the vector is declared not as a reference, performance will be lost!
        //const vector< vector< WeightedVertex > > BAD = g2.AdjacencyLists();
        const std::vector<std::vector<Privet::Algorithms::WeightedVertex <TVertex, TWeight> > >& adjacencyLists = g.AdjacencyLists();

        const size_t vertexCount = adjacencyLists.size();
        if (vertexCount < 2)
        {
            throw std::runtime_error("The graph must have at least 2 vertices.");
        }

        parentVertices.assign(vertexCount, AbsentParent);
        totalWeight = 0;

        //TODO: p2. check. All indexes must be within [0..(vertexCount-1)]
        PriorityQueue_DecreaseKey q(GetVertex, vertexCount);
        AddVerticesToQueue(vertexCount, q);

        do
        {
            const auto& top_Temp = q.top();
            const auto top = top_Temp;
            q.pop();

            totalWeight += top.Weight;
            UpdateAdjacents(adjacencyLists, top.Vertex, parentVertices, q);
        } while (!q.empty());

        if (shallCheckConnectivity)
        {
            //TODO: p2. CheckConnectivity(vertexCount, visitedVertices);
        }
    }

    //TODO: p2. In the original, queue by vertices must be used and
    // it must support decreasing the vertex weight.
    static void PrimByEdges(
        const Privet::Algorithms::Graph<TVertex, TWeight>& g,
        std::vector< int >& parentVertices,
        long long& totalWeight,
        const bool shallCheckConnectivity = true)
    {
        //Ahtung! If the vector is declared not as a reference, performance will be lost!
        //const vector< vector< WeightedVertex > > BAD = g2.AdjacencyLists();
        const std::vector<std::vector<Privet::Algorithms::WeightedVertex <TVertex, TWeight> > >& adjacencyLists = g.AdjacencyLists();

        const size_t vertexCount = adjacencyLists.size();
        if (vertexCount < 2)
        {
            throw std::runtime_error("The graph must have at least 2 vertices.");
        }

        parentVertices.assign(vertexCount, AbsentParent);
        totalWeight = 0;

        std::vector< bool > visitedVertices(vertexCount);
        std::vector< int > weights(vertexCount, PlusInfinity);
        weights[initialVertex] = PlusInfinity - 1;

        IntPriorityQueue q;
        {
            const int emptyWeight = 0;
            const Privet::Algorithms::WeightedVertex<TVertex, TWeight> w(initialVertex, emptyWeight);
            q.push(w);
        }

        size_t visitedCount = 0;
        do
        {
            const Privet::Algorithms::WeightedVertex<TVertex, TWeight> top = q.top();
            q.pop();

            const int vertex0 = top.Vertex;
            if (!visitedVertices[vertex0])
            {
                visitedVertices[vertex0] = true;

                totalWeight += top.Weight;
                ++visitedCount;
                if (visitedCount == vertexCount)
                {//It has become a tree.
                    break;
                }

                AddAdjacentEdges(adjacencyLists, vertex0, visitedVertices, weights, parentVertices, q);
            }
        } while (!q.empty());

        if (shallCheckConnectivity)
        {
            CheckConnectivity(vertexCount, visitedVertices);
        }
    }

private:
    static inline void UpdateAdjacents(
        const std::vector<std::vector<Privet::Algorithms::WeightedVertex <TVertex, TWeight> > >& adjacencyLists,
        const int vertex0,
        std::vector< int >& parentVertices,
        PriorityQueue_DecreaseKey& q)
    {
        Privet::Algorithms::WeightedVertex<TVertex, TWeight> edgeInQueue;

        const std::vector<Privet::Algorithms::WeightedVertex < TVertex, TWeight > >& adjacencyList = adjacencyLists[vertex0];
        const size_t size = adjacencyList.size();
        for (size_t i = 0; i < size; ++i)
        {
            const auto edge = adjacencyList[i];
            if (q.TryGetItem(edge, edgeInQueue) && edge.Weight < edgeInQueue.Weight)
            {
                parentVertices[edge.Vertex] = vertex0;
                q.DecreaseKey(edge);
            }
        }
    }

    static inline void AddAdjacentEdges(
        const std::vector<std::vector<Privet::Algorithms::WeightedVertex <TVertex, TWeight> > >& adjacencyLists,
        const int vertex0,
        const std::vector< bool >& visitedVertices,
        std::vector< int >& weights,
        std::vector< int >& parentVertices,
        IntPriorityQueue& q)
    {
#ifdef _DEBUG
        if (!visitedVertices[vertex0])
        {
            throw std::runtime_error("Error2. The first vertex of an edge must have been visited.");
        }
#endif

        const std::vector<Privet::Algorithms::WeightedVertex < TVertex, TWeight > >& adjacencyList = adjacencyLists[vertex0];
        const size_t size = adjacencyList.size();
        for (size_t i = 0; i < size; ++i)
        {
            const auto edge = adjacencyList[i];
            const int vertex2 = edge.Vertex;
            if (!visitedVertices[vertex2])
            {
                const int weightOld = weights[vertex2];
                const int weightNew = edge.Weight;

                //q.push(WeightedVertex(vertex2, p.second));
                if (weightNew < weightOld)
                {//Only add useful edges!
                    weights[vertex2] = weightNew;
                    parentVertices[vertex2] = vertex0;

                    const Privet::Algorithms::WeightedVertex<TVertex, TWeight> item(vertex2, weightNew);
                    q.push(item);
                }
            }
        }
    }

    void static CheckConnectivity(const size_t vertexCount, const std::vector< bool >& visitedVertices)
    {
        for (size_t i = 0; i < vertexCount; ++i)
        {
            if (!visitedVertices[i])
            {
                std::ostringstream ss;
                ss << "The graph is not connected: the vertex (" << i << ") has not been visited.";
                StreamUtilities::throw_exception(ss);
            }
        }
    }

    static inline void AddVerticesToQueue(
        const size_t vertexCount, PriorityQueue_DecreaseKey& q)
    {
        const int emptyWeight = 0;
        const Privet::Algorithms::WeightedVertex<TVertex, TWeight> w(initialVertex, emptyWeight);
        q.push(w);

        const size_t skipInitialVertex = 1;
        for (int vertex = skipInitialVertex; vertex < vertexCount; ++vertex)
        {
            const Privet::Algorithms::WeightedVertex<TVertex, TWeight> w2(vertex, PlusInfinity);
            q.push(w2);
        }
    }
};