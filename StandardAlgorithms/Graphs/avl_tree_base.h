#pragma once
#include"avl_balance.h"
#include"binary_tree_utilities.h"
#include"bst_rotate.h"
#include"bst_validate.h"

namespace Standard::Algorithms::Trees
{
    constexpr void avl_update_height(auto &node);

    // AVL tree subtree heights can differ by at most 1.
    // Duplicates are not allowed.
    // AVL tree has faster selects than red-black tree.
    template<class key_t1, class node_t1, class avl_update_height_func_t = void (*)(node_t1 &)>
    struct avl_tree_base final
    {
        using key_t = key_t1;
        using node_t = node_t1;

        constexpr avl_tree_base() noexcept = default;

        avl_tree_base(const avl_tree_base &) = delete;
        auto operator= (const avl_tree_base &) & -> avl_tree_base & = delete;
        avl_tree_base(avl_tree_base &&) noexcept = delete;
        auto operator= (avl_tree_base &&) &noexcept -> avl_tree_base & = delete;

        constexpr ~avl_tree_base() noexcept = default;

        [[nodiscard]] constexpr auto size() const noexcept
        {
            const auto *const root1 = Unique_root.get();
            return root1 != nullptr ? root1->count : 0U;
        }

        [[nodiscard]] constexpr auto is_empty() const noexcept
        {
            const auto *const root1 = Unique_root.get();
            return root1 != nullptr;
        }

        [[nodiscard]] constexpr auto root() const &noexcept -> const node_t *
        {
            const auto *const root1 = Unique_root.get();
            return root1;
        }

        [[nodiscard]] constexpr auto lower_bound_rank(const key_t &key) const noexcept
            -> std::pair<const node_t *, std::size_t>
        {
            return ::Standard::Algorithms::Trees::lower_bound_rank<const node_t, key_t>(Unique_root.get(), key);
        }

        [[nodiscard]] constexpr auto find(const key_t &key) const noexcept -> const node_t *
        {
            return lower_bound_rank(key).first;
        }

        [[nodiscard]] constexpr auto contains(const key_t &key) const noexcept -> bool
        {
            const auto node = find(key);
            return node != nullptr && node->key == key;
        }

        // The tree must be not empty; the rank must be less than the size.
        [[nodiscard]] constexpr auto select(std::size_t rank) const noexcept(false) -> const node_t *
        {
            return select_by_rank<node_t>(Unique_root.get(), rank);
        }

        [[maybe_unused]] constexpr auto insert(const key_t &key) -> std::pair<const node_t *, bool>
        {
            auto root1 = Unique_root.get();
            auto ins = insert_impl(root1, key);

            assert(ins.first != nullptr && (!ins.second || ins.second->key == key));

            if (ins.second && root1 != ins.first)
            {
                Unique_root.release();
                Unique_root.reset(ins.first);
            }

            return { ins.second ? ins.second : ins.first, ins.second != nullptr };
        }

        [[maybe_unused]] constexpr auto erase(const key_t &key) -> bool
        {
            auto *root1 = Unique_root.get();

            auto [future_root, to_delete] = erase_by_key(root1, key);
            assert(!to_delete || to_delete->key == key);

            if (to_delete == nullptr)
            {
                assert(root1 == future_root);
                return false;
            }

            to_delete->left = to_delete->right = nullptr;

            Unique_root.release();
            Unique_root.reset(to_delete); // delete to_delete;
            Unique_root.reset(future_root);

            return true;
        }

        constexpr void clear()
        {
            Unique_root.reset(nullptr);
        }

        constexpr void validate() const
        {
            bst_validate<key_t, node_t>(Unique_root.get());
        }

private:
        // When added, return { future_root, new_node }.
        // Else return { the existing node with the given key, nullptr }.
        [[nodiscard]] static constexpr auto insert_impl(node_t *node, const key_t &key
#if _DEBUG
            ,
            std::uint32_t depth = {}
#endif
            ) -> std::pair<node_t *, node_t *>
        {
#if _DEBUG
            if (::Standard::Algorithms::Utilities::stack_max_size < ++depth) [[unlikely]]
            {
                throw std::runtime_error("Too deep stack in AVL insert.");
            }
#endif

            if (node == nullptr)
            {
                auto owner = std::make_unique<node_t>();
                auto *new_node = owner.get();
                new_node->key = key;
                avl_update_height(*new_node);

                owner.release();
                return { new_node, new_node };
            }

            node_t *__restrict__ *ppt = nullptr;

            if (key < node->key)
            {
                ppt = &(node->left);
            }
            else if (node->key < key)
            {
                ppt = &(node->right);
            }
            else
            {
                return { node, nullptr };
            }

            auto ins = insert_impl(*ppt, key
#if _DEBUG
                ,
                depth
#endif
            );

            assert(ins.first != nullptr);

            if (ins.second == nullptr)
            {
                return { ins.first, nullptr };
            }

            *ppt = ins.first;

            const auto balance = avl_balance(*node);
            if (balance < -1)
            {
                if (node->left->key < key)
                {
                    node->left = left_rotate(*(node->left));
                }

                return { right_rotate(*node), ins.second };
            }

            if (1 < balance)
            {
                if (key < node->right->key)
                {
                    node->right = right_rotate(*(node->right));
                }

                return { left_rotate(*node), ins.second };
            }

            avl_update_height(*node);
            return { node, ins.second };
        }

        [[nodiscard]] static constexpr auto successor_as_future_root(node_t &node
#if _DEBUG
            ,
            const std::uint32_t depth
#endif
            ) -> node_t *
        {
            assert(node.left != nullptr && node.right != nullptr);

            const auto par_successor = Standard::Algorithms::Trees::find_successor(node);

            // NOLINTNEXTLINE
            auto sp = const_cast<node_t *>(par_successor.first);

            // NOLINTNEXTLINE
            auto successor = const_cast<node_t *>(par_successor.second);
            assert(sp != nullptr && successor != nullptr && successor->left == nullptr && &node != successor &&
                (sp->left == successor || sp->right == successor));

            //     node (==sp, to be deleted)
            // lc              successor
            //             null              src
            //
            //     successor
            // lc              src
            successor->left = node.left;

            if (sp != &node)
            {
                //         node
                // lc                            sp
                //                   successor
                //             null           src
                //
                //
                //         successor
                // lc                            sp
                //                   node
                //             null           src
                node.left = nullptr;
                std::swap(successor->right, node.right);
                avl_update_height(node);
                sp->left = &node;

                auto par = erase_by_key(successor->right, node.key
#if _DEBUG
                    ,
                    depth
#endif
                ); // todo(p2): faster.
                assert(par.second == &node);

                successor->right = par.first;
            }

            avl_update_height(*successor);
            return successor;
        }

        [[nodiscard]] static constexpr auto fix_balance_erased(node_t &node) -> node_t *
        {
            const auto balance = avl_balance(node);
            if (balance < -1)
            {
                assert(node.left != nullptr);

                const auto balance2 = avl_balance(*(node.left));
                if (0 < balance2)
                {
                    node.left = left_rotate(*node.left);
                }

                return right_rotate(node);
            }

            if (1 < balance)
            {
                assert(node.right != nullptr);

                const auto balance2 = avl_balance(*node.right);
                if (balance2 < 0)
                {
                    node.right = right_rotate(*node.right);
                }

                return left_rotate(node);
            }

            avl_update_height(node);
            return &node;
        }

        // Return { future_root, node_to_delete }.
        [[nodiscard]] static constexpr auto erase_by_key(node_t *node, const key_t &key
#if _DEBUG
            ,
            std::uint32_t depth = {}
#endif
            ) -> std::pair<node_t *, node_t *>
        {
#if _DEBUG
            if (::Standard::Algorithms::Utilities::stack_max_size < ++depth) [[unlikely]]
            {
                throw std::runtime_error("Too deep stack in AVL erase_by_key.");
            }
#endif

            if (node == nullptr)
            {
                return { nullptr, nullptr };
            }

            node_t *to_delete{};

            if (key < node->key)
            {
                auto par = erase_by_key(node->left, key
#if _DEBUG
                    ,
                    depth
#endif
                );

                if (par.second == nullptr)
                {
                    return { node, nullptr };
                }

                node->left = par.first;
                to_delete = par.second;
            }
            else if (node->key < key)
            {
                auto par = erase_by_key(node->right, key
#if _DEBUG
                    ,
                    depth
#endif
                );

                if (par.second == nullptr)
                {
                    return { node, nullptr };
                }

                node->right = par.first;
                to_delete = par.second;
            }
            else
            {
                if (node->left != nullptr && node->right != nullptr)
                {
                    to_delete = node;
                    node = successor_as_future_root(*node
#if _DEBUG
                        ,
                        depth
#endif
                    );
                }
                else
                {
                    auto *temp = node->left != nullptr ? node->left : node->right;

                    if (const auto is_leaf = temp == nullptr; is_leaf)
                    {
                        return { nullptr, node };
                    }

                    to_delete = node;
                    node = temp;
                }
            }

            assert(node != nullptr && to_delete != nullptr);

            auto *fixed = fix_balance_erased(*node);

            return { fixed, to_delete };
        }

        [[nodiscard]] static constexpr auto left_rotate(node_t &node) -> node_t *
        {
            return ::Standard::Algorithms::Trees::left_rotate<node_t, avl_update_height_func_t>(node, avl_update_height);
        }

        [[nodiscard]] static constexpr auto right_rotate(node_t &node) -> node_t *
        {
            return ::Standard::Algorithms::Trees::right_rotate<node_t, avl_update_height_func_t>(node, avl_update_height);
        }

        using delete_func_t = void (*)(node_t *);

        std::unique_ptr<node_t, delete_func_t> Unique_root =
            std::unique_ptr<node_t, delete_func_t>(nullptr, free_tree<node_t>);
    };
} // namespace Standard::Algorithms::Trees
