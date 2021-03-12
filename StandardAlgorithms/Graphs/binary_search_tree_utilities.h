#pragma once
// "binary_search_tree_utilities.h"
#include"../Utilities/require_utilities.h"
#include"binary_tree_node.h"
#include<map>
#include<memory>
#include<sal.h>
#include<tuple>
#include<unordered_set>
#include<vector>

namespace Standard::Algorithms::Trees
{
    template<class value_t, class node_t = binary_tree_node<value_t>>
    struct binary_search_tree_utilities final
    {
        binary_search_tree_utilities() = delete;

        _Ret_maybenull_ static constexpr auto find(const _In_opt_ node_t *node, const _In_ value_t &value) noexcept
            -> const node_t *
        {
            while (node != nullptr && value != node->value)
            {
                node = value < node->value ? node->left : node->right;
            }

            return node;
        }

        _Ret_maybenull_ static constexpr auto minimum_value_node(const _In_opt_ node_t *node) noexcept -> const node_t *
        {
            if (nullptr == node)
            {
                return nullptr;
            }

            while (node->left != nullptr)
            {
                node = node->left;
            }

            return node;
        }

        _Ret_maybenull_ static constexpr auto maximum_value_node(const _In_opt_ node_t *node) noexcept -> const node_t *
        {
            if (nullptr == node)
            {
                return nullptr;
            }

            while (node->right != nullptr)
            {
                node = node->right;
            }

            return node;
        }

        static constexpr auto minimum_value(const _In_ node_t *node) -> const value_t &
        {
            throw_if_null("node", node);
            node = minimum_value_node(node);
            assert(node != nullptr);

            return node->value;
        }

        static constexpr auto maximum_value(const _In_ node_t *node) -> const value_t &
        {
            throw_if_null("node", node);
            node = maximum_value_node(node);
            assert(node != nullptr);

            return node->value;
        }

        // Whether the tree is a BST (binary search tree) WITHOUT duplicates.
        // Assume that left sub-tree values must be strictly less than current node value;
        // right sub-tree values strictly greater.
        static constexpr auto is_bst_without_duplicates(const _In_opt_ node_t *node) -> bool
        {
            if (nullptr == node)
            {
                return true;
            }

            const auto &actual_min = minimum_value(node);

            // todo(p3): can be done without minValue.
            const auto min_value = static_cast<value_t>(actual_min - static_cast<value_t>(1));

            if (actual_min < min_value)
            {// todo(p3): fix "min(max) -(+) value_t(1)" issue.
                auto str = ::Standard::Algorithms::Utilities::w_stream();
                str << "Left-most node has minimum value (" << actual_min << ") that is smaller than that minus one ("
                    << min_value << ").";
                throw_exception(str);
            }

            const auto &actual_max = maximum_value(node);
            const auto max_value = static_cast<value_t>(actual_max + static_cast<value_t>(1));
            if (max_value < actual_max)
            {
                auto str = ::Standard::Algorithms::Utilities::w_stream();
                str << "Right-most node has maximum value (" << actual_max << ") that is smaller than that plus one ("
                    << max_value << ").";
                throw_exception(str);
            }

            auto result = is_bst_no_duplicates_helper(*node, min_value, max_value);
            return result;
        }

        // Whether the tree is a weak binary search tree WITH duplicates.
        // Assume that left sub-tree values must be not greater than current node value;
        // right sub-tree values not less.
        static constexpr auto is_bst_duplicates_allowed(const _In_opt_ node_t *const root) -> bool
        {
            if (nullptr == root)
            {
                return true;
            }

            auto min_value = minimum_value(root); // to be changed.
            auto result = is_bst_duplicates_allowed_helper(*root, min_value);
            return result;
        }

        // Find the largest BST, WITH duplicates,
        // having all its descendants, if any exists.
        static constexpr auto largest_binary_search_tree_size(const _In_opt_ node_t *root)
            -> std::pair<std::size_t, const node_t *>
        {
            if (nullptr == root)
            {
                return {};
            }

            const auto temp = largest_bst_size_helper(*root);
            return { std::get<static_cast<std::int32_t>(index_kind::size)>(temp),
                std::get<static_cast<std::int32_t>(index_kind::node)>(temp) };
        }

        // Assume the "node" has a link to its parent.
        _Ret_maybenull_ static constexpr auto next_in_order_successor(const _In_opt_ node_t *node) noexcept
            -> const node_t *
        {
            if (nullptr == node)
            {
                return nullptr;
            }

            if (node->right != nullptr)
            {
                node = node->right;
                while (node->left != nullptr)
                {
                    node = node->left;
                }

                return node;
            }

            for (;;)
            {
                if (nullptr == node->parent)
                {
                    return nullptr;
                }

                if (node == node->parent->left)
                {
                    return node->parent;
                }

                node = node->parent;
            }
        }

        static constexpr void convert_bst_to_double_linked_list(_In_opt_ node_t **p_root)
        {
            if (nullptr == p_root || nullptr == *p_root)
            {
                return;
            }

            auto root = *p_root;

            // It is annoying to duplicate a function.
            // NOLINTNEXTLINE
            auto leftMost = const_cast<node_t *>(minimum_value_node(root));
            assert(leftMost != nullptr);

            node_t *parent{};
            do
            {
                while (root->left != nullptr)
                {
                    rotate_right(parent, &root);
                }

                parent = root;
                root = root->right;
            } while (root != nullptr);

            *p_root = root = leftMost;

            while (root->right != nullptr)
            {// Repair the backward links.
                (root->right)->left = root;
                root = root->right;
            }
        }

        // Use "convert_bst_to_double_linked_list" which is more reliable.
        static constexpr void convert_bst_to_double_linked_list_slow(_In_opt_ node_t **p_root)
        {
            if (nullptr == p_root || nullptr == *p_root)
            {
                return;
            }

            node_t *last_visited{};
            to_double_linked_list_slow_helper(*p_root, &last_visited);

            // It is annoying to duplicate a function.
            // NOLINTNEXTLINE
            auto head = const_cast<node_t *>(minimum_value_node(*p_root));

            if constexpr (::Standard::Algorithms::is_debug)
            {
                throw_if_null("minimum_value_node Slow", head);
            }

            *p_root = head;
        }

        // The (*pRoot)->right must be not null!
        static constexpr void rotate_left(_Inout_opt_ node_t *parent, _Inout_ node_t **p_root)
        {
            if constexpr (::Standard::Algorithms::is_debug)
            {
                throw_if_null("RotateLeft.pRoot", p_root);
                throw_if_null("RotateLeft.*pRoot", *p_root);
            }

            // The parent can have R as right node as well.
            //
            //                             parent
            //              R                        x
            //        left           right
            //                  rightLeft rightRight
            //
            // Left rotation:
            // Before: parent(R-, x), R(left, right-), right(rightLeft-, rightRight).
            // After: parent(right+, x), R(left, rightLeft+); right(R+, rightRight) is new root.
            //
            //                             parent
            //               right                   x
            //         R           rightRight
            //     left rightLeft
            auto *const root = *p_root;
            auto *const right = root->right;

            if constexpr (::Standard::Algorithms::is_debug)
            {
                throw_if_null("RotateLeft.right", right);
            }

            root->right = right->left;
            right->left = root;

            if (parent != nullptr)
            {
                if (root == parent->left)
                {
                    parent->left = right;
                }
                else
                {
                    if constexpr (::Standard::Algorithms::is_debug)
                    {
                        if (root != parent->right) [[unlikely]]
                        {
                            throw std::runtime_error("Inner error in RotateLeft: root != parent->right.");
                        }
                    }

                    parent->right = right;
                }
            }

            *p_root = right;
        }

        // The (*pRoot)->left must be not null!
        static constexpr void rotate_right(_Inout_opt_ node_t *parent, _Inout_ node_t **p_root)
        {
            if constexpr (::Standard::Algorithms::is_debug)
            {
                throw_if_null("RotateRight.pRoot", p_root);
                throw_if_null("RotateRight.*pRoot", *p_root);
            }

            // The parent can have R as left node as well.
            //
            //      parent
            // x              R
            //        left           right
            // leftLeft leftRight
            //
            // Right rotation:
            // Before: parent(x, R-), R(left-, right), left(leftLeft, leftRight-).
            // After: parent(x, left+), R(leftRight+, right); left(leftLeft, R+) is new root.
            //
            //    parent
            // x             left
            //       leftLeft             R
            //                   leftRight right
            auto *const root = *p_root;
            auto *const left = root->left;

            if constexpr (::Standard::Algorithms::is_debug)
            {
                throw_if_null("RotateRight.left", left);
            }

            root->left = left->right;
            left->right = root;

            if (parent != nullptr)
            {
                if (root == parent->right)
                {
                    parent->right = left;
                }
                else
                {
                    if constexpr (::Standard::Algorithms::is_debug)
                    {
                        if (root != parent->left) [[unlikely]]
                        {
                            throw std::runtime_error("Inner error in RotateRight: root != parent->left.");
                        }
                    }

                    parent->left = left;
                }
            }

            *p_root = left;
        }

        // Given Distinct (!) numbers, construct a BST:
        // 0-th element becomes the root;
        // if smaller, go left, else go right;
        // nowhere to go - create a (left or right) node.
        //
        // Return an array, having (size - 1) numbers,
        // specifying, for each number, its parent number.
        // E.g. given {2,4,1,6,5}, the tree is:
        //    2
        //  1   4
        //        6
        //      5
        //  The parent values are:
        //  2   2
        //        4
        //      6
        template<class unique_ptr_tree>
        static auto construct_from_distinct_numbers(const std::vector<value_t> &distinct_data, unique_ptr_tree &tree)
            -> std::vector<value_t>
        {
            throw_if_empty("distinct_data", distinct_data);

            const auto size = distinct_data.size();
            std::map<value_t, node_t *> numbers;
            {
                const auto &top = distinct_data.at(0);

                // auto uni = std::make_unique<node_t>(top);
                auto uni = std::make_unique<node_t>();
                auto node = uni.get();
                node->value = top;

                tree.reset(uni.release());
                numbers.emplace(top, node);
            }

            std::vector<value_t> parents(size - 1U);
            std::unordered_set<value_t> left;

            for (std::size_t index = 1; index < size; ++index)
            {
                auto ite = numbers.upper_bound(distinct_data[index]);
                // if (numbers.end() != ite &&left.end() == left.find(*ite))
                //{
                //   //left[*ite] = distinct_data[index];
                //   left.insert(*ite);
                // }
                // else
                //{
                //   --ite;
                //   //right[*ite] = distinct_data[index];
                // }
                node_t **parent_node{};
                if (numbers.end() == ite || !(left.insert(ite->first).second))
                {
                    --ite;
                    parent_node = &(ite->second->right);
                }
                else
                {
                    parent_node = &(ite->second->left);
                }

                // auto uni = std::make_unique<node_t>(distinct_data[index]);
                auto uni = std::make_unique<node_t>();
                auto node = uni.get();
                node->value = distinct_data[index];

                parents[index - 1U] = ite->first;
                numbers.emplace(distinct_data[index], node);
                *parent_node = uni.release();
            }

            return parents;
        }

private:
        // It is pre-order traversal.
        // In-order uses one fewer parameter.
        static constexpr auto is_bst_no_duplicates_helper(
            const node_t &node, const value_t &min_value, const value_t &max_value) -> bool
        {
            const auto &current = node.value;

            if (const auto has_match = min_value < current && current < max_value; !has_match)
            {
                return false;
            }

            if (const auto left_subtree_match =
                    nullptr == node.left || is_bst_no_duplicates_helper(*(node.left), min_value, current);
                !left_subtree_match)
            {
                return false;
            }

            const auto right_subtree_match =
                nullptr == node.right || is_bst_no_duplicates_helper(*(node.right), current, max_value);
            return right_subtree_match;
        }

        // Use in-order traversal: items must be sorted.
        static constexpr auto is_bst_duplicates_allowed_helper(const node_t &node, value_t &min_value) -> bool
        {
            if (const auto left_subtree_match =
                    nullptr == node.left || is_bst_duplicates_allowed_helper(*(node.left), min_value);
                !left_subtree_match)
            {
                return false;
            }

            // In-order.
            if (node.value < min_value)
            {
                return false;
            }

            min_value = node.value;

            const auto right_subtree_match =
                nullptr == node.right || is_bst_duplicates_allowed_helper(*(node.right), min_value);
            return right_subtree_match;
        }

        enum class index_kind : std::uint8_t
        {
            size,
            min,
            max,
            node,
        };

        using size_min_max_node_t = std::tuple<std::size_t, value_t, value_t, const node_t *>;

        // Post-order.
        static constexpr auto largest_bst_size_helper(const node_t &root) -> size_min_max_node_t
        {
            if (nullptr == root.left)
            {
                if (const auto is_leaf = nullptr == root.right; is_leaf)
                {
                    return { 1U, root.value, root.value, &root };
                }

                const auto only_right = largest_bst_size_helper(*(root.right));

                if (const auto is_bst =
                        root.right == std::get<static_cast<std::int32_t>(index_kind::node)>(only_right) &&
                        !(std::get<static_cast<std::int32_t>(index_kind::min)>(only_right) < root.value);
                    !is_bst)
                {
                    return only_right;
                }

                return { std::get<static_cast<std::int32_t>(index_kind::size)>(only_right) + 1U, root.value,
                    std::get<static_cast<std::int32_t>(index_kind::max)>(only_right), &root };
            }

            if (const auto is_only_left = nullptr == root.right; is_only_left)
            {
                const auto only_left = largest_bst_size_helper(*(root.left));

                if (const auto is_bst = root.left == std::get<static_cast<std::int32_t>(index_kind::node)>(only_left) &&
                        !(root.value < std::get<static_cast<std::int32_t>(index_kind::max)>(only_left));
                    !is_bst)
                {
                    return only_left;
                }

                return { std::get<static_cast<std::int32_t>(index_kind::size)>(only_left) + 1U,
                    std::get<static_cast<std::int32_t>(index_kind::min)>(only_left), root.value, &root };
            }

            // Both
            const auto left_result = largest_bst_size_helper(*(root.left));
            const auto right_result = largest_bst_size_helper(*(root.right));

            if (const auto is_bst_both =
                    root.left == std::get<static_cast<std::int32_t>(index_kind::node)>(left_result) &&
                    !(root.value < std::get<static_cast<std::int32_t>(index_kind::max)>(left_result))
                    //
                    && root.right == std::get<static_cast<std::int32_t>(index_kind::node)>(right_result) &&
                    !(std::get<static_cast<std::int32_t>(index_kind::min)>(right_result) < root.value);
                is_bst_both)
            {
                return { std::get<static_cast<std::int32_t>(index_kind::size)>(left_result) +
                        std::get<static_cast<std::int32_t>(index_kind::size)>(right_result) + 1U,
                    std::get<static_cast<std::int32_t>(index_kind::min)>(left_result),
                    std::get<static_cast<std::int32_t>(index_kind::max)>(right_result), &root };
            }

            const auto is_left_smaller = std::get<static_cast<std::int32_t>(index_kind::size)>(left_result) <
                std::get<static_cast<std::int32_t>(index_kind::size)>(right_result);

            return is_left_smaller ? right_result : left_result;
        }

        static constexpr void to_double_linked_list_slow_helper(node_t *root, node_t **p_last_visited)
        {
            if (root->left != nullptr)
            {
                to_double_linked_list_slow_helper(root->left, p_last_visited);

                if constexpr (::Standard::Algorithms::is_debug)
                {
                    throw_if_null("*pLastVisited at left node", *p_last_visited);
                }
            }

            root->left = *p_last_visited;
            if (*p_last_visited != nullptr)
            {
                (*p_last_visited)->right = root;
            }

            *p_last_visited = root;

            if (root->right == nullptr)
            {
                return;
            }

            to_double_linked_list_slow_helper(root->right, p_last_visited);

            if constexpr (::Standard::Algorithms::is_debug)
            {
                throw_if_null("right node", root->right);
            }
        }
    };
} // namespace Standard::Algorithms::Trees
