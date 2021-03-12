#pragma once
#include<cassert>

namespace Standard::Algorithms::Trees
{
    template<class node_t, class func_t>
    [[nodiscard]] constexpr auto left_rotate(node_t &xxx, func_t update_func) -> node_t *
    {
        auto *zzz = xxx.right;
        assert(zzz != nullptr);

        xxx.right = zzz->left;
        zzz->left = &xxx;

        update_func(xxx);
        update_func(*zzz);
        return zzz;
    }

    template<class node_t, class func_t>
    [[nodiscard]] constexpr auto right_rotate(node_t &zzz, func_t update_func) -> node_t *
    {
        auto *xxx = zzz.left;
        assert(xxx != nullptr);
        //       z6                x4
        //       /  \               /  \;
        //     x4   7           2   z6
        //     / \        -->         / \;
        //    2  y5              y5   7

        zzz.left = xxx->right;
        xxx->right = &zzz;

        update_func(zzz);
        update_func(*xxx);
        return xxx;
    }

    template<class node_t>
    [[nodiscard]] constexpr auto left_rotate(node_t &xxx) -> node_t *
    {
        const auto fun = [](node_t &)
        {
        };
        return left_rotate<node_t, decltype(fun)>(xxx, fun);
    }

    template<class node_t>
    [[nodiscard]] constexpr auto right_rotate(node_t &zzz) -> node_t *
    {
        const auto fun = [](node_t &)
        {
        };
        return right_rotate<node_t, decltype(fun)>(zzz, fun);
    }
} // namespace Standard::Algorithms::Trees
