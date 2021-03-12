#pragma once
#include"../Utilities/project_constants.h"
#include<algorithm>
#include<utility>

namespace Standard::Algorithms::Trees
{
    // Can be used as a locator because, once assigned, the key sticks to a node and never changes.
    template<class key_t1>
    requires(std::is_default_constructible_v<key_t1>)
    struct avl_node final
    {
        using key_t = key_t1;

        key_t key{};

        // Should be O(log(count)).
        std::int32_t height = 1;

        std::size_t count = 1U;
        avl_node *__restrict__ left{};
        avl_node *__restrict__ right{};
    };

    template<class key_t>
    constexpr void avl_update_height(avl_node<key_t> &node) noexcept
    {
        node.height =
            1 + std::max(node.left != nullptr ? node.left->height : 0, node.right != nullptr ? node.right->height : 0);

        node.count = static_cast<std::size_t>(1) + (node.left != nullptr ? node.left->count : 0U) +
            (node.right != nullptr ? node.right->count : 0U);

        assert(0 < node.height && node.height <= Standard::Algorithms::Utilities::stack_max_size && 0U < node.count);
    }
} // namespace Standard::Algorithms::Trees
