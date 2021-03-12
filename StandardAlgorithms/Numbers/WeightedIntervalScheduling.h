#pragma once
#include <vector>
#include <algorithm>
#include "Arithmetic.h"
#include "WithIdUtilities.h"
#include "Interval.h"
#include "../Utilities/ExceptionUtilities.h"
#include "../Utilities/StreamUtilities.h"

namespace Privet
{
    namespace Algorithms
    {
        namespace Numbers
        {
            // Interval with Id.
            template <typename Distance,
                typename TWeight>
                class WeightedSegmentWithId final : public
                Privet::Algorithms::Numbers::WeightedInterval<Distance, TWeight>
            {
                using base_t = Privet::Algorithms::Numbers::WeightedInterval<Distance, TWeight>;

            public:

                size_t Id;

                WeightedSegmentWithId(
                    const Distance& start = {},
                    const Distance& finish = {},
                    const TWeight& weight = {},
                    const size_t id = {})
                    : base_t(start, finish, weight),
                    Id(id)
                {
                }

                WeightedSegmentWithId(const WeightedSegmentWithId&) = default;

                WeightedSegmentWithId(const base_t& a,
                    const size_t id = {})
                    : base_t(a), Id{ id }
                {
                }

                WeightedSegmentWithId& operator = (const base_t& a) override
                {
                    auto b = dynamic_cast<const WeightedSegmentWithId*>(&a);
                    if (b)
                    {
                        base_t::operator = (static_cast<base_t const&>(a));
                        this->Id = b->Id;
                        return *this;
                    }

                    throw throw_type_mismatch<base_t>();
                }

                //TODO: Is it required?
                WeightedSegmentWithId<Distance, TWeight>& operator = (
                    const WeightedSegmentWithId<Distance, TWeight>& a) = default;

                //TODO: Is it required?
                WeightedSegmentWithId(WeightedSegmentWithId&&) = default;

                WeightedSegmentWithId& operator = (base_t&& a) override
                {
                    return operator = (static_cast<const base_t&>(a));
                }

                WeightedSegmentWithId& operator = (WeightedSegmentWithId&&) = default;
            };

            template <typename Distance,
                typename Weight>
                class WeightedIntervalScheduling final
            {
                WeightedIntervalScheduling() = delete;

                using TWeightedSegmentWithId = WeightedSegmentWithId<Distance, Weight>;

                static const unsigned int FirstSegmentIndex = 1;

            public:

                using TWeightedSegment = Numbers::WeightedInterval<Distance, Weight>;

                //There is no check for overflows.
                static Weight FindMinWeightSchedule(
                    const std::vector< TWeightedSegment >& segments,
                    std::vector< size_t >& chosenSegmentIndexes);

            private:
                static void Validate(
                    const std::vector< TWeightedSegment >& segments);

                static inline int SortByFinishTime(const void* a, const void* b)
                {
                    TWeightedSegmentWithId* pa = const_cast<TWeightedSegmentWithId*>(static_cast<const TWeightedSegmentWithId*>(a));
                    TWeightedSegmentWithId* pb = const_cast<TWeightedSegmentWithId*>(static_cast<const TWeightedSegmentWithId*>(b));

                    if (nullptr == pa)
                    {
                        return nullptr == pb ? 0 : -1;
                    }

                    if (nullptr == pb)
                    {
                        return 1;
                    }

                    const int result = compare<Distance>(
                        pa->Stop,
                        pb->Stop);

                    return result;
                }

                static void PrepareWorkingCopy(
                    const std::vector< TWeightedSegment >& segments,
                    std::vector< TWeightedSegmentWithId >& segmentsWorkingCopy);

                static void ComputeLargestNonOverlappingSmallerIndex(
                    const std::vector< TWeightedSegmentWithId >& segments,
                    std::vector< size_t >& largestNonOverlappingSmallerIndex);

                static void ComputeMatrix(
                    const std::vector< TWeightedSegmentWithId >& segments,
                    const std::vector< size_t >& largestNonOverlappingSmallerIndex,
                    std::vector< Weight >& minWeightsMatrix);

                static void BackTrackSolution(
                    const std::vector< TWeightedSegmentWithId >& segments,
                    const std::vector< size_t >& largestNonOverlappingSmallerIndex,
                    const std::vector< Weight >& minWeightsMatrix,
                    std::vector< size_t >& chosenSegmentIndexes);
            };

            template <typename Distance,
                typename Weight>
                Weight
                Privet::Algorithms::Numbers::WeightedIntervalScheduling<Distance, Weight>
                ::FindMinWeightSchedule(
                    const std::vector< TWeightedSegment >& segments,
                    std::vector< size_t >& chosenSegmentIndexes)
            {
                const size_t segmentsSize = segments.size();

                Validate(segments);

                //if (1 == segmentsSize)
                //{
                //  chosenSegmentIndexes.assign(1, segments[0].Id);
                //  return segments[0].Weight;
                //}

                std::vector< TWeightedSegmentWithId > segmentsWorkingCopy;
                PrepareWorkingCopy(segments, segmentsWorkingCopy);

                std::vector< size_t > largestNonOverlappingSmallerIndex;
                ComputeLargestNonOverlappingSmallerIndex(segmentsWorkingCopy, largestNonOverlappingSmallerIndex);

                std::vector< Weight > minWeightsMatrix;
                ComputeMatrix(segmentsWorkingCopy, largestNonOverlappingSmallerIndex,
                    minWeightsMatrix);

                BackTrackSolution(segmentsWorkingCopy, largestNonOverlappingSmallerIndex,
                    minWeightsMatrix, chosenSegmentIndexes);

                //Elements may be anywhere - sort the returned indexes.
                sort(chosenSegmentIndexes.begin(), chosenSegmentIndexes.end());

                const Weight result = minWeightsMatrix[segmentsSize];
                if (result <= 0)
                {
                    std::ostringstream ss;
                    ss << "Inner error. The resulting weight (" << result
                        << ") must be positive.";
                    StreamUtilities::throw_exception(ss);
                }

                return result;
            }

            template <typename Distance,
                typename Weight>
                void
                Privet::Algorithms::Numbers::WeightedIntervalScheduling<Distance, Weight>
                ::Validate(
                    const std::vector< TWeightedSegment >& segments)
            {
                const size_t segmentsSize = segments.size();
                if (0 == segmentsSize)
                {
                    throw std::runtime_error("There must be segments.");
                }

                size_t index = 0;
                do
                {
                    segments[index].Validate();
                } while ((++index) < segmentsSize);
            }

            template <typename Distance,
                typename Weight>
                void Privet::Algorithms::Numbers::
                WeightedIntervalScheduling<Distance, Weight>
                ::PrepareWorkingCopy(
                    const std::vector< TWeightedSegment >& segments,
                    std::vector< TWeightedSegmentWithId >& segmentsWorkingCopy)
            {
                const size_t segmentsSize = segments.size();

                WithIdUtilities< TWeightedSegment, TWeightedSegmentWithId >
                    ::CopyAndAssignSequentialId(segments, segmentsWorkingCopy);

                qsort(segmentsWorkingCopy.data(), segmentsSize,
                    sizeof(TWeightedSegmentWithId), SortByFinishTime);
            }

            //To distinguish between 0-th segment and no segment:
            //A) Indexes in "minWeightsMatrix" start with FirstSegmentIndex (1).
            //B) 0 will mean there is no smaller value.
            template <typename Distance,
                typename Weight>
                void Privet::Algorithms::Numbers::
                WeightedIntervalScheduling<Distance, Weight>
                ::ComputeLargestNonOverlappingSmallerIndex(
                    const std::vector< TWeightedSegmentWithId >& segments,
                    std::vector< size_t >& largestNonOverlappingSmallerIndex)
            {
                const size_t segmentsSize = segments.size();

                const size_t noValue = 0;
                largestNonOverlappingSmallerIndex.resize(segmentsSize, noValue);

                const size_t zeroIndexElementHasNoPrevious = 1;
                for (size_t current = zeroIndexElementHasNoPrevious; current < segmentsSize; ++current)
                {
                    const Distance start = segments[current].Start;
                    size_t previous = current - 1;

                    do
                    {
                        if (segments[previous].Stop <= start)
                        {
                            largestNonOverlappingSmallerIndex[current] = FirstSegmentIndex + previous;
                            break;
                        }
                    } while (0 != (previous--));
                }
            }

            template <typename Distance,
                typename Weight>
                void Privet::Algorithms::Numbers::
                WeightedIntervalScheduling<Distance, Weight>
                ::ComputeMatrix(
                    const std::vector< TWeightedSegmentWithId >& segments,
                    const std::vector< size_t >& largestNonOverlappingSmallerIndex,
                    std::vector< Weight >& minWeightsMatrix)
            {
                const size_t segmentsSize = segments.size();

                minWeightsMatrix.reserve(1 + segmentsSize);

                //0-th element will have 0 value.
                Weight previous = 0;
                minWeightsMatrix.push_back(previous);

                size_t index = 0;
                do
                {
                    const size_t smallerIndex = largestNonOverlappingSmallerIndex[index];

                    const Weight withElement = segments[index].Weight
                        + minWeightsMatrix[smallerIndex];

                    if (previous < withElement)
                    {
                        previous = withElement;
                    }

                    minWeightsMatrix.push_back(previous);
                } while ((++index) < segmentsSize);
            }

            template <typename Distance,
                typename Weight>
                void Privet::Algorithms::Numbers::
                WeightedIntervalScheduling<Distance, Weight>
                ::BackTrackSolution(
                    const std::vector< TWeightedSegmentWithId >& segments,
                    const std::vector< size_t >& largestNonOverlappingSmallerIndex,
                    const std::vector< Weight >& minWeightsMatrix,
                    std::vector< size_t >& chosenSegmentIndexes)
            {
                chosenSegmentIndexes.clear();

                const size_t segmentsSize = segments.size();
                const Weight maxValue = minWeightsMatrix[segmentsSize];
                if (maxValue <= 0)
                {
                    throw std::runtime_error("Internal error: the maximum weight must be positive.");
                }

#ifdef _DEBUG
                Weight remainingWeight = maxValue;
#endif
                size_t index = segmentsSize - 1;
                do
                {
                    const size_t smallerIndex = largestNonOverlappingSmallerIndex[index];
                    const Weight selectCurrentWeight = segments[index].Weight
                        + minWeightsMatrix[smallerIndex];

                    const Weight skipCurrentWeight = minWeightsMatrix[index];
                    if (skipCurrentWeight <= selectCurrentWeight)
                    {
                        chosenSegmentIndexes.push_back(segments[index].Id);
#ifdef _DEBUG
                        if (remainingWeight < segments[index].Weight)
                        {
                            std::ostringstream ss;
                            ss << "remainingWeight (" << remainingWeight << ") "
                                << " < segments[" << index << "].Weight = " << segments[index].Weight
                                << ", maxValue=" << maxValue << ".";
                            StreamUtilities::throw_exception(ss);
                        }

                        remainingWeight -= segments[index].Weight;
#endif
                        index = smallerIndex;
                    }
                } while (0 != (index--));//Cannot go left.

#ifdef _DEBUG
                if (remainingWeight > 0)
                {
                    std::ostringstream ss;
                    ss << "The last remainingWeight (" << remainingWeight << ") > 0"
                        << ", maxValue=" << maxValue << ".";
                    StreamUtilities::throw_exception(ss);
                }
#endif
            }
        }
    }
}