#pragma once
#include"../Utilities/project_constants.h"
#include"../Utilities/zu_string.h"
#include<array>
#include<cassert>
#include<cstddef>
#include<stdexcept>
#include<string>

namespace Standard::Algorithms::Heaps
{
    extern const std::array<std::string, ::Standard::Algorithms::Utilities::max_logn_algorithm_depth + 1> &prefix_cache;

    template<class node_t, class shall_continue_t>
    constexpr void print_heap_helper(
        const node_t *root, std::string &descr, shall_continue_t &shall_continue, const std::uint32_t depth = {})
    {
        assert(root != nullptr && depth < ::Standard::Algorithms::Utilities::max_logn_algorithm_depth);

        const auto &prefix = prefix_cache.at(depth);
        assert(depth == 0U || !prefix.empty());

        const auto *node = root;

        do
        {
            descr += prefix;
            descr += node_to_string(*node);
            descr += '\n';

            if (node->child)
            {
                print_heap_helper<node_t, shall_continue_t>(node->child, descr, shall_continue, depth + 1);
            }

            node = node->right;
        } while (node != nullptr && shall_continue(root, node));
    }

    template<class node_t>
    constexpr void print_heap_helper(const node_t *root, std::string &descr)
    {
        const auto shall_continue = [] [[nodiscard]] (const node_t *, const node_t *) -> bool
        {
            return true;
        };
        using shall_continue_t = decltype(shall_continue);

        print_heap_helper<node_t, shall_continue_t>(root, descr, shall_continue);
    }

    template<bool root_has_left, class node_t, class shall_continue_t, class node_test_t>
    constexpr auto validate_heap_helper(const node_t *root, shall_continue_t &shall_continue, node_test_t &node_test,
        const std::uint32_t depth = {}) -> std::size_t
    {
        assert(root != nullptr && depth < ::Standard::Algorithms::Utilities::max_logn_algorithm_depth);

        if constexpr (!root_has_left)
        {
            if (root->left != nullptr) [[unlikely]]
            {
                const auto err = std::string("The node ")
                                     .append(node_to_string(*root))
                                     .append(" has left ")
                                     .append(node_to_string(*(root->left)));

                throw std::runtime_error(err);
            }
        }

        std::size_t count{};
        const auto *node = root;
        do
        {
            assert(node != nullptr);

            if (const auto parent1 = node->parent; parent1 != nullptr && node->key < parent1->key) [[unlikely]]
            {
                const auto err = std::string("The node ")
                                     .append(node_to_string(*node))
                                     .append(" has parent ")
                                     .append(node_to_string(*parent1));

                throw std::runtime_error(err);
            }

            if (const auto *child1 = node->child; child1 != nullptr)
            {
                assert(node == child1->parent);

                count += validate_heap_helper<root_has_left, node_t, shall_continue_t, node_test_t>(
                    child1, shall_continue, node_test, depth + 1U);
            }

            ++count;

            if (node->right != nullptr)
            {
                node_test(node, node->right);
            }

            node = node->right;
        } while (node != nullptr && shall_continue(root, node));

        return count;
    }

    template<bool root_has_left, class node_t, class shall_continue_t, class node_test_t>
    constexpr void validate_heap(
        const node_t *root, const std::size_t expected_size, shall_continue_t &shall_continue, node_test_t &node_test)
    {
        assert((root != nullptr) == (0U < expected_size));

        const auto actual_count =
            root != nullptr ? validate_heap_helper<root_has_left>(root, shall_continue, node_test) : 0U;

        if (expected_size == actual_count) [[likely]]
        {
            return;
        }

        const auto err = std::string("Actual nodes ")
                             .append(::Standard::Algorithms::Utilities::zu_string(actual_count))
                             .append(" != ")
                             .append(::Standard::Algorithms::Utilities::zu_string(expected_size))
                             .append(" expected size.");

        throw std::runtime_error(err);
    }

    template<bool root_has_left, class node_t, class node_test_t>
    constexpr void validate_heap(const node_t *root, const std::size_t expected_size, node_test_t &node_test)
    {
        const auto shall_continue = [] [[nodiscard]] (const node_t *, const node_t *) -> bool
        {
            return true;
        };
        using shall_continue_t = decltype(shall_continue);

        validate_heap<root_has_left, node_t, shall_continue_t, node_test_t>(
            root, expected_size, shall_continue, node_test);
    }
} // namespace Standard::Algorithms::Heaps
