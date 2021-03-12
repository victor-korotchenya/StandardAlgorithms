#pragma once
#include "lowest_common_ancestor.h"

namespace Privet::Algorithms::Trees
{
    // Light heavy undirected tree decomposition in O(n*log(n)*log(n)).
    // A heavy edge goes to a child with max descendant count; rest are light edges.
    // Nodes start from 1.
    template <class edge_t, class get_to_t //= int(*)(const edge_t&)
    >
        struct light_heavy_tree_decomposition final : public lowest_common_ancestor<edge_t, get_to_t>
    {
        explicit light_heavy_tree_decomposition(get_to_t get_to) : lowest_common_ancestor<edge_t, get_to_t>(get_to)
        {
        }

        void dfs(const int root = 1) override
        {
            const auto size = this->n() + 1;
            assert(root > 0 && root < size);

            subtree_sizes.assign(size, 1);
            lowest_common_ancestor<edge_t, get_to_t>::dfs(root);

            node_to_chain.resize(size);
            chain_heads.reserve(size);
            chain_heads.assign(1, 0);
            node_visit_orders.assign(size, 0);
            visit_order = 0;

            dfs2(root);
        }

        const std::vector<int>& get_node_visit_orders() const
        {
            return node_visit_orders;
        }

        // Time O(log(n) * O(sum_op)).
        // Note. Update can be applied directly to the sum tree.
        template<class sum_op_t, class weight_t>
        void query_up(const int parent, int cur, sum_op_t& sum_op, weight_t& ans) const
        {
            assert(parent > 0 && parent <= this->n() && cur > 0 && cur <= this->n());
            assert(this->depths[parent] <= this->depths[cur]);

            const auto& par_chain = node_to_chain[parent];
            for (;;)
            {
                const auto& chi_chain = node_to_chain[cur];
                if (chi_chain != par_chain)
                {
                    const auto& head = chain_heads[chi_chain];
                    const auto& order_min = node_visit_orders[head],
                        & order_max = node_visit_orders[cur];
                    sum_op(order_min - 1, order_max, ans);

                    cur = this->parents[head][0];
                    assert(cur > 0 && cur <= this->n());
                    continue;
                }

                if (cur != parent)
                {
                    const auto& order_min = node_visit_orders[parent],
                        & order_max = node_visit_orders[cur];
                    sum_op(order_min, order_max, ans);
                }

                return;
            }
        }

    protected:
        void visit_child(const int current, const int child) override
        {
            assert(child > 0 && current > 0 && current != child);
            subtree_sizes[current] += subtree_sizes[child];
        }

    private:
        void dfs2(const int cur, const int prev = 0)
        {
            assert(cur > 0 && cur <= this->n() && prev >= 0 && cur != prev && prev <= this->n());

            if (!chain_heads.back())
                chain_heads.back() = cur;

            node_to_chain[cur] = static_cast<int>(chain_heads.size()) - 1;
            node_visit_orders[cur] = ++visit_order;

            const auto& nodes = this->tree[cur];
            auto largest_id = -1;
            for (auto i = 0, size2 = 0; i < static_cast<int>(nodes.size()); ++i)
            {
                const auto& to = this->get_to(nodes[i]);
                assert(subtree_sizes[to] > 0);

                if (to != prev && size2 < subtree_sizes[to])
                    largest_id = i, size2 = subtree_sizes[to];
            }

            auto largest_descendant = 0;
            if (largest_id >= 0)
            {
                const auto& node = nodes[largest_id];
                const auto& to = this->get_to(node);
                largest_descendant = to;
                dfs2(to, cur);
            }

            for (const auto& node : nodes)
            {
                const auto& to = this->get_to(node);
                if (to != prev && to != largest_descendant)
                {
                    chain_heads.push_back(0);
                    dfs2(to, cur);
                }
            }
        }

        std::vector<int> subtree_sizes, node_to_chain, chain_heads, node_visit_orders;
        int visit_order = 0;
    };
}