#pragma once
#include"../Utilities/project_constants.h"
#include<algorithm>
#include<cassert>
#include<cstddef>
#include<cstdint>
#include<stdexcept>
#include<string>
#include<vector>

namespace Standard::Algorithms::Trees
{
    namespace Inner
    {
        inline constexpr void pop_back_false(std::vector<bool> &storage)
        {
            assert(!storage.empty() && !storage.back());
            assert(std::find(storage.cbegin(), storage.cend(), true) != storage.cend());

            do
            {
                assert(!storage.empty());
                storage.pop_back();
            } while (!storage.empty() && // Pacify the compiler.
                !storage.back());

            assert(!storage.empty());
        }

        inline constexpr void ensure_byte_size(std::vector<bool> &storage)
        {
            constexpr auto byte_bits = 8U;
            constexpr auto mask = byte_bits - 1U;

            auto leftover = storage.size() & mask;
            if (leftover == 0U)
            {
                return;
            }

            leftover = byte_bits - leftover;
            assert(0U < leftover && leftover <= mask);

            storage.resize(storage.size() + leftover);
            assert((storage.size() & mask) == 0U);
        }

        template<class node_t>
        constexpr void bin3_ser_rec(const node_t *parent, std::vector<bool> &storage, std::uint32_t depth = {})
        {
            if (::Standard::Algorithms::Utilities::stack_max_size < ++depth) [[unlikely]]
            {
                throw std::runtime_error("Too deep stack in binary tree serialize.");
            }

            for (;;)
            {
                assert(parent != nullptr && (parent->left != nullptr || parent->right != nullptr));

                const auto &left = parent->left;
                const auto is_left = left != nullptr;
                storage.push_back(is_left);

                if (is_left)
                {
                    if (left->left == nullptr && left->right == nullptr)
                    {
                        storage.resize(storage.size() + 2U);
                    }
                    else
                    {
                        bin3_ser_rec<node_t>(left, storage, depth);
                    }
                }

                parent = parent->right;

                const auto is_right = parent != nullptr;
                storage.push_back(is_right);

                if (!is_right)
                {
                    return;
                }

                if (parent->left == nullptr && parent->right == nullptr)
                {
                    storage.resize(storage.size() + 2U);
                    return;
                }
            }
        }
    } // namespace Inner

    // Only the binary tree structure is serialized, excluding the first 1 and the last 0.
    // Given a tree on 5 nodes:
    //        A
    //      /
    //    B
    //   /  \;
    // C      D
    //           \;
    //             E
    // the full sequence has 5 ones and 6 zeros: 11100 10 100 0,
    // but the actually written sequence is: 1100 10 1
    // where the first 1 and all the tail 0s are discarded,
    // and then the storage might be padded with 0s to ensure that
    // the storage size is divisible by 8 which is the # of bits in a byte.
    // Thus, this binary tree uses just 1 byte of memory which very economical.
    // This code works best when all the nodes posses shallow left depth,
    // resulting in small stack size.
    template<class node_t>
    constexpr void binary_tree_serialize(const node_t *const root, std::vector<bool> &storage)
    {
        constexpr auto byte_bits = 8U;
        constexpr auto mask = byte_bits - 1U;
        assert((storage.size() & mask) == 0U);

        if (root == nullptr)
        {
            return;
        }

        const auto &left = root->left;
        const auto &right = root->right;
        if (left == nullptr && right == nullptr)
        {
            storage.resize(storage.size() + byte_bits);
            return;
        }

        Inner::bin3_ser_rec<node_t>(root, storage);

        Inner::pop_back_false(storage);
        Inner::ensure_byte_size(storage);
    }

    namespace Inner
    {
        template<class node_t, class build_node_t>
        struct bin3_deser_context final
        {
            const std::vector<bool> &storage;
            build_node_t &build_node;
            std::size_t index{};
        };

        template<class node_t, class build_node_t>
        [[nodiscard]] constexpr auto create_node_is_done(
            bin3_deser_context<node_t, build_node_t> &context, node_t *&node) -> bool
        {
            assert(node == nullptr && context.index <= context.storage.size());

            node = context.build_node();
            assert(node != nullptr);

            const auto done = context.index == context.storage.size();
            return done;
        }

        template<class node_t, class build_node_t>
        [[nodiscard]] constexpr auto has_processed_leaf(bin3_deser_context<node_t, build_node_t> &context) -> bool
        {
            assert(context.index < context.storage.size());

            const auto two_nulls = context.index + 2U <= context.storage.size() && !context.storage[context.index] &&
                !context.storage[context.index + 1U];

            if (two_nulls)
            {
                context.index += 2U;
            }

            return two_nulls;
        }

        template<class node_t, class build_node_t>
        constexpr void bin3_rec_deser(
            bin3_deser_context<node_t, build_node_t> &context, node_t *parent, std::uint32_t depth = {})
        {
            if (::Standard::Algorithms::Utilities::stack_max_size < ++depth) [[unlikely]]
            {
                throw std::runtime_error("Too deep stack in binary tree deserialize.");
            }

            for (;;)
            {
                assert(parent != nullptr && context.index < context.storage.size());

                const auto is_left = context.storage.at(context.index++);
                if (is_left)
                {
                    if (create_node_is_done<node_t, build_node_t>(context, parent->left))
                    {
                        return;
                    }

                    if (!has_processed_leaf<node_t, build_node_t>(context))
                    {
                        bin3_rec_deser<node_t, build_node_t>(context, parent->left, depth);
                    }
                }

                if (context.index == context.storage.size())
                {
                    return;
                }

                assert(context.index < context.storage.size());

                const auto is_right = context.storage.at(context.index++);
                if (!is_right || create_node_is_done<node_t, build_node_t>(context, parent->right) ||
                    has_processed_leaf<node_t, build_node_t>(context))
                {
                    return;
                }

                parent = parent->right;
            }
        }
    } // namespace Inner

    template<class node_t, class build_node_t>
    constexpr auto binary_tree_deserialize(const std::vector<bool> &storage, build_node_t build_node) -> node_t *
    {
        if (storage.empty())
        {
            return nullptr;
        }

        if (const auto offset = storage.size() & 7U; offset != 0U) [[unlikely]]
        {
            throw std::runtime_error("The storage offset(" + std::to_string(offset) + ") is non zero.");
        }

        Inner::bin3_deser_context<node_t, build_node_t> context{ storage, build_node };

        auto root = build_node();
        if (root == nullptr) [[unlikely]]
        {
            throw std::runtime_error("The first built root node is nullptr.");
        }

        Inner::bin3_rec_deser<node_t, build_node_t>(context, root);

        assert(0U < context.index && context.index <= storage.size());

        return root;
    }
} // namespace Standard::Algorithms::Trees
