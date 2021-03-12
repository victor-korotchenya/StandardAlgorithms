#pragma once
#include "Heap.h"

namespace Privet::Algorithms::Heaps
{
    class HeapUtilities final
    {
        HeapUtilities() = delete;

    public:
        //Returns the k-th smallest (greatest) element from the min (max) heap.
        //The returned pointer might become invalid when the heap is changed.
        template <typename TItem,
            class THeap,
            class THeapTwo >
            static const TItem& KSmallestElement(
                const THeap& heap,
                const size_t zero_based_index,
                //A temporary heap.
                THeapTwo& heapTemp);
    };

    template <typename TItem, class THeap, class THeapTwo >
    const TItem& HeapUtilities::KSmallestElement(
        const THeap& heap,
        const size_t zero_based_index,
        THeapTwo& heapTemp)
    {
        //TODO: p3. Make it more C++.
        heapTemp.Clear();

        const size_t size = heap.size();
        if (size <= zero_based_index)
        {
            std::ostringstream ss;
            ss << "The heap size(" << size
                << ") must be greater than index(" << zero_based_index << ").";
            StreamUtilities::throw_exception(ss);
        }

        const auto& data = heap.data();
        TItem* originalData = const_cast<TItem*>(data.data());

        heapTemp.push(originalData);

        size_t index = 0;
        for (;;)
        {
            const TItem* const top = heapTemp.top();
            heapTemp.pop();

            if (zero_based_index == index)
                return *top;

            const size_t topIndex = top - originalData;
            const auto leftIndex = Privet::Algorithms::Trees::NodeUtilities::LeftChild(topIndex);

            if (leftIndex < size)
            {
                TItem* left = originalData + leftIndex;
                heapTemp.push(left);

                const auto rightIndex = Privet::Algorithms::Trees::NodeUtilities::RightChild(topIndex);
                if (rightIndex < size)
                {
                    TItem* right = originalData + rightIndex;
                    heapTemp.push(right);
                }
            }

            ++index;
        }
    }
}