#pragma once

#include <cassert>
#include <queue>
#include <stdexcept>
#include <vector>
#include "../Utilities/ExceptionUtilities.h"

namespace Privet
{
    namespace Algorithms
    {
        namespace Graphs
        {
            //Return the vertices sorted in the topological order.
            //Vertices must be from 0 to (size - 1).
            //If the directed graph is not acyclic, throw an exception.
            template <typename graph_t>
            std::vector<int> topological_sort(const graph_t& dag, const int size)
            {
                RequirePositive(size, "digraph.size.");

                std::vector<int> result, in_degrees(size);
                result.reserve(size);

                for (auto i = 0; i < size; i++)
                {
                    for (const auto& u : dag[i])
                    {
                        const auto vertex = static_cast<const int>(u);
                        assert(0 <= vertex && vertex < size);
                        ++in_degrees[vertex];
                    }
                }

                std::queue<int> zero_degree_vertices;
                for (auto i = 0; i < size; ++i)
                {
                    if (0 == in_degrees[i])
                        zero_degree_vertices.push(i);
                }

                while (!zero_degree_vertices.empty())
                {
                    const auto vertex = zero_degree_vertices.front();
                    zero_degree_vertices.pop();
                    result.push_back(vertex);

                    for (const auto& u : dag[vertex])
                    {
                        const auto vertex_to = static_cast<const int>(u);
                        if (0 == --in_degrees[vertex_to])
                            zero_degree_vertices.push(vertex_to);
                    }
                }

                if (result.size() != static_cast<size_t>(size))
                    throw std::runtime_error("The graph must be acyclic.");

                return result;
            }
        }
    }
}
