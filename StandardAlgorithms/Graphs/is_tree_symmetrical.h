#pragma once
#include<cassert>
#include<stack>
#include<utility>

namespace Standard::Algorithms::Trees
{
    template<class node_t, class pair_t = std::pair<const node_t *, const node_t *>>
    [[nodiscard]] constexpr auto is_tree_symmetrical(const node_t *const root)
    {
        if (nullptr == root)
        {
            return true;
        }

        std::stack<pair_t> pairs;
        pairs.emplace(root, root);

        do
        {
            const auto top = pairs.top();

            if (top.first->value != top.second->value ||
                (nullptr == top.first->left) != (nullptr == top.second->right) ||
                (nullptr == top.first->right) != (nullptr == top.second->left))
            {
                return false;
            }

            pairs.pop();

            if (nullptr != top.first->left)
            // Skip unnecessary check for second node.
            {
                pairs.emplace(top.first->left, top.second->right);
            }

            if (nullptr != top.first->right)
            {
                pairs.emplace(top.first->right, top.second->left);
            }
        } while (!pairs.empty());

        return true;
    }
} // namespace Standard::Algorithms::Trees
