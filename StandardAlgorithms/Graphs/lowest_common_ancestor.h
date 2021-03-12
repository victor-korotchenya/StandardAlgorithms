#pragma once
#include <cassert>
#include <vector>
#include "bits/stdc++.h"
#include "tree_utilities.h"
#include "../Numbers/disjoint_set.h"

namespace Privet::Algorithms::Trees
{
    // Return "to".
    template <class node_t>
    int refl(const node_t& a)
    {
        return a;
    }

    // Lowest common ancestor LCA for a tree in O(n * log(n)).
    // Nodes start from 1.
    // Usage sample:
    //
    // auto size = 32;
    // lowest_common_ancestor<int> lca2(refl<int>);
    // auto &tree = lca2.reset(size);
    template <class edge_t, class get_to_t// = int(*)(const edge_t&)
    >
        struct lowest_common_ancestor
    {
        explicit lowest_common_ancestor(get_to_t get_to) : get_to(get_to)
        {
        }

        virtual ~lowest_common_ancestor() = default;

        std::vector<std::vector<edge_t>>& reset(const int n)
        {
            assert(n >= 2);

            tree.clear();
            tree.resize(n + 1ll);
            return tree;
        }

        virtual void dfs(const int root = 1)
        {
            assert(root > 0 && root <= n());

            const auto size = n() + 1ll;
            firsts.resize(size);
            lasts.resize(size);
            depths.resize(size);

            parents.clear();
            parents.resize(size);

            tour.clear();
            tour.reserve(size << 1);

            time = 0;
            dfs1(root);
        }

        int lca(int u, int v) const
        {
            assert(u > 0 && v > 0);
            if (depths[u] > depths[v])
                std::swap(u, v);

            equal_depth(u, v);
            if (u == v)
                return v;

            move_up(u, v);
            return parents[v][0];
        }

        // todo: p2. test is_parent_child - not used.
        bool are_parent_child(const int parent, const int child) const
        {
            assert(parent >= 0 && parent <= n() && child >= 0 && child <= n() && parent != child);
            const auto r = firsts[parent] <= firsts[child] &&
                lasts[child] <= lasts[parent];
            return r;
        }

        // todo: p2. test lca_distance - not used.
        int lca_distance(const int a, const int b) const
        {
            assert(a >= 0 && a <= n() && b >= 0 && b <= n());
            const auto c = lca(a, b);
            const auto r = depths[a] + depths[b] - 2 * depths[c];
            return r;
        }

        // todo: p2. test is_path - not used.
        //a <- b <- c
        //a -> b -> c
        bool is_path(const int a, const int b, const int c) const
        {
            const auto ac = lca_distance(a, c),
                ab = lca_distance(a, b),
                bc = lca_distance(b, c);
            const auto r = ac == ab + bc;
            return r;
        }

        // todo: p2. test first_meet_point - not used.
        // Return the first meet point of 2 paths: (s..a), (w..a).
        int first_meet_point(int s, int w, const int a) const
        {
            assert(s != w || s == w && w == a);
            if (depths[w] < depths[s])
                std::swap(s, w);
            // depth[s] <= depth[w]

            // s <- a <- w
            if (are_parent_child(s, a) && are_parent_child(a, w))
                return a;

            //     sw - a
            //     ^^
            //   /  |
            // s    |
            //      w
            const auto sw = lca(s, w);
            if (is_path(s, sw, a) && is_path(w, sw, a))
                return sw;

            // 1 - s
            // |
            // aw
            // ^^
            // | \
                    // a  w
                    //
                    // s <- w <- a
            const auto aw = lca(a, w);
            if (is_path(s, aw, a))
                return aw;

            //1 - w
            // \
                    //  as
                    //  ^^
                    //  | \
                    //  a   s
            const auto as = lca(a, s);
            if (is_path(w, as, a))
                return as;

            throw std::runtime_error("must not occur.");
        }

        // todo: p2. test count_shared_nodes - not used.
        int count_shared_nodes(const int s, const int w, int const a) const
        {
            const auto meet_point = first_meet_point(s, w, a);
            const auto r = lca_distance(meet_point, a) + 1;
            return r;
        }

        const std::vector<std::vector<int>>& get_parents() const
        {
            return parents;
        }

        const std::vector<int>& get_depths() const
        {
            return depths;
        }

        std::vector<int>& get_tour()
        {
            return tour;
        }

    protected:
        int n() const
        {
            assert(tree.size());
            const auto r = static_cast<int>(tree.size()) - 1;
            return r;
        }

        virtual void visit_child(const int current, const int child)
        {}

        get_to_t get_to;
        std::vector<std::vector<edge_t>> tree;

        // Sparse table for log(n) retrieval.
        std::vector<std::vector<int>> parents;
        std::vector<int> firsts, lasts, depths;

    private:
        void equal_depth(const int u, int& v) const
        {
            auto diff = depths[v] - depths[u];
            assert(parents[v].size() && diff >= 0);

            while (diff > 0)
            {
                const auto bit = __builtin_ctz(static_cast<unsigned>(diff));
                const auto i = 1 << bit;
                assert(i & diff && bit < static_cast<int>(parents[v].size()));
                v = parents[v][bit];
                diff ^= i;
            }

            assert(parents[v].size() && depths[u] == depths[v]);
        }

        void move_up(int u, int& v) const
        {
            auto i = static_cast<int>(parents[v].size()) - 1;
            do
            {
                assert(parents[u].size() && parents[u].size() == parents[v].size());
                i = std::min(i, static_cast<int>(parents[u].size()) - 1);
                const auto& au = parents[u][i], & av = parents[v][i];
                if (au != av)
                    u = au, v = av;

                assert(parents[u].size() && parents[u].size() == parents[v].size());
            } while (--i >= 0);
        }

        void set_parents(const int cur, int prev)
        {
            assert(cur != prev && cur > 0 && prev >= 0);

            auto& ps = parents[cur];
            ps.push_back(prev);

            for (auto i = 0u; ; ++i)
            {
                const auto& ps2 = parents[prev];
                if (i >= ps2.size())
                    break;

                ps.push_back(prev = ps2[i]);

                assert(cur != prev && prev >= 0);
            }
        }

        void dfs1(const int cur, const int prev = 0)
        {
            assert(cur > 0 && cur <= n() && prev >= 0 && prev <= n() && cur != prev);

            tour.push_back(cur);
            firsts[cur] = ++time;
            depths[cur] = depths[prev] + 1;

            set_parents(cur, prev);

            for (const auto& chi : tree[cur])
            {
                const auto to = get_to(chi);
                if (to == prev)
                    continue;

                dfs1(to, cur);
                tour.push_back(cur);
                visit_child(cur, to);
            }

            lasts[cur] = ++time;
        }

        int time = 0;
        std::vector<int> tour;
    };

    // Given m>0 LCA requests [{from,to}], and a rooted tree on n>0 nodes,
    // Tarjan finds lowest/min common/shared ancestor in time O(n+m).
    // Nodes start from 1.
    template <class edge_t,
        //to,LCA
        class request_t,
        class get_to_t //= int(*)(const edge_t&)
    >
        struct lowest_common_ancestor_offline_fast final
    {
        lowest_common_ancestor_offline_fast(const std::vector<std::vector<edge_t>>& tree,
            std::vector<std::vector<request_t>>& lca_requests,
            get_to_t get_to, int root = 1)
            : tree(tree), lca_requests(lca_requests), visited(tree.size()), dsu(static_cast<int>(tree.size())), get_to(get_to)
        {
            assert(tree.size() >= 2 && lca_requests.size() && root > 0 && root < tree.size() && is_tree(tree, true));

            dfs(root);
        }

    private:
        void dfs(const int cur, const int prev = 0)
        {
            assert(cur > 0 && cur < tree.size() && prev >= 0 && prev < tree.size() && cur != prev);

            for (const auto& chi : tree[cur])
            {
                const auto to = get_to(chi);
                if (to == prev)
                    continue;

                dfs(to, cur);

                dsu.unite(cur, to);
                dsu.set_parent(cur);
            }

            auto& requests = lca_requests[cur];
            for (auto& request : requests)
            {
                const auto& to = request.first;
                if (visited[to])
                    request.second = dsu.get_parent(to);
            }

            assert(!visited[cur]);
            visited[cur] = true;
        }

        const std::vector<std::vector<edge_t>>& tree;

        std::vector<std::vector<request_t>>& lca_requests;
        std::vector<bool> visited;
        disjoint_set<int> dsu;
        get_to_t get_to;
    };
}