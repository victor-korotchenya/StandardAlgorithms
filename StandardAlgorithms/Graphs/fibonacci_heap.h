#pragma once
#include"../Utilities/is_debug.h"
#include"fib_node.h"
#include"heap_common.h"
#include<cstdint>
#include<gsl/gsl>
#include<memory>

namespace Standard::Algorithms::Heaps
{
    // Minimum Fibonacci heap.
    template<class key_t, class node_t = fib_node<key_t>,
        std::int32_t max_log = ::Standard::Algorithms::Utilities::max_logn_algorithm_depth>
    requires(0 < max_log && max_log <= ::Standard::Algorithms::Utilities::max_logn_algorithm_depth)
    struct fibonacci_heap final
    {
        constexpr fibonacci_heap() = default;

        constexpr ~fibonacci_heap() noexcept
        {
            if (root_node != nullptr)
            {
                delete_nodes(root_node);
            }
        }

        // Time O(1).
        [[nodiscard]] constexpr auto size() const noexcept
        {
            return node_count;
        }

        // Time O(1).
        [[nodiscard]] constexpr auto top() const &noexcept
        {
            return static_cast<node_t *>(root_node);
        }

        // Time O(1).
        constexpr auto push(const key_t &key) &
        {
            auto owner = std::make_unique<node_t>();
            auto node = owner.get();
            node->key = key;
            node->left = node->right = node;

            merge_nodes(root_node, owner.release());
            ++node_count;
            assert(root_node != nullptr && 0U < node_count);

            return node;
        }

        // Time O(1).
        constexpr void merge(fibonacci_heap &other) &noexcept
        {
            if (other.root_node == nullptr || this == &other)
            {
                return;
            }

            assert(other.root_node != nullptr && 0U < other.node_count);

            merge_nodes(root_node, other.root_node);
            node_count += other.node_count;

            other.root_node = {};
            other.node_count = {};
        }

        // Beware of the key duplicates!
        // Average time O(1).
        constexpr void decrease_key(node_t &node, const key_t &key) &
        {
            assert(root_node != nullptr && 0U < node_count);

            if constexpr (::Standard::Algorithms::is_debug)
            {
                assert(node_exists(root_node, &node));
            }

            if (!(key < node.key))
            {
                return;
            }

            node.key = key;

            auto parent = node.parent;
            if (parent == nullptr)
            {
                if (key < root_node->key)
                {
                    root_node = static_cast<gsl::owner<node_t *>>(&node);
                }

                return;
            }

            if (!(key < parent->key))
            {
                return;
            }

            cut(root_node, &node);
            node.parent = nullptr;

            while (parent != nullptr && parent->marked)
            {
                cut(root_node, parent);

                auto par2 = parent;
                parent = parent->parent;
                par2->parent = nullptr;
            }

            if (parent != nullptr && parent->parent != nullptr) // Not a root.
            {
                parent->marked = true;
            }

            assert(node.key == key);
        }

        // O(log(n)) double linked list nodes, and average time.
        constexpr void pop()
        {
            assert(root_node != nullptr && 0U < node_count);

            auto new_root = pop_root(root_node);
            delete root_node;
            root_node = static_cast<gsl::owner<node_t *>>(new_root);
            --node_count;
        }

        constexpr void print(std::string &descr) const
        {
            assert((root_node == nullptr) == (node_count == 0U));

            descr += std::to_string(node_count);
            descr += " nodes in the Fibonacci heap.\n";

            if (root_node == nullptr)
            {
                return;
            }

            print_heap_helper(root_node, descr, shall_continue);
        }

        constexpr void validate() const
        {
            auto node_test = [](const node_t *const, const node_t *const)
            {
                // todo(p2): nothing to check for now.
            };

            validate_heap<true>(root_node, node_count, shall_continue, node_test);
        }

        fibonacci_heap(fibonacci_heap &) = delete;
        auto operator= (fibonacci_heap &) & -> fibonacci_heap & = delete;
        // todo(p3): make movable.
        fibonacci_heap(fibonacci_heap &&) = delete;
        auto operator= (fibonacci_heap &&) & -> fibonacci_heap & = delete;

private:
        inline static constexpr auto shall_continue(const node_t *const root1, const node_t *const node)
        {
            return root1 != node;
        }

        static constexpr auto pop_root(node_t *root1)
        {
            assert(root1 && !root1->parent);
            root1 = promote_children_to_root(root1);
            if (root1 == nullptr)
            {
                return root1;
            }

            root1 = link_same_degree(root1);
            assert(root1);

            root1 = find_minimum(root1);
            assert(root1);
            return root1;
        }

        static constexpr auto promote_children_to_root(node_t *root1)
        {
            assert(root1 && !root1->parent);

            if (root1->child)
            {
                reset_parent_marked(root1->child);
            }

            if (root1->left == root1)
            {
                return root1->child;
            }

            // Unlink the root.
            root1->left->right = root1->right;
            root1->right->left = root1->left;

            // todo(p2): check it.
            merge_nodes(root1->left, root1->child);
            assert(root1->left);
            return root1->left;
        }

        static constexpr void reset_parent_marked(node_t *const node)
        {
            assert(node);

            auto cur = node;
            do
            {
                cur->parent = nullptr;
                cur->marked = false;
                cur = cur->left;
            } while (cur != node);
        }

        // In the result, up to log(n) distinct degree trees.
        static constexpr auto link_same_degree(node_t *root1)
        {
            std::array<node_t *, max_log> size_nodes{};

            for (;;)
            {
                assert(root1 != nullptr);

                auto &node = size_nodes[root1->children_count];
                if (!node)
                {
                    node = root1;
                    root1 = root1->left;
                    continue;
                }

                if (node == root1)
                {
                    return root1;
                }

                // Both root, node are in the same linked list.
                if (!(root1->key < node->key))
                {
                    std::swap(root1, node);
                }

                // Unlink the node.
                node->left->right = node->right;
                node->right->left = node->left;
                node->left = node->right = node;

                node->parent = root1;
                merge_nodes(root1->child, node);
                ++(root1->children_count);

                node = nullptr;
            }
        }

        [[nodiscard]] static constexpr auto find_minimum(node_t *const root1) -> node_t *
        {
            assert(root1 != nullptr);

            auto opt = root1;
            auto cur = root1;

            while ((cur = cur->left) != root1)
            {
                if (cur->key < opt->key)
                {
                    opt = cur;
                }
            }

            return opt;
        }

        static constexpr void delete_nodes(gsl::owner<node_t *> node
#if _DEBUG
            ,
            std::uint32_t depth = {}
#endif
        )
        {
            assert(node != nullptr);

#if _DEBUG
            assert(depth < ::Standard::Algorithms::Utilities::max_logn_algorithm_depth);
            ++depth;
#endif

            auto cur = static_cast<node_t *>(node);
            do
            {
                if (cur->child != nullptr)
                {
                    auto cur2 = static_cast<node_t *>(node);

                    node = static_cast<gsl::owner<node_t *>>(cur->child);
                    delete_nodes(node
#if _DEBUG
                        ,
                        depth
#endif
                    );

                    node = static_cast<gsl::owner<node_t *>>(cur2);
                }

                auto left1 = cur->left;
                {
                    auto cur2 = static_cast<node_t *>(node);

                    node = static_cast<gsl::owner<node_t *>>(cur);
                    delete node;

                    node = static_cast<gsl::owner<node_t *>>(cur2);
                }

                cur = left1;
            } while (cur != node);
        }

        // Merge 2 doubly linked lists into one, returning the min key node.
        static constexpr void merge_nodes(gsl::owner<node_t *> &aaa, node_t *bbb) noexcept
        {
            if (bbb == nullptr)
            {
                return;
            }

            assert(aaa != bbb);
            if (aaa == nullptr)
            {
                aaa = static_cast<gsl::owner<node_t *>>(bbb);
                return;
            }

            if (bbb->key < aaa->key)
            {
                std::swap(aaa, bbb);
            }

            /*     a          b
                 /   \      /   \;
Before:         1     2    6     7
                 \   /      \   /;
                   3          8

                   a -------- b
                 /              \;
After:          1     2 -- 6     7
                 \   /      \   /;
                   3          8    */
            aaa->right->left = bbb->left;
            bbb->left->right = aaa->right;
            aaa->right = bbb;
            bbb->left = aaa;
        }

        static constexpr void cut(gsl::owner<node_t *> &root1, node_t *node)
        {
            assert(root1 && node && node->parent);

            if (node->left == node)
            {// The only child.
                node->parent->child = nullptr;
            }
            else
            {
                // Unlink the node.
                node->left->right = node->right;
                node->right->left = node->left;

                // Ensure the child is no longer the node.
                node->parent->child = node->left;

                node->left = node->right = node;
            }

            node->marked = false;

            auto &cnt = node->parent->children_count;
            assert(0 < cnt);
            --cnt;

            merge_nodes(root1, node);
        }

        [[nodiscard]] static constexpr auto node_exists(const node_t *const root1, const node_t *const node) -> bool
        {
            assert(root1 && node);

            auto cur = root1;
            do
            {
                if (cur == node || (cur->child && node_exists(cur->child, node)))
                {
                    return true;
                }

                cur = cur->left;
            } while (cur != root1);

            return false;
        }

        gsl::owner<node_t *> root_node{};
        std::size_t node_count{};
    };
} // namespace Standard::Algorithms::Heaps
