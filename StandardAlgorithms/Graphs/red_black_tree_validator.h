#pragma once
#include"../Utilities/require_utilities.h"
#include<unordered_map>

namespace Standard::Algorithms::Trees
{
    // Ensure the leaves have the same black height; no two reds in a row.
    // Note. Whether the keys are sorted and unique isn't checked here.
    template<class node_t>
    struct red_black_tree_validator final
    {
        constexpr red_black_tree_validator(const node_t &sentinel, std::size_t leaf_black_depth, const node_t &root)
            : sentinel(validate_sentinel(sentinel))
            , leaf_black_depth(require_positive(leaf_black_depth, "leaf black depth"))
            , node_heights{ std::make_pair(&root, 1ZU) }
        {
            if (&sentinel == &root) [[unlikely]]
            {
                throw std::runtime_error("The root cannot be the sentinel.");
            }
        }

        constexpr void operator() (const node_t &parent, const node_t &child)
        {
            if (&parent == &sentinel) [[unlikely]]
            {
                throw std::runtime_error("A parent is equal to the sentinel.");
            }

            if (&child == &sentinel) [[unlikely]]
            {
                throw std::runtime_error("A child is equal to the sentinel.");
            }

            if (&parent == &child) [[unlikely]]
            {
                throw std::runtime_error("A parent is equal to its child.");
            }

            if (parent.parent == &sentinel) [[unlikely]]
            {
                throw std::runtime_error("The parent's parent is equal to the sentinel.");
            }

            if (&parent != child.parent) [[unlikely]]
            {
                throw std::runtime_error("A child's parent must be equal to its actual parent.");
            }

            throw_if_null("parent's left", parent.left);
            throw_if_null("parent's right", parent.right);
            throw_if_null("child's left", child.left);
            throw_if_null("child's right", child.right);

            if (parent.left != &child && parent.right != &child) [[unlikely]]
            {
                throw std::runtime_error("A child must be the parent's left or right node.");
            }

            if (parent.left == parent.right) [[unlikely]]
            {
                throw std::runtime_error("The parent's children must be different nodes.");
            }

            if (parent.is_red && child.is_red) [[unlikely]]
            {
                throw std::runtime_error("A red parent has a red child.");
            }

            const auto parent_height = find_parent_height(parent);
            update_height(parent_height, child);
        }

private:
        [[nodiscard]] static constexpr auto validate_sentinel(const node_t &sentinel) -> const node_t &
        {
            throw_if_not_null("sentinel's parent", sentinel.parent);
            throw_if_not_null("sentinel's left", sentinel.left);
            throw_if_not_null("sentinel's right", sentinel.right);

            if (sentinel.is_red) [[unlikely]]
            {
                throw std::runtime_error("The sentinel is red.");
            }

            return sentinel;
        }

        [[nodiscard]] constexpr auto find_parent_height(const node_t &parent) const -> std::size_t
        {
            assert(!node_heights.empty());

            const auto piter = node_heights.find(&parent);

            if (piter == node_heights.end()) [[unlikely]]
            {
                throw std::runtime_error("The parent black height should have been set before its child.");
            }

            auto parent_height = require_positive(piter->second, "parent black height");

            return parent_height;
        }

        constexpr void update_height(const std::size_t parent_height, const node_t &child)
        {
            assert(0U < parent_height && 0U < leaf_black_depth);

            if (const auto citer = node_heights.find(&child); citer != node_heights.end()) [[unlikely]]
            {
                auto err = "The child black height (" + std::to_string(citer->second) +
                    ") is already set. Parent black height is " + std::to_string(parent_height) + ".";

                throw std::runtime_error(err);
            }

            const auto child_height =
                require_positive(parent_height + (child.is_red ? 0ZU : 1ZU), "computed child height");

            if (const auto is_leaf = child.left == &sentinel && child.right == &sentinel; is_leaf)
            {
                if (leaf_black_depth == child_height) [[likely]]
                {
                    return;
                }

                auto err = "The child black height (" + std::to_string(child_height) + ") must be " +
                    std::to_string(leaf_black_depth);

                throw std::runtime_error(err);
            }

            node_heights[&child] = child_height;
        }

        const node_t &sentinel;
        const std::size_t leaf_black_depth;

        // It is faster to pass in the current black depth in BFS/DFS,
        // but here it is simpler to reuse the existing code.
        std::unordered_map<const node_t *, std::size_t> node_heights;
    };
} // namespace Standard::Algorithms::Trees
