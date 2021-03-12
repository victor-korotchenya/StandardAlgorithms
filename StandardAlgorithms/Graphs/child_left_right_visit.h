#pragma once
#include<cassert>
#include<cstddef>
#include<functional>

namespace Standard::Algorithms::Trees
{
    // root1
    //   /
    // child1 - next1 - next2 - next3
    //   /            /           /          /
    //                                     child8 - next100 - next101 - result
    //                                       /            /              /
    template<class node_t, class buffer_t>
    constexpr auto top_rightmost_leaf(const node_t &node, buffer_t &buffer1, std::size_t &index) -> const node_t &
    {
        assert(!buffer1.empty());

        const auto *root1 = &node;

        while (root1->child != nullptr)
        {
            assert(index < buffer1.size());

            buffer1.at(index++) =
                // Using 2 storages for pointers is not economic.
                // NOLINTNEXTLINE
                const_cast<node_t *>(root1);

            root1 = root1->child;

            while (root1->next != nullptr)
            {
                root1 = root1->next;
            }
        }

        assert(root1 != nullptr);

        return *root1;
    }

    template<class node_t, class buffer_t, class func_t>
    constexpr auto go_up_until_another_child(const node_t &node, buffer_t &buffer1, func_t &func, std::size_t &index)
        -> const node_t *
    {
        for (const auto *root1 = &node;;)
        {
            assert(root1 != nullptr && index < buffer1.size());

            const auto *parent1 = 0U < index ? buffer1[index - 1U] : nullptr;

            const auto *pre = root1->prev; // root1 might be deleted in func.
            func(parent1, root1);

            if (pre == nullptr)
            {
                assert(index == 0U);
                return pre;
            }

            if (pre->child == nullptr)
            {
                root1 = pre;
                continue;
            }

            if (pre->child != root1)
            {
                return pre;
            }

            assert(0U < index);
            --index;
            root1 = pre;
        }
    }

    // Time O(n). Small stack.
    template<class node_t, class buffer_t, class func_t>
    constexpr void child_left_right_visit(const node_t &node, buffer_t &buffer1,
        // std::function<void(const node_t*parent1, const node_t*child1)>
        func_t &func)
    {
        assert(!buffer1.empty());

        const auto *root1 = &node;

        for (std::size_t index{};;)
        {
            assert(root1 != nullptr);

            root1 = &(top_rightmost_leaf(*root1, buffer1, index));

            assert(root1 != nullptr && root1->child == nullptr && root1->next == nullptr);

            root1 = go_up_until_another_child(*root1, buffer1, func, index);
            if (root1 == nullptr)
            {
                assert(index == 0U);
                return;
            }
        }
    }
} // namespace Standard::Algorithms::Trees
