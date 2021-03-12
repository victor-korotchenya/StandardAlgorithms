#pragma once
#include <numeric>
#include <vector>
#include "../Utilities/ExceptionUtilities.h"
#include "tree_utilities.h"

namespace Privet::Algorithms::Trees
{
    // A centroid node of a tree t: max size of a subtree <= floor(|t.nodes|/2).
    // Centroid tree height is O(log(n)).
    // Time O(n*log(n)).
    template<class edge_t, bool set_distances = false>
    struct centroid_decomposition
    {
        centroid_decomposition(const std::vector<std::vector<edge_t>>& tree, const bool ignore_0th = false)
            : tree(tree), centroid_tree(tree.size()), subtree_sizes(tree.size()),
            parents(tree.size()* static_cast<unsigned>(set_distances), -1),
            parent_distances(tree.size()* static_cast<unsigned>(set_distances)),
            visited_centroids(tree.size()), root(ignore_0th)
        {
            RequirePositive(static_cast<int>(tree.size()) - static_cast<int>(ignore_0th), ignore_0th ? "tree.size-1" : "tree.size");
            assert(is_tree(tree, ignore_0th));

            root = build_centroid_tree(root);
            if constexpr (set_distances)
                parent_distances[root] = 0;
#if _DEBUG
            {
                const auto marked = std::accumulate(visited_centroids.begin(), visited_centroids.end(), size_t(), [](const size_t s, const bool b) {
                    return s + static_cast<unsigned>(b);
                    });
                assert(marked + static_cast<int>(ignore_0th) == tree.size());
            }
#endif
        }

        std::vector<std::vector<int>>& get_centroid_tree() noexcept
        {
            return centroid_tree;
        }

        template <typename = std::enable_if_t<set_distances>>
        std::vector<int>& get_parents() noexcept
        {
            return parents;
        }

        template <typename = std::enable_if_t<set_distances>>
        std::vector<int>& get_parent_distances() noexcept
        {
            return parent_distances;
        }

        int get_root() const noexcept
        {
            return root;
        }

        virtual ~centroid_decomposition() = default;

        // Reuse unused memory.
        std::vector<int>& get_temp() noexcept
        {
            return subtree_sizes;
        }

    private:
        void dfs_subtree_sizes(const int node, const int par = -1, const int depth = 1)
        {
            assert(node >= 0 && node < tree.size() && !visited_centroids[node]);

            subtree_sizes[node] = 1;

            if constexpr (set_distances)
                parent_distances[node] = depth;

            const auto& edges = tree[node];
            for (const auto& edge : edges)
            {
                const auto id = static_cast<int>(edge);
                if (id == par || visited_centroids[id])
                    continue;

                dfs_subtree_sizes(id, node, depth + 1);

                subtree_sizes[node] += subtree_sizes[id];
                assert(1 < subtree_sizes[node] && 0 < subtree_sizes[id] && subtree_sizes[node] <= tree.size());
            }
        }

        int find_centroid(int node, int n)
        {
            assert(n > 0);
            n >>= 1;

            auto par = -1;
            for (;;)
            {
                assert(node >= 0 && node < tree.size() && !visited_centroids[node]);
                auto changed = false;
                const auto& edges = tree[node];

                for (const auto& edge : edges)
                {
                    const auto id = static_cast<int>(edge);
                    if (id != par && !visited_centroids[id] && subtree_sizes[id] > n)
                    {
                        changed = true;
                        par = node;
                        node = id;
                        break;
                    }
                }

                if (!changed)
                    return node;
            }
        }

        int build_centroid_tree(int node)
        {
            assert(node >= 0 && node < tree.size() && !visited_centroids[node]);
            dfs_subtree_sizes(node);
            node = find_centroid(node, subtree_sizes[node]);
            assert(node >= 0 && node < tree.size() && !visited_centroids[node]);

            visited_centroids[node] = true;

            const auto& edges = tree[node];

            for (size_t i = 0; i < edges.size(); ++i)
            {
                const auto& edge = edges[i];
                const auto id = static_cast<int>(edge);
                if (visited_centroids[id])
                    continue;

                const auto subtree_centroid = build_centroid_tree(id);
                assert(subtree_centroid != node);

                centroid_tree[node].push_back(subtree_centroid);
                centroid_tree[subtree_centroid].push_back(node);

                if constexpr (set_distances)
                {
                    parents[subtree_centroid] = node;
                }
            }

            return node;
        }

        const std::vector<std::vector<edge_t>>& tree;

        std::vector<std::vector<int>> centroid_tree;
        std::vector<int> subtree_sizes;

        // Exist only when (set_distances == true).
        std::vector<int> parents, parent_distances;

        std::vector<bool> visited_centroids;
        int root;
    };
}