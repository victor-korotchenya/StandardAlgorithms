#pragma once
// "reconstruct_tree.h"
#include"../Numbers/arithmetic.h"
#include"../Utilities/require_utilities.h"
#include<gsl/gsl>
#include<stack>
#include<unordered_set>
#include<vector>

namespace Standard::Algorithms::Trees::Inner
{
    // Pre-index, in-index, length, editable node.
    template<class node_t>
    using construct_tuple_t = std::tuple<std::size_t, std::size_t, std::size_t, node_t *>;

    template<class node_t>
    constexpr void validate_construct_tuple(const construct_tuple_t<node_t> &tup, const std::size_t size)
    {
        const auto &pre_index = std::get<0>(tup);
        const auto &in_index = std::get<1>(tup);
        const auto &length = std::get<2>(tup);
        const auto &root = std::get<3>(tup);

        bool has_error{};
        auto str = ::Standard::Algorithms::Utilities::w_stream();

        if (length <= 1U)
        {
            has_error = true;
            str << " Bad length " << length << ".";
        }

        if (!Standard::Algorithms::Numbers::is_within_inclusive(pre_index, { std::size_t{}, size - 1U }))
        {
            has_error = true;
            str << " Bad preIndex " << pre_index << ".";
        }

        if (!Standard::Algorithms::Numbers::is_within_inclusive(in_index, { std::size_t{}, size - 1U }))
        {
            has_error = true;
            str << " Bad inIndex " << in_index << ".";
        }

        if (nullptr == root)
        {
            has_error = true;
            str << "The root is null.";
        }

        if (has_error)
        {
            str << " preIndex " << pre_index << ", inIndex " << in_index << ", length " << length << ", size " << size;

            if (root)
            {
                str << ", root->value " << root->value << ".";
            }
            else
            {
                str << ", root->value null.";
            }

            throw_exception(str);
        }
    }

    template<class int_t, class data>
    constexpr void require_unique_permuted(const data &pre_order, const std::size_t size, const data &in_order)
    {
        std::unordered_set<int_t> pre_set;

        for (std::size_t index{}; index < size; ++index)
        {
            const auto &value = pre_order[index];
            const auto inserted = pre_set.insert(value);
            if (!inserted.second)
            {
                auto str = ::Standard::Algorithms::Utilities::w_stream();
                str << "The value (" << value << ") has already been added to the first set.";
                throw_exception(str);
            }
        }

        for (std::size_t index{}; index < size; ++index)
        {
            const auto &value = in_order[index];
            const auto deleted = pre_set.erase(value);
            if (1U != deleted)
            {
                auto str = ::Standard::Algorithms::Utilities::w_stream();
                str << "The value (" << value << ") must belong to the second subset, deleted " << deleted << ".";
                throw_exception(str);
            }
        }
    }

    template<class int_t, class data>
    constexpr auto find_in_order_value(
        const data &in_order, const std::size_t in_index, const int_t &value, const std::size_t length) -> std::size_t
    {
        const auto max_index_inclusive = in_index + length - 1U;
        auto result = in_index + 1U;

        while (result <= max_index_inclusive && value != in_order[result])
        {
            ++result;
        }

        if (max_index_inclusive < result)
        {
            auto str = ::Standard::Algorithms::Utilities::w_stream();
            str << "The item value " << value << " is not found in the in-order list.";
            throw_exception(str);
        }

        return result;
    }

    template<class int_t, class node_t, class data>
    constexpr void reconstruct_tree_step(const data &pre_order, const data &in_order,
#ifdef _DEBUG
        const std::size_t size,
#endif
        std::stack<construct_tuple_t<node_t>> &sta)
    {
        assert(!sta.empty());

        const auto top = sta.top();

        const auto &pre_index = std::get<0>(top);
        const auto &in_index = std::get<1>(top);
        const auto &length = std::get<2>(top);
        auto root = std::get<3>(top);

#if _DEBUG
        validate_construct_tuple(top, size);
#endif

        sta.pop();

        const auto create_node = [](const auto &datum, gsl::owner<node_t *> *ppp)
        {
            assert(ppp != nullptr && *ppp == nullptr);

            gsl::owner<node_t *> &node = *ppp;
            node = new node_t();
            node->value = datum;
        };

        if (const auto is_right_subtree_only = pre_order[pre_index] == in_order[in_index]; is_right_subtree_only)
        {
            create_node(pre_order[pre_index + 1U], &(root->right));

            if (const auto go_right = 2U < length; go_right)
            {
                sta.emplace(pre_index + 1U, in_index + 1U, length - 1U, root->right);
            }

            return;
        }

        // There must be left subtree.
        create_node(pre_order[pre_index + 1U], &(root->left));

        const auto root_index = find_in_order_value<int_t, data>(in_order, in_index, pre_order[pre_index], length);

        const auto root_delta = root_index - in_index;
        const auto go_left = 1U < root_delta;
        if (go_left)
        {
            sta.emplace(pre_index + 1U, in_index, root_delta, root->left);
        }

        if (const auto has_right = root_delta + 1U < length; !has_right)
        {
            return;
        }

        create_node(pre_order[pre_index + root_delta + 1U], &(root->right));

        if (const auto go_right = root_delta + 2U < length; !go_right)
        {
            return;
        }

        sta.emplace(pre_index + root_delta + 1U, root_index + 1U, length - (root_delta + 1U), root->right);
    }
} // namespace Standard::Algorithms::Trees::Inner

namespace Standard::Algorithms::Trees
{
    // The tree must have unique elements.
    template<class int_t, class node_t, class data>
    constexpr void reconstruct_tree(
        const data &pre_order, const data &in_order, const std::size_t size, gsl::owner<node_t *> *head)
    {
        throw_if_null("head", head);
        throw_if_not_null("*head", *head);

        if (0U == size)
        {
            return;
        }

        Inner::require_unique_permuted<int_t>(pre_order, size, in_order);

        gsl::owner<node_t *> &owner = *head;
        owner = new node_t();
        owner->value = pre_order[0];

        if (1U == size)
        {
            return;
        }

        std::stack<Inner::construct_tuple_t<node_t>> sta;
        sta.emplace(0U, 0U, size, owner);

        do
        {
            Inner::reconstruct_tree_step<int_t, node_t, data>(pre_order, in_order,
#ifdef _DEBUG
                size,
#endif
                sta);
        } while (!sta.empty());
    }
} // namespace Standard::Algorithms::Trees
