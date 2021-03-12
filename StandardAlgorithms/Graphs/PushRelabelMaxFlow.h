#pragma once
#include <vector>
#include <sstream>
#include <unordered_set>
#include "Graph.h"
#include "../Numbers/BinarySearch.h"
#include "flow_edge.h"

namespace Privet::Algorithms::Graphs
{
    //Finds max flow.
    template <typename TWeight, typename TEdge = WeightedVertex<int, TWeight>>
    class PushRelabelMaxFlow final
    {
        PushRelabelMaxFlow() = delete;
        //TODO: p2. finish.

    public:
        static TWeight Calc(
            const std::vector<std::vector<TEdge>>& graph,
            const int sourceIndex,
            const int targetIndex,
            std::vector<std::vector<TEdge>>& flows);

    private:

        using flow_edge = flow_edge<TWeight>;

        inline static void CheckIndex(
            const int index, const int size, const std::string& name);

        static void FillFlowEdges(
            const std::vector<std::vector<TEdge>>& graph,
            const int sourceIndex,
            const int targetIndex,
            std::vector<std::vector<flow_edge>>& flowEdges);

        inline static void CheckEdge(
            const int source,
            const int target,
            const TWeight& weight,
            const int size,
            std::unordered_set<int>& vertices);

        static TWeight CalcSourceExcess(const std::vector<TEdge>& sourceEdges);

        inline static void Enqueue(
            const int vertex,
            const std::vector<TWeight>& excess,
            const std::vector<int>& dist,
            const int size,
            std::vector<bool>& active,
            std::vector<std::vector<int >>& B,
            int& b);

        static void Push(
            const int fromVertex,
            const int size,
            const TWeight amount,
            const  std::vector<int>& dist,
            std::vector<TWeight>& excess,
            //std::vector<std::vector<flow_edge>> &flowEdges,
            flow_edge& edge,
            std::vector<bool>& active,
            std::vector<std::vector<int>>& B,
            int& b);

        static void Gap(
            const int fromDistance,
            const int size,
            std::vector<int>& dist,
            std::vector<int>& count,
            std::vector<bool>& active,
            std::vector<std::vector<int >>& B,
            int& b,
            std::vector<TWeight>& excess);

        static void Relabel(
            const int fromVertex,
            const int size,
            const std::vector<TWeight>& excess,
            const std::vector<std::vector<flow_edge>>& flowEdges,
            std::vector<int>& dist,
            std::vector<int>& count,
            std::vector<bool>& active,
            std::vector<std::vector<int >>& B,
            int& b);

        static void Discharge(
            const int fromVertex,
            const int size,
            std::vector<int>& dist,
            std::vector<int>& count,
            std::vector<TWeight>& excess,
            std::vector<std::vector<flow_edge>>& flowEdges,
            std::vector<bool>& active,
            std::vector<std::vector<int >>& B,
            int& b);
    };

    template <typename TWeight, typename TEdge >
    TWeight PushRelabelMaxFlow< TWeight, TEdge >::Calc(
        const std::vector<std::vector<TEdge>>& graph,
        const int sourceIndex,
        const int targetIndex,
        std::vector<std::vector<TEdge>>& flows)
    {
        const int size = static_cast<int>(graph.size());
        if (0 == size)
        {
            throw std::runtime_error("The graph must be not empty.");
        }

        CheckIndex(sourceIndex, size, "sourceIndex");
        CheckIndex(targetIndex, size, "targetIndex");

        flows.clear();
        flows.resize(size);

        const TWeight sourceExcess = CalcSourceExcess(graph[sourceIndex]);
        if (0 == sourceExcess)
        {
            return 0;
        }

        std::vector<std::vector<flow_edge>> flowEdges;
        FillFlowEdges(graph, sourceIndex, targetIndex, flowEdges);

        std::vector<TWeight > excess;
        excess.resize(size, 0);

        std::vector<int > dist, count;
        dist.resize(size, 0);
        count.resize(size + 1ll, 0);

        std::vector<bool > active;
        active.resize(size);

        std::vector<std::vector<int > > B;
        B.resize(size);

        excess[sourceIndex] = sourceExcess;
        count[0] = size;

        int b = 0;

        Enqueue(sourceIndex,
            excess,
            dist,
            size,
            active,
            B,
            b);

        active[targetIndex] = true;

        for (;;)
        {
            if (B[b].empty())
            {
                if (0 == b--)
                {
                    break;
                }
            }
            else
            {
                const int fromVertex = B[b].back();
                B[b].pop_back();

                active[fromVertex] = false;

                Discharge(
                    fromVertex,
                    size,
                    dist,
                    count,
                    excess,
                    flowEdges,
                    active,
                    B,
                    b);
            }
        }

        const TWeight result = excess[targetIndex];

        for (int i = 0; i < size; ++i)
        {
            std::vector<flow_edge>& line = flowEdges[i];
            std::vector<TEdge>& resultFlow = flows[i];

            const size_t lineSize = line.size();

            for (size_t j = 0; j < lineSize; j++)
            {
                if (TWeight(0) < line[j].flow)
                {
                    TEdge newFlow;
                    newFlow.Vertex = line[j].to;
                    newFlow.Weight = line[j].flow;
                    resultFlow.push_back(newFlow);
                }
            }
        }

        return result;
    }

    template <typename TWeight, typename TEdge >
    TWeight PushRelabelMaxFlow< TWeight, TEdge >::CalcSourceExcess(
        const std::vector<TEdge>& sourceEdges)
    {
        TWeight result = TWeight(0);

        for (const TEdge& e : sourceEdges)
        {
            result += e.Weight;
        }

        return result;
    }

    template <typename TWeight, typename TEdge >
    void PushRelabelMaxFlow< TWeight, TEdge >::Enqueue(
        const int vertex,
        const std::vector<TWeight>& excess,
        const std::vector<int>& dist,
        const int size,
        std::vector<bool>& active,
        std::vector<std::vector<int >>& B,
        int& b)
    {
        if (!active[vertex] && 0 < excess[vertex] && dist[vertex] < size)
        {
            const int& dist1 = dist[vertex];

            active[vertex] = true;
            B[dist1].push_back(vertex);

            if (b < dist1)
            {
                b = dist1;
            }
        }
    }

    template <typename TWeight, typename TEdge >
    void PushRelabelMaxFlow< TWeight, TEdge >::Push(
        const int fromVertex,
        const int size,
        const TWeight amount,
        const std::vector<int>& dist,
        std::vector<TWeight>& excess,
        //std::vector<std::vector<flow_edge>> &flowEdges,
        flow_edge& edge,
        std::vector<bool>& active,
        std::vector<std::vector<int >>& B,
        int& b)
    {
#ifdef _DEBUG
        //printf(" Push (%d-%d) %d, cap=%d, old=%d\n", fromVertex, edge.to, amount,
        //  edge.Cap, edge.Flow);
#endif
        edge.flow += amount;

        ////flowEdges[edge.to][edge.index].Flow -= amount;
        ////flowEdges[edge.to][fromVertex].Flow -= amount;
        ////
        //std::vector<flow_edge> &lines = flowEdges[edge.to];
        //const size_t linesSize = lines.size();
        //if (linesSize)
        //{
        //  flow_edge* left = lines.data();
        //  flow_edge * rightInclusive = left + linesSize - 1;
        //
        //  typed ef int (*CastFunction)(const flow_edge &value1);
        //
        //  flow_edge* pBackEdge = BinarySearch::SearchWithCast< flow_edge, int, CastFunction >
        //    (left, rightInclusive,
        //    fromVertex, &flow_edge::GetTo);
        //  if (nullptr == pBackEdge)
        //  {
        //  //  ostringstream ss;
        //  //  ss << "nullptr == pBackEdge. fromVertex=" << fromVertex << ".";
        //  //  StreamUtilities::throw_exception(ss);
        //  }
        //  ////TODO: p1. not sure it is needed?
        //  //pBackEdge->Flow -= amount;
        //}

        excess[fromVertex] -= amount;
        excess[edge.to] += amount;

        Enqueue(
            edge.to,
            excess,
            dist,
            size,
            active,
            B,
            b);
    }

    template <typename TWeight, typename TEdge >
    void PushRelabelMaxFlow< TWeight, TEdge >::Gap(
        const int fromDistance,
        const int size,
        std::vector<int>& dist,
        std::vector<int>& count,
        std::vector<bool>& active,
        std::vector<std::vector<int >>& B,
        int& b,
        std::vector<TWeight>& excess)
    {
        for (int vertex = 0; vertex < size; ++vertex)
        {
            const int d = dist[vertex];
            if (fromDistance <= d)
            {
                const int newDist = std::max(d, size);
                dist[vertex] = newDist;

                --count[d];
                ++count[newDist];

                Enqueue(
                    vertex,
                    excess,
                    dist,
                    size,
                    active,
                    B,
                    b);
            }
        }
    }

    template <typename TWeight, typename TEdge >
    void PushRelabelMaxFlow< TWeight, TEdge >::Relabel(
        const int fromVertex,
        const int size,
        const std::vector<TWeight>& excess,
        const std::vector<std::vector<flow_edge>>& flowEdges,
        std::vector<int>& dist,
        std::vector<int>& count,
        std::vector<bool>& active,
        std::vector<std::vector<int >>& B,
        int& b)
    {
#ifdef _DEBUG
        if (dist[fromVertex] > size)
        {
            throw std::runtime_error("dist[fromVertex] > size");
        }

        if (0 == count[dist[fromVertex]])
        {
            throw std::runtime_error("0 == count[ dist[fromVertex] ]");
        }
#endif
        --count[dist[fromVertex]];

        int newDistance = size;

        const std::vector<flow_edge >& edges = flowEdges[fromVertex];

        for (const flow_edge& e : edges)
        {
            if (e.flow < e.capacity)
            {
                const int to1 = dist[e.to] + 1;
                if (to1 < newDistance)
                {
                    newDistance = to1;
                }
            }
        }

        dist[fromVertex] = newDistance;
        ++count[newDistance];

        Enqueue(
            fromVertex,
            excess,
            dist,
            size,
            active,
            B,
            b);
    }

    template <typename TWeight, typename TEdge >
    void PushRelabelMaxFlow< TWeight, TEdge >::Discharge(
        const int fromVertex,
        const int size,
        std::vector<int>& dist,
        std::vector<int>& count,
        std::vector<TWeight>& excess,
        std::vector<std::vector<flow_edge>>& flowEdges,
        std::vector<bool>& active,
        std::vector<std::vector<int >>& B,
        int& b)
    {
        std::vector<flow_edge>& edges = flowEdges[fromVertex];

        for (flow_edge& edge : edges)
        {
            const TWeight ex1 = excess[fromVertex];
            if (TWeight(0) == ex1)
            {
                break;
            }

            const TWeight underFlow = edge.capacity - edge.flow;
            const TWeight amount = std::min(ex1, underFlow);

            if (TWeight(0) < amount
                && dist[fromVertex] == (dist[edge.to] + 1))
            {
                Push(
                    fromVertex,
                    size,
                    amount,
                    dist,
                    excess,
                    //flowEdges,
                    edge,
                    active,
                    B,
                    b);
            }
        }

        if (0 == excess[fromVertex])
        {
            return;
        }

        if (count[dist[fromVertex]] == 1)
        {
            Gap(
                dist[fromVertex],
                size,
                dist,
                count,
                active,
                B,
                b,
                excess);
        }
        else
        {
            Relabel(
                fromVertex,
                size,
                excess,
                flowEdges,
                dist,
                count,
                active,
                B,
                b);
        }
    }

    template <typename TWeight, typename TEdge >
    void PushRelabelMaxFlow< TWeight, TEdge >::CheckIndex(
        const int index, const int size, const std::string& name)
    {
        const bool isOk = 0 <= index && index < size;
        if (!isOk)
        {
            std::ostringstream ss;
            ss << "The '" << name << "' (" << index
                << ") must be between 0 and " << (size - 1) << ".";
            StreamUtilities::throw_exception(ss);
        }
    }

    template <typename TWeight, typename TEdge >
    void PushRelabelMaxFlow<TWeight, TEdge>::FillFlowEdges(
        const std::vector<std::vector<TEdge>>& graph,
        const int sourceIndex,
        const int targetIndex,
        std::vector<std::vector<flow_edge>>& flowEdges)
    {
        const int size = static_cast<int>(graph.size());
        flowEdges.resize(size);

        std::unordered_set< int > uniqueVertices;

        for (int i = 0; i < size; ++i)
        {
            const std::vector<TEdge>& adjacents = graph[i];

            const int adjacentsSize = static_cast<int>(adjacents.size());
            if (size < adjacentsSize)
            {
                std::ostringstream ss;
                ss << "There are " << adjacentsSize << " edges at index=" << i
                    << " which must not exceed size=" << size << ".";
                StreamUtilities::throw_exception(ss);
            }

            std::vector<flow_edge>& flowLine = flowEdges[i];
            flowLine.reserve(adjacentsSize);

            for (int j = 0; j < adjacentsSize; ++j)
            {
                const TEdge& edge = adjacents[j];

                CheckEdge(i, edge.Vertex, edge.Weight, size, uniqueVertices);

                if (sourceIndex == edge.Vertex
                    || targetIndex == i)
                {//Ignore edges coming into the source and
                  //outgoing from the target.
                    continue;
                }

                flow_edge newEdge(edge.Vertex, 0, edge.Weight);
                flowLine.push_back(newEdge);
            }

            uniqueVertices.clear();
        }
    }

    template <typename TWeight, typename TEdge >
    void PushRelabelMaxFlow<TWeight, TEdge>::CheckEdge(
        const int source,
        const int target,
        const TWeight& weight,
        const int size,
        std::unordered_set<int>& vertices)
    {
        if (source == target)
        {
            std::ostringstream ss;
            ss << "Self-edges (" << source << ", " << source << ") are forbidden.";
            StreamUtilities::throw_exception(ss);
        }

        if (size <= target)
        {
            std::ostringstream ss;
            ss << "Target vertex of an edge (" << source << ", " << target
                << ") must be smaller than size (" << size << ").";
            StreamUtilities::throw_exception(ss);
        }

        const auto inserted = vertices.insert(target);
        if (!inserted.second)
        {
            std::ostringstream ss;
            ss << "The edge (" << source << ", " << target
                << ") has already been added.";
            StreamUtilities::throw_exception(ss);
        }

        if (!(0 <= weight))
        {
            std::ostringstream ss;
            ss << "The edge capacity=" << weight
                << " at (" << source << ", " << target
                << ") must be non-negative.";
            StreamUtilities::throw_exception(ss);
        }
    }
}