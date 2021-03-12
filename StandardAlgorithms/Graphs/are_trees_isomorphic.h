#pragma once
#include<utility>

namespace Standard::Algorithms::Trees
{
    // This is only for doubly-linked lists.
    //
    // If isomorphic, return a pair of nulls.
    // Else, the returned nodes:
    // - either have different values,
    // - or one pointer is null.
    template<class node_t, class pair_t = std::pair<const node_t *, const node_t *>>
    [[nodiscard]] constexpr auto are_trees_isomorphic(const node_t *root1, const node_t *root2) noexcept -> pair_t
    {
        while (root1 != root2)
        {
            if (nullptr == root1)
            {
                return { nullptr, root2 };
            }

            if (nullptr == root2)
            {
                return { root1, nullptr };
            }

            if (root1->value != root2->value)
            {
                return { root1, root2 };
            }

            root1 = root1->right; // todo(p3): left?
            root2 = root2->right;
        }

        return {};
    }
} // namespace Standard::Algorithms::Trees
