#pragma once
#include "../Numbers/Arithmetic.h"
#include "../Utilities/ExceptionUtilities.h"
#include "Heap.h"

#ifdef _DEBUG
#include <unordered_map>
#include <unordered_set>
#endif

namespace Privet::Algorithms::Heaps
{
    //Implements minimum heap that can decrease keys
    // to be used in MST and Dijkstra algorithms.
    //
    //Careful: All data must have unique index!
    template <typename TItem,
        typename TGetIndexFunction,
        class TContainer = std::vector<TItem>,
        class TComparer = std::less_equal<TItem>>
        class PriorityQueue final
        : public Heap<TItem, TContainer, TComparer>
    {
        static constexpr size_t InfinityPosition = 0 - size_t(1);

        using base_t = Heap<TItem, TContainer, TComparer>;

        std::vector<size_t> _Positions;

        TGetIndexFunction _GetIndexFunction;

#ifdef _DEBUG
        //To check that "All data must have unique index".
        std::unordered_set<size_t> UsedIndexes;
#endif

    public:
        explicit PriorityQueue(
            TGetIndexFunction getIndexFunction = nullptr,
            const size_t initialCapacity = 0);

        ~PriorityQueue() override = default;

        PriorityQueue& operator = (const base_t& a)
        {
            auto b = dynamic_cast<const PriorityQueue*>(&a);
            if (b)
            {
                if (this != &a)
                {
                    base_t::operator = (static_cast<base_t const&>(a));

                    _Positions = b->_Positions;
                    _GetIndexFunction = b->_GetIndexFunction;
#ifdef _DEBUG
                    UsedIndexes = b->UsedIndexes;
#endif
                }

                return *this;
            }

            throw throw_type_mismatch<base_t>();
        }

        PriorityQueue& operator = (base_t&& a)
        {
            auto b = dynamic_cast<const PriorityQueue*>(&a);
            if (b)
            {
                if (this != &a)
                {
                    _Positions = std::move(b->_Positions);
                    _GetIndexFunction = std::move(b->_GetIndexFunction);
#ifdef _DEBUG
                    UsedIndexes = std::move(b->UsedIndexes);
#endif
                }

                return *this;
            }

            throw throw_type_mismatch<base_t>();
        }

        void Clear() noexcept override;

        void EnsureIndexesSize(
            //Starts from 0.
            const size_t maxIndex);

        //Adds an item.
        //
        //Careful: All data must have unique index.
        void push(const TItem& item) override;

        //Deletes the top element.
        void pop() override;

        void DecreaseKey(const TItem& itemToUpdate);

        bool inline TryGetItem(const TItem& currentItem, TItem& queueItem) const;

    protected:

        inline void Swap(const size_t index1, const size_t index2) override;

        //If an exception is thrown,
        // the instance may have the corrupted data: call clear() to fix it up.
        void CopyData(const TItem* const data, const size_t count) override;

    private:

#ifdef _DEBUG
        void CheckPosition(const size_t index) const;
#endif

        void UpdateTopPosition();
    };

    template <typename TItem,
        typename TGetIndexFunction, class TContainer, class TComparer
    >
        PriorityQueue<TItem, TGetIndexFunction, TContainer, TComparer>::
        PriorityQueue(TGetIndexFunction getIndexFunction, const size_t initialCapacity)
        : base_t(initialCapacity),
        _Positions(initialCapacity, InfinityPosition),
        _GetIndexFunction(ThrowIfNull(getIndexFunction, "getIndexFunction"))
#ifdef _DEBUG
        , UsedIndexes()
#endif
    {
    }

    template <typename TItem,
        typename TGetIndexFunction, class TContainer, class TComparer
    >
        void PriorityQueue<TItem, TGetIndexFunction, TContainer, TComparer>
        ::Clear() noexcept
    {
        _Positions.clear();
#ifdef _DEBUG
        UsedIndexes.clear();
#endif
        Heap< TItem, TContainer, TComparer >::Clear();
    }

    template <typename TItem,
        typename TGetIndexFunction, class TContainer, class TComparer
    >
        void PriorityQueue<TItem, TGetIndexFunction, TContainer, TComparer >
        ::push(const TItem& item)
    {
        const size_t index = _GetIndexFunction(item);
        EnsureIndexesSize(index);

        const size_t dataSizeOld = this->data().size();

#ifdef _DEBUG
        const auto inserted = UsedIndexes.insert(index);
        if (!inserted.second)
        {
            if (_Positions.size() <= index)
            {
                std::ostringstream ss;
                ss << "The existing item index=" << index
                    << " exceeds the _Positions.size()==" << _Positions.size()
                    << "; and new item '" << item << "' maps to the same index.";
                StreamUtilities::throw_exception(ss);
            }

            const size_t oldIndex = _Positions[index];
            if (dataSizeOld <= oldIndex)
            {
                std::ostringstream ss;
                ss << "The existing item index=" << index
                    << " _Positions to oldIndex==" << oldIndex
                    << " which exceeds the dataSizeOld==" << dataSizeOld
                    << "; and new item '" << item << "' maps to the same index.";
                StreamUtilities::throw_exception(ss);
            }

            const TItem& oldItem = this->data()[oldIndex];

            std::ostringstream ss;
            ss << "The existing item '" << oldItem
                << "' at [" << oldIndex
                << "] and new item '" << item << "' have the same index=" << index << ".";
            StreamUtilities::throw_exception(ss);
        }
#endif
        _Positions[index] = dataSizeOld;

        try
        {
            this->data().push_back(item);
        }
        catch (...)
        {//Restore.
            _Positions[index] = InfinityPosition;
#ifdef _DEBUG
            UsedIndexes.erase(index);
#endif
            throw;
        }

        if (dataSizeOld > 0)
        {//There are at least 2 elements.
            this->RepairUpwards(dataSizeOld);
        }

#ifdef _DEBUG
        this->Validate(std::string("Add."));
#endif
    }

    template <typename TItem,
        typename TGetIndexFunction, class TContainer, class TComparer
    >
        void PriorityQueue<TItem, TGetIndexFunction, TContainer, TComparer >
        ::pop()
    {
        const size_t initialSize = this->data().size();
        if (0 == initialSize)
        {
            throw std::runtime_error("The heap is empty - cannot delete the top element.");
        }

        const size_t newSize = initialSize - 1;
        const TItem lastItem = this->data()[newSize];

        const size_t index = _GetIndexFunction(lastItem);
#ifdef _DEBUG
        CheckPosition(index);
#endif

        UpdateTopPosition();

        const size_t newPosition = 0;
        this->data()[newPosition] = lastItem;
        _Positions[index] = newPosition;

        //Cannot remove from _Positions because the last item might have (N-1) position.
        this->data().pop_back();

        if (1 < newSize)
        {//There are at least 2 elements left after removal.
            this->RepairDownwards(newPosition);
        }

#ifdef _DEBUG
        this->Validate(std::string("Delete top."));
#endif
    }

    template <typename TItem,
        typename TGetIndexFunction, class TContainer, class TComparer
    >
        void PriorityQueue<TItem, TGetIndexFunction, TContainer, TComparer >
        ::DecreaseKey(const TItem& itemToUpdate)
    {
        const size_t index = _GetIndexFunction(itemToUpdate);
#ifdef _DEBUG
        CheckPosition(index);
#endif
        const size_t position = _Positions[index];

#ifdef _DEBUG
        if (this->data().size() <= position)
        {
            std::ostringstream ss;
            ss << "Error in DecreaseKey: " << this->data().size() << "=data().size() <= position=" << position << ".";
            StreamUtilities::throw_exception(ss);
        }

        const bool hasDecreasedOrSame = this->Compare(itemToUpdate, this->data()[position]);
        if (!hasDecreasedOrSame)
        {
            std::ostringstream ss;
            ss << "_Debug: item at " << position << " has not decreased key, "
                << "old item=" << this->data()[position] << ", new item=" << itemToUpdate << ".";
            StreamUtilities::throw_exception(ss);
        }
#endif
        this->data()[position] = itemToUpdate;
        if (position > 0)
        {//There are at least 2 elements.
            this->RepairUpwards(position);
        }
    }

    template <typename TItem,
        typename TGetIndexFunction, class TContainer, class TComparer
    >
        bool PriorityQueue<TItem, TGetIndexFunction, TContainer, TComparer >
        ::TryGetItem(const TItem& currentItem, TItem& queueItem) const
    {
        const size_t index = _GetIndexFunction(currentItem);
#ifdef _DEBUG
        CheckPosition(index);
#endif
        const size_t position = _Positions[index];
        const bool result = position != InfinityPosition;
        if (result)
        {
#ifdef _DEBUG
            if (this->data().size() <= position)
            {
                std::ostringstream ss;
                ss << "Error in TryGetItem: " << this->data().size() << "=data().size() <= position=" << position << ".";
                StreamUtilities::throw_exception(ss);
            }
#endif
            queueItem = this->data()[position];
        }

        return result;
    }

    template <typename TItem,
        typename TGetIndexFunction, class TContainer, class TComparer
    >
        void PriorityQueue<TItem, TGetIndexFunction, TContainer, TComparer >
        ::Swap(
            const size_t index1, const size_t index2)
    {
        TItem const temp = this->data()[index1];

        const size_t indexA = _GetIndexFunction(temp);
        const size_t indexB = _GetIndexFunction(this->data()[index2]);
#ifdef _DEBUG
        if (index1 == index2)
        {
            std::ostringstream ss;
            ss << "Error: index1 == index2 = " << index2 << " in PriorityQueue::Swap.";
            StreamUtilities::throw_exception(ss);
        }
        CheckPosition(indexA);
        CheckPosition(indexB);
        if (indexA == indexB)
        {
            std::ostringstream ss;
            ss << "Error: indexA == indexB = " << indexB
                << " in PriorityQueue::Swap; index1=" << index1
                << ", index2=" << index2;
            StreamUtilities::throw_exception(ss);
        }
#endif
        this->data()[index1] = this->data()[index2];
        this->data()[index2] = temp;

        _Positions[indexA] = index2;
        _Positions[indexB] = index1;
    }

    template <typename TItem,
        typename TGetIndexFunction, class TContainer, class TComparer
    >
        void PriorityQueue<TItem, TGetIndexFunction, TContainer, TComparer >
        ::CopyData(const TItem* const data, const size_t count)
    {
        Heap<TItem, TContainer, TComparer >::CopyData(data, count);

        size_t maxIndex = count - 1;
        EnsureIndexesSize(maxIndex);

#ifdef _DEBUG
        std::unordered_map<size_t, size_t> uniqueIndexes;
#endif
        for (size_t i = 0; i < count; ++i)
        {
            const TItem& newItem = data[i];
            const size_t index = _GetIndexFunction(newItem);
#ifdef _DEBUG
            const auto inserted = uniqueIndexes.insert({ index, i });
            if (!inserted.second)
            {
                const auto found = uniqueIndexes.find(index);

                const size_t oldIndex = found->second;
                if (oldIndex >= count)
                {
                    std::ostringstream ss;
                    ss << "Error: oldIndex(" << oldIndex << ") >= count(" << count << ").";
                    StreamUtilities::throw_exception(ss);
                }

                const TItem& oldItem = data[oldIndex];

                std::ostringstream ss;
                ss << "At least two items '" << oldItem
                    << "' at [" << oldIndex
                    << "] and '" << newItem << "' at [" << i
                    << "] have the same index=" << index << ".";
                StreamUtilities::throw_exception(ss);
            }
#endif
            if (index > maxIndex)
            {
                EnsureIndexesSize(index);
                maxIndex = _Positions.size() - 1;
            }

            _Positions[index] = i;
        }
    }

    template <typename TItem,
        typename TGetIndexFunction, class TContainer, class TComparer
    >
        void PriorityQueue<TItem, TGetIndexFunction, TContainer, TComparer >
        ::EnsureIndexesSize(
            const size_t maxIndex)
    {
        const size_t indexesSize = _Positions.size();
        if (indexesSize <= maxIndex)
        {
            const size_t newSizeRaw = maxIndex << 1;
            const size_t newSize = RoundToGreaterPowerOfTwo_t(newSizeRaw);
            if (newSize <= maxIndex)
            {
                std::ostringstream ss;
                ss << "Error: newSize (" << newSize << ") < maxIndex (" << maxIndex << ").";
                StreamUtilities::throw_exception(ss);
            }

            _Positions.resize(newSize, InfinityPosition);
        }
    }

#ifdef _DEBUG
    template <typename TItem,
        typename TGetIndexFunction, class TContainer, class TComparer
    >
        void PriorityQueue<TItem, TGetIndexFunction, TContainer, TComparer >
        ::CheckPosition(const size_t index) const
    {
        if (_Positions.size() <= index)
        {
            std::ostringstream ss;
            ss << "Error in CheckIndex: " << _Positions.size() << "=_Positions.size() <= index=" << index << ".";
            StreamUtilities::throw_exception(ss);
        }
    }
#endif

    template <typename TItem,
        typename TGetIndexFunction, class TContainer, class TComparer
    >
        void PriorityQueue<TItem, TGetIndexFunction, TContainer, TComparer >
        ::UpdateTopPosition()
    {
        const size_t index = _GetIndexFunction(this->data()[0]);
#ifdef _DEBUG
        CheckPosition(index);

        const auto deleted = UsedIndexes.erase(index);
        if (0 == deleted)
        {
            std::ostringstream ss;
            ss << "The UsedIndexes must have the index=" << index
                << " while deleting the top item '" << this->data()[0] << "'.";
            StreamUtilities::throw_exception(ss);
        }
#endif

        _Positions[index] = InfinityPosition;
    }
}