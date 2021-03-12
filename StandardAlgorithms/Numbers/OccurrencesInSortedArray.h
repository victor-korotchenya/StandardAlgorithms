#pragma once
#include<vector>

#ifdef _DEBUG
#include "../Utilities/StreamUtilities.h"
#include "../Utilities/VectorUtilities.h"
#endif

#include "BinarySearch.h"

namespace Privet::Algorithms::Numbers
{
    //Given a sorted array A, and an element,
    // returns the two indexes of the first and last occurrences of the element.
    //If the element occurs once, both indexes are equal to the index of the element.
    //If the element does not occur, both indexes are equal to -1.
    class OccurrencesInSortedArray final
    {
        OccurrencesInSortedArray() = delete;

    public:
        using Pair = std::pair<size_t, size_t>;

        //Use "FindOccurrences" which is faster.
        //O(n) time.
        template <typename Element >
        static Pair FindOccurrencesSlow(
            const std::vector<Element>& data,
            const Element& element);

        //O(log(n)) time.
        template <typename Element >
        static Pair FindOccurrences(
            const std::vector<Element>& data,
            const Element& element);

    private:
        template <typename Element >
        static bool FindHelper(
            const std::vector<Element>& data,
            const Element& element,
            size_t& low, size_t& middle, size_t& high);

        template <typename Element >
        static void LeftMost(
            const std::vector<Element>& data,
            const Element& element,
            size_t& low,
            size_t& best);

        template <typename Element >
        static void RightMost(
            const std::vector<Element>& data,
            const Element& element,
            size_t& best,
            size_t& high);
    };

    template <typename Element>
    OccurrencesInSortedArray::Pair OccurrencesInSortedArray::FindOccurrencesSlow(
        const std::vector<Element>& data, const Element& element)
    {
#ifdef _DEBUG
        Privet::Algorithms::VectorUtilities::RequireArrayIsSorted(data, "IsSorted");
#endif
        const size_t size = data.size();
        if (0 < size)
        {
            auto pElement = binary_search_inclusive(data.data(), data.data() + data.size() - 1, element);

            if (pElement)
            {//Found
                size_t low = pElement - data.data();
#ifdef _DEBUG
                if (!(low < size))
                {
                    std::ostringstream ss;
                    ss << "Error: low(" << low << ") must be less than size(" << size << ").";
                    StreamUtilities::throw_exception(ss);
                }
#endif
                size_t high = low;

                while (0 < low && element == data[low - 1])
                {
                    --low;
                }

                while (high + 1 < size && element == data[high + 1])
                {
                    ++high;
                }

                return Pair(low, high);
            }
        }

        const size_t minusOne = size_t(0) - size_t(1);
        return Pair(minusOne, minusOne);
    }

    template <typename Element>
    OccurrencesInSortedArray::Pair OccurrencesInSortedArray::FindOccurrences(
        const std::vector<Element>& data, const Element& element)
    {
#ifdef _DEBUG
        Privet::Algorithms::VectorUtilities::RequireArrayIsSorted(data, "IsSorted");
#endif
        const size_t size = data.size();
        if (0 < size)
        {
            size_t low = 0, high = size - 1, middle;

            if (FindHelper(data, element, low, middle, high))
            {//Found at least one position.
                auto bestLow = middle, bestHigh = middle;

                LeftMost(data, element, low, bestLow);
                RightMost(data, element, bestHigh, high);

                return Pair(bestLow, bestHigh);
            }
        }

        const size_t minusOne = size_t(0) - size_t(1);
        return Pair(minusOne, minusOne);
    }

    template <typename Element>
    bool OccurrencesInSortedArray::FindHelper(
        const std::vector<Element>& data, const Element& element,
        size_t& low, size_t& middle, size_t& high)
    {
        do
        {
            middle = low + ((high - low) >> 1);

            if (element == data[middle])
            {
                return true;
            }

            if (element < data[middle])
            {
                if (0 == middle)
                {
                    break;
                }

                high = middle - 1;
            }
            else
            {
                low = middle + 1;
            }
        } while (low <= high);

        return false;
    }

    template <typename Element>
    void OccurrencesInSortedArray::LeftMost(
        const std::vector<Element>& data,
        const Element& element,
        size_t& low,
        size_t& best)
    {
        while (low < best && element == data[best - 1])
        {//Can go left?
            size_t high = best - 1;
            FindHelper(data, element, low, best, high);
        }
    }

    template <typename Element>
    void OccurrencesInSortedArray::RightMost(
        const std::vector<Element>& data,
        const Element& element,
        size_t& best,
        size_t& high)
    {
        while (best < high && element == data[best + 1])
        {//Can go right?
            size_t low2 = best + 1;
            FindHelper(data, element, low2, best, high);
        }
    }
}