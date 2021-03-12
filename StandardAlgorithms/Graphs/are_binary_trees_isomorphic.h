#pragma once
#include<cassert>
#include<stack>
#include<utility>

namespace Standard::Algorithms::Trees
{
    // This is only for binary trees.
    //
    // If yes, return the pair of nulls.
    // Else, the returned nodes:
    // - either have different values,
    // - or one pointer is null.
    template<class node_t, class pair_t = std::pair<const node_t *, const node_t *>>
    constexpr auto are_binary_trees_isomorphic(const node_t *const root1, const node_t *const root2) -> pair_t
    {
        if (root1 == root2)
        {
            return {};
        }

        std::stack<pair_t> sta;
        sta.emplace(root1, root2);

        auto try_emplace = [&sta](const auto *uno, const auto *duo)
        {
            if (nullptr != uno || nullptr != duo)
            {
                sta.emplace(uno, duo);
            }
        };

        do
        {
            const auto top = sta.top();
            assert(nullptr != top.first || nullptr != top.second);
            sta.pop();

            if ((nullptr == top.first && nullptr != top.second) || (nullptr != top.first && nullptr == top.second) ||
                top.first->value != top.second->value)
            {
                return top;
            }

            try_emplace(top.first->left, top.second->left);

            try_emplace(top.first->right, top.second->right);
        } while (!sta.empty());

        return {};
    }
} // namespace Standard::Algorithms::Trees
