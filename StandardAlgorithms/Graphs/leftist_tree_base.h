#pragma once
#include"binary_tree_utilities.h"
#include"lhb_node.h"
#include<memory>
#include<queue>

namespace Standard::Algorithms::Trees
{
    // Can serve as a min heap, or min priority queue, duplicates are allowed.
    // 1. The parents' key is not greater than children's.
    // 2. The height of the left child is not less than that of the right (Fenwick's idea?).
    // 3. A child height can be greater than parent's if e.g. another child is null.
    // The node key is not changed unless calling decrease_key.
    template<class leftist_tree_derived, class key_t1, class node_t1, bool has_parent1>
    struct leftist_tree_base
    {
        using key_t = key_t1;
        using node_t = node_t1;
        static constexpr bool has_parent = has_parent1;

protected:
        constexpr leftist_tree_base() = default;

public:
        leftist_tree_base(leftist_tree_base &) = delete;
        auto operator= (leftist_tree_base &) & -> leftist_tree_base = delete;
        leftist_tree_base(leftist_tree_base &&) = delete; // todo(p3): make movable.
        auto operator= (leftist_tree_base &&) & -> leftist_tree_base = delete;

        constexpr virtual ~leftist_tree_base() noexcept
        {
            if (root_node != nullptr)
            {
                free_tree<node_t>(root_node);
            }
        }

        // Time O(1).
        [[nodiscard]] constexpr auto top() const &noexcept -> const node_t *
        {
            return static_cast<node_t *>(root_node);
        }

        // O(n) time when initially empty.
        // O(n + log(n + old_n)) otherwise.
        template<class it_t>
        constexpr void push_many(it_t start, it_t stop)
        {
            if (start == stop)
            {
                return;
            }

            std::queue<std::unique_ptr<node_t>> que;
            do
            {
                que.push(std::make_unique<node_t>());
                que.back()->key = *start;
            } while (++start != stop);

            while (1U < que.size())
            {
                auto one = std::move(que.front());
                que.pop();

                auto two = std::move(que.front());
                que.pop();

                auto *merged = one.release();
                leftist_tree_derived::merge_nodes(merged, two.release());
                one.reset(merged);
                que.push(std::move(one));
            }

            {
                assert(!que.empty());

                auto uni = std::move(que.front());
                assert(uni);

                leftist_tree_derived::merge_nodes(root_node, uni.release());
                assert(root_node != nullptr);

                if constexpr (has_parent)
                {
                    root_node->parent = nullptr;
                }
            }
        }

        // Time O(log(n)).
        constexpr auto push(const key_t &key) -> node_t *
        {
            auto owner = std::make_unique<node_t>();
            auto *node = owner.get();
            node->key = key;

            leftist_tree_derived::merge_nodes(root_node, node);
            assert(root_node != nullptr);

            if constexpr (has_parent)
            {
                root_node->parent = nullptr;
            }

            owner.release();
            return node;
        }

        // Time O(log(n)).
        constexpr void merge(leftist_tree_derived &other) noexcept
        {
            if (this == &other)
            {
                return;
            }

            leftist_tree_derived::merge_nodes(root_node, other.root_node);

            if constexpr (has_parent)
            {
                if (root_node != nullptr)
                {
                    root_node->parent = nullptr;
                }
            }

            other.root_node = nullptr;
        }

        // Time O(log(n)).
        constexpr void pop() noexcept
        {
            assert(root_node != nullptr);

            auto *new_root = root_node->left;
            leftist_tree_derived::merge_nodes(new_root, root_node->right);

            if constexpr (has_parent)
            {
                if (new_root != nullptr)
                {
                    new_root->parent = nullptr;
                }
            }

            delete root_node;

            root_node = static_cast<gsl::owner<node_t *>>(new_root);
        }

        constexpr void validate(const std::string &message) const
        {
            assert(!message.empty());

            if (this->root_node == nullptr)
            {
                return;
            }

            if (this->root_node->height <= 0) [[unlikely]]
            {
                throw std::out_of_range(
                    message + " The root height " + std::to_string(this->root_node->height) + " must be positive.");
            }

            if constexpr (has_parent)
            {
                if (this->root_node->parent != nullptr) [[unlikely]]
                {
                    throw std::out_of_range(message + " The root has a parent.");
                }
            }

            std::queue<const node_t *> que;
            que.push(this->root_node);

            do
            {
                const auto *const node = que.front();
                assert(node != nullptr);
                que.pop();

                leftist_tree_derived::validate_node(node, que, node->left, message);
                leftist_tree_derived::validate_node(node, que, node->right, message);

                const auto hei = node->left != nullptr ? node->left->height : 0;
                const auto hei2 = node->right != nullptr ? node->right->height : 0;

                if (hei < hei2) [[unlikely]]
                {
                    throw std::out_of_range(message + " The left height " + std::to_string(hei) +
                        " is less than right " + std::to_string(hei2));
                }
            } while (!que.empty());
        }

protected:
        [[nodiscard]] static constexpr auto node_exists(const node_t *root1, const node_t *const node) -> bool
        {
            assert(root1 != nullptr && node != nullptr);

            for (;;)
            {
                assert(root1 != nullptr);

                if (root1 == node)
                {
                    return true;
                }

                const auto *const left = root1->left;
                const auto *const right = root1->right;

                if (left == nullptr)
                {
                    assert(right == nullptr);
                    return false;
                }

                if (right != nullptr && node_exists(right, node))
                {
                    return true;
                }

                root1 = left;
            }
        }

private:
        gsl::owner<node_t *> root_node{};
    };
} // namespace Standard::Algorithms::Trees
