#pragma once
#include <algorithm>
#include <cassert>
#include <utility>
#include <vector>
#include "../Numbers/disjoint_set.h"
#include "../Utilities/Random.h"

namespace
{
    template <typename node_t>
    bool is_tree_dfs(
        const std::vector<std::vector<node_t>>& tree, std::vector<bool>& visited,
        const int node = 0, const int parent = -1)
    {
        if (visited[node])
            return false;// cycle

        visited[node] = true;

        for (const auto c : tree[node])
        {
            int to;
            if constexpr (std::is_integral_v<decltype(c)>)
            {
                to = c;
            }
            else
            {//hack.
                to = c.first;
            }

            if (parent == to)
                continue;

            if (!is_tree_dfs<node_t>(tree, visited, to, node))
                return false;
        }

        return true;
    }

    template <class add_edge_t, class edge_t>
    std::vector<std::vector<edge_t>> build_random_tree_base(add_edge_t& add_edge, const int size, const bool ignore_0th)
    {
        RequirePositive(size, "size");

        std::vector<std::vector<edge_t>> result(size + static_cast<int64_t>(ignore_0th));
        IntRandom r;
        disjoint_set<int> dsu(size + static_cast<int64_t>(ignore_0th));

        std::vector<int> availables(size + static_cast<int64_t>(ignore_0th));
        std::iota(availables.begin(), availables.end(), 0);

        for (auto left = size - static_cast<int>(!ignore_0th); left > static_cast<int>(ignore_0th); --left)
        {
            for (;;)
            {
                const auto pos1 = r(ignore_0th, left), pos2 = r(ignore_0th, left);
                if (pos1 == pos2)
                    continue;

                const auto from = availables[pos1], to = availables[pos2];
                assert(from != to);

                const auto root_from = dsu.get_parent(from);

#if _DEBUG
                {
                    const auto root_to = dsu.get_parent(to);
                    assert(root_from != root_to);

                    const auto has =
#endif
                        dsu.unite(from, to);
#if _DEBUG
                    assert(has);
                }
#endif

                const auto root = dsu.get_parent(from);
#if _DEBUG
                {
                    const auto root2 = dsu.get_parent(to);
                    assert(root == root2);
                }
#endif

                const auto to_delete = root == root_from ? pos2 : pos1;
                std::swap(availables[to_delete], availables.back());
                availables.pop_back();

                add_edge(result, from, to);
                break;
            }
        }

        return result;
    }
}

namespace Privet::Algorithms::Trees
{
    // 1 edge tree.
    constexpr auto min_size_tree_1based = 3;

    //When 'ignore_0th' is false, then 0 based else 1 based.
    template <typename node_t>
    bool is_tree(const std::vector<std::vector<node_t>>& tree, const bool ignore_0th = false)
    {
        const auto size = static_cast<int>(tree.size());
        RequirePositive(size - static_cast<int>(ignore_0th), ignore_0th ? "tree.size-1" : "tree.size");

        if (1 == size)
            return tree[0].empty();

        std::vector<bool> visited(size);
        if (!is_tree_dfs<node_t>(tree, visited, ignore_0th))
            return false;

        for (int i = ignore_0th; i < size; ++i)
            if (!visited[i])
                return false;

        return true;
    }

    // See also BuildRandomTree.
    template <class int_t>
    std::vector<std::vector<int_t>> build_random_tree(const int size, const bool ignore_0th = false)
    {
        const auto add_edge = [](std::vector<std::vector<int_t>>& graph,
            const int from, const int to)
        {
            graph[from].push_back(to);
            graph[to].push_back(from);
        };
        const auto result = build_random_tree_base<decltype(add_edge), int_t>(add_edge, size, ignore_0th);

        assert(is_tree(result, ignore_0th));
        return result;
    }

    template <class edge_t, class weight_t>
    std::vector<std::vector<edge_t>> build_random_tree_weighted(const int size,
        const weight_t min_weight = {}, const weight_t max_weight = std::numeric_limits<weight_t>::max(), const bool ignore_0th = false)
    {
        assert(min_weight <= max_weight);
        RandomGenerator<weight_t, std::uniform_int_distribution<weight_t>> rw;

        const auto add_edge = [&rw, min_weight, max_weight](std::vector<std::vector<edge_t>>& graph,
            const int from, const int to)
        {
            const auto w = rw(min_weight, max_weight);
            graph[from].emplace_back(to, w);
            graph[to].emplace_back(from, w);
        };
        const auto result = build_random_tree_base<decltype(add_edge), edge_t>(add_edge, size, ignore_0th);

        assert(is_tree(result, ignore_0th));
        return result;
    }
}