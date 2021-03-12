#pragma once
#include <set>
#include <sstream>
#include <vector>
#include "Graph.h"
#include "PriorityQueue.h"

namespace Privet
{
    namespace Algorithms
    {
        namespace Graphs
        {
            //Find 1 source shortest path tree for nonnegative cost graph.
            //Time O(n*n*log(n)) - suitable for a sparse graph.
            template <class graph_t, class weight_t>
            void dijkstra_sparse(
                const graph_t& graph,
                const int source,
                std::vector<weight_t>& distances,
                std::vector<int>& parents,
                std::set<std::pair<weight_t, int>>& dist_vertex_temp,
                const weight_t inf = std::numeric_limits<weight_t>::max() / 2)
            {
                const auto size = graph.size();
                assert(source >= 0 && source < size&& size > 0 && inf > 0);

                distances.assign(size, inf);
                distances[source] = weight_t();
                parents.assign(size, -1);

                dist_vertex_temp.clear();
                dist_vertex_temp.insert(std::make_pair(weight_t(), source));
                do
                {
                    const auto from = dist_vertex_temp.begin()->second;
                    dist_vertex_temp.erase(dist_vertex_temp.begin());

                    const auto& edges = graph[from];
                    const auto& dist_from = distances[from];

                    for (const auto& edge : edges)
                    {
                        const auto& to = edge.Vertex;
                        assert(from != to);
                        const auto& weight = edge.Weight;

                        const auto cand = dist_from + weight;
                        assert(weight >= 0 && cand >= 0);

                        auto& cur = distances[to];
                        if (cur <= cand)
                            continue;

                        if (cur != inf)
                        {
                            const auto it = dist_vertex_temp.find(std::make_pair(cur, to));
                            assert(it != dist_vertex_temp.end());
                            dist_vertex_temp.erase(it);
                        }

                        cur = cand;
                        dist_vertex_temp.insert(std::make_pair(cur, to));
                        parents[to] = from;
                    }
                } while (dist_vertex_temp.size());
            }

            //Time O(n*n) - good for a dense graph.
            template <class graph_t, class weight_t>
            void dijkstra_dense(
                const graph_t& graph,
                int source,
                std::vector<weight_t>& distances,
                std::vector<int>& parents,
                const weight_t inf = std::numeric_limits<weight_t>::max() / 2)
            {
                const auto size = graph.size();
                assert(source >= 0 && source < size&& size > 0 && inf > 0);

                // +1 to have fewer checks when no vertex is found.
                distances.assign(size + 1, inf);
                distances[source] = weight_t();
                parents.assign(size, -1);

                std::vector<bool> processed(size + 1);

                for (size_t att = 0; // A tree has v-1 edges.
                    att < size - 1; ++att)
                {
                    if (att)
                    {
                        source = 0;
                        while (processed[source])
                            ++source;

                        if (distances[source] == inf)
                            break;

                        for (auto j = source + 1; j < size; ++j)
                            if (!processed[j] && distances[j] < distances[source])
                                source = j;
                        assert(source >= 0 && source < size);
                    }
                    assert(!processed[source]);
                    processed[source] = true;

                    const auto& edges = graph[source];
                    const auto& dist_source = distances[source];

                    for (const auto& edge : edges)
                    {
                        const auto& to = edge.Vertex;
                        assert(source != to && to >= 0 && to < size);
                        const auto& weight = edge.Weight;

                        const auto cand = dist_source + weight;
                        assert(weight >= 0 && cand >= 0);

                        auto& cur = distances[to];
                        if (cur <= cand)
                            continue;

                        cur = cand;
                        parents[to] = source;
                    }
                }

                distances.pop_back();
            }

            template <typename Vertex,
                typename Weight>
                class DijkstraAlgorithm final
            {
                DijkstraAlgorithm() = delete;

            public:
                static void Run(
                    const Graph<Vertex, Weight>& graph,
                    // The indicator that the parent is missing e.g. -1.
                    const Vertex absentParent,
                    // The maximum value for the given type e.g. such as (LLONG_MAX/2).
                    const Weight infiniteWeight,
                    //The source vertex.
                    const Vertex sourceVertex,
                    //Returned minimum distances from "sourceVertex" to given vertex.
                    std::vector<Weight>& distances,
                    std::vector<Vertex>& parentVertices)
                {
                    const std::vector<std::vector<NonNegativeWeightedVertex>>& adjacencyLists = graph.AdjacencyLists();

                    const size_t vertexCount = adjacencyLists.size();
                    if (vertexCount < 2)
                        throw std::runtime_error("The graph must have at least 2 vertices.");

                    if (vertexCount <= sourceVertex)
                    {
                        std::ostringstream ss;
                        ss << "The sourceVertex " << sourceVertex << " must be in [0, "
                            << (vertexCount - 1) << "].";
                        StreamUtilities::throw_exception(ss);
                    }

                    distances.assign(vertexCount, infiniteWeight);
                    parentVertices.assign(vertexCount, absentParent);

                    distances[sourceVertex] = 0;

                    Privet::Algorithms::Heaps::PriorityQueue<
                        NonNegativeWeightedVertex,
                        GetIndex_Function,
                        std::vector< NonNegativeWeightedVertex >,
                        NonNegativeWeightedVertex
                    > q(GetIndex, vertexCount);

                    for (Vertex vertex = 0; vertex < vertexCount; ++vertex)
                    {
                        q.push(NonNegativeWeightedVertex(vertex, infiniteWeight));
                    }

                    q.DecreaseKey(NonNegativeWeightedVertex(Vertex(sourceVertex), Weight(0)));

                    do
                    {
                        const NonNegativeWeightedVertex& top_Temp = q.top();
                        const NonNegativeWeightedVertex top = top_Temp;
                        q.pop();

                        const std::vector< NonNegativeWeightedVertex >& adjacencyList = adjacencyLists[top.Vertex];
                        const size_t adjacencyListSize = adjacencyList.size();
                        const Weight topDistance = distances[top.Vertex];

                        for (size_t i = 0; i < adjacencyListSize; ++i)
                        {
                            const NonNegativeWeightedVertex& vertex2 = adjacencyList[i];

                            const Weight newDistance = topDistance + vertex2.Weight;
                            if (newDistance < distances[vertex2.Vertex])
                            {
#ifdef _DEBUG
                                NonNegativeWeightedVertex edgeInQueue;
                                if (!q.TryGetItem(
                                    NonNegativeWeightedVertex(vertex2.Vertex, vertex2.Weight),
                                    edgeInQueue))
                                {
                                    std::ostringstream ss;
                                    ss << "Priority queue must have vertex "
                                        << vertex2.Vertex << " with weight " << vertex2.Weight
                                        << " from vertex " << top.Vertex
                                        << ".";
                                    StreamUtilities::throw_exception(ss);
                                }
#endif
                                distances[vertex2.Vertex] = newDistance;
                                parentVertices[vertex2.Vertex] = top.Vertex;
                                q.DecreaseKey(NonNegativeWeightedVertex(vertex2.Vertex, newDistance));
                            }
                        }
                    } while (q.size());
                }

                static void FindPath(
                    // The indicator that the parent is missing e.g. -1.
                    const Vertex absentParent,
                    //To.
                    const Vertex targetVertex,
                    const std::vector<Vertex>& parentVertices,
                    std::vector<Vertex>& path)
                {
                    path.clear();

                    Vertex vertex = targetVertex;
                    while (absentParent != vertex)
                    {
                        path.push_back(vertex);
                        vertex = parentVertices[vertex];
                    }

                    std::reverse(path.begin(), path.end());
                }

            private:
                using NonNegativeWeightedVertex = WeightedVertex<Vertex, Weight>;

                using GetIndex_Function = size_t(*)(const NonNegativeWeightedVertex& vertex);

                static size_t GetIndex(const NonNegativeWeightedVertex& vertex)
                {
                    return size_t(vertex.Vertex);
                }
            };
        }
    }
}