#pragma once
#include"../Utilities/project_constants.h"
#include"../Utilities/w_stream.h"
#include<cstdint>
#include<functional>
#include<stdexcept>
#include<string>

namespace Standard::Algorithms::Trees::Inner
{
    template<class key_t, class node_t, bool use_extra_func, class extra_func_t>
    constexpr void bst_validate_impl(const key_t &min, const node_t &node, const key_t &max,
        const node_t *const sentinel, extra_func_t extra_func, std::uint32_t depth = {})
    {
        if (::Standard::Algorithms::Utilities::stack_max_size < ++depth) [[unlikely]]
        {
            throw std::runtime_error("Too deep stack in BST validation.");
        }

        if (node.key < min || max < min || max < node.key) [[unlikely]]
        {
            auto str = ::Standard::Algorithms::Utilities::w_stream();
            str << "Error at node key '" << node.key << "', max '" << max << "', min '" << min << "'.";

            throw std::runtime_error(str.str());
        }

        if (node.left != sentinel)
        {
            if (node.left->key == node.key) [[unlikely]]
            {
                auto str = ::Standard::Algorithms::Utilities::w_stream();
                str << "Duplicate key '" << node.key << "', left '" << node.left->key << "'.";

                throw std::runtime_error(str.str());
            }

            if constexpr (use_extra_func)
            {
                extra_func(node, *node.left);
            }

            bst_validate_impl<key_t, node_t, use_extra_func, extra_func_t>(
                min, *node.left, node.key, sentinel, extra_func, depth);
        }

        if (node.right != sentinel)
        {
            if (node.right->key == node.key) [[unlikely]]
            {
                auto str = ::Standard::Algorithms::Utilities::w_stream();
                str << "Duplicate key '" << node.key << "', right '" << node.right->key << "'.";

                throw std::runtime_error(str.str());
            }

            if constexpr (use_extra_func)
            {
                extra_func(node, *node.right);
            }

            bst_validate_impl<key_t, node_t, use_extra_func, extra_func_t>(
                node.key, *node.right, max, sentinel, extra_func, depth);
        }
    }
} // namespace Standard::Algorithms::Trees::Inner

namespace Standard::Algorithms::Trees
{
    template<class key_t, class node_t, bool use_extra_func = false,
        class extra_func_t = std::function<void(const node_t &parent, const node_t &child)>>
    constexpr void bst_validate(const key_t &min, const node_t &node, const key_t &max,
        const node_t *const sentinel = nullptr, extra_func_t extra_func = {})
    {
        Inner::bst_validate_impl<key_t, node_t, use_extra_func, extra_func_t>(min, node, max, sentinel, extra_func);
    }

    template<class key_t, class node_t, bool use_extra_func = false,
        class extra_func_t = std::function<void(const node_t &parent, const node_t &child)>>
    constexpr void bst_validate(
        const node_t *const root, const node_t *const sentinel = nullptr, extra_func_t extra_func = {})
    {
        if (root == sentinel)
        {
            return;
        }

        assert(root != nullptr);

        auto *min_node = root;

        while (min_node->left != sentinel)
        {
            min_node = min_node->left;
        }

        auto *max_node = root;

        while (max_node->right != sentinel)
        {
            max_node = max_node->right;
        }

        bst_validate<key_t, node_t, use_extra_func, extra_func_t>(
            min_node->key, *root, max_node->key, sentinel, extra_func);
    }
} // namespace Standard::Algorithms::Trees
