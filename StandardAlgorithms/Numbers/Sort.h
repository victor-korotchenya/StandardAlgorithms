#pragma once

#include <array>
#include <vector>
#include "../Utilities/ExceptionUtilities.h"
#include "../Utilities/StreamUtilities.h"

namespace
{
    template <typename TSize, typename TCounts>
    void propagate_counts(const TSize maxValueInclusive, TCounts& counts)
    {
        assert(maxValueInclusive > 0);

        TSize i = 0, previous = 0;
        do
        {
            const auto next = static_cast<TSize>(previous + counts[i]);
            counts[i] = previous;
            previous = next;
        } while (++i <= maxValueInclusive);
    }

    template <typename Collection, typename TSize, typename get_size_t>
    std::pair<TSize, TSize> empty_count_max_length(
        get_size_t get_size, const Collection& collection)
    {
        RequirePositive(collection.size(), "collection.size");

        TSize emptyCount = 0, elementMaxLength = 0;

        for (const auto& item : collection)
        {
            const auto itemSize = static_cast<TSize>(get_size(item));
#ifdef _DEBUG
            if (itemSize != get_size(item))
            {
                std::ostringstream ss;
                ss << "TSize is too small to store " << get_size(item) << ".";
                StreamUtilities::throw_exception(ss);
            }
#endif
            if (0 == itemSize)
                ++emptyCount;
            else if (elementMaxLength < itemSize)
                elementMaxLength = itemSize;
        }

        return{ emptyCount, elementMaxLength };
    }

    template <typename Collection, typename TSize, typename get_size_t>
    void move_empty_to_start(
        get_size_t get_size,
        TSize emptyCount,
        Collection& collection,
        Collection& temp)
    {
        RequirePositive(emptyCount, "emptyCount");

        const auto size = static_cast<TSize>(collection.size());
        if (size <= emptyCount)
        {
            std::ostringstream ss;
            ss << "The collection.size (" << size
                << ") must be greater than emptyCount=" << emptyCount << ".";
            StreamUtilities::throw_exception<std::out_of_range>(ss);
        }

        temp.resize(size);
        --emptyCount;

        TSize i = 0, empty = TSize() - TSize(1);
        do
        {
            const auto pos = 0 == get_size(collection[i]) ? ++empty : ++emptyCount;
            temp[pos] = collection[i];
        } while (++i < size);

        std::copy(temp.begin(), temp.end(), collection.begin());
    }

    template <typename Collection, typename digit_t,
        typename TSize,
        typename get_size_t,
        typename get_item_t>
        void count_at_digit(
            get_size_t get_size,
            get_item_t get_item,
            const TSize digit,
            const TSize maxValue,
            const Collection& collection,
            std::vector<TSize>& counts,
            TSize index)
    {
        counts.assign(maxValue + 1, 0);
        const auto size = static_cast<TSize>(collection.size());

        do
        {
            const auto pos = std::min(digit,
                static_cast<TSize>(get_size(collection[index]) - 1));
#ifdef _DEBUG
            if (0 == get_size(collection[index]))
            {
                std::ostringstream ss;
                ss << "0 == collection[" << index << "].size().";
                StreamUtilities::throw_exception(ss);
            }

            if (maxValue < pos)
            {
                std::ostringstream ss;
                ss << "The value (" << pos << ") at digit="
                    << digit << " exceeds maxValue(" << maxValue << ").";
                StreamUtilities::throw_exception<std::out_of_range>(ss);
            }
#endif
            const auto symbol = static_cast<digit_t>(
                get_item(collection[index], pos));

            ++counts[symbol];
        } while (++index < size);
    }

    template <typename Collection, typename digit_t,
        typename TSize,
        typename get_size_t,
        typename get_item_t>
        void copy_to_buffer(
            get_size_t get_size,
            get_item_t get_item,
            const Collection& collection,
            const TSize digit,
            Collection& temp,
            std::vector<TSize>& counts,
            TSize index)
    {
        const auto size = static_cast<TSize>(collection.size());
        do
        {
            const auto& item = collection[index];
            const auto pos = std::min(digit,
                static_cast<TSize>(get_size(item) - 1));

            const auto symbol = static_cast<digit_t>(get_item(item, pos));
            temp[counts[symbol]++] = item;
        } while (++index < size);
    }

    template <typename Collection, typename digit_t,
        typename TSize,
        typename get_size_t,
        typename get_item_t>
        void sort_by_digit(
            get_size_t get_size,
            get_item_t get_item,
            const TSize digit,
            const TSize emptyCount,
            const TSize maxValue,
            Collection& collection,
            Collection& temp,
            std::vector<TSize>& counts)
    {
        count_at_digit<Collection, digit_t, TSize, get_size_t, get_item_t>(
            get_size, get_item,
            digit, maxValue, collection, counts, emptyCount);

        propagate_counts<TSize>(maxValue, counts);

        copy_to_buffer<Collection, digit_t, TSize, get_size_t, get_item_t>(
            get_size, get_item,
            collection,
            digit,
            temp,
            counts, emptyCount);

        std::copy(temp.begin(), temp.end(), collection.begin() + emptyCount);
    }

    template <typename T>
    void InsertionSortStep_Unsafe(T* base, size_t index)
    {
        //Note. It cannot be a reference.
        const T current = base[index];

        while (0u < index && current < base[index - 1u])
        {
            base[index] = base[index - 1u];
            --index;
        }

        base[index] = current;
    }
}

namespace Privet::Algorithms::Numbers
{
    // Sorts the "last item" in the array.
    template <typename T>
    void InsertionSortStep(T* base, const size_t length)
    {
        if (length <= 1)
            return;

        InsertionSortStep_Unsafe(base, length - 1u);
    }

    //Implements Insertion Sort of the "base" array.
    template <typename T>
    void InsertionSort(T* base, const size_t size)
    {
        for (size_t index = 1; index < size; ++index)
            InsertionSortStep_Unsafe(base, index);
    }

    template <typename T>
    void swap_simple(T* const a, T* const b)
    {
        const T temp = *a;
        *a = *b;
        *b = temp;
    }

    //Returns the minimum of 3.
    template <typename T>
    T Min3(const T& a, const T& b, const T& c)
    {
        if (a <= b)//Ignore b.
            return a <= c ? a : c;

        //Ignore a.
        return b <= c ? b : c;
    }

    template <typename T>
    void SwapIfGreater(T* a, T* b)
    {
        if ((*b) < *a)
        {
            const T temp = *a;
            *a = *b;
            *b = temp;
        }
    }

    // The array must have 3 elements to sort.
    template <typename T>
    void FastSort3(T* data)
    {
        //SwapIfGreater(data, data + 1);
        ////SwapIfGreater(data + 1, data + 2);
        ////SwapIfGreater(data, data + 1);
        //
        //const T temp = data[1];
        //if (temp > data[2])
        //{
        //  data[1] = data[2];
        //  data[2] = temp;
        //
        //  SwapIfGreater(data, data + 1);
        //}

        //Fastest code maybe longer.
        if (data[1] < data[0])
        {// 1 < 0.
            if (data[2] < data[0])
            {// 1 < 0. // 2 < 0.
                if (data[2] < data[1])
                {// 2 < 1. // 1 < 0. // 2 < 0.
                 // 2 < 1 < 0.
                    const T temp = data[0];
                    data[0] = data[2];
                    data[2] = temp;
                }
                else
                {// 1 < 0. // 2 < 0. // 1 <= 2.
                 // 1 <= 2 < 0.
                    const T temp = data[0];
                    data[0] = data[1];
                    data[1] = data[2];
                    data[2] = temp;
                }
            }
            else
            {// 1 < 0. // 0 <= 2.
             // 1 < 0 <= 2.
                const T temp = data[0];
                data[0] = data[1];
                data[1] = temp;
            }
        }
        else if (data[2] < data[1])
        {// 0 <= 1. // 2 < 1.
            if (data[2] < data[0])
            {// 0 <= 1. // 2 < 1. // 2 < 0.
             // 2 < 0 <= 1.
                const T temp = data[0];
                data[0] = data[2];
                data[2] = data[1];
                data[1] = temp;
            }
            else
            {// 0 <= 1. // 2 < 1. // 0 <= 2.
             // 0 <= 2 < 1.
                const T temp = data[1];
                data[1] = data[2];
                data[2] = temp;
            }
        }

#ifdef _DEBUG
        const bool isOk = data[0] <= data[1] && data[1] <= data[2];
        if (!isOk)
        {
            throw std::runtime_error("Error in FastSort3.");
        }
#endif
    }

    // The array must have 4 elements to sort.
    template <typename T>
    void FastSort4(T* data)
    {
        //TODO: p3. make it faster.
        SwapIfGreater(data + 0, data + 1);
        SwapIfGreater(data + 2, data + 3);

        //Min
        SwapIfGreater(data + 0, data + 2);

        //Max
        SwapIfGreater(data + 1, data + 3);

        //Middle
        SwapIfGreater(data + 1, data + 2);

#ifdef _DEBUG
        const bool isOk = data[0] <= data[1] && data[1] <= data[2] && data[2] <= data[3];
        if (!isOk)
        {
            throw std::runtime_error("Error in FastSort4.");
        }
#endif
    }

    // The array must have 5 elements to sort.
    template <typename T>
    void FastSort5(T* data)
    {
        //TODO: p2. make it faster.
        SwapIfGreater(data + 0, data + 1);
        SwapIfGreater(data + 3, data + 4);

        //Min
        SwapIfGreater(data + 0, data + 2);
        SwapIfGreater(data + 0, data + 3);

        SwapIfGreater(data + 1, data + 2);
        SwapIfGreater(data + 2, data + 3);

        SwapIfGreater(data + 1, data + 4);
        SwapIfGreater(data + 1, data + 2);

        SwapIfGreater(data + 3, data + 4);

#ifdef _DEBUG
        const bool isOk = data[0] <= data[1] && data[1] <= data[2] && data[2] <= data[3] && data[3] <= data[4];
        if (!isOk)
        {
            throw std::runtime_error("Error in FastSort5.");
        }
#endif
    }

    //Implements Selection Sort of the "base" array.
    template <typename T>
    void SelectionSort(
        T* base,
        //Number of array elements.
        const size_t length,
        int(*comparer)(const T*, const T*),
        //Whether to use the stable sorting.
        const bool isStable)
    {
        const T* end_incl = base + length - 1;

        for (; base < end_incl; ++base)
        {
            auto minValueAddress = base;

            //Find the minimum value.
            for (T* currentAddress = base + 1;
                currentAddress <= end_incl;
                ++currentAddress)
            {
                const int comparisonResult = comparer(minValueAddress, currentAddress);
                if (comparisonResult > 0)
                    minValueAddress = currentAddress;
            }

            if (isStable)
            {
                const T minValue = *minValueAddress;

                //Shift sub-array of values from [startingAddress, minValueAddress - 1]
                // to the right by one element to preserve the order.
                for (T* shiftAddress = minValueAddress;
                    base < shiftAddress;
                    --shiftAddress)
                {
                    *shiftAddress = *(shiftAddress - 1);
                }

                *base = minValue;
            }
            else
            {//Not stable sorting which will be faster.
                swap_simple(base, minValueAddress);
            }
        }
    }

    //This is 15 per cent slower than "SelectionSort".
    template <typename Iterator>
    void selection_sort_slow(Iterator begin, Iterator end)
    {
        while (begin != end)
        {
            auto min1 = std::min_element(begin, end);
            std::iter_swap(begin, min1);
            ++begin;
        }
    }

    // Bucket/count sort.
    // The buffer size must be >= (stop - start).
    template <class int_t, int_t index_max, class it_t, class it_t2,
        // std::function(int_t(it_t))
        class to_index_t>
        void counting_sort(it_t start, it_t stop, it_t2 buf, to_index_t& to_index)
    {
        static_assert(std::is_unsigned_v<int_t> && index_max > 1);

        assert(start <= stop);
        if (stop - start <= 1)
            return; // size <= 1

        std::array<size_t, index_max> counts = { 0 };
        {
            auto st = start;
            do
            {
                const auto index = to_index(st);
                assert(index < index_max);

                ++counts[index];
            } while (++st != stop);
        }

        using larger_type = size_t;
        propagate_counts(static_cast<larger_type>(index_max - 1), counts);

        {
            auto s1 = start;
            do
            {
                const auto index = to_index(s1);
                assert(index < index_max);

                auto& cnt = counts[index];
                *(buf + cnt) = *s1;
                ++cnt;
            } while (++s1 != stop);
        }

        std::copy(buf, buf + (stop - start), start);
    }

    template <typename number_t, int n_max = 256>
    void radix_sort_unsigned(number_t* ar, const size_t size, number_t* buf)
    {
        static_assert(std::is_unsigned<number_t>::value && n_max > 0);
        if (size <= 1u)
            return;

        std::array<size_t, n_max> counts;

        size_t dig{};
        do
        {
            counts.fill(0);
            const auto pos = dig << 3;
            {
                size_t i{};
                do { ++counts[(ar[i] >> pos) & (n_max - 1)]; } while (++i < size);
            }
            {//sum up.
                size_t j{}, prev{};
                do
                {
                    const auto next = prev + counts[j];
                    counts[j] = prev; prev = next;
                } while (++j < n_max);
            }
            {
                size_t i{};
                do buf[counts[(ar[i] >> pos) & (n_max - 1)]++] = ar[i];
                while (++i < size);
            }

            std::memcpy(ar, buf, static_cast<size_t>(size) * sizeof(number_t));
        } while (++dig < sizeof(number_t));
    }

    template <typename Collection,
        //It had better be "unsigned char" for a string
        // because ((char)255) is -1, that leads to index overflow.
        typename digit_t,
        //"unsigned" can be used if both the collection and each item
        // have fewer than (power(2, 32) - 1) items
        // in order to save memory.
        typename TSize = size_t,
        typename get_size_t,
        typename get_item_t,
        const TSize maxValue = std::numeric_limits<digit_t>::max()>
        void radix_sort(
            Collection& collection,
            get_size_t get_size = {},
            get_item_t get_item = {})
    {
        static_assert(std::is_unsigned_v<digit_t> && maxValue < maxValue + 1);

        RequirePositive(maxValue, "maxValue");

        if (std::numeric_limits<TSize>::max() <= collection.size())
        {
            std::ostringstream ss;
            ss << "The TSize is too small to have " << collection.size() << " items.";
            StreamUtilities::throw_exception<std::out_of_range>(ss);
        }

        const auto size = static_cast<TSize>(collection.size());
        if (size <= 1) //Already sorted.
            return;

        const auto el = empty_count_max_length<Collection, TSize, get_size_t>
            (get_size, collection);
        const TSize emptyCount = el.first;
        if (size == emptyCount) //Already sorted as all items are empty.
            return;

        Collection temp;
        if (0 < emptyCount)
            move_empty_to_start<Collection, TSize, get_size_t>
            (get_size, emptyCount, collection, temp);

        temp.resize(size - emptyCount);
        std::vector<TSize> counts(maxValue + 1);

        //From least- to most- significant digit.
        auto digit = el.second - 1;

        do sort_by_digit<Collection, digit_t, TSize, get_size_t, get_item_t>(
            get_size, get_item,
            digit, emptyCount, maxValue,
            collection,
            temp,
            counts);
        while (0 != digit--);
    }
}