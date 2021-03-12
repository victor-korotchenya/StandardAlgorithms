#pragma once
#include <algorithm> // find_if
#include <cassert>
#include <functional> // bind

namespace Privet
{
    namespace Algorithms
    {
        namespace Graphs
        {
            template <class edge_t>
            bool is_edge_to(const edge_t& edge, const int to)
            {
                const auto& to2 = static_cast<int>(edge);
                return to2 == to;
            }

            // Weight is ignored.
            template <class digraph_t>
            bool has_edge_slow(const digraph_t& digraph, const int from, const int to)
            {
                const auto node_count = static_cast<int>(digraph.size());
                assert(from != to && from >= 0 && from < node_count&& to >= 0 && to < node_count);

                const auto& edges = digraph[from];
                // todo: p3. faster search.
                const auto pred = std::bind(is_edge_to<decltype(digraph[0][0])>, std::placeholders::_1, to);

                const auto it = std::find_if(edges.begin(), edges.end(), pred);
                const auto has = it != edges.end();
                return has;
            }

            // Return infinity_w is no edge found.
            template <class digraph_t, class weight_t, weight_t infinity_w>
            weight_t edge_weight_slow(const digraph_t& digraph, const int from, const int to)
            {
                const auto node_count = static_cast<int>(digraph.size());
                assert(from != to && from >= 0 && from < node_count&& to >= 0 && to < node_count);

                const auto& edges = digraph[from];
                const auto pred = std::bind(is_edge_to<decltype(digraph[0][0])>, std::placeholders::_1, to);
                const auto it = std::find_if(edges.begin(), edges.end(), pred);
                if (it == edges.end())
                    return infinity_w;

                const auto& e = *it;
                assert(e.Vertex == to);

                const auto& edge_cost = e.Weight;
                assert(edge_cost >= 0 && edge_cost <= infinity_w);
                return edge_cost;
            }
        }
    }
}