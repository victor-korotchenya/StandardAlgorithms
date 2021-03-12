#pragma once
#include"interval_node.h"
// The 'node' must come before the 'tree'.
#include"avl_tree_base.h"

namespace Standard::Algorithms::Trees::Inner
{
    constexpr void all_overlaping_intervals_impl(
        const auto &parent, const auto &source_interval, auto &matches, std::uint32_t depth = {})
    {
        assert(is_valid_interval(parent.key));

        if (::Standard::Algorithms::Utilities::stack_max_size < ++depth) [[unlikely]]
        {
            throw std::runtime_error("Too deep stack in all_overlaping_intervals_impl.");
        }

        // Using an unsorted output should be faster; here prefer slower and sorted as the depth is logarithmic.

        for (const auto *parent_ptr = &parent;;)
        {
            assert(parent_ptr != nullptr && is_valid_interval(parent_ptr->key));

            if (parent_ptr->subtree_max < source_interval.first)
            {
                assert(!are_intervals_overlapping(source_interval, parent_ptr->key));

                return;
            }

            // todo(p4): could 'subtree_min' be beneficial here?

            if (const auto *const left = parent_ptr->left;
                left != nullptr && !(left->subtree_max < source_interval.first))
            {
                all_overlaping_intervals_impl(*left, source_interval, matches, depth);
            }

            if (const auto &this_cannot_decrease = parent_ptr->key.first; source_interval.second < this_cannot_decrease)
            {
                return;
            }

            if (are_intervals_overlapping(source_interval, parent_ptr->key))
            {
                matches.push_back(parent_ptr->key);
            }

            parent_ptr = parent_ptr->right;

            if (parent_ptr == nullptr)
            {
                return;
            }
        }
    }
} // namespace Standard::Algorithms::Trees::Inner

namespace Standard::Algorithms::Trees
{
    // The interval ends are inclusive.
    template<class coord_t, class key_t = std::pair<coord_t, coord_t>, class node_t = interval_node<coord_t, key_t>>
    using interval_tree = avl_tree_base<key_t, node_t>;

    [[nodiscard]] constexpr auto some_overlaping_interval(
        const auto &interval_tree, const auto &source_interval) noexcept
    {
        assert(is_valid_interval(source_interval));

        for (const auto *node = interval_tree.root();;)
        {
            if (node == nullptr || are_intervals_overlapping(source_interval, node->key))
            {
                return node;
            }

            const auto *const left1 = node->left;
            const auto go_left = left1 != nullptr && !(left1->subtree_max < source_interval.first);

            node = go_left ? node->left : node->right;
        }
    }

    constexpr void all_overlaping_intervals(const auto &interval_tree, const auto &source_interval, auto &matches)
    {
        assert(is_valid_interval(source_interval));

        matches.clear();

        const auto *node = interval_tree.root();
        if (node == nullptr)
        {
            return;
        }

        Inner::all_overlaping_intervals_impl(*node, source_interval, matches);
    }
} // namespace Standard::Algorithms::Trees
