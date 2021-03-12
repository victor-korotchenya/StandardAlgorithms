#pragma once

#include <cassert>
#include "../Utilities/ExceptionUtilities.h"
#include "Sort.h"

namespace
{
    constexpr auto _PartitionNumber = 5;

    template <class TItem, class TInteger>
    TInteger SortSmallPartition(
        TItem* data,
        const TInteger stopIndexInclusive,
        const TInteger statisticOrderZeroBased)
    {
        assert(statisticOrderZeroBased >= 0 && statisticOrderZeroBased <= stopIndexInclusive && stopIndexInclusive < _PartitionNumber);

        Privet::Algorithms::Numbers::InsertionSort(data, stopIndexInclusive + 1llu);
        return statisticOrderZeroBased;
    }

    template <class TItem, class TInteger>
    TInteger Partite(TItem* data, const TInteger stopIndexInclusive)
    {
        const TInteger partitions_Temp = (1 + stopIndexInclusive) / _PartitionNumber;
        const TInteger partitions = partitions_Temp + (1 + stopIndexInclusive !=
            partitions_Temp * _PartitionNumber);
        assert(partitions > 0);

        TInteger left = 0, right = _PartitionNumber - 1;
        for (TInteger part = 0; part < partitions;
            ++part, left += _PartitionNumber, right += _PartitionNumber)
        {
            if (right > stopIndexInclusive)
                right = stopIndexInclusive;

            const TInteger orderTemp = (right - left) >> 1;
            const TInteger median = SortSmallPartition(data + left, right - left, orderTemp);

            // Move the median to the beginning.
            std::swap(data[part], data[median]);
        }

        return partitions;
    }

    template <class TItem, class TInteger>
    void CountLessEqual(
        const TItem* const data,
        const TInteger stopIndexInclusive,
        const TInteger medianOfMediansIndex,
        TInteger& lesserSize,
        TInteger& equalSize)
    {
        const TItem medianDatum = data[medianOfMediansIndex];

        for (TInteger j = 0; j <= stopIndexInclusive; ++j)
        {
            if (data[j] < medianDatum)
                ++lesserSize;
            else if (medianDatum == data[j])
                ++equalSize;
        }
    }

    template <class TItem, class TInteger>
    void MoveSmallerToBeginning(
        TItem* data,
        const TInteger stopIndexInclusive,
        const TInteger medianIndex,
        const TInteger lesserSize)
    {
        assert(lesserSize > 0);

        const TItem mediumDatum = data[medianIndex];
        const TInteger lesserSizeMinusOne = lesserSize - 1;
        TInteger smallerIndex = 0;

        while (data[smallerIndex] < mediumDatum)
        {//Skip smaller items at the start.
            if (lesserSizeMinusOne == smallerIndex)
                return;

            ++smallerIndex;
        }

        TInteger rightIndex = stopIndexInclusive;
        for (;;)
        {
            if (data[rightIndex] < mediumDatum)
            {
                std::swap(data[smallerIndex], data[rightIndex]);

                if (lesserSizeMinusOne == smallerIndex)
                    return;

                ++smallerIndex;

                while (data[smallerIndex] < mediumDatum)
                {//Skip smaller items at the start to have fewer swaps.
                    if (lesserSizeMinusOne == smallerIndex)
                        return;

                    ++smallerIndex;
                }
            }

            --rightIndex;
            assert(rightIndex + TInteger(1));
        }
    }

    template <class TItem, class TInteger>
    void MoveGreaterToEnding(
        TItem* data,
        const TInteger stopIndexInclusive,
        const TInteger medianIndex,
        const TInteger greaterSize)
    {
        assert(greaterSize > 0);

        const TItem mediumDatum = data[medianIndex];
        const TInteger greaterSizeLastIndex = 1 + stopIndexInclusive - greaterSize;
        TInteger greaterIndex = stopIndexInclusive;

        while (mediumDatum < data[greaterIndex])
        {//Skip greater items at the end.
            if (greaterSizeLastIndex == greaterIndex)
                return;

            --greaterIndex;
        }

        TInteger leftIndex = 0;
        for (;;)
        {
            if (mediumDatum < data[leftIndex])
            {
                std::swap(data[greaterIndex], data[leftIndex]);

                if (greaterSizeLastIndex == greaterIndex)
                    return;

                --greaterIndex;

                while (mediumDatum < data[greaterIndex])
                {//Skip greater items at the end to have fewer swaps.
                    if (greaterSizeLastIndex == greaterIndex)
                        return;

                    --greaterIndex;
                }
            }

            ++leftIndex;
            assert(leftIndex <= stopIndexInclusive);
        }
    }
}

namespace Privet::Algorithms::Numbers
{
    template <class TItem, class TInteger>
    class MedianSelection final
    {
    public:
        //The std::nth_element is faster in pactice but not linear as it uses quick select.
        static TItem* SelectOrderStatistic(
            TItem* data, const TInteger dataSize,
            //Starts with 0.
            const TInteger statisticOrder)
        {
            assert(data);
            RequirePositive(dataSize, "dataSize");
            RequireNonNegative(statisticOrder, "statisticOrder");
            RequireGreater(dataSize, statisticOrder, "statisticOrder");

            const auto selectionIndex = SelectImpl(data, dataSize - 1, statisticOrder);
            auto result = data + selectionIndex;
            return result;
        }

    private:
        static TInteger SelectImpl(
            TItem* data,
            TInteger stopIndexInclusive,
            TInteger statisticOrderZeroBased)
        {
            //To have fewer recursive calls.
            TInteger shift = 0;

        LABEL_SelectImpl:
            if (stopIndexInclusive < _PartitionNumber)
            {//Fewer than "_PartitionNumber" elements - use brute-force.
                const TInteger result = shift
                    + SortSmallPartition(data, stopIndexInclusive, statisticOrderZeroBased);
                return result;
            }

            const TInteger partitions = Partite(data, stopIndexInclusive);

            //There should be fewer than Log(n)/Log(10) recursive calls.
            const TInteger medianOfMediansIndex = SelectImpl(data, partitions - 1, partitions >> 1);

            TInteger lesserSize = 0;
            TInteger equalSize = 0;

            //TODO: p1. join Count2Partitions + MoveSmallerToBeginning + MoveGreaterToEnding.
            CountLessEqual(
                data,
                stopIndexInclusive,
                medianOfMediansIndex,
                lesserSize,
                equalSize);

            if (statisticOrderZeroBased < lesserSize)
            {
                MoveSmallerToBeginning(data, stopIndexInclusive, medianOfMediansIndex, lesserSize);

                stopIndexInclusive = lesserSize - 1;
                goto LABEL_SelectImpl;
            }

            if (lesserSize + equalSize <= statisticOrderZeroBased)
            {
                const TInteger greaterSize = 1 + stopIndexInclusive - lesserSize - equalSize;

                MoveGreaterToEnding(data, stopIndexInclusive, medianOfMediansIndex, greaterSize);

                const TInteger startIndexNew = lesserSize + equalSize;
                const TInteger stopIndexInclusiveNew = stopIndexInclusive - startIndexNew;
                const TInteger statisticOrderNew = statisticOrderZeroBased - startIndexNew;

                shift += startIndexNew;
                data += startIndexNew;
                stopIndexInclusive = stopIndexInclusiveNew;
                statisticOrderZeroBased = statisticOrderNew;

                goto LABEL_SelectImpl;
            }

            return shift + medianOfMediansIndex;
        }
    };

    namespace Tests
    {
        void MedianSelectionTests();
    }
}