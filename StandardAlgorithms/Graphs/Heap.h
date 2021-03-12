#pragma once
#include <exception>
#include <functional>
#include <sstream>
#include <vector>
#include "NodeUtilities.h"
#include "../Utilities/PrintConstants.h"
#include "../Utilities/StreamUtilities.h"
#include "../Utilities/ExceptionUtilities.h"

namespace Privet::Algorithms::Heaps
{
    //Implements binary minimum heap (for now).
    template <typename TItem,
        class TContainer = std::vector< TItem >,
        class TComparer = std::less_equal< TItem >>
        class Heap
    {
        friend class HeapUtilities;

        TComparer _Comparer;

        TContainer _Data;

    public:
        explicit Heap(const size_t initialCapacity = 0);

        virtual ~Heap() = default;

        Heap(const Heap&) = default;

        virtual Heap& operator = (const Heap& a) noexcept(false)
#if (1 == 2)
            noexcept(false) = default;//It does not work.
#else
        {
            //Problem - had to implement explicitly. TODO: can use default?
            if (this != &a)
            {
                _Comparer = a._Comparer;
                _Data = a._Data;
            }

            return *this;
        }
#endif

        Heap(Heap&&) = default;

        virtual Heap& operator= (Heap&& a) noexcept(false)
#if (1 == 2)
            noexcept(false) = default;//It does not work.
#else
        {
            //Problem - had to implement explicitly. TODO: can use default?
            if (this != &a)
            {
                _Comparer = std::move(a._Comparer);
                _Data = std::move(a._Data);
            }

            return *this;
        }
#endif

        size_t size() const noexcept
        {
            const size_t result = _Data.size();
            return result;
        }

        bool empty() const noexcept
        {
            const bool result = _Data.empty();
            return result;
        }

        //Returns the top element.
        const TItem& top() const
        {
            if (_Data.empty())
            {
                throw std::runtime_error("The heap is empty - cannot get the top element.");
            }

            return _Data[0];
        }

        virtual void Clear() noexcept;

        //Clears and builds the heap from the given array of elements.
        //
        //If an exception is thrown,
        // the instance may have the corrupted data: call clear() to fix it up.
        virtual void ClearAndBuild(const TItem* const data, const size_t count);

        //Sorts the array using heap sort algorithm.
        //
        //If an exception is thrown,
        // the instance may have the corrupted data: call clear() to fix it up.
        //
        //The TComparer should be "greater_equal< T >" in
        // order to sort in the ascending order.
        void HeapSort(TItem* data, const size_t count);

        //Adds an item.
        virtual void push(const TItem& item);

        //Deletes the top element.
        virtual void pop();

        //Validates the heap internals.
        void Validate(const std::string& message) const;

    protected:

        const TContainer& data() const
        {
            return _Data;
        }

        TContainer& data()
        {
            return _Data;
        }

        inline virtual void Swap(const size_t index1, const size_t index2);

        //If an exception is thrown,
        // the instance may have the corrupted data: call clear() to fix it up.
        virtual void CopyData(const TItem* const data, const size_t count);

        //Compares whether parentItem <= currentItem.
        template <typename T2 >
        bool Compare(const T2& parentItem, const T2& currentItem) const
        {
            const bool result = _Comparer(parentItem, currentItem);
            return result;
        }

        void RepairUpwards(const size_t lastItemIndex);

        void RepairDownwards(const size_t parentIndex);
    };

    template <typename TItem,
        class TContainer,
        class TComparer
    >
        Heap< TItem, TContainer, TComparer >::Heap(
            const size_t initialCapacity)
        : _Comparer(TComparer()),
        _Data(TContainer())
    {
        if (initialCapacity > 0)
        {
            _Data.reserve(initialCapacity);
        }
    }

    template <typename TItem,
        class TContainer,
        class TComparer >
        void Heap< TItem, TContainer, TComparer >::Clear() noexcept
    {
        _Data.clear();
    }

    template <typename TItem,
        class TContainer,
        class TComparer >
        void Heap< TItem, TContainer, TComparer >::
        ClearAndBuild(const TItem* const data, const size_t count)
    {
        ThrowIfNull(data, "data in ClearAndBuild.");
        if (0 == count)
        {
            throw std::runtime_error("The count(0) must be positive.");
        }

        Clear();
        CopyData(data, count);

        if (1 == count)
        {//1 item is already sorted.
            return;
        }

        size_t parentIndex = Privet::Algorithms::Trees::NodeUtilities::ParentNode(count - 1);

        do
        {
            RepairDownwards(parentIndex);
        } while (parentIndex--);

#ifdef _DEBUG
        this->Validate(std::string("ClearAndBuild."));
#endif
    }

    template <typename TItem,
        class TContainer,
        class TComparer >
        void Heap< TItem, TContainer, TComparer >::
        HeapSort(TItem* data, const size_t count)
    {
        //TODO: p3. Sort can be done without data copy.
        ClearAndBuild(data, count);

        size_t index = count - 1;
        do
        {
            data[index] = top();
            pop();
        } while (index--);
    }

    template <typename TItem,
        class TContainer,
        class TComparer >
        void Heap< TItem, TContainer, TComparer >::push(const TItem& item)
    {
        _Data.push_back(item);

        const size_t lastItemIndex = _Data.size() - 1;
        if (0 < lastItemIndex)
        {//There are at least 2 elements.
            RepairUpwards(lastItemIndex);
        }

#ifdef _DEBUG
        Validate(std::string("Add."));
#endif
    }

    template <typename TItem,
        class TContainer,
        class TComparer
    >
        void Heap< TItem, TContainer, TComparer >::pop()
    {
        const size_t initialSize = _Data.size();
        if (0 == initialSize)
        {
            throw std::runtime_error("The heap is empty - cannot delete the top element.");
        }

        const size_t newSize = initialSize - 1;
        const TItem lastItem = _Data[newSize];
        const size_t newPosition = 0;
        _Data[newPosition] = lastItem;

        _Data.pop_back();

        if (1 < newSize)
        {//There are at least 2 elements left after removal.
            RepairDownwards(newPosition);
        }

#ifdef _DEBUG
        Validate(std::string("Delete top."));
#endif
    }

    template <typename TItem,
        class TContainer,
        class TComparer
    >
        void Heap< TItem, TContainer, TComparer >::Swap(
            const size_t index1, const size_t index2)
    {
#ifdef _DEBUG
        if (index1 == index2)
        {
            std::ostringstream ss;
            ss << "Error: index1 == index2 = " << index2 << " in Heap::Swap.";
            StreamUtilities::throw_exception(ss);
        }
#endif
        std::swap(_Data[index1], _Data[index2]);
    }

    template <typename TItem,
        class TContainer,
        class TComparer
    >
        void Heap< TItem, TContainer, TComparer >::
        CopyData(const TItem* const data, const size_t count)
    {
        ThrowIfNull(data, "data in CopyData.");
        if (0 == count)
        {
            throw std::runtime_error("0 == count in CopyData.");
        }

        _Data.resize(count);

        TItem* const pData = _Data.data();
        const size_t bytes = count * sizeof(TItem);

        std::memcpy(pData, data, bytes);
    }

    template <typename TItem,
        class TContainer,
        class TComparer
    >
        void Heap< TItem, TContainer, TComparer >::Validate(
            const std::string& message) const
    {
        const size_t size = _Data.size();

        const size_t rootHasNoParent = 1;
        for (size_t index = rootHasNoParent; index < size; ++index)
        {
            const TItem& currentItem = _Data[index];

            const size_t parentIndex = Privet::Algorithms::Trees::NodeUtilities::ParentNode(index);
            const TItem& parentItem = _Data[parentIndex];
            const bool isValid = Compare(parentItem, currentItem);
            if (!isValid)
            {
                std::ostringstream ss;
                ss << "The heap is broken at index " << index
                    << ", parentItem=" << PrintConstants::Begin << parentItem << PrintConstants::End
                    << ", currentItem=" << PrintConstants::Begin << currentItem << PrintConstants::End
                    << ". " << message;
                StreamUtilities::throw_exception(ss);
            }
        }
    }
    //#endif

    template <typename TItem,
        class TContainer,
        class TComparer >
        void Heap< TItem, TContainer, TComparer >::RepairUpwards(
            const size_t lastItemIndex)
    {
#ifdef _DEBUG
        if (0 == lastItemIndex)
        {
            throw std::runtime_error("0 == lastItemIndex in RepairUpwards.");
        }
#endif

        size_t index = lastItemIndex;
        do
        {
            const size_t parentIndex = Privet::Algorithms::Trees::NodeUtilities::ParentNode(index);
            if (Compare(_Data[parentIndex], _Data[index]))
            {//The heap is OK now - there is no sense to go further.
                break;
            }

            Swap(index, parentIndex);
            index = parentIndex;
        } while (index);
    }

    template <typename TItem,
        class TContainer,
        class TComparer
    >
        void Heap< TItem, TContainer, TComparer >::RepairDownwards(
            const size_t parentIndex)
    {
        const size_t size = _Data.size();
#ifdef _DEBUG
        if (size <= parentIndex)
        {
            throw std::runtime_error("size <= parentIndex in RepairDownwards.");
        }
#endif
        size_t parent = parentIndex;
        for (;;)
        {
            const size_t left = Privet::Algorithms::Trees::NodeUtilities::LeftChild(parent);
            if (size <= left)
            {//Done.
                return;
            }

            const size_t right = Privet::Algorithms::Trees::NodeUtilities::RightChild(parent);
            if (size == right)
            {//There is only one child: left.
                if (!Compare(_Data[parent], _Data[left]))
                {
                    Swap(parent, left);
                }

                return;
            }

            //There are both children.
            size_t changeIndex = right;
            if (Compare(_Data[parent], _Data[left]))
            {
                if (Compare(_Data[parent], _Data[right]))
                {//Everything is in place.
                    return;
                }

                //"right < parent <= left" - only right sub-tree must be fixed.
            }
            else
            {//left < parent
                if (!Compare(_Data[right], _Data[left]))
                {//right > left
                    changeIndex = left;
                }
            }

            Swap(parent, changeIndex);
            parent = changeIndex;
        }
    }
}