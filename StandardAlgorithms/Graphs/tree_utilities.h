#pragma once
#include"../Numbers/disjoint_set.h"
#include"../Utilities/iota_vector.h"
#include"../Utilities/is_debug.h"
#include"../Utilities/is_integral_pure.h"
#include"../Utilities/random.h"

namespace Standard::Algorithms::Trees::Inner
{
    template<class node_t>
    [[nodiscard]] constexpr auto is_tree_dfs(const std::vector<std::vector<node_t>> &tree, const std::int32_t node,
        std::vector<bool> &visited, const std::int32_t parent = -1) -> bool
    {
        if (visited[node])
        {// cycle
            return false;
        }

        visited[node] = true;

        for (const auto &edges = tree[node]; const auto &edge : edges)
        {
            std::int32_t tod{};

            if constexpr (is_integral_pure<node_t>)
            {
                tod = edge;
            }
            else
            {// todo(p3): hack.
                tod = edge.first;
            }

            if (parent == tod)
            {
                continue;
            }

            if (!is_tree_dfs<node_t>(tree, tod, visited, node))
            {
                return false;
            }
        }

        return true;
    }

    [[nodiscard]] constexpr auto random_from_to(auto &rnd, const bool ignore_0th, std::int32_t &left,
        std::vector<std::int32_t> &availables, auto &dsu) -> std::pair<std::int32_t, std::int32_t>
    {
        for (;;)
        {
            const auto pos1 = rnd(ignore_0th ? 1 : 0, left);
            const auto pos2 = rnd(ignore_0th ? 1 : 0, left);

            if (pos1 == pos2)
            {
                continue;
            }

            const auto from = availables[pos1];
            const auto tod = availables[pos2];
            assert(from != tod);

            const auto root_from = dsu.parent(from);

            if constexpr (::Standard::Algorithms::is_debug)
            {
                const auto root_tod = dsu.parent(tod);
                assert(root_from != root_tod);
            }

            {
                [[maybe_unused]] const auto has = dsu.unite(from, tod);
                assert(has);
            }

            const auto root = dsu.parent(from);

            if constexpr (::Standard::Algorithms::is_debug)
            {
                const auto root2 = dsu.parent(tod);
                assert(root == root2);
            }

            const auto to_delete = root == root_from ? pos2 : pos1;
            std::swap(availables[to_delete], availables.back());
            availables.pop_back();

            return std::make_pair(from, tod);
        }
    }

    template<class add_edge_t, class edge_t>
    [[nodiscard]] constexpr auto build_random_tree_base(
        add_edge_t add_edge, const std::int32_t size, const bool ignore_0th) -> std::vector<std::vector<edge_t>>
    {
        require_positive(size, "size");

        std::vector<std::vector<edge_t>> result(size + (ignore_0th ? 1LL : 0LL));

        Standard::Algorithms::Utilities::random_t<std::int32_t> rnd{};

        Standard::Algorithms::Numbers::disjoint_set<std::int32_t> dsu(result.size());

        auto availables = ::Standard::Algorithms::Utilities::iota_vector<std::int32_t>(result.size());

        for (auto left = size - (ignore_0th ? 0 : 1); (ignore_0th ? 1 : 0) < left; --left)
        {
            const auto [from, tod] = random_from_to(rnd, ignore_0th, left, availables, dsu);

            add_edge(result, from, tod);
        }

        return result;
    }
} // namespace Standard::Algorithms::Trees::Inner

namespace Standard::Algorithms::Trees
{
    // 1 edge tree.
    constexpr auto min_size_tree_1based = 3;

    // When 'ignore_0th' is false, then 0 based else 1 based.
    template<class node_t>
    [[nodiscard]] constexpr auto is_tree(const std::vector<std::vector<node_t>> &tree, const bool ignore_0th = false)
        -> bool
    {
        const auto size = static_cast<std::int32_t>(tree.size());

        require_positive(size - (ignore_0th ? 1 : 0), ignore_0th ? "tree.size-1" : "tree.size");

        if (1 == size)
        {
            return tree[0].empty();
        }

        std::vector<bool> visited(size);

        if (!Inner::is_tree_dfs<node_t>(tree, ignore_0th ? 1 : 0, visited))
        {
            return false;
        }

        const auto iter = std::find(visited.cbegin() + (ignore_0th ? 1 : 0), visited.cend(), false);

        return iter == visited.cend();
    }

    // See also build_random_tree.
    template<class int_t>
    [[nodiscard]] constexpr auto build_random_tree(const std::int32_t size, const bool ignore_0th = false)
        -> std::vector<std::vector<int_t>>
    {
        const auto add_edge =
            [](std::vector<std::vector<int_t>> &graph, const std::int32_t from, const std::int32_t tod)
        {
            graph[from].push_back(tod);
            graph[tod].push_back(from);
        };

        auto result = Inner::build_random_tree_base<decltype(add_edge), int_t>(add_edge, size, ignore_0th);

        assert(is_tree(result, ignore_0th));

        return result;
    }

    template<class edge_t, class weight_t>
    [[nodiscard]] constexpr auto build_random_tree_weighted(const std::int32_t size, const weight_t min_weight = {},
        const weight_t max_weight = std::numeric_limits<weight_t>::max(), const bool ignore_0th = false)
        -> std::vector<std::vector<edge_t>>
    {
        assert(min_weight <= max_weight);

        Standard::Algorithms::Utilities::random_t<weight_t> rnd(min_weight, max_weight);

        const auto add_edge =
            [&rnd](std::vector<std::vector<edge_t>> &graph, const std::int32_t from, const std::int32_t tod)
        {
            const auto weigh = rnd();
            graph[from].emplace_back(tod, weigh);
            graph[tod].emplace_back(from, weigh);
        };

        auto result = Inner::build_random_tree_base<decltype(add_edge), edge_t>(add_edge, size, ignore_0th);

        assert(is_tree(result, ignore_0th));

        return result;
    }
} // namespace Standard::Algorithms::Trees
