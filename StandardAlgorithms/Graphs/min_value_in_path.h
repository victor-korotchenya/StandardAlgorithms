#pragma once
#include"../Utilities/require_utilities.h"
#include"light_heavy_tree_decomposition.h"
#include"tree_utilities.h"
#include<queue>
#include<tuple>
#include<vector>

namespace Standard::Algorithms::Trees
{
    // Given a weighted undirected tree:
    // 1) Find the min weight edge on the non-empty path a->b.
    // 2) Change some edge weight.
    // 1 based.

    // Slow time O(n) per 1 read query.
    template<class edge_t, class weight_t = decltype(edge_t{}.second)>
    constexpr auto min_value_in_path_select_slow(
        const std::int32_t from, const std::vector<std::vector<edge_t>> &tree, const std::int32_t tod) -> weight_t
    {
        const auto size = static_cast<std::int32_t>(tree.size());

        assert(!(size < min_size_tree_1based) && from != tod && 0 < from && from < size && 0 < tod && tod < size);

        assert(is_tree(tree, true));

        assert(!tree.at(from).empty() && !tree.at(tod).empty());

        // from, to, best weight
        std::queue<std::tuple<std::int32_t, std::int32_t, weight_t>> que;

        for (const auto &edges = tree[from]; const auto &edge : edges)
        {
            if (tod == edge.first)
            {
                return edge.second;
            }

            que.emplace(from, edge.first, edge.second);
        }

        do
        {
            const auto [parent, child, value] = que.front();
            que.pop();

            for (const auto &edges = tree[child]; const auto &edge : edges)
            {
                if (parent == edge.first)
                {
                    continue;
                }

                const auto new_value = std::min(value, edge.second);
                if (tod == edge.first)
                {
                    return new_value;
                }

                que.emplace(child, edge.first, new_value);
            }
        } while (!que.empty());

        throw std::runtime_error("min_value_in_path_slow error: The tree must be connected.");
    }

    // Slow.
    template<class edge_t, class weight_t>
    constexpr void min_value_in_path_update_slow(
        const std::int32_t from, std::vector<std::vector<edge_t>> &tree, const std::int32_t tod, const weight_t weight)
    {
        const auto size = static_cast<std::int32_t>(tree.size());

        assert(3 <= size && from != tod && 0 < from && from < size && 0 < tod && tod < size);

        assert(is_tree(tree, true));

        assert(!tree.at(from).empty() && !tree.at(tod).empty());

        const auto update = [&tree, weight](const std::int32_t node, const std::int32_t node2)
        {
            auto &edges = tree[node];

            auto ite = std::find_if(edges.begin(), edges.end(),
                [node2](const auto &edge)
                {
                    return node2 == edge.first;
                });

            if (ite == edges.end()) [[unlikely]]
            {
                throw std::runtime_error("Cannot find tree edge " + std::to_string(node) + "-" + std::to_string(node2));
            }

            ite->second = weight;
        };

        update(from, tod);
        update(tod, from);
    }
} // namespace Standard::Algorithms::Trees
