#pragma once
#include"leftist_tree_base.h"
#include"lhb_node.h"

namespace Standard::Algorithms::Trees
{
    // 3. The height is the min # of hops down to a leaf.
    template<class key_t1, class node_t1 = lhb_node<key_t1>, bool has_parent1 = node_t1::has_parent>
    struct leftist_height_biased_tree final
        : leftist_tree_base<leftist_height_biased_tree<key_t1, node_t1, has_parent1>, key_t1, node_t1, has_parent1>
    {
        using key_t = key_t1;
        using node_t = node_t1;

        constexpr leftist_height_biased_tree() = default;

        leftist_height_biased_tree(leftist_height_biased_tree &) = delete;
        auto operator= (leftist_height_biased_tree &) & -> leftist_height_biased_tree = delete;
        leftist_height_biased_tree(leftist_height_biased_tree &&) = delete; // todo(p3): make movable.
        auto operator= (leftist_height_biased_tree &&) & -> leftist_height_biased_tree = delete;

        constexpr ~leftist_height_biased_tree() noexcept = default;

        // Time O(log(n)).
        // todo(p3): void erase(node_t &node, const key_t &key)

        // Time O(log(n)).
        template<bool has_parent2 = has_parent1>
        constexpr auto decrease_key(node_t &node, const key_t &key) -> typename std::enable_if_t<has_parent2, void>
        {
            // todo(p2): ensure time O(log(n)).
            assert(this->top() != nullptr);

            if constexpr (::Standard::Algorithms::is_debug)
            {
                assert(this->node_exists(this->top(), &node));
            }

            if (!(key < node.key))
            {
                return;
            }

            node.key = key;

            for (auto *cur = &node;;)
            {
                assert(cur != nullptr);

                auto *parent1 = cur->parent;
                if (parent1 == nullptr)
                {
                    assert(this->top() == cur);
                    break;
                }

                if (!(key < parent1->key))
                {
                    break;
                }

                swap_with_parent(parent1, cur);

                assert(cur == parent1->parent && (cur->left == parent1 || cur->right == parent1));
            }

            assert(node.key == key);
        }

        static constexpr void merge_nodes(gsl::owner<node_t *> &root1, gsl::owner<node_t *> child
#if _DEBUG
            ,
            std::uint32_t depth = {}
#endif
            ) noexcept
        {
#if _DEBUG
            ++depth;

            assert(0U < depth && depth <= ::Standard::Algorithms::Utilities::stack_max_size);
            // todo(depth_check): depth <= ::Standard::Algorithms::Utilities::max_logn_algorithm_depth);
#endif

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

            merge_nodes(root1->right, child
#if _DEBUG
                ,
                depth
#endif
            );

            assert(root1->right != nullptr);

            if constexpr (has_parent1)
            {
                root1->right->parent = root1;

                assert(root1->left == nullptr || root1->left->parent == root1);
            }

            if (root1->left == nullptr)
            {
                std::swap(root1->left, root1->right);
                root1->height = 1;
                return;
            }

            if (root1->left->height < root1->right->height)
            {
                std::swap(root1->left, root1->right);
            }

            root1->height = root1->right->height + 1;
            assert(0 < root1->height);
        }

        static constexpr void validate_node(
            const node_t *parent1, std::queue<const node_t *> &que, const node_t *child, const std::string &message)
        {
            assert(parent1 != nullptr && parent1 != child && (parent1->left == child || parent1->right == child));

            if (child == nullptr)
            {
                return;
            }

            if (child->key < parent1->key) [[unlikely]]
            {
                throw std::out_of_range(message + " The child key is less than parent's.");
            }

            if constexpr (has_parent1)
            {
                if (child->parent != parent1) [[unlikely]]
                {
                    throw std::out_of_range(message + " A child must have a parent.");
                }
            }

            que.push(child);
        }

private:
        template<bool has_parent2 = has_parent1>
        static constexpr auto swap_with_parent(node_t *parent1, node_t *cur) ->
            typename std::enable_if_t<has_parent2, void>
        {
            assert(
                parent1 != nullptr && cur && parent1 == cur->parent && (parent1->left == cur || parent1->right == cur));

            {// parent2
                //    |
                // parent1
                //    |
                //   cur
                auto *parent2 = parent1->parent;
                assert(parent2 != parent1);

                cur->parent = parent2;
                parent1->parent = cur;

                if (parent2 != nullptr)
                {
                    if (parent2->left == parent1)
                    {
                        parent2->left = cur;
                    }
                    else
                    {
                        assert(parent2->right == parent1);

                        parent2->right = cur;
                    }
                }
            }

            if (parent1->left == cur)
            {
                //  parent1
                //   /    \;
                // cur   child2
                // / \  ..
                parent1->left = cur->left;
                cur->left = parent1;

                auto *child2 = parent1->right;
                parent1->right = cur->right;
                cur->right = child2;
            }
            else
            {
                //  parent1
                //   /    \;
                // child   cur
                //         / \  ..
                parent1->right = cur->right;
                cur->right = parent1;

                auto *child = parent1->left;
                parent1->left = cur->left;
                cur->left = child;
            }

            update_height(*parent1);
            update_height(*cur);

            assert(!(cur->height < parent1->height));
        }

        static constexpr void update_height(node_t &node) noexcept
        {
            assert(&node != node.left);
            assert(&node != node.right);
            assert(node.left != node.right || (nullptr == node.left && nullptr == node.right));

            node.height = 1 + std::min(node.left ? node.left->height : 0, node.right ? node.right->height : 0);

            assert(0 < node.height);
        }
    };
} // namespace Standard::Algorithms::Trees
