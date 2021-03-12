#pragma once
#include"binary_tree_utilities.h"
#include"bst_validate.h"
#include"red_black_tree_validator.h"
#include<array>

namespace Standard::Algorithms::Trees
{
    template<class key_t1>
    requires(std::is_default_constructible_v<key_t1>)
    struct red_black_node final
    {
        using key_t = key_t1;

        key_t key{};
        red_black_node *parent{};
        red_black_node *left{}; // No " __restrict__ ", because left might be == right.
        red_black_node *right{};
        bool is_red{}; // todo(p4): use 0-th bit of parent?
    };

    enum class node_direction : std::uint8_t
    {
        left,
        right,
    };

    template<class key_t1, class node_t1 = red_black_node<key_t1>>
    struct red_black_tree final
    {
        using key_t = key_t1;
        using node_t = node_t1;

        constexpr red_black_tree() noexcept(std::is_nothrow_constructible_v<key_t>) = default;

        red_black_tree(const red_black_tree &) = delete;
        auto operator= (const red_black_tree &) & -> red_black_tree & = delete;
        red_black_tree(red_black_tree &&) noexcept = delete;
        auto operator= (red_black_tree &&) &noexcept -> red_black_tree & = delete;

        constexpr ~red_black_tree() noexcept = default;

        [[nodiscard]] inline static constexpr auto sentinel_node() noexcept -> const node_t &
        {
            assert(!sentinel.is_red && sentinel.parent == nullptr);
            return sentinel;
        }

        [[nodiscard]] constexpr auto is_empty() const noexcept -> bool
        {
            const auto *const root1 = root();
            auto is_empty = root1 == &sentinel;
            return is_empty;
        }

        [[nodiscard]] constexpr auto lower_bound_rank(const key_t &key) const noexcept
            -> std::pair<const node_t *, std::size_t>
        {
            const auto *const root1 = root();

            auto res = ::Standard::Algorithms::Trees::lower_bound_rank<const node_t, key_t>(root1, key, &sentinel);
            return res;
        }

        // Can return nullptr.
        [[nodiscard]] constexpr auto find(const key_t &key) const noexcept -> const node_t *
        {
            auto *const res = lower_bound_rank(key).first;
            return res;
        }

        [[nodiscard]] constexpr auto contains(const key_t &key) const noexcept -> bool
        {
            const auto *const node = find(key);

            return node != nullptr && node->key == key;
        }

        [[maybe_unused]] constexpr auto insert(const key_t &key) -> std::pair<const node_t *, bool>
        {
            node_t *parent1{};

            {
                auto *curr = root();

                assert(curr != nullptr && curr->parent == nullptr && !curr->is_red && !sentinel.is_red);

                while (curr != &sentinel)
                {
                    parent1 = curr;

                    if (key < curr->key)
                    {
                        curr = curr->left;
                    }
                    else if (curr->key < key)
                    {
                        curr = curr->right;
                    }
                    else
                    {
                        assert(key == curr->key);

                        return std::make_pair(curr, false);
                    }
                }
            }

            assert(parent1 == nullptr || (parent1 != &sentinel && parent1->key != key));

            auto owner = std::make_unique<node_t>();
            auto new_node = owner.get();
            new_node->key = key;
            new_node->parent = parent1;
            new_node->left = new_node->right = // NOLINTNEXTLINE
                const_cast<node_t *>(&sentinel);

            owner.release();

            if (parent1 == nullptr)
            {
                root(new_node);
                return std::make_pair(new_node, true);
            }

            new_node->is_red = true;

            {
                auto *&child = key < parent1->key ? parent1->left : parent1->right;

                assert(child == &sentinel);

                child = new_node;
            }

            balance_after_insertion(new_node);

            auto *const root1 = root();
            assert_node(root1);

            root1->is_red = false;

            return std::make_pair(new_node, true);
        }

        [[maybe_unused]] constexpr auto erase(const key_t &key) noexcept -> bool
        {
            if (is_empty())
            {
                return false;
            }

            auto *const initial_root = root();
            auto *const node_to_delete =
                ::Standard::Algorithms::Trees::lower_bound_rank<node_t, key_t>(initial_root, key, &sentinel).first;

            if (node_to_delete == nullptr) [[unlikely]]
            {// Should never happen.
                assert(0);
                std::unreachable();
                return false;
            }

            if (node_to_delete->key != key)
            {
                return false;
            }

            // Needed when to_balance is the sentinel. Can be nullptr.
            auto *to_delete_pare = node_to_delete->parent;
            node_t *to_balance{};
            auto was_red = node_to_delete->is_red;

            if (node_to_delete->left == &sentinel)
            {
                //         to_delete_pare
                //        node_to_delete (could be left or right child of to_delete_pare)
                // sentinel            to_balance
                to_balance = node_to_delete->right;
                relink_node(node_to_delete, to_balance);
                // After the changes.
                //         to_delete_pare
                //         to_balance  (the same child direction as node_to_delete has been)
            }
            else if (node_to_delete->right == &sentinel)
            {
                to_balance = node_to_delete->left;
                relink_node(node_to_delete, to_balance);
            }
            else
            {
                to_balance = prepare_inner_node(to_delete_pare, *node_to_delete, was_red);
            }

            if (!was_red)
            {
                balance_after_erasion(to_delete_pare, to_balance);
            }

            {// Pacify the compiler.
                std::unique_ptr<node_t> uniq{ node_to_delete };
            }

            return true;
        }

        constexpr void validate() const
        {
            const auto *const root1 = root();
            throw_if_null("root", root1);

            if (root1->is_red) [[unlikely]]
            {
                throw std::runtime_error("The root is red.");
            }

            if (root1 == &sentinel)
            {
                return;
            }

            using extra_func_t = red_black_tree_validator<node_t>;

            const auto leaf_black_depth = minimum_black_depth(root1).second;
            extra_func_t tree_validator(sentinel, leaf_black_depth, *root1);

            bst_validate<key_t, node_t, true, extra_func_t &>(root1, &sentinel, tree_validator);
        }

private:
        constexpr void balance_after_insertion(node_t *nod) noexcept
        {
            assert(nod != nullptr && nod != &sentinel && nod->parent != nullptr && nod->is_red);

            while (nod->parent != nullptr && nod->parent->is_red)
            {// 2 reds in a row.
                auto *grand = nod->parent->parent; // A red node must have a black parent.

                assert_node(nod->parent);
                assert_node(grand);
                assert(nod->is_red && grand != nullptr && !grand->is_red);

                const auto is_father_left = nod->parent == grand->left;

                if (auto *const uncle = is_father_left ? grand->right : grand->left; uncle->is_red)
                {// The black grand has 2 red children: father + uncle; swap their colors.
                    // Let * be "a red node". Before:
                    //        grand
                    //   *father   *uncle
                    // *nod
                    //
                    // Here it does not matter whether (nod or father) is (left or right). After:
                    //        *grand
                    //    father    uncle
                    // *nod
                    uncle->is_red = nod->parent->is_red = false;
                    grand->is_red = true;
                    nod = grand;
                    continue;
                }

                // Here, the uncle is black.

                //        grand ; case 1, already a line
                //   *father   uncle ; is_father_left
                // *nod

                //        grand ; case 2
                //   *father   uncle ; is_father_left
                //        *nod

                //        grand ; case 3
                //   uncle   *father ; not is_father_left
                //            *nod

                //        grand ; case 4, already a line too
                //   uncle   *father ; not is_father_left
                //                       *nod

                if (const auto *const zizag = is_father_left ? nod->parent->right : nod->parent->left; nod == zizag)
                {// The cases 2, 3 will have to become a straight line as in the cases 1, 4.
                    //        grand ; case 2 before
                    //   *father   uncle ; is_father_left
                    //        *nod
                    //
                    //        grand ; case 2' after
                    //    *nod'   uncle ; is_father_left
                    // *father' ; here (father' == nod)

                    //        grand ; case 3 before
                    //   uncle   *father ; not is_father_left
                    //            *nod
                    //
                    //        grand ; case 3' after
                    //   uncle   *nod'; not is_father_left
                    //                        *father' ; here (father' == nod)
                    nod = nod->parent;

                    rb_rotate(*nod, is_father_left ? node_direction::left : node_direction::right);
                }

                //        grand ; case 1, already a line
                //   *father   uncle ; is_father_left
                // *nod
                //
                //        father ; case 1"
                // *nod     *grand ; is_father_lefts
                //                     uncle

                //        grand ; case 4, already a line too
                //   uncle   *father ; not is_father_left
                //                       *nod
                //
                //        father ; case 4", already a line too
                //    *grand  *nod ; not is_father_left
                // uncle
                auto *const father = nod->parent;
                assert_node(father);
                assert(father != nullptr && father->is_red && nod->is_red);
                assert(grand == father->parent && !grand->is_red);

                father->is_red = false;
                grand->is_red = true;

                rb_rotate(*grand, is_father_left ? node_direction::right : node_direction::left);

                return;
            }
        }

        [[nodiscard]] constexpr auto prepare_inner_node(node_t *&to_delete_pare, node_t &node_to_delete, bool &was_red)
            -> node_t *
        {
            assert_node(&node_to_delete);
            assert_node(node_to_delete.left);
            assert_node(node_to_delete.right);

            auto *const larger = // NOLINTNEXTLINE
                const_cast<node_t *>( // It is easier to re-use the already written code.
                    minimum_black_depth(node_to_delete.right).first);
            assert_node(larger);
            assert(larger != nullptr && larger->left == &sentinel);

            auto *const to_balance = larger->right;
            assert(to_balance == &sentinel || (to_balance != nullptr && to_balance->is_red));

            was_red = larger->is_red;
            larger->is_red = node_to_delete.is_red;

            if (const auto is_right_min = node_to_delete.right == larger; is_right_min)
            {
                //     node_to_delete
                // left                        larger
                //                    sentinel     to_balance
                to_delete_pare = larger;
                // The larger will soon be placed to node_to_delete,
                // so the parent of to_balance should be larger.
            }
            else
            {
                //     node_to_delete
                // left                           right
                //                             ../ (0 or more nodes)
                //                      to_delete_pare (could be right)
                //               larger
                //    sentinel       to_balance
                to_delete_pare = larger->parent;
                relink_node(larger, to_balance);
                larger->right = node_to_delete.right;
                larger->right->parent = larger;
                //                larger
                // sentinel                        right
                //                       ../ (0 or more nodes)
                //             to_delete_pare (could be right)
                //    to_balance
            }

            relink_node(&node_to_delete, larger);
            larger->left = node_to_delete.left;
            larger->left->parent = larger;
            //     larger ; in case when (is_right_min)
            // left      to_balance

            //     larger ; in case when (!is_right_min)
            // left                      right
            return to_balance;
        }

        [[nodiscard]] constexpr auto paint_brother_black(node_t &pare, const bool is_left, node_t &bro) noexcept
            -> node_t *
        {// When subtree black height > 0, a red node must have 2 black children.
            // A red node in general must have a black parent.
            assert_node(&pare);
            assert_node(&bro);

            assert(bro.is_red && !pare.is_red && bro.parent == &pare);
            assert(pare.left == &bro || pare.right == &bro);

            assert(bro.left != &sentinel && bro.left != nullptr && !bro.left->is_red);
            assert(bro.right != &sentinel && bro.right != nullptr && !bro.right->is_red);

            //             pare
            //    bro*        to_balance (e.g. is_left is false; could have been true)
            // left  right
            bro.is_red = false;
            pare.is_red = true;
            rb_rotate(pare, is_left ? node_direction::left : node_direction::right);
            //          bro
            // left                                       pare*
            //                  right(new_brother)       to_balance
            auto *const new_brother = is_left ? pare.right : pare.left;
            assert_node(new_brother);

            return new_brother;
        }

        [[nodiscard]] constexpr auto make_nephew_red(node_t &pare, const bool is_left, node_t *&bro) noexcept
            -> node_t *
        {
            assert_node(bro);
            assert(&pare != bro && bro != nullptr && bro->parent == &pare && bro->left != bro->right);

            auto *red_nephew = is_left ? bro->left : bro->right;
            assert_node(red_nephew);
            assert(red_nephew != nullptr && red_nephew->is_red);

            //                        pare?
            //          bro                            to_balance (e.g. is_left is false)
            // nephew   red_nephew*
            red_nephew->is_red = false;
            bro->is_red = true;
            rb_rotate(*bro, is_left ? node_direction::right : node_direction::left);
            //                                                   pare?
            //           red_nephew(new_brother)                to_balance
            //     bro*
            // nephew
            bro = is_left ? pare.right : pare.left;
            assert_node(bro);

            red_nephew = is_left ? bro->right : bro->left;

            assert(red_nephew != nullptr && red_nephew != &sentinel && red_nephew->is_red);

            return red_nephew;
        }

        constexpr void balance_after_erasion(node_t *pare, node_t *to_balance) noexcept
        {
            assert(to_balance != nullptr); // (*to_balance) can be sentinel.

            assert(pare == nullptr || pare->left == to_balance || pare->right == to_balance);

            while (!to_balance->is_red && to_balance != root())
            {// Black node deficit?
                assert_node(pare);
                assert(pare->left == to_balance || pare->right == to_balance);

                const auto is_left = to_balance == pare->left;
                auto *bro = is_left ? pare->right : pare->left; // When a black node,
                assert_node(bro); // which is not the root, is being deleted, it must have a brother.

                if (bro->is_red)
                {
                    bro = paint_brother_black(*pare, is_left, *bro);
                    assert(bro != &sentinel && bro != nullptr && !bro->is_red);
                }

                if (!bro->left->is_red && !bro->right->is_red)
                {
                    //                pare?
                    //      bro                   to_balance
                    // left    right
                    bro->is_red = true; // Decrease the potential deficit.
                    to_balance = pare;
                    pare = pare->parent;
                    continue;
                }
                // At least 1 nephew is red.

                auto *nephew = is_left ? bro->right : bro->left;
                if (!nephew->is_red)
                {// The "nephew" can be the sentinel node.
                    nephew = make_nephew_red(*pare, is_left, bro);
                }

                assert_node(nephew);
                //                            pare?
                //               bro                        to_balance (e.g. is_left is false)
                // nephew*   nephew_2#
                bro->is_red = pare->is_red;
                pare->is_red = false;
                nephew->is_red = false;
                rb_rotate(*pare, is_left ? node_direction::left : node_direction::right);
                //               bro? ; The color must have been good as before.
                // nephew                           pare
                //                       nephew_2#       to_balance
                to_balance = root();
                break;
            }

            if (to_balance != &sentinel)
            {
                assert_node(to_balance);
                to_balance->is_red = false;
            }
        }

        constexpr void relink_node(node_t *const from, node_t *const tod) noexcept
        {
            assert_node(from);
            assert(from != tod && tod != nullptr);

            auto *const old_parent = from->parent;

            if (old_parent == nullptr)
            {
                root(tod);
            }
            else
            {
                assert_node(old_parent);
                assert(from == old_parent->left || from == old_parent->right);

                const auto is_left = from == old_parent->left;
                (is_left ? old_parent->left : old_parent->right) = tod;
            }

            if (tod == &sentinel)
            {
                return;
            }

            assert_node(tod);

            tod->parent = old_parent;
        }

        [[maybe_unused]] inline static constexpr void assert_node(const node_t *const node) noexcept
        {
            assert(node != nullptr && node != &sentinel && node->left != nullptr && node->right != nullptr);
        }

        [[nodiscard]] static constexpr auto minimum_black_depth(const node_t *node) noexcept
            -> std::pair<const node_t *, std::size_t>
        {
            assert(!sentinel.is_red);
            assert_node(node);

            std::size_t black_depth = node->is_red ? 0 : 1;

            while (node->left != &sentinel)
            {
                node = node->left;

                assert_node(node);

                black_depth += node->is_red ? 0 : 1;
            }

            assert_node(node);
            {
                [[maybe_unused]] const auto *const right_1 = node->right;

                assert(right_1 != nullptr && (right_1 == &sentinel || (right_1->is_red && !node->is_red)));
            }

            return { node, black_depth };
        }

        constexpr void rb_rotate(node_t &xxx, const node_direction direction) noexcept
        {
            assert_node(&xxx);
            assert(static_cast<std::uint8_t>(direction) < 2U);

            const auto is_right = node_direction::right == direction;
            auto *const zzz = is_right ? xxx.left : xxx.right;

            assert_node(zzz);
            assert(zzz != nullptr && zzz != &xxx && zzz->parent == &xxx);

            auto *const yyy = is_right ? zzz->right : zzz->left;

            assert(yyy != nullptr && yyy != &xxx && yyy != zzz);
            // Before left rotate:
            // xxx
            //       \;
            //          zzz
            //        /;
            //     yyy
            // After left rotate:
            //          zzz
            //       /;
            // xxx
            //    \;
            //      yyy
            (is_right ? xxx.left : xxx.right) = yyy;
            (is_right ? zzz->right : zzz->left) = &xxx;

            if (yyy != &sentinel)
            {
                yyy->parent = &xxx;
            }

            auto *const parent1 = xxx.parent;
            zzz->parent = parent1;
            xxx.parent = zzz;

            if (parent1 == nullptr)
            {
                root(zzz);
            }
            else
            {
                (&xxx == parent1->left ? parent1->left : parent1->right) = zzz;
            }
        }

public:
        [[nodiscard]] constexpr auto root() const noexcept -> const node_t *
        {
            auto *const root1 = Unique_root.get();
            return root1;
        }

private:
        [[nodiscard]] constexpr auto root() noexcept -> node_t *
        {
            auto *const root1 = Unique_root.get();
            return root1;
        }

        constexpr void root(node_t *new_root) noexcept
        {
            assert(new_root != nullptr);

            Unique_root.release();
            Unique_root.reset(new_root);
        }

        // Only for the children pointers: left, right.
        // Note. The nullptr is used for the root parent pointer.
        // todo(p3): Will it be faster without the sentinel - measure?
        inline static constexpr node_t sentinel{};

        using delete_func_t = void (*)(node_t *);

        std::unique_ptr<node_t, delete_func_t> Unique_root = std::unique_ptr<node_t, delete_func_t>( // NOLINTNEXTLINE
            const_cast<node_t *>(&sentinel),
            [](gsl::owner<node_t *> root1)
            {
                free_tree_2<node_t>(root1, &sentinel);
            });
    };

    namespace Inner
    {
        template<class key_t, class node_t>
        constexpr void rbtree_print_helper(
            const node_t &sentinel0, std::ostream &str, const node_t &node, std::uint32_t depth = {})
        {
            assert(&sentinel0 != &node);

            if (::Standard::Algorithms::Utilities::stack_max_size < ++depth) [[unlikely]]
            {
                throw std::runtime_error("Too deep stack in red black tree print.");
            }

            if (const auto *const child = node.left; &sentinel0 != child)
            {
                assert(child != nullptr);
                rbtree_print_helper<key_t, node_t>(sentinel0, str, *child, depth);
            }

            for (std::uint32_t dep{}; ++dep < depth;)
            {// todo(p5): opt. Maybe pass in a std::string ?
                constexpr auto separ = '.';
                str << separ;
            }

            str << node << "\n";

            if (const auto *const child = node.right; &sentinel0 != child)
            {
                assert(child != nullptr);
                rbtree_print_helper<key_t, node_t>(sentinel0, str, *child, depth);
            }
        }
    } // namespace Inner

    template<class key_t, class node_t>
    void print_tree(std::ostream &str, const red_black_tree<key_t, node_t> &tree)
    {
        if (tree.empty())
        {
            str << "Empty red black tree.";
            return;
        }

        static constexpr const auto &sentinel0 = red_black_tree<key_t, node_t>::sentinel_node();
        static_assert(!sentinel0.is_red);

        const auto *const root1 = tree.root();
        assert(root1 != nullptr && root1 != &sentinel0);

        Inner::rbtree_print_helper<key_t, node_t>(sentinel0, str, *root1);
    }
} // namespace Standard::Algorithms::Trees
