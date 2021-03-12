#pragma once
#include <sstream>
#include <vector>
#include "WeightedVertex.h"

namespace Privet
{
    namespace Algorithms
    {
        template <typename Vertex, typename Weight>
        class Graph final
        {
            std::vector<std::vector<WeightedVertex<Vertex, Weight>>> _AdjacencyLists;

            Graph() = delete;

        public:

            explicit Graph(size_t const vertexCount)
            {
                if (vertexCount <= 0)
                {
                    std::ostringstream ss;
                    ss << "The argument vertexCount (" << vertexCount << ") must be positive.";
                    StreamUtilities::throw_exception(ss);
                }

                _AdjacencyLists.assign(vertexCount, {});
            }

            std::vector<std::vector<std::pair<int, int64_t>>> to_plain_graph() const
            {
                std::vector<std::vector<std::pair<int, int64_t>>> g(_AdjacencyLists.size());
                auto i = 0;
                for (const auto& s : _AdjacencyLists)
                {
                    auto& t = g[i];
                    t.reserve(s.size());

                    for (const auto& p : s)
                        t.push_back(p.to_pair());

                    ++i;
                }
                return g;
            }

            const std::vector<std::vector<WeightedVertex<Vertex, Weight>>>&
                AdjacencyLists()
                const noexcept
            {
                return _AdjacencyLists;
            }

            //TODO: p1. There is no check for duplicates.
            void AddEdge(
                const Vertex vertex0, const Vertex vertex1, const Weight weight = 1,
                const bool addReverseEdge = true)
            {
                if (vertex0 == vertex1)
                {
                    std::ostringstream ss;
                    ss << "Cannot add self loop for the vertex (" << vertex0 << ").";
                    StreamUtilities::throw_exception(ss);
                }

                const size_t vertexCount = _AdjacencyLists.size();
                CheckVertex(vertex0, vertexCount);
                CheckVertex(vertex1, vertexCount);

                const WeightedVertex<Vertex, Weight> p0(vertex1, weight);
                _AdjacencyLists[vertex0].push_back(p0);

                if (addReverseEdge)
                {
                    const WeightedVertex<Vertex, Weight> p1(vertex0, weight);
                    _AdjacencyLists[vertex1].push_back(p1);
                }
            }

        private:

            static inline void CheckVertex(const Vertex vertex, const size_t vertexCount)
            {
                if (vertex < 0 || vertexCount <= vertex)
                {
                    std::ostringstream ss;
                    ss << "The vertex (" << vertex <<
                        ") must be inclusively between 0 and " << vertexCount << ".";
                    StreamUtilities::throw_exception(ss);
                }
            }
        };
    }
}