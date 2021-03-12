#pragma once
#include"binary_tree_utilities.h"

namespace Standard::Algorithms::Heaps
{
    // Return the k-th smallest (greatest) element from the min (max) heap.
    // The returned pointer might become invalid when the heap is changed.
    template<class item_t, class heap_t, class heap_t_2>
    [[nodiscard]] constexpr auto k_smallest_element(
        const heap_t &heap, const std::size_t zero_based_index, heap_t_2 &temp_heap) -> const item_t &
    {
        assert(static_cast<const void *>(&heap) != static_cast<const void *>(&temp_heap));

        const auto size = heap.size();
        require_greater(size, zero_based_index, "heap index");

        temp_heap.clear();

        const auto &data = heap.data();
        const auto *const original_data = data.data();
        assert(original_data != nullptr);

        temp_heap.push(original_data);

        for (std::size_t index{};;)
        {
            const auto *const top = temp_heap.top();
            assert(top != nullptr);

            temp_heap.pop();

            if (zero_based_index == index)
            {
                return *top;
            }

            const auto top_index = static_cast<std::size_t>(top - original_data);

            if (const auto left_index = Standard::Algorithms::Trees::binary_tree_utilities<>::left_child(top_index);
                left_index < size)
            {
                const auto *const left = original_data + left_index;
                assert(left != nullptr);

                temp_heap.push(left);

                if (const auto right_index = Standard::Algorithms::Trees::binary_tree_utilities<>::right_child(top_index);
                    right_index < size)
                {
                    const auto *const right = original_data + right_index;
                    assert(right != nullptr);

                    temp_heap.push(right);
                }
            }

            ++index;
        }
    }
} // namespace Standard::Algorithms::Heaps
