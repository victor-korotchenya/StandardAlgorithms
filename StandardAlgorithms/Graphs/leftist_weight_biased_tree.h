#pragma once
#include"../Utilities/is_debug.h"
#include"leftist_tree_base.h"
#include"lhb_node.h"

namespace Standard::Algorithms::Trees
{
    // 3. The height is the # of subtree nodes including the node.
    template<class key_t1, class node_t1 = lhb_node<key_t1>>
    struct leftist_weight_biased_tree final
        : leftist_tree_base<leftist_weight_biased_tree<key_t1, node_t1>, key_t1, node_t1, false // has_parent
              >
    {
        using key_t = key_t1;
        using node_t = node_t1;

        static_assert(!node_t::has_parent, "Node erase, key decrease are slow O(n).");

        constexpr leftist_weight_biased_tree() = default;

        leftist_weight_biased_tree(leftist_weight_biased_tree &) = delete;
        auto operator= (leftist_weight_biased_tree &) & -> leftist_weight_biased_tree = delete;
        leftist_weight_biased_tree(leftist_weight_biased_tree &&) = delete; // todo(p3): make movable.
        auto operator= (leftist_weight_biased_tree &&) & -> leftist_weight_biased_tree = delete;

        constexpr ~leftist_weight_biased_tree() noexcept = default;

        static constexpr void merge_nodes(gsl::owner<node_t *> &root1, gsl::owner<node_t *> child) noexcept
        {
            if (child == nullptr)
            {
                return;
            }

            assert(root1 != child && 0 < child->height);

            if (root1 == nullptr)
            {
                root1 = child;
                return;
            }

            assert(0 < root1->height);

            if (child->key < root1->key || (root1->height < child->height && child->key == root1->key))
            {
                std::swap(root1, child);
            }

            merge_nodes_both(root1, child);
        }

        static constexpr void validate_node(
            const node_t *parent1, std::queue<const node_t *> &que, const node_t *child, const std::string &message)
        {
            assert(parent1 != nullptr && parent1 != child && (parent1->left == child || parent1->right == child));

            assert(0 < parent1->height);

            if (child == nullptr)
            {
                return;
            }

            if (child->key < parent1->key) [[unlikely]]
            {
                throw std::out_of_range(message + " The child key is less than parent's.");
            }

            if (!(child->height < parent1->height)) [[unlikely]]
            {
                throw std::out_of_range(message + " The child height " + std::to_string(child->height) +
                    " must be less than parent's " + std::to_string(parent1->height));
            }

            que.push(child);
        }

private:
        static constexpr void merge_nodes_both(node_t *root1, node_t *child) noexcept
        {
            assert(root1 != nullptr && child != nullptr && root1 != child && 0 < root1->height && 0 < child->height);

            [[maybe_unused]] std::int32_t depth{};

            for (;;)
            {
                if constexpr (::Standard::Algorithms::is_debug)
                {
                    ++depth;
                    assert(0 < depth);
                }

                assert(root1 != nullptr && child != nullptr && root1 != child);

                root1->height += child->height;
                assert(0 < root1->height);

                if (root1->left == nullptr)
                {
                    assert(root1->right == nullptr);
                    root1->left = child;
                    return;
                }

                if (root1->right == nullptr)
                {
                    if (root1->left->height < child->height)
                    {
                        root1->right = root1->left;
                        root1->left = child;
                    }
                    else
                    {
                        root1->right = child;
                    }

                    return;
                }

                const auto rc_hei = root1->right->height + child->height;
                const auto shall_swap = root1->left->height < rc_hei;
                if (shall_swap)
                {
                    std::swap(root1->left, root1->right);
                }

                auto &chosen_branch = shall_swap ? root1->left : root1->right;
                if (!(child->key < chosen_branch->key))
                {
                    root1 = chosen_branch;
                    continue;
                }

                std::swap(chosen_branch, child);
                root1 = chosen_branch;
            }
        }
    };
} // namespace Standard::Algorithms::Trees
