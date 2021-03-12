#pragma once
#include"../Numbers/disjoint_set.h"
#include"../Utilities/require_utilities.h"
#include"tree_utilities.h"
#include<vector>

namespace Standard::Algorithms::Trees
{
    // Given m>0 LCA requests [{from, to}], and a rooted tree on n>0 nodes,
    // Tarjan finds lowest/min common/shared ancestor in time O(n+m).
    // Nodes start from 1.
    template<class edge_t,
        // to, LCA
        class request_t,
        class get_to_t //= std::int32_t(*)(const edge_t&)
        >
    struct lowest_common_ancestor_offline_fast final
    {
        constexpr lowest_common_ancestor_offline_fast(const std::vector<std::vector<edge_t>> &tree,
            std::vector<std::vector<request_t>> &lca_requests, get_to_t get_to, std::int32_t one_based_root = 1)
            : Tree(tree)
            , Lca_requests(lca_requests)
            , Visited(tree.size())
            , Dsu(static_cast<std::int32_t>(tree.size()))
            , Get_tod(get_to)
        {
            assert(1U < tree.size() && !lca_requests.empty() && 0 < one_based_root &&
                static_cast<std::size_t>(one_based_root) < tree.size());

            assert(is_tree(tree, true));

            dfs(one_based_root);
        }

private:
        constexpr void dfs(const std::int32_t cur, const std::int32_t prev = 0)
        {
            assert(0 < cur && static_cast<std::size_t>(cur) < Tree.size() && !(prev < 0) &&
                static_cast<std::size_t>(prev) < Tree.size() && cur != prev);

            for (const auto &chis = Tree[cur]; const auto &chi : chis)
            {
                const auto tod = Get_tod(chi);

                if (tod == prev)
                {
                    continue;
                }

                dfs(tod, cur);

                Dsu.unite(cur, tod);
                Dsu.self_parent(cur);
            }

            auto &requests = Lca_requests[cur];

            for (auto &request : requests)
            {
                const auto &tod = request.first;
                if (Visited[tod])
                {
                    request.second = Dsu.parent(tod);
                }
            }

            assert(!Visited[cur]);
            Visited[cur] = true;
        }

        const std::vector<std::vector<edge_t>> &Tree;

        std::vector<std::vector<request_t>> &Lca_requests;
        std::vector<bool> Visited;
        Standard::Algorithms::Numbers::disjoint_set<std::int32_t> Dsu;
        get_to_t Get_tod;
    };
} // namespace Standard::Algorithms::Trees
