#pragma once
#include"../Utilities/is_debug.h"
#include"binomial_node.h"
#include"heap_common.h"
#include<gsl/gsl>

namespace Standard::Algorithms::Heaps
{
    // Minimum binomial heap.
    // O(log(n)) time for many ops.
    template<class key_t, class node_t = binomial_node<key_t>>
    struct binomial_heap final
    {
        constexpr binomial_heap() = default;

        // NOLINTNEXTLINE
        constexpr ~binomial_heap() noexcept(false) // todo(p2): faster, and noexcept.
        {
            while (0U < node_count)
            {
                pop();
            }
        }

        // Time O(1).
        [[nodiscard]] constexpr auto size() const noexcept
        {
            return node_count;
        }

        [[nodiscard]] constexpr auto top() const &noexcept(false) // node_test might throw.
            -> node_t *
        {
            assert(
                0U < node_count && root_node != nullptr && root_node->parent == nullptr && root_node->left == nullptr);

            auto *best = static_cast<node_t *>(root_node);
            auto *node = best;

            while ((node = node->right) != nullptr)
            {
                assert(node != root_node && !node->parent && node_test(node->left, node));

                if (node->key < best->key)
                {
                    best = node;
                }
            }

            return best;
        }

        // Time O(log(n)).
        // Time to add n keys into an empty tree is O(n).
        // todo(p2): With lazy merge, make it O(1) amortized time.
        constexpr auto push(const key_t &key) noexcept(false) -> node_t *
        {
            assert((root_node == nullptr) == (node_count == 0U));

            auto owner = std::make_unique<node_t>();
            auto *node = owner.get();
            node->key = key;

            if (root_node == nullptr)
            {
                root_node = static_cast<gsl::owner<node_t *>>(node);
            }
            else
            {
                unite(root_node, node);
            }

            ++node_count;
            owner.release();

            assert(root_node != nullptr && root_node->parent == nullptr && root_node->left == nullptr &&
                0U < node_count && child_test(*root_node));

            return node;
        }

        // todo(p2): make it meldable, mergeable.

        // todo(p3): Many nodes might change their keys.
        // Beware of the key duplicates!
        constexpr void decrease_key(node_t &node, const key_t &key) noexcept(false)
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

            node.key = key; // might throw.

            auto *ptr = &node;

            while (ptr->parent != nullptr && ptr->key < ptr->parent->key)
            {
                std::swap(ptr->key, ptr->parent->key);
                ptr = ptr->parent;
            }
        }

        constexpr void pop() noexcept(false) // todo(p3): noexcept
        {
            assert(
                root_node != nullptr && root_node->parent == nullptr && root_node->left == nullptr && 0U < node_count);

            auto *best = top();
            assert(best != nullptr && child_test(*best));

            if (best->left != nullptr)
            {
                assert(node_test(best->left, best));

                // Unlink best.
                link(best->left, best->right);
            }
            else
            {
                assert(root_node == best);

                root_node = static_cast<gsl::owner<node_t *>>(best->right);

                if (root_node != nullptr)
                {
                    root_node->left = nullptr;
                }
            }

            if (best->child != nullptr)
            {
                auto *ch2 = reverse_children_reset_parent(best);
                if (root_node == nullptr)
                {
                    root_node = static_cast<gsl::owner<node_t *>>(ch2);
                }
                else
                {
                    unite(root_node, ch2);
                }

                assert(root_node != nullptr && root_node != best && root_node->parent == nullptr &&
                    root_node->left == nullptr && child_test(*root_node));
            }

            std::swap(best, root_node);
            delete root_node; // todo(p2): Use of memory after it is freed?
            std::swap(best, root_node);
            --node_count;

            assert((root_node == nullptr) == (node_count == 0U));
        }

        constexpr void print(std::string &str) const noexcept(false)
        {
            assert((root_node == nullptr) == (node_count == 0U));

            str += ::Standard::Algorithms::Utilities::zu_string(node_count);
            str += " nodes in the binomial heap.\n";

            if (node_count == 0U)
            {
                return;
            }

            print_heap_helper(root_node, str);
            str.pop_back();
        }

        constexpr void validate() const
        {
            // todo(p1): warning: Use of memory after it is freed [clang-analyzer-cplusplus.NewDelete]
            validate_heap<false>(root_node, node_count, node_test);
        }

        binomial_heap(binomial_heap &) = delete;
        auto operator= (binomial_heap &) & -> binomial_heap = delete;
        // todo(p3): make it movable.
        binomial_heap(binomial_heap &&) = delete;
        auto operator= (binomial_heap &&) & -> binomial_heap = delete;

private:
        static constexpr auto child_test(const node_t &node) -> bool
        {
            if (const auto good = node.child == nullptr ||
                    (node.child->left == nullptr && node.child->parent == &node && node.child->degree < node.degree);
                good) [[likely]]
            {
                return true;
            }

            const auto err = std::string("The node ")
                                 .append(node_to_string(node))
                                 .append(" has a child ")
                                 .append(node_to_string(*(node.child)))
                                 .append(".");

            throw std::runtime_error(err);
        }

        static constexpr auto node_test(const node_t *const left1, const node_t *const right1) -> bool
        {
            static const auto *const child_reason = "The children must decrease the degree.";
            static const auto *const root_reason = "The root trees must increase the degree.";
            static const auto *const other_reason = "Bad left, right, parent pointers.";

            const char *reason = nullptr;

            if (left1 != nullptr && right1 != nullptr && (left1->parent == nullptr) == (right1->parent == nullptr) &&
                left1->right == right1 && right1->left == left1)
            {
                if (left1->child != nullptr)
                {
                    child_test(*left1);
                }

                if (right1->child != nullptr)
                {
                    child_test(*right1);
                }

                if (const auto is_child = left1->parent != nullptr && right1->parent != nullptr; is_child)
                {
                    if (right1->degree < left1->degree)
                    {
                        return true;
                    }

                    reason = child_reason;
                }
                else
                {
                    if (left1->degree < right1->degree)
                    {
                        return true;
                    }

                    reason = root_reason;
                }
            }
            else
            {
                assert(0);
                reason = other_reason;
            }

            assert(reason != nullptr);

            const auto err = std::string(reason)
                                 .append(" Left ")
                                 .append(left1 != nullptr ? node_to_string(*left1) : std::string())
                                 .append(" vs right ")
                                 .append(left1 != right1 ? node_to_string(*right1) : std::string())
                                 .append(".");

            throw std::runtime_error(err);
        }

        static constexpr void link(node_t *left1, node_t *right1) noexcept
        {
            assert(left1 != nullptr && right1 != left1 &&
                (right1 == nullptr || (left1->parent == nullptr) == (right1->parent == nullptr)));

            left1->right = right1;

            if (right1 != nullptr)
            {
                right1->left = left1;
            }
        }

        // meld.
        static constexpr void merge_nodes(gsl::owner<node_t *> &root1, node_t *other)
        {
            assert(root1 != nullptr && root1->parent == nullptr && root1->left == nullptr);
            assert(other != nullptr && other->parent == nullptr && other->left == nullptr);
            assert(child_test(*root1));
            assert(child_test(*other));

            if (other->degree < root1->degree || (root1->degree == other->degree && other->key < root1->key))
            {
                std::swap(root1, other);
            }

            for (auto *root_2 = static_cast<node_t *>(root1);;)
            {
                assert(root_2->right == nullptr || node_test(root_2, root_2->right));
                assert(other->right == nullptr || node_test(other, other->right));

                if (root_2->degree <= other->degree)
                {
                    if (root_2->right == nullptr)
                    {
                        link(root_2, other);
                        break;
                    }

                    root_2 = root_2->right;
                    continue;
                }

                // Since (other->degree < root_2->degree),
                // insert other to the left of root_2.
                auto *ale = root_2->left;
                assert(ale != nullptr && ale->degree <= other->degree && node_test(ale, root_2));

                auto *bri = other->right;
                link(ale, other);
                link(other, root_2);

                if (bri == nullptr)
                {
                    break;
                }

                other = bri;
            }

            assert(root1 != nullptr && root1->parent == nullptr && root1->left == nullptr);

            if constexpr (::Standard::Algorithms::is_debug)
            {
                after_merge_nodes_debug(*root1);
            }
        }

        [[maybe_unused]] static constexpr void after_merge_nodes_debug(const node_t &root1)
        {
            assert(root1.parent == nullptr && root1.left == nullptr);

            for (const auto *temp = &root1;;)
            {
                if (temp->right == nullptr)
                {
                    return;
                }

                assert(temp->degree <= temp->right->degree && temp->right->left == temp);

                temp = temp->right;
                assert(temp != &root1);
            }
        }

        static constexpr auto distinct_increasing_degrees(node_t *root1) -> node_t *
        {
            assert(root1 != nullptr && root1->right != nullptr);

            node_t *pre{};
            auto *cur = root1;
            auto *nex = root1->right;
            do
            {
                if (cur->degree != nex->degree || (nex->right && nex->right->degree == cur->degree))
                {
                    pre = cur;
                    cur = nex;
                }
                else if (!(nex->key < cur->key))
                {
                    auto *right = nex->right;
                    link(cur, right);

                    nex->left = nullptr;
                    parent_child(cur, nex);
                }
                else
                {
                    if (pre != nullptr)
                    {
                        pre->right = nex;
                        nex->left = pre;
                    }
                    else
                    {
                        root1 = nex;
                    }

                    parent_child(nex, cur);
                    nex->left = cur->left;
                    cur->left = nullptr;

                    if (nex->left != nullptr)
                    {
                        nex->left->right = nex;
                    }

                    cur = nex;
                }

                nex = cur->right;
            } while (nex != nullptr);

            return root1;
        }

        static constexpr void parent_child(node_t *parent, node_t *child)
        {
            assert(parent != nullptr && child != nullptr && parent != child && !(child->key < parent->key) &&
                parent->parent == nullptr && child->parent == nullptr);

            child->parent = parent;

            auto *parc = parent->child;
            link(child, parc);
            parent->child = child;
            ++(parent->degree);
        }

        static constexpr void unite(gsl::owner<node_t *> &root1, gsl::not_null<node_t *> root2)
        {
            assert(root1 != nullptr && root2 != nullptr && root1 != root2);

            merge_nodes(root1, root2);

            auto *result = distinct_increasing_degrees(root1);
            root1 = static_cast<gsl::owner<node_t *>>(result);
        }

        static constexpr auto reverse_children_reset_parent(node_t *const parent) -> node_t *
        {
            assert(parent != nullptr);

            auto *cur = parent->child;
            assert(cur != nullptr && cur->left == nullptr);

            for (;;)
            {
                assert(cur->parent == parent);
                assert(child_test(*cur));
                assert(cur->right == nullptr || node_test(cur, cur->right));

                cur->parent = nullptr;

                auto *nex = cur->right;
                cur->right = cur->left;
                cur->left = nex;

                if (nex == nullptr)
                {
                    return cur;
                }

                cur = nex;
            }
        }

        [[maybe_unused]] static constexpr auto node_exists_helper(
            const node_t *root1, const node_t *const node, std::int32_t depth = {}) -> bool
        {
            assert(root1 != nullptr && node != nullptr &&
                depth < ::Standard::Algorithms::Utilities::max_logn_algorithm_depth);

            ++depth;
            do
            {
                if (root1 == node || (root1->child != nullptr && node_exists_helper(root1->child, node, depth)))
                {
                    return true;
                }

                root1 = root1->right;
            } while (root1 != nullptr);

            return false;
        }

        [[nodiscard]] [[maybe_unused]] static constexpr auto node_exists(const node_t *root1, const node_t *const node)
            -> bool
        {
            const auto exist = root1 != nullptr && node != nullptr && node_exists_helper(root1, node);
            return exist;
        }

        std::size_t node_count{};

        // A binomial tree has:
        // - height k;
        // - 2**k nodes;
        // - at most k subtrees.

        // 4 nodes in the binomial heap. 15 is key, 2 - degree.
        // 15 (2)
        // | 48 (1)
        // | | 48 (0)
        // | 97 (0)

        // From lower to higher degrees for the roots only.
        gsl::owner<node_t *> root_node{};
    };
} // namespace Standard::Algorithms::Heaps
