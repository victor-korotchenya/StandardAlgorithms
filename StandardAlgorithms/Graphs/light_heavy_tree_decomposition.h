#pragma once
#include"lowest_common_ancestor.h"

namespace Standard::Algorithms::Trees
{
    // Light heavy undirected tree decomposition in O(n*log(n)*log(n)).
    // A hefty edge goes to a child with max descendant count; rest are light edges.
    // Nodes start from 1.
    template<class edge_t, class get_to_t //= std::int32_t(*)(const edge_t&)
        >
    struct light_heavy_tree_decomposition final : public lowest_common_ancestor<edge_t, get_to_t>
    {
        constexpr explicit light_heavy_tree_decomposition(get_to_t get_to = {})
            : lowest_common_ancestor<edge_t, get_to_t>(get_to)
        {
        }

        constexpr void dfs(const std::int32_t one_based_root) override
        {
            const auto size = this->n() + 1;
            assert(0 < one_based_root && one_based_root < size);

            Subtree_sizes.assign(size, 1);
            lowest_common_ancestor<edge_t, get_to_t>::dfs(one_based_root);

            node_to_chain.resize(size);
            Chain_heads.reserve(size);
            Chain_heads.assign(1, 0);
            Node_visit_orders.assign(size, 0);
            Visit_order = 0;

            dfs2(one_based_root);
        }

        [[nodiscard]] constexpr auto node_visit_orders() const &noexcept -> const std::vector<std::int32_t> &
        {
            return Node_visit_orders;
        }

        // Time O(log(n) * O(sum_op)).
        // Update can be applied directly to the sum tree.
        template<class sum_op_t, class weight_t>
        constexpr void query_up(const std::int32_t parent, std::int32_t cur, sum_op_t &sum_op, weight_t &ans) const
        {
            assert(0 < parent && parent <= this->n() && 0 < cur && cur <= this->n());

            assert(this->depths()[parent] <= this->depths()[cur]);

            const auto &par_chain = node_to_chain[parent];
            for (;;)
            {
                const auto &chi_chain = node_to_chain[cur];
                if (chi_chain != par_chain)
                {
                    const auto &head = Chain_heads[chi_chain];
                    const auto &order_min = Node_visit_orders[head];
                    const auto &order_max = Node_visit_orders[cur];
                    sum_op(order_min - 1, order_max, ans);

                    cur = this->parents()[head][0];
                    assert(0 < cur && cur <= this->n());

                    continue;
                }

                if (cur != parent)
                {
                    const auto &order_min = Node_visit_orders[parent];
                    const auto &order_max = Node_visit_orders[cur];
                    sum_op(order_min, order_max, ans);
                }

                return;
            }
        }

protected:
        constexpr void visit_child(const std::int32_t current, const std::int32_t child) override
        {
            assert(0 < child && 0 < current && current != child);

            Subtree_sizes[current] += Subtree_sizes[child];
        }

private:
        constexpr void dfs2(const std::int32_t cur, const std::int32_t prev = 0)
        {
            assert(
                0 < cur && cur <= this->n() && !(prev < 0) && cur != prev && prev <= this->n() && !Chain_heads.empty());

            if (Chain_heads.back() == 0)
            {
                Chain_heads.back() = cur;
            }

            node_to_chain[cur] = static_cast<std::int32_t>(Chain_heads.size()) - 1;
            Node_visit_orders[cur] = ++Visit_order;

            const auto &nodes = this->tree()[cur];
            auto largest_id = -1;

            for (std::int32_t index{}, size2{}; index < static_cast<std::int32_t>(nodes.size()); ++index)
            {
                const auto &tod = this->get_to()(nodes[index]);
                assert(0 < Subtree_sizes[tod]);

                if (tod != prev && size2 < Subtree_sizes[tod])
                {
                    largest_id = index;
                    size2 = Subtree_sizes[tod];
                }
            }

            std::int32_t largest_descendant{};

            if (0 <= largest_id)
            {
                const auto &node = nodes[largest_id];
                const auto &tod = this->get_to()(node);
                largest_descendant = tod;
                dfs2(tod, cur);
            }

            for (const auto &node : nodes)
            {
                const auto &tod = this->get_to()(node);

                if (tod != prev && tod != largest_descendant)
                {
                    Chain_heads.push_back(0);
                    dfs2(tod, cur);
                }
            }
        }

        std::vector<std::int32_t> Subtree_sizes{};
        std::vector<std::int32_t> node_to_chain{};
        std::vector<std::int32_t> Chain_heads{};
        std::vector<std::int32_t> Node_visit_orders{};
        std::int32_t Visit_order{};
    };
} // namespace Standard::Algorithms::Trees
