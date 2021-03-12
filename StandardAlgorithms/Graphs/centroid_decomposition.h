#pragma once
#include"../Utilities/require_utilities.h"
#include"tree_utilities.h"
#include<numeric> // accumulate
#include<tuple>
#include<vector>

namespace Standard::Algorithms::Trees
{
    // A centroid node of a tree t: max size of a subtree <= floor(|t.nodes|/2).
    // Centroid tree height is O(log(n)).
    // Time O(n*log(n)).
    template<class edge_t, bool shall_set_distances = false>
    struct centroid_decomposition
    {
        // Build time O(n*log(n)).
        constexpr explicit centroid_decomposition(
            const std::vector<std::vector<edge_t>> &tree, const bool ignore_0th = false)
            : Tree(tree)
            , Centroid_tree(tree.size())
            , Subtree_sizes(tree.size())
            , Parents(tree.size() * static_cast<std::uint32_t>(shall_set_distances), -1)
            , Parent_distances(tree.size() * static_cast<std::uint32_t>(shall_set_distances))
            , Visited_centroids(tree.size())
            , Root(ignore_0th ? 1 : 0)
        {
            require_positive(static_cast<std::int32_t>(tree.size()) - static_cast<std::int32_t>(ignore_0th),
                ignore_0th ? "tree.size-1" : "tree.size");

            if constexpr (::Standard::Algorithms::is_debug)
            {
                assert(is_tree(tree, ignore_0th));
            }

            Root = build_centroid_tree(Root);

            if constexpr (shall_set_distances)
            {
                Parent_distances[Root] = 0;
            }

            if constexpr (::Standard::Algorithms::is_debug)
            {
                const auto marked = std::accumulate(Visited_centroids.cbegin(), Visited_centroids.cend(), std::size_t{},
                    [] [[nodiscard]] (const std::size_t sum1, const bool boo)
                    {
                        return sum1 + static_cast<std::uint32_t>(boo);
                    });

                assert(marked + static_cast<std::int32_t>(ignore_0th) == tree.size());
            }
        }

        constexpr centroid_decomposition(const centroid_decomposition &) = default;
        constexpr auto operator= (const centroid_decomposition &) & -> centroid_decomposition & = default;
        constexpr centroid_decomposition(centroid_decomposition &&) noexcept = default;
        constexpr auto operator= (centroid_decomposition &&) &noexcept -> centroid_decomposition & = default;

        constexpr virtual ~centroid_decomposition() noexcept = default;

        // todo(p2): Is copy-paste required in modern C++?
        /*
        template<class ded_using>
        [[nodiscard]] constexpr auto centroid_tree(this ded_using &&ded) noexcept -> auto
        {
            return std::forward<ded_using>(ded).Centroid_tree;
        }
        */

        [[nodiscard]] constexpr auto centroid_tree() const &noexcept -> const std::vector<std::vector<std::int32_t>> &
        {
            return Centroid_tree;
        }

        [[nodiscard]] constexpr auto centroid_tree() &noexcept -> std::vector<std::vector<std::int32_t>> &
        {
            return Centroid_tree;
        }

        [[nodiscard]] constexpr auto root() const noexcept -> std::int32_t
        {
            return Root;
        }

        // Reuse unused memory.
        [[nodiscard]] constexpr auto temp() const &noexcept -> const std::vector<std::int32_t> &
        {
            return Subtree_sizes;
        }

        [[nodiscard]] constexpr auto temp() &noexcept -> std::vector<std::int32_t> &
        {
            return Subtree_sizes;
        }

        // These methods exist only when (shall_set_distances == true).
        template<bool shall_2 = shall_set_distances>
        [[nodiscard]] constexpr auto parents() const &noexcept ->
            typename std::enable_if_t<shall_2, const std::vector<std::int32_t> &>
        {
            return Parents;
        }

        template<bool shall_2 = shall_set_distances>
        [[nodiscard]] constexpr auto parents() &noexcept ->
            typename std::enable_if_t<shall_2, std::vector<std::int32_t> &>
        {
            return Parents;
        }

        template<bool shall_2 = shall_set_distances>
        [[nodiscard]] constexpr auto parent_distances() const &noexcept ->
            typename std::enable_if_t<shall_2, const std::vector<std::int32_t> &>
        {
            return Parent_distances;
        }

        template<bool shall_2 = shall_set_distances>
        [[nodiscard]] constexpr auto parent_distances() &noexcept ->
            typename std::enable_if_t<shall_2, std::vector<std::int32_t> &>
        {
            return Parent_distances;
        }

private:
        constexpr void dfs_subtree_sizes(const std::tuple<std::int32_t, std::int32_t, std::int32_t> &node_parent_depth)
        {
            const auto &node = std::get<0>(node_parent_depth);
            const auto &par = std::get<1>(node_parent_depth);
            const auto &depth = std::get<2>(node_parent_depth);

            assert(!(node < 0) && static_cast<std::size_t>(node) < Tree.size() && !Visited_centroids[node]);

            Subtree_sizes[node] = 1;

            if constexpr (shall_set_distances)
            {
                Parent_distances[node] = depth;
            }

            const auto &edges = Tree[node];

            for (const auto &edge : edges)
            {
                const auto ide = static_cast<std::int32_t>(edge);
                if (ide == par || Visited_centroids[ide])
                {
                    continue;
                }

                dfs_subtree_sizes({ ide, node, depth + 1 });

                Subtree_sizes[node] += Subtree_sizes[ide];

                assert(2 <= Subtree_sizes[node] && 0 < Subtree_sizes[ide] &&
                    static_cast<std::size_t>(Subtree_sizes[node]) <= Tree.size());
            }
        }

        [[nodiscard]] constexpr auto find_centroid(std::pair<std::int32_t, std::int32_t> node_size) const noexcept
            -> std::int32_t
        {
            auto &node = node_size.first;
            auto &size1 = node_size.second;
            assert(0 < size1);

            size1 /= 2;

            for (auto par = -1;;)
            {
                assert(!(node < 0) && static_cast<std::size_t>(node) < Tree.size() && !Visited_centroids[node]);

                const auto &edges = Tree[node];
                auto changed = false;

                for (const auto &edge : edges)
                {
                    const auto ide = static_cast<std::int32_t>(edge);

                    if (ide != par && !Visited_centroids[ide] && size1 < Subtree_sizes[ide])
                    {
                        changed = true;
                        par = node;
                        node = ide;
                        break;
                    }
                }

                if (!changed)
                {
                    return node;
                }
            }
        }

        [[nodiscard]] constexpr auto build_centroid_tree(std::int32_t node) -> std::int32_t
        {
            assert(!(node < 0) && static_cast<std::size_t>(node) < Tree.size() && !Visited_centroids[node]);

            {
                constexpr auto par = -1;
                constexpr auto depth = 1;
                dfs_subtree_sizes({ node, par, depth });
            }

            node = find_centroid({ node, Subtree_sizes[node] });

            assert(!(node < 0) && static_cast<std::size_t>(node) < Tree.size() && !Visited_centroids[node]);

            Visited_centroids[node] = true;

            const auto &edges = Tree[node];

            for (const auto &edge : edges)
            {
                const auto ide = static_cast<std::int32_t>(edge);
                if (Visited_centroids[ide])
                {
                    continue;
                }

                const auto subtree_centroid = build_centroid_tree(ide);
                assert(subtree_centroid != node);

                Centroid_tree[node].push_back(subtree_centroid);
                Centroid_tree[subtree_centroid].push_back(node);

                if constexpr (shall_set_distances)
                {
                    Parents[subtree_centroid] = node;
                }
            }

            return node;
        }

        const std::vector<std::vector<edge_t>> &Tree;

        std::vector<std::vector<std::int32_t>> Centroid_tree;
        std::vector<std::int32_t> Subtree_sizes;


        // todo(p3): Exist only when (true == shall_set_distances).
        std::vector<std::int32_t> Parents;
        std::vector<std::int32_t> Parent_distances;


        std::vector<bool> Visited_centroids;
        std::int32_t Root;
    };
} // namespace Standard::Algorithms::Trees
