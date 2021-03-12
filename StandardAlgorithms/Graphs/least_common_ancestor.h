#pragma once
#include"../Utilities/require_utilities.h"
#include<algorithm>
#include<cassert>
#include<cstddef>
#include<unordered_map>
#include<utility>
#include<vector>

namespace Standard::Algorithms::Trees::Inner
{
    // Swaps the element at "index" with the last item
    //  and pops the last element, all in O(1).
    template<class item_t>
    constexpr void erase_by_swap_with_last(std::vector<item_t> &data, const std::size_t index)
    {
        assert(index < data.size());

        auto &curr = data.at(index);
        std::swap(curr, data.back());
        data.pop_back();
    }

    template<class node_t, class level_t>
    constexpr auto add_found_node(const node_t *parent, const std::size_t depth, const node_t *node,
        std::vector<const node_t *> &source_nodes_left, level_t &levels, const std::size_t index,
        std::size_t &left_size) -> bool
    {
        const std::pair<const node_t *, std::pair<std::size_t, const node_t *>> level_entry(node, { depth, parent });

        const auto is_inserted = levels.insert(level_entry);
        if (!is_inserted.second) [[unlikely]]
        {
            auto str = ::Standard::Algorithms::Utilities::w_stream();
            str << "Error: cannot insert a node (" << node->value << ") a second time."
                << " Probably sourceNodes has repeating values.";
            throw std::runtime_error(str.str());
        }

        if (1U == left_size)
        {
            return true;
        }

        erase_by_swap_with_last(source_nodes_left, index);
        --left_size;

        return false;
    }

    // Return true if no more nodes left to match.
    template<class node_t, class level_t>
    constexpr auto match_and_unmark(const std::size_t depth, const node_t *current,
        std::vector<const node_t *> &source_nodes_left, level_t &levels) -> bool
    {
        const auto left = current->left;
        const auto right = current->right;
        auto left_size = source_nodes_left.size();

        for (std::size_t index{}; index < left_size;)
        {
            const auto *node = source_nodes_left[index];
            if (left == node || right == node)
            {
                if (add_found_node(current, 1U + depth, node, source_nodes_left, levels, index, left_size))
                {
                    return true;
                }
            }
            else if (current == node)
            {
                const node_t *unknown_parent = nullptr;

                if (add_found_node(unknown_parent, depth, node, source_nodes_left, levels, index, left_size))
                {
                    return true;
                }
            }
            else
            {
                ++index;
            }
        }

        return false;
    }

    template<class node_t, class level_t>
    constexpr void explore_level(const std::size_t depth, const std::vector<const node_t *> &current_list,
        std::vector<const node_t *> &next_list, level_t &levels, std::vector<const node_t *> &source_nodes_left)
    {
        const auto current_size = current_list.size();

        for (std::size_t index{}; index < current_size; ++index)
        {
            auto *current = current_list[index];

            if (const auto has_found_everything = match_and_unmark(depth, current, source_nodes_left, levels);
                has_found_everything)
            {// Clear to stop the search.
                next_list.clear();
                return;
            }

            if (current->left != nullptr)
            {
                next_list.push_back(current->left);
            }

            if (current->right != nullptr)
            {
                next_list.push_back(current->right);
            }
        }
    }

    template<class node_t, class level_t>
    constexpr void find_node_levels(const node_t *const root, const std::vector<const node_t *> &source_nodes,
        level_t &levels, const std::size_t depth_max = 0U)
    {
        throw_if_null("root", root);

        const auto size = source_nodes.size();
        if (0U == size) [[unlikely]]
        {
            throw std::runtime_error("There must be sourceNodes in Find Node Levels.");
        }

        levels.clear();

        // Do not search more than necessary.
        const auto depth_max_verified = 0U == depth_max ? std::numeric_limits<std::size_t>::max() : depth_max;

        // Nodes will be deleted once they are found to make the search faster.
        auto source_nodes_left = source_nodes;

        std::vector<const node_t *> current_list;
        current_list.push_back(root);

        std::vector<const node_t *> next_list;

        std::size_t depth{};

        // Run BFS.
        do
        {
            next_list.clear();

            explore_level(depth, current_list, next_list, levels, source_nodes_left);

            std::swap(current_list, next_list);
        } while (!current_list.empty() && ((++depth) <= depth_max_verified));
    }

    template<class node_t, class level_t>
    constexpr auto check_node_exists(const node_t *const root, const level_t &levels, const node_t *node)
        -> std::pair<std::size_t, const node_t *>
    {
        const auto end = levels.end();
        const auto found = levels.find(node);

        if (end != found) [[likely]]
        {
            return found->second;
        }

        auto str = ::Standard::Algorithms::Utilities::w_stream();
        str << "The node (" << node->value << ") was not found in the tree rooted at (" << root->value << ").";
        throw std::runtime_error(str.str());
    }

    template<class node_t, class level_t>
    constexpr auto match_subtree(const std::vector<const node_t *> &source_nodes, const node_t **result,
        const node_t *try_node, std::size_t &finds, const node_t *other_node, std::size_t &level_max,
        const char *const current_branch_name, level_t &levels, const char *const other_branch_name)
    {
        if (try_node == nullptr)
        {
            return false;
        }

        find_node_levels(try_node, source_nodes, levels, level_max);

        finds = levels.size();

        if (2U == finds)
        {
            --level_max;
            *result = try_node;
            return true;
        }

        if (0U != finds)
        {
            return false;
        }

        if (nullptr != other_node) [[likely]]
        {
            --level_max;
            *result = other_node;
            return true;
        }

        auto str = ::Standard::Algorithms::Utilities::w_stream();
        str << "Internal error: node (" << (*result)->value << ") has no '" << other_branch_name
            << "' node, and nodes to find and not in the '" << current_branch_name << "' subtree.";

        throw std::runtime_error(str.str());
    }
} // namespace Standard::Algorithms::Trees::Inner

namespace Standard::Algorithms::Trees
{
    // Memory use must be minimized - tree can be big.
    // There is no "parent" link.
    template<class node_t, class level_t = std::unordered_map<const node_t *, std::pair<std::size_t, const node_t *>>>
    constexpr auto least_common_ancestor_without_parent(
        const node_t *const root, const node_t *const node1, const node_t *const node2) -> const node_t *
    {
        throw_if_null("root", root);
        throw_if_null("node1", node1);
        throw_if_null("node2", node2);

        // Some shortcuts.
        if (node1 == root || node1 == node2 || node1->left == node2 || node1->right == node2)
        {
            return node1;
        }

        if (node2 == root || node2->left == node1 || node2->right == node1)
        {
            return node2;
        }

        std::vector<const node_t *> source_nodes;
        source_nodes.push_back(node1);
        source_nodes.push_back(node2);

        level_t levels{};
        Inner::find_node_levels<node_t, level_t>(root, source_nodes, levels);

        const auto pair1 = Inner::check_node_exists<node_t, level_t>(root, levels, node1);
        const auto pair2 = Inner::check_node_exists<node_t, level_t>(root, levels, node2);

        ////These shortcuts rarely happen.
        // if (nullptr != pair1.second)
        //{
        //   if (pair1.second == pair2.second)
        //   {//The same parent.
        //     return pair1.second;
        //   }
        //
        //   if (node2->left == pair1.second || node2->right == pair1.second)
        //   {
        //     return node2;
        //   }
        // }
        //
        // if (nullptr != pair2.second
        //   &&(node1->left == pair2.second || node1->right == pair2.second))
        //{
        //   return node1;
        // }

        auto result = root;
        auto level_max = std::max(pair1.first, pair2.first);

        for (;;)
        {
            if (node1 == result || node2 == result)
            {
                break;
            }

            const auto *const left_branch_name = "left";
            const auto *const right_branch_name = "right";
            std::size_t left_finds{};

            if (Inner::match_subtree<node_t, level_t>(source_nodes, &result, result->left, left_finds, result->right,
                    level_max, left_branch_name, levels, right_branch_name))
            {
                continue;
            }

            std::size_t right_finds{};

            if (Inner::match_subtree<node_t, level_t>(source_nodes, &result, result->right, right_finds, result->left,
                    level_max, right_branch_name, levels, left_branch_name))
            {
                continue;
            }

            if (1U == left_finds && 1U == right_finds) [[likely]]
            {
                break;
            }

            {
                auto str = ::Standard::Algorithms::Utilities::w_stream();
                str << "Internal error: Should never get here."
                    << " leftFinds " << left_finds << ", rightFinds " << right_finds << ", result->value '"
                    << result->value << "'.";

                throw std::runtime_error(str.str());
            }
        }

        return result;
    }
} // namespace Standard::Algorithms::Trees
