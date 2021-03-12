#pragma once
#include <algorithm>
#include <cassert>
#include <vector>
#include "../Utilities/ExceptionUtilities.h"
#include "../Numbers/bit_utilities.h"

namespace
{
    template <class graph_t, class u_number_t>
    int count_edges_cover(const graph_t& graph,
        std::vector<std::vector<bool>>& vis,
        const u_number_t mask)
    {
        const auto node_count = graph.size();
        for (u_number_t from = 0; from < node_count; ++from)
            vis[from].assign(node_count, false);

        auto covered_edges = 0;
        for (u_number_t from = 0; from < node_count; ++from)
        {
            if (!((mask >> from) & 1u))
                continue;

            for (const auto& to : graph[from])
            {
                const auto mi = std::min<u_number_t>(from, to), ma = std::max<u_number_t>(from, to);
                if (!vis[mi][ma])
                {// Mark an edge.
                    vis[mi][ma] = true;
                    ++covered_edges;
                }
            }
        }

        return covered_edges;
    }

    // Return the mask if a subset of cand_vc nodes, that is a vertex cover, exists.
    // Return 0 if not.
    template<class graph_t, class u_number_t, int max_nodes>
    u_number_t vertex_cover(const graph_t& graph,
        std::vector<std::vector<bool>>& vis,
        const int edge_count, const int cand_vc)
    {
        assert(edge_count > 0 && cand_vc > 0);

        const auto node_count = graph.size();
        assert(node_count > 0 && node_count < max_nodes&& cand_vc <= node_count);

        constexpr u_number_t one = 1u;
        const u_number_t max_mask = one << node_count;
        u_number_t mask = (one << cand_vc) - one;
        do
        {
            const auto covered_edges = count_edges_cover<graph_t, u_number_t>(graph, vis, mask);
            if (covered_edges == edge_count)
                return mask;

            assert(covered_edges >= 0 && covered_edges < edge_count);

            const auto next_mask = Privet::Algorithms::Numbers::next_greater_same_pop_count(mask);
            mask = next_mask;
        } while (mask < max_mask);

        return {};
    }
}

namespace Privet
{
    namespace Algorithms
    {
        namespace Graphs
        {
            // Simple undirected graph, might be disconnected.
            // Nodes start from 0.
            // Vertex cover: for every edge either node is selected.
            //
            // TODO: p2. When node degree is 1, include the edge other node - the graph had better have node degrees at least 2.
            // Time complexity: O(2**n * n * log(n)).
            template<class graph_t, class u_number_t, int max_nodes = (sizeof(u_number_t) << 3) - 1>
            void minimum_vertex_cover(const graph_t& graph,
                const int edge_count,
                std::vector<int>& result,
                std::vector<std::vector<bool>>& vis)
            {
                static_assert(std::is_unsigned_v<u_number_t> && max_nodes > 0 && max_nodes <= (sizeof(u_number_t) << 3) - 1);
                result.clear();

                const auto node_count = static_cast<int>(graph.size());
                RequireNotGreater(node_count, max_nodes, "node_count");

                assert(edge_count <= node_count * (node_count - 1ll) >> 1);
                if (node_count <= 1 || edge_count <= 0)
                    return;

                vis.resize(node_count);

                u_number_t best_mask{};
                auto left = 0, right = node_count;
                while (left < right)
                {
                    const auto mid = (left + right) >> 1;
                    const auto mask = mid ? vertex_cover<graph_t, u_number_t, max_nodes>(graph, vis, edge_count, mid) : 0;
                    if (mask)
                        right = mid, best_mask = mask;
                    else
                        left = mid + 1;
                }

                assert(left && best_mask);
                for (auto i = 0; i < node_count; ++i)
                {
                    if ((best_mask >> i) & 1u)
                        result.push_back(i);
                }
            }
        }
    }
}