#pragma once
#include <queue>
#include <tuple>
#include <vector>
#include "../Utilities/ExceptionUtilities.h"
#include "tree_utilities.h"
#include "light_heavy_tree_decomposition.h"

namespace Privet::Algorithms::Trees
{
    // Given a weighted undirected tree:
    // 1) Find the min weight edge on the non-empty path a->b.
    // 2) Change some edge weight.
    // 1 based.

    // Slow time O(n) per 1 read query.
    template <class edge_t, class weight_t = decltype(edge_t().second)>
    weight_t min_value_in_path_select_slow(const std::vector<std::vector<edge_t>>& tree, const int from, const int to)
    {
        const auto size = static_cast<int>(tree.size());
        assert(size >= min_size_tree_1based && from != to && 0 < from && from < size && 0 < to && to < size&& is_tree(tree, true) && tree[from].size() && tree[to].size());

        // from,to,best weight
        std::queue<std::tuple<int, int, weight_t>> q;

        for (const auto& edge : tree[from])
            if (to == edge.first)
                return edge.second;
            else
                q.push(std::make_tuple(from, edge.first, edge.second));

        do
        {
            const auto [parent, child, value] = q.front();
            q.pop();

            const auto& edges = tree[child];
            for (const auto& edge : edges)
            {
                if (parent == edge.first)
                    continue;

                const auto new_value = std::min(value, edge.second);
                if (to == edge.first)
                    return new_value;

                q.push(std::make_tuple(child, edge.first, new_value));
            }
        } while (q.size());

        throw std::runtime_error("min_value_in_path_slow error: The tree must be connected.");
    }

    // Slow.
    template <class edge_t, class weight_t>
    void min_value_in_path_update_slow(std::vector<std::vector<edge_t>>& tree, const int from, const int to, const weight_t weight)
    {
        const auto size = static_cast<int>(tree.size());
        assert(size > 2 && from != to && 0 < from && from < size && 0 < to && to < size&& is_tree(tree, true) && tree[from].size() && tree[to].size());

        const auto update = [&tree, weight](const int node, const int node2) {
            auto& edges = tree[node];
            auto it = find_if(edges.begin(), edges.end(), [node2](const auto& edge) {
                return node2 == edge.first;
                });
            if (it == edges.end())
                throw std::runtime_error("Cannot find tree edge " + std::to_string(node) + "-" + std::to_string(node2));
            it->second = weight;
        };

        update(from, to);
        update(to, from);
    }
}