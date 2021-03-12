#pragma once
#include"../Numbers/to_unsigned.h"
#include"binary_tree_utilities.h"
#include<vector>

namespace Standard::Algorithms::Heaps
{
    // Min-max heap, or double-ended priority queue, DEPQ.
    // Min(max) on even(odd) levels; the root level is 0.
    // O(log(n)) time for many operations.
    template<class item_t>
    struct min_max_heap final
    {
        // Time O(1).
        [[nodiscard]] constexpr auto size() const noexcept
        {
            return Data.size();
        }

        // Time O(1).
        [[nodiscard]] constexpr auto is_empty() const noexcept
        {
            return size() == 0U;
        }

        // Time O(1).
        [[nodiscard]] constexpr auto data() const &noexcept -> const std::vector<item_t> &
        {
            return Data;
        }

        // Time O(1).
        [[nodiscard]] constexpr auto min() const &noexcept -> const item_t &
        {
            assert(!is_empty());
            return Data.at(0);
        }

        // Time O(1).
        [[nodiscard]] constexpr auto max() const &noexcept -> const item_t &
        {
            const auto index = max_index();
            assert(index < size());

            return Data.at(index);
        }

        // todo(p3): template<class item_t2>
        // void push(item_t2 &&item) noexcept(false)
        // Data.push_back(std::forward(item));
        constexpr void push(const item_t &item) noexcept(false)
        {
            auto index = size();
            Data.push_back(item);

            if (const auto was_empty = index == 0U; was_empty)
            {
                return;
            }

            auto parent = Standard::Algorithms::Trees::binary_tree_utilities<>::parent_node(index);
            auto &parent_item = Data[parent];
            auto &cur = Data[index];

            if (parent_item == cur)
            {
                return;
            }

            const auto level = ::Standard::Algorithms::Numbers::to_unsigned(
                                   ::Standard::Algorithms::Trees::binary_tree_utilities<>::level(index)) &
                1U;

            const auto is_min = cur < parent_item;
            if (is_min)
            {
                if (level != 0U)
                {
                    // min| par.
                    // max| cur. (cur < par).
                    std::swap(cur, parent_item); // todo(p3): no swaps.
                    if (parent == 0U)
                    {
                        return;
                    }

                    index = parent;
                    parent = Standard::Algorithms::Trees::binary_tree_utilities<>::parent_node(index);
                }
            }
            else
            {
                if (level == 0U)
                {
                    // max| par.
                    // min| cur. (cur < par)
                    std::swap(cur, parent_item); // todo(p2): no swaps.
                    if (parent == 0U)
                    {
                        return;
                    }

                    index = parent;
                    parent = Standard::Algorithms::Trees::binary_tree_utilities<>::parent_node(index);
                }
            }

            push_up_grandparents(parent, is_min, index);
        }

        // todo(p3): merge(min_max_heap& other)

        // todo(p3): push_many in O(n).

        // todo(p3): void decrease_key(node_t*const node, const item_t &item)

        constexpr void pop_min()
        {
            if (is_empty()) [[unlikely]]
            {
                throw std::runtime_error("Cannot pop min from an empty heap.");
            }

            Data.front() = std::move(Data.back());
            Data.pop_back();

            if (size() <= 1U)
            {
                return;
            }

            move_down(0U, true);
        }

        constexpr void pop_max()
        {
            if (is_empty()) [[unlikely]]
            {
                throw std::runtime_error("Cannot pop max from an empty heap.");
            }

            auto index = max_index();
            Data.at(index) = std::move(Data.back());
            Data.pop_back();

            if (size() <= 3U)
            {
                return;
            }

            move_down(index, false);
        }

        constexpr void validate() const
        {
            const auto siz = size();

            for (std::size_t index = 1U; index < siz; ++index)
            {
                const auto parent = Standard::Algorithms::Trees::binary_tree_utilities<>::parent_node(index);

                const auto level = ::Standard::Algorithms::Numbers::to_unsigned(
                                       ::Standard::Algorithms::Trees::binary_tree_utilities<>::level(index)) &
                    1U;

                const auto &parent_item = Data[parent];
                const auto &item = Data[index];

                if (level == 0U)
                {
                    if (parent_item < item) [[unlikely]]
                    {
                        throw std::runtime_error("parent_item < item at min level, index " + std::to_string(index));
                    }
                }
                else if (item < parent_item) [[unlikely]]
                {
                    throw std::runtime_error("parent_item > item at max level, index " + std::to_string(index));
                }
            }
        }

private:
        [[nodiscard]] constexpr auto max_index() const noexcept -> std::size_t
        {
            const auto siz = size();

            if (3U <= siz)
            {
                return Data[1] < Data[2] ? 2 : 1;
            }

            assert(0U < siz);

            return siz - 1U;
        }

        constexpr void push_up_grandparents(std::size_t parent, const bool is_min, std::size_t index) noexcept(false)
        {
            assert(parent < index);

            while (parent != 0U)
            {
                const auto grand_parent = Standard::Algorithms::Trees::binary_tree_utilities<>::parent_node(parent);
                auto &grand_parent_item = Data[grand_parent];
                auto &cur_item = Data[index];

                if ((is_min && !(cur_item < grand_parent_item)) || (!is_min && !(grand_parent_item < cur_item)))
                {
                    return;
                }

                std::swap(cur_item, grand_parent_item); // todo(p3): no swaps.
                if (grand_parent == 0U)
                {
                    return;
                }

                index = grand_parent;
                parent = Standard::Algorithms::Trees::binary_tree_utilities<>::parent_node(grand_parent);
            }
        }

        constexpr void move_down(std::size_t index, const bool is_minimum) noexcept(false)
        {
            assert(index < size());

            for (;;)
            {
                const auto parent = index;
                if (!swap_grandchild(parent, is_minimum, index))
                {
                    return;
                }

                //         0
                //   1         2
                // 3   4   5   6
                static constexpr auto min_grand_child = 3U;

                assert(!(index < min_grand_child) && index < size());

                const auto child = Standard::Algorithms::Trees::binary_tree_utilities<>::parent_node(index);
                assert(parent != child);

                if ((is_minimum && Data[child] < Data[index]) || (!is_minimum && Data[index] < Data[child]))
                {
                    //            10* Pop min sample.
                    //     50          70
                    // 20    30   40    60
                    //
                    //            60*
                    //     50          70
                    // 20    30   40
                    //
                    //            20*
                    //     50          70 // here 50 and 60 must be changed as 50 is no longer max.
                    // 60*   30   40
                    //
                    //            20
                    //     60*         70
                    // 50*   30   40
                    std::swap(Data[index], Data[child]);
                }
            }
        }

        constexpr auto swap_grandchild(const std::size_t parent, const bool is_minimum, std::size_t &index) noexcept
            -> bool
        {
            const auto cur_size = size();
            index = Standard::Algorithms::Trees::binary_tree_utilities<>::left_child(parent);

            assert(1U < cur_size && 0U < index);

            if (const auto childless = !(index < cur_size); childless)
            {
                return false;
            }

            auto grand_child = Standard::Algorithms::Trees::binary_tree_utilities<>::left_child(index);

            if (const auto right_child = index + 1U; right_child < cur_size)
            {
                if ((is_minimum && Data[right_child] < Data[index]) || (!is_minimum && Data[index] < Data[right_child]))
                {
                    ++index;
                }
            }

            static constexpr auto grandchildren_count = 4U;

            const auto last_grand_child = std::min(grand_child + grandchildren_count, cur_size) - 1U;

            auto is_grand = false;

            for (; grand_child <= last_grand_child; ++grand_child)
            {
                const auto prefer_grand_child = (is_minimum && !(Data[index] < Data[grand_child])) ||
                    (!is_minimum && !(Data[grand_child] < Data[index]));

                if (prefer_grand_child)
                {
                    index = grand_child;
                    is_grand = true;
                }
            }

            assert(0U < index && index < cur_size);

            if ((is_minimum && !(Data[index] < Data[parent])) || (!is_minimum && !(Data[parent] < Data[index])))
            {
                return false;
            }

            std::swap(Data[index], Data[parent]);
            return is_grand;
        }

        std::vector<item_t> Data{};
    };
} // namespace Standard::Algorithms::Heaps
