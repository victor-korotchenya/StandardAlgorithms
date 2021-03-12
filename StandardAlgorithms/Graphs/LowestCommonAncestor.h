#pragma once
#include <vector>
#include "../Utilities/ExceptionUtilities.h"

namespace
{
    template <typename t, typename first_time_t, typename tour_t>
    struct et_context final
    {
        const std::vector<std::vector<t>>& tree;
        first_time_t& first_time;
        tour_t& tour;
        int time = 0;

        et_context(const std::vector<std::vector<t>>& tree, first_time_t& first_time, tour_t& tour)
            : tree(tree), first_time(first_time), tour(tour)
        {
        }
    };

    template <typename t, typename first_time_t, typename tour_t>
    void dfs_et(et_context<t, first_time_t, tour_t>& context, const int node = 0, const int parent = -1)
    {
        context.tour[context.time] = node;
        context.first_time[node] = context.time++;
        for (const auto& child : context.tree[node])
        {
            if (parent == child)
                continue;
            dfs_et(context, child, node);
            context.tour[context.time++] = node;
        }
    }
}

namespace Privet::Algorithms::Trees
{
    // See also "lowest_common_ancestor".
    //
    // Naive implementation of Lowest Common Ancestor LCA of a binary tree.
    template <typename T>
    T* simple_binary_lca(const T* root, const T* a, const T* b)
    {
        if (nullptr == root || a == root || b == root)
        {
            return const_cast<T*>(root);
        }

        T* left = simple_binary_lca(root->Left, a, b);
        T* right = simple_binary_lca(root->Right, a, b);

        T* result = nullptr == left
            ? right
            : (nullptr == right ? left : const_cast<T*>(root));
        return result;
    }

    //zero based index.
    template <typename t, typename first_time_t, typename tour_t>
    void euler_tour(const std::vector<std::vector<t>>& tree, first_time_t& first_time, tour_t& tour)
    {
        RequirePositive(tree.size(), "tree size");

        et_context<t, first_time_t, tour_t> context(tree, first_time, tour);
        dfs_et(context);
    }
}