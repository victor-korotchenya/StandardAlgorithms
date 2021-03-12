#pragma once
#include"../Utilities/require_utilities.h"
#include"tree_utilities.h"
#include<array>
#include<vector>

namespace Standard::Algorithms::Trees::Inner
{
    template<class node_t>
    constexpr void dfs_tree_longest_path_down(const std::vector<std::vector<node_t>> &tree, const std::int32_t parent,
        std::vector<std::int32_t> &down_path_lengths, const std::int32_t node)
    {
        auto &len = down_path_lengths[node];

        for (const auto &children = tree[node]; const auto &child : children)
        {
            if (parent == child)
            {
                continue;
            }

            dfs_tree_longest_path_down(tree, node, down_path_lengths, child);

            len = std::max(len, down_path_lengths[child] + 1);
        }
    }

    template<class node_t>
    constexpr void dfs_tree_longest_path_up(const std::vector<std::vector<node_t>> &tree, const std::int32_t parent,
        const std::vector<std::int32_t> &down_path_lengths, const std::int32_t parent_max,
        std::vector<std::int32_t> &result, const std::int32_t node)
    {
        {
            auto &len = result[node];
            len = std::max(len, parent_max + 1);
        }

        std::array<std::int32_t, 3> second_largest{ -1, -1, -1 };

        for (const auto &children = tree[node]; const auto &child : children)
        {
            if (parent == child)
            {
                continue;
            }

            second_largest[0] = down_path_lengths[child];
            std::sort(second_largest.begin(), second_largest.end());
        }

        for (const auto &children = tree[node]; const auto &child : children)
        {
            if (parent == child)
            {
                continue;
            }

            assert(0 <= second_largest[2]);

            const auto parent_max2 = 1 +
                std::max(
                    parent_max, down_path_lengths[child] != second_largest[2] ? second_largest[2] : second_largest[1]);

            dfs_tree_longest_path_up(tree, node, down_path_lengths, parent_max2, result, child);
        }
    }

    template<class node_t>
    [[nodiscard]] constexpr auto dfs_tree_longest_path_slow(const std::int32_t parent,
        const std::vector<std::vector<node_t>> &tree, const std::int32_t node) -> std::int32_t
    {
        std::int32_t result{};

        for (const auto &children = tree[node]; const auto &child : children)
        {
            if (parent == child)
            {
                continue;
            }

            const auto temp = dfs_tree_longest_path_slow(node, tree, child);

            result = std::max(result, temp + 1);
        }

        return result;
    }

    template<class node_t, class downdepth_max_t = std::pair<std::int32_t, std::int32_t>>
    [[nodiscard]] constexpr auto tree_diameter_dfs(const std::int32_t parent,
        const std::vector<std::vector<node_t>> &tree, const std::int32_t from) -> downdepth_max_t
    {
        auto first_largest = -1;
        auto second_largest = -1;
        auto child_max = 0;

        for (const auto &tods = tree[from]; const auto &tod : tods)
        {
            if (tod == parent)
            {
                continue;
            }

            const auto tod_res = tree_diameter_dfs<node_t, downdepth_max_t>(from, tree, tod);
            child_max = std::max(child_max, tod_res.second);

            const auto &tod_depth = tod_res.first;

            if (!(tod_depth < first_largest))
            {
                second_largest = first_largest;
                first_largest = tod_depth;
            }
            else if (second_largest < tod_depth)
            {
                second_largest = tod_depth;
            }
        }

        const auto from_depth_down = first_largest + 1;
        const auto from_depth_up = first_largest + 2 + second_largest;

        return { from_depth_down, std::max(from_depth_up, child_max) };
    }
} // namespace Standard::Algorithms::Trees::Inner

namespace Standard::Algorithms::Trees
{
    // Paths start at every node - find path max length.
    // The first vertex is 0.
    template<class node_t>
    [[nodiscard]] constexpr auto tree_longest_path_slow(const std::vector<std::vector<node_t>> &tree)
        -> std::vector<std::int32_t>
    {
        const auto size = require_positive(static_cast<std::int32_t>(tree.size()), "tree size");

        std::vector<std::int32_t> result(size);

        for (std::int32_t parent{}; parent < size; ++parent)
        {
            result[parent] = Inner::dfs_tree_longest_path_slow(parent, tree, parent);
        }

        return result;
    }

    // Use 2 DFS.
    template<class node_t>
    [[nodiscard]] constexpr auto tree_longest_path(const std::vector<std::vector<node_t>> &tree)
        -> std::vector<std::int32_t>
    {
        require_positive(tree.size(), "tree size");

        std::vector<std::int32_t> result(tree.size());

        Inner::dfs_tree_longest_path_down(tree, 0, result, 0);

        const auto down_path_lengths = result;

        Inner::dfs_tree_longest_path_up(tree, 0, down_path_lengths, -1, result, 0);

        return result;
    }

    // The diameter is measured in edges.
    // 1 DFS should be faster than 2 DFS, but might require a deep stack.
    // todo(p4): use std::stack instead of the recursive tree_diameter_dfs to be deleted.
    template<class node_t>
    [[nodiscard]] constexpr auto tree_diameter(const std::vector<std::vector<node_t>> &tree) -> std::int32_t
    {
        if (tree.size() <= 1U)
        {
            return {};
        }

        auto result = Inner::tree_diameter_dfs(0, tree, 0);

        return result.second;
    }
} // namespace Standard::Algorithms::Trees
