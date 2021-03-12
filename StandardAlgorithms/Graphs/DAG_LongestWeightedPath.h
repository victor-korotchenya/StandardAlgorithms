#pragma once
#include <cstdint>
#include <vector>
#include "WeightedVertex.h"

namespace Privet
{
    namespace Algorithms
    {
        namespace Graphs
        {
            using dag_t = std::vector<std::vector<Privet::Algorithms::WeightedVertex<int, int>>>;

            //Cost from any to any vertex.
            //There must be at least 1 edge.
            //Vertices are [0..(n - 1)].
            //The n must be >= 2.
            std::int64_t dag_longest_weighted_path(const dag_t& dag, const int n);

            //The returned value is the maximum cost from source to target.
            //When there is no path, return numeric_limits<int64_t>::min().
            //When "target_vertex" is -1, any vertex, reachable from the source, can be used.
            std::int64_t dag_longest_weighted_path_slow(
                const dag_t& dag, const int n,
                const int source_vertex,
                const int target_vertex = -1);

            // The max/min cost in DAG with non-negative weihts:
            // just use the DFS as the topological sort isn't needed!
            std::int64_t dag_longest_weighted_path(
                const dag_t& dag, const int n,
                const int source_vertex,
                const int target_vertex = -1);
        }
    }
}
