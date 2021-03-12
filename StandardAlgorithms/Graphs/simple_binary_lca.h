#pragma once
#include<cassert>

namespace Standard::Algorithms::Trees
{
    // See also "lowest_common_ancestor.h".
    //
    // Naive implementation of Lowest Common Ancestor LCA of a binary tree
    // todo(p3): stack overflow?.
    template<class t>
    constexpr auto simple_binary_lca(const t *root, const t *one, const t *two) noexcept -> const t *
    {
        if (nullptr == root || one == root || two == root)
        {
            return root;
        }

        assert(one != nullptr && two != nullptr);

        //        if (one == two)
        //        {
        //            return one;
        //        }

        const auto *const left = simple_binary_lca(root->left, one, two);
        const auto *const right = simple_binary_lca(root->right, one, two);

        const auto *const result = nullptr == left ? right : (nullptr == right ? left : root);
        return result;
    }
} // namespace Standard::Algorithms::Trees
