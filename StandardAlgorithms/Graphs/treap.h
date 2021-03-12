#pragma once
#include"../Utilities/is_debug.h"
#include"../Utilities/project_constants.h"
#include"binary_tree_utilities.h"
#include<cassert>
#include<concepts>
#include<cstddef>
#include<cstdint>
#include<gsl/gsl>
#include<limits>
#include<memory>
#include<stdexcept>
#include<utility>

namespace Standard::Algorithms::Heaps::Inner
{
    template<class node_t, class key_t>
    [[nodiscard]] constexpr auto binary_search_closest(const node_t *node, const key_t &key) noexcept -> const node_t *
    {
        if (node == nullptr)
        {
            return node;
        }

        for (;;)
        {
            assert(node != nullptr);

            if (key < node->key)
            {
                if (node->left == nullptr)
                {
                    return node;
                }

                node = node->left;
            }
            else if (node->key < key)
            {
                if (node->right == nullptr)
                {
                    return node;
                }

                node = node->right;
            }
            else
            {
                return node;
            }
        }
    }
} // namespace Standard::Algorithms::Heaps::Inner

namespace Standard::Algorithms::Heaps
{
    // sizeof(priority) is desirable to be at least n*n, where n is max duce size.
    template<class key_t, std::integral priority_t>
    struct treap_node final
    {
        key_t key{};

        priority_t priority{}; // Not necessarily unique.

        treap_node *parent{}; // todo(p3): to remove, and use a vector instead.
        treap_node *left{};
        treap_node *right{};
    };

    // Treap (Duce) is an unbalanced BST with distinct keys (this impl.),
    // and a heap, where each node has random priority.
    // The search time is O(tree height): amortized O(log(n)), worst O(n).
    // The root, if any, has max priority.
    // Nodes with higher priorities will be kept closer to the root.
    // todo(p3): Finger search (just a hint), starting from prev pos
    // - could be useful when searching for a nearby key.
    template<class random_t1, class key_t1, class node_t1 = treap_node<key_t1, decltype(std::declval<random_t1>()())>>
    requires std::default_initializable<random_t1>
    struct treap final
    {
        using random_t = random_t1;
        using key_t = key_t1;
        using priority_t = decltype(std::declval<random_t1>()());
        using node_t = node_t1;

        constexpr treap() noexcept(noexcept(random_t{})) = default;

        treap(treap &) = delete;
        auto operator= (treap &) & -> treap = delete;
        // todo(p3): make it movable.
        treap(treap &&) = delete;
        auto operator= (treap &&) & -> treap = delete;

        constexpr ~treap()
        {
            Standard::Algorithms::Trees::free_tree_slow(
                root_node); // todo(p2): O(1) memory is enough with the parent pointers, not O(n).
        }

        [[nodiscard]] constexpr auto size() const noexcept
        {
            check_invariants();

            return node_count;
        }

        [[nodiscard]] constexpr auto is_empty() const noexcept
        {
            return size() == 0U;
        }

        // Return the closest node.
        [[nodiscard]] constexpr auto find(const key_t &key) const &noexcept -> const node_t *
        {
            check_invariants();

            const auto *const root = static_cast<const node_t *>(root_node);
            const auto *const node = Inner::binary_search_closest(root, key);
            return node;
        }

        [[maybe_unused]] constexpr auto insert(const key_t &key) noexcept(false) -> std::pair<node_t *, bool>
        {
            check_invariants();

            if (!(node_count < ::Standard::Algorithms::Utilities::stack_max_size)) [[unlikely]]
            {
                throw std::runtime_error("Cannot add more nodes to a treap.");
            }

            if (root_node == nullptr)
            {
                auto owner = new_node(key);
                root_node = static_cast<gsl::owner<node_t *>>(owner.release());
                ++node_count;

                check_invariants();

                return std::make_pair(static_cast<node_t *>(root_node), true);
            }

            auto *par =
                // NOLINTNEXTLINE
                const_cast<node_t *>(find(key));

            assert(par != nullptr);

            if (par->key == key)
            {
                return std::make_pair(par, false);
            }

            auto *&kid = key < par->key ? par->left : par->right;
            assert(kid == nullptr);

            auto owner = new_node(key);
            kid = owner.get();
            kid->parent = par;
            ++node_count;

            repair_heap_upwards(kid);
            check_invariants();

            return std::make_pair(owner.release(), true);
        }

        [[maybe_unused]] constexpr auto erase(const key_t &key) noexcept -> bool
        {
            auto *cur =
                // NOLINTNEXTLINE
                const_cast<node_t *>(find(key));

            if (cur == nullptr || cur->key != key)
            {
                return false;
            }

            assert(0U < node_count && root_node != nullptr && root_node->parent == nullptr);

            --node_count;
            auto *par = cur->parent;

            if (cur->left == nullptr || cur->right == nullptr)
            {
                auto *successor = cur->right == nullptr ? cur->left : cur->right;
                try_set_root(par, *cur, successor);

                if (successor != nullptr)
                {
                    successor->parent = par;
                }
            }
            else
            {// both children
                // NOLINTNEXTLINE
                auto *successor = const_cast<node_t *>(Standard::Algorithms::Trees::find_successor(*cur).second);

                assert(0U < node_count && successor != nullptr && successor->parent != nullptr &&
                    successor->left == nullptr);

                //     par*
                //                 cur*
                // left_child*     right_child*
                //                    /                  \;
                //            successor*             other
                //           /           \;
                //  nullptr          sri* (might exist)
                //
                //     par
                //                 successor
                // left_child       right_child
                //                    /                  \;
                //           sri (might exist)    other
                successor->left = cur->left;
                cur->left->parent = successor;

                if (auto *sup = successor->parent; sup != cur)
                {
                    auto *&sri = successor->right;
                    child(*sup, successor, sri);

                    if (sri != nullptr)
                    {
                        sri->parent = sup;
                    }

                    sri = cur->right;
                    assert(sri != nullptr);

                    sri->parent = successor;
                }

                try_set_root(par, *cur, successor);
                successor->parent = par;
                repair_heap_downwards(successor);
            }

            std::swap(root_node, cur);
            delete root_node;
            std::swap(root_node, cur);

            check_invariants();

            return true;
        }

        // todo(p3): constexpr void merge(treap &other)

        constexpr void validate(const key_t &min = std::numeric_limits<key_t>::min(),
            const key_t &max = std::numeric_limits<key_t>::max()) const
        {
            if constexpr (::Standard::Algorithms::is_debug)
            {
                assert(min <= max);
            }

            check_invariants();

            if (root_node == nullptr)
            {
                return;
            }

            if (root_node->parent != nullptr) [[unlikely]]
            {
                throw std::runtime_error("The root has a parent.");
            }

            validate_helper(*root_node, min, max);
        }

private:
        [[nodiscard]] constexpr auto new_node(const key_t &key) noexcept(false) -> std::unique_ptr<node_t>
        {
            auto owner = std::make_unique<node_t>();
            owner->key = key;
            owner->priority = rnd();

            return owner;
        }

        constexpr void repair_heap_upwards(node_t *kid)
        {
            for (;;)
            {
                assert(kid != nullptr && kid->parent != nullptr);

                auto *const par = kid->parent;

                if (const auto is_good_heap = !(par->priority < kid->priority); is_good_heap)
                {
                    return;
                }

                {
                    auto *grand = par->parent;
                    kid->parent = grand;

                    if (grand != nullptr)
                    {
                        child(*grand, par, kid);
                    }
                }

                const auto is_left_child = par->left == kid;

                assert(is_left_child || par->right == kid);

                auto *&other_heir = is_left_child ? kid->right : kid->left;

                if (other_heir != nullptr)
                {
                    other_heir->parent = par;
                }

                //                c,10* (grand)
                //                                        q,8* (par)
                //      u,20* (kid will rotate right, is_left_child == true)           d,6
                // t,4           w,2* (other_heir)
                //
                //        c,10* (grand)
                //                u,20 (kid will rotate right)
                // t,4                            q,8 (par)
                //                 w,2 (other_heir)       d,6
                //
                //
                // c,10* (grand)
                //           q,8* (par)
                //    d,6             u,20* (kid will rotate left)
                //              t,4* (other_heir)     w,2
                //
                // c,10 (grand)
                //                 u,20 (kid)
                //         q,8 (par)        w,2
                //    d,6     t,4 (other_heir)

                (is_left_child ? par->left : par->right) = other_heir;
                other_heir = par;
                par->parent = kid;

                if (kid->parent == nullptr)
                {
                    root_node = static_cast<gsl::owner<node_t *>>(kid);
                    return;
                }
            }
        }

        constexpr void repair_heap_downwards(node_t *cur)
        {
            for (;;)
            {
                assert(cur != nullptr);

                if (const auto is_leaf = cur->left == nullptr && cur->right == nullptr; is_leaf)
                {
                    return;
                }

                const auto is_right_larger =
                    cur->left == nullptr || (cur->right != nullptr && cur->left->priority < cur->right->priority);

                auto *successor = is_right_larger ? cur->right : cur->left;
                assert(successor != nullptr);

                if (const auto is_good_heap = !(cur->priority < successor->priority); is_good_heap)
                {
                    return;
                }

                auto *par = cur->parent;
                try_set_root(par, *cur, successor);

                successor->parent = par;
                cur->parent = successor;

                //  par*
                //    cur*
                // cl         successor* (rotate left, is_right_larger == true)
                //         other*     lr
                //
                //  par
                //           successor
                //    cur             lr
                // cl    other
                auto *&other = is_right_larger ? successor->left : successor->right;

                (is_right_larger ? cur->right : cur->left) = other;
                //       par*
                //                                    cur*
                //   successor* (rotate right)      cr
                // ll          other*
                //
                //    par*
                //    successor
                // ll               cur
                //           other    cr
                if (other != nullptr)
                {
                    other->parent = cur;
                }

                other = cur;
            }
        }

        [[maybe_unused]] inline constexpr void check_invariants() const noexcept
        {
            assert((root_node == nullptr) == (node_count == 0U));
        }

        static constexpr void child(node_t &parent1, const node_t *const old_child, node_t *const new_child) noexcept
        {
            (parent1.left == old_child ? parent1.left : parent1.right) = new_child;
        }

        constexpr void try_set_root(node_t *par, node_t &cur, node_t *successor) noexcept
        {
            if (const auto is_root = par == nullptr; is_root)
            {
                assert(root_node == &cur);

                root_node = static_cast<gsl::owner<node_t *>>(successor);

                return;
            }

            //  par
            //         cur*
            // successor
            child(*par, &cur, successor);
        }

        static constexpr void validate_helper(
            const node_t &node, const key_t &min, const key_t &max, std::uint32_t depth = {})
        {
            assert(&node != node.left && &node != node.right);

            if (::Standard::Algorithms::Utilities::stack_max_size < ++depth) [[unlikely]]
            {
                throw std::runtime_error("Too deep stack in validate_helper.");
            }

            if (node.key < min || max < min || max < node.key) [[unlikely]]
            {
                throw std::runtime_error("Error at node key '" + std::to_string(node.key) + "', max '" +
                    std::to_string(max) + "', min '" + std::to_string(min) + "'");
            }

            if (node.left != nullptr)
            {
                if (node.left->key == node.key) [[unlikely]]
                {
                    throw std::runtime_error("Duplicate key '" + std::to_string(node.key) + "', left '" +
                        std::to_string(node.left->key) + "'");
                }

                if (node.priority < node.left->priority) [[unlikely]]
                {
                    throw std::runtime_error("Heap property violated, key '" + std::to_string(node.key) +
                        "' priority " + std::to_string(node.priority) + ", left '" + std::to_string(node.left->key) +
                        "' priority " + std::to_string(node.left->priority));
                }

                validate_helper(*node.left, min, node.key, depth);
            }

            if (node.right != nullptr)
            {
                if (node.right->key == node.key) [[unlikely]]
                {
                    throw std::runtime_error("Duplicate key '" + std::to_string(node.key) + "', right '" +
                        std::to_string(node.right->key) + "'");
                }

                if (node.priority < node.right->priority) [[unlikely]]
                {
                    // todo(p4): format.
                    throw std::runtime_error("Heap property violated, key '" + std::to_string(node.key) +
                        "' priority " + std::to_string(node.priority) + ", right '" + std::to_string(node.right->key) +
                        "' priority " + std::to_string(node.right->priority));
                }

                validate_helper(*node.right, node.key, max, depth);
            }
        }

        random_t rnd{};
        gsl::owner<node_t *> root_node{};
        std::size_t node_count{};
    };
} // namespace Standard::Algorithms::Heaps
