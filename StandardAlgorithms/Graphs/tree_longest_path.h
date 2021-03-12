#pragma once
#include <algorithm>
#include <cassert>
#include <vector>
#include "../Utilities/ExceptionUtilities.h"
#include "tree_utilities.h"

namespace
{
    template <typename node_t>
    void dfs_tree_longest_path_down(const std::vector<std::vector<node_t>>& tree,
        std::vector<int>& down_path_lengths,
        const int node = 0, const int parent = -1)
    {
        auto& len = down_path_lengths[node];

        for (const auto& child : tree[node])
        {
            if (parent == child)
                continue;

            dfs_tree_longest_path_down(tree, down_path_lengths, child, node);

            len = std::max(len, down_path_lengths[child] + 1);
        }
    }

    template <typename node_t>
    void dfs_tree_longest_path_up(const std::vector<std::vector<node_t>>& tree, const std::vector<int>& down_path_lengths,
        std::vector<int>& result,
        const int node = 0, const int parent = -1, const int parent_max = -1)
    {
        {
            auto& len = result[node];
            len = std::max(len, parent_max + 1);
        }

        int second_largest[3] = { -1,-1,-1 };
        for (const auto& child : tree[node])
        {
            if (parent == child)
                continue;

            second_largest[0] = down_path_lengths[child];
            std::sort(second_largest, second_largest + 3);
        }

        for (const auto& child : tree[node])
        {
            if (parent == child)
                continue;

            // todo. p1. check?
            assert(0 <= second_largest[2]);

            const auto parent_max2 = std::max(
                parent_max,
                down_path_lengths[child] != second_largest[2]
                ? second_largest[2] : second_largest[1]) + 1;

            dfs_tree_longest_path_up(tree, down_path_lengths, result, child, node, parent_max2);
        }
    }

    template <typename node_t>
    int dfs_tree_longest_path_slow(const std::vector<std::vector<node_t>>& tree, const int node, const int parent = -1)
    {
        auto result = 0;

        for (const auto& child : tree[node])
        {
            if (parent == child)
                continue;
            const auto temp = dfs_tree_longest_path_slow(tree, child, node);
            result = std::max(result, temp + 1);
        }

        return result;
    }
}

namespace Privet::Algorithms::Trees
{
    //first index is 0.
    template <typename node_t>
    std::vector<int> tree_longest_path_slow(const std::vector<std::vector<node_t>>& tree)
    {
        RequirePositive(tree.size(), "size");
        const auto size = static_cast<int>(tree.size());
        std::vector<int> result(size);

        for (auto i = 0; i < size; ++i)
        {
            result[i] = dfs_tree_longest_path_slow(tree, i);
        }

        return result;
    }

    //Paths start at every node - find max len.
    template <typename node_t>
    std::vector<int> tree_longest_path(const std::vector<std::vector<node_t>>& tree)
    {
        RequirePositive(tree.size(), "size");

        std::vector<int> result(tree.size());
        dfs_tree_longest_path_down(tree, result);

        const auto down_path_lengths = result;
        dfs_tree_longest_path_up(tree, down_path_lengths, result);
        return result;
    }
}