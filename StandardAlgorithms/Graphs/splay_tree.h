#pragma once
#include"binary_tree_utilities.h"
#include"bst_rotate.h"
#include"bst_validate.h"
#include<cassert>
#include<limits>
#include<memory>
#include<stdexcept>
#include<string>
#include<utility>

namespace Standard::Algorithms::Trees
{
    template<class key_t>
    struct splay_node final
    {
        key_t key{};
        splay_node *left{};
        splay_node *right{};
    };

    // Splay tree is BST.
    // No key duplicates.
    // Place the last found nodes near the root - good for repetitive searches.
    // Halve the depth of the nodes along the path to the root.
    // Reading through all n values in [1..n] sequentially takes Theta(n) time.
    // Bad for: real-time, read-only storage, multi-threading.
    // todo(p3): It works for small tree height <= stack_max_size. Use RAM for 1.0E9 nodes.
    // O(n) worst, O(log(n)) amortized time per operation.
    template<class key_t1, class node_t1 = splay_node<key_t1>>
    struct splay_tree final
    {
        using key_t = key_t1;
        using node_t = node_t1;

        constexpr splay_tree() noexcept = default;

        splay_tree(splay_tree &) = delete;
        auto operator= (splay_tree &) & -> splay_tree = delete;
        splay_tree(splay_tree &&) = delete; // todo(p3): make movable.
        auto operator= (splay_tree &&) & -> splay_tree = delete;

        constexpr ~splay_tree() noexcept
        {
            if (root_node != nullptr)
            {
                free_tree<node_t>(root_node);
            }
        }

        // Return the closest node.
        constexpr auto find(const key_t &key) -> node_t *
        {
            root_node =
                static_cast<gsl::owner<node_t *>>(root_node == nullptr ? root_node : dangerous_splay(root_node, key));
            return static_cast<node_t *>(root_node);
        }

        constexpr auto insert(const key_t &key) noexcept(false) -> std::pair<node_t *, bool>
        {
            if (!(node_count < ::Standard::Algorithms::Utilities::stack_max_size)) [[unlikely]]
            {
                throw std::runtime_error("Cannot add more nodes due to dangerous_splay.");
            }

            if (root_node == nullptr)
            {
                auto owner = new_node(key);
                assert(owner != nullptr && node_count == 0U);

                root_node = static_cast<gsl::owner<node_t *>>(owner.release());
                ++node_count;

                return std::make_pair(static_cast<node_t *>(root_node), true);
            }

            root_node = static_cast<gsl::owner<node_t *>>(dangerous_splay(root_node, key));

            assert(root_node != nullptr && 0U < node_count);

            if (root_node->key == key)
            {
                return std::make_pair(static_cast<node_t *>(root_node), false);
            }

            auto owner = new_node(key);
            auto *node = owner.get();

            if (key < root_node->key)
            {
                node->right = root_node;
                node->left = root_node->left;
                root_node->left = nullptr;
            }
            else
            {
                node->left = root_node;
                node->right = root_node->right;
                root_node->right = nullptr;
            }

            std::swap(root_node, node);
            owner.release();
            ++node_count;

            return std::make_pair(static_cast<node_t *>(root_node), true);
        }

        constexpr auto erase(const key_t &key) -> bool
        {
            root_node =
                static_cast<gsl::owner<node_t *>>(root_node == nullptr ? nullptr : dangerous_splay(root_node, key));

            if (root_node == nullptr || root_node->key != key)
            {
                return false;
            }

            node_t *new_root = nullptr;
            if (root_node->left == nullptr)
            {
                new_root = root_node->right;
            }
            else
            {
                // All keys of root_node->left are less than key.
                assert(root_node->left->key < key);

                new_root = dangerous_splay(root_node->left, key);
                assert(new_root != nullptr && new_root->right == nullptr);

                new_root->right = root_node->right;
            }

            assert(0U < node_count && root_node != new_root);

            delete root_node;
            std::swap(root_node, new_root);
            --node_count;

            return true;
        }

        //        todo(p3): void merge(splay_tree &other)
        //        {
        //            if (this == &other)
        //            {
        //                return;
        //            }

        constexpr void validate(const key_t &min = std::numeric_limits<key_t>::min(),
            const key_t &max = std::numeric_limits<key_t>::max()) const
        {
            assert(min <= max);

            if (root_node == nullptr)
            {
                return;
            }

            bst_validate(min, *root_node, max);
        }

private:
        // Place the found node at the root.
        static constexpr auto dangerous_splay(
            gsl::not_null<node_t *> root, const key_t &key, const std::uint32_t depth = {}) noexcept(false) -> node_t *
        {
            assert(root != nullptr);

            if (::Standard::Algorithms::Utilities::stack_max_size < depth) [[unlikely]]
            {
                throw std::runtime_error("Too deep stack in dangerous_splay.");
            }

            if (root->key == key)
            {
                return root;
            }

            if (key < root->key)
            {
                if (root->left == nullptr)
                {
                    return root;
                }

                if (key < root->left->key)
                {
                    //      root
                    //      /
                    //    left
                    //    /
                    // ?
                    if (root->left->left != nullptr)
                    {
                        root->left->left = dangerous_splay(root->left->left, key, depth + 1U);
                        root = right_rotate(*root);
                        // todo(p3): efficient double rotation instead of 2 singles?
                    }
                }
                else if (root->left->key < key)
                {
                    if (root->left->right != nullptr)
                    {
                        root->left->right = dangerous_splay(root->left->right, key, depth + 1);
                        root->left = left_rotate(*(root->left));
                    }
                }

                return right_rotate(*root);
            }

            // Here root->key < key

            if (root->right == nullptr)
            {
                return root;
            }

            if (root->right->key < key)
            {
                // root
                //   \;
                //    right
                //      \;
                //        ?
                if (root->right->right != nullptr)
                {
                    root->right->right = dangerous_splay(root->right->right, key, depth + 1U);
                    root = left_rotate(*root);
                }
            }
            else if (key < root->right->key)
            {
                if (root->right->left != nullptr)
                {
                    root->right->left = dangerous_splay(root->right->left, key, depth + 1U);
                    root->right = right_rotate(*(root->right));
                }
            }

            return left_rotate(*root);
        }

        [[nodiscard]] static constexpr auto new_node(const key_t &key) noexcept(false) -> std::unique_ptr<node_t>
        {
            auto owner = std::make_unique<node_t>();
            owner->key = key;

            return owner;
        }

        gsl::owner<node_t *> root_node{};
        std::size_t node_count{};
    };
} // namespace Standard::Algorithms::Trees
