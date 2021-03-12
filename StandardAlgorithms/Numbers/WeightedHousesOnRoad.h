#pragma once
#include <vector>
#include <sstream>
#include <algorithm>
#include "Arithmetic.h"
#include "../Utilities/StreamUtilities.h"

namespace Privet
{
    namespace Algorithms
    {
        namespace Numbers
        {
            template <typename TDistance = unsigned int,
                typename TWeight = unsigned long int>
                struct DistanceAndWeight final
            {
                TDistance Distance;
                TWeight Weight;

                //Sort by Distance.
                inline bool operator < (
                    DistanceAndWeight const& b) const
                {
                    return Distance < b.Distance;
                }

                void Validate() const
                {
                    if (Weight <= 0)
                    {
                        std::ostringstream ss;
                        ss << "The Weight (" << Weight
                            << ") must be positive."
                            << " Distance=" << Distance
                            << ".";
                        StreamUtilities::throw_exception(ss);
                    }
                }
            };

            template <typename Distance = unsigned int,
                typename Weight = unsigned long int>
                class WeightedHousesOnRoad final
            {
                WeightedHousesOnRoad() = delete;

                using TDistanceAndWeight = DistanceAndWeight <Distance, Weight >;

            public:
                //TODO: There is no check for overflows.
                static Weight Solve(
                    //It must have at least one house.
                    const std::vector< TDistanceAndWeight >& houses,
                    const Distance radius,
                    const size_t maximumStationCount,
                    //const Weight maximumAllowedTotalWeight,
                    std::vector< Distance >& chosenPlacements);

            private:
                static void CheckInput(
                    const std::vector< TDistanceAndWeight >& houses,
                    const Distance radius,
                    const size_t maximumStationCount);

                static bool TryAllWithinDiameter(
                    const std::vector< TDistanceAndWeight >& houses,
                    const size_t diameter,
                    std::vector< Distance >& chosenPlacements,
                    Weight& weight);

                static Weight SumWeight(
                    const std::vector< TDistanceAndWeight >& houses,
                    const size_t initialIndexInclusive,
                    const size_t lastIndexExclusive);

                //Subtracts left-most house distance from every house
                // to normalize the distances.
                //Also, if 2 or more houses are at the same distance, they are merged into one house.
                static void NormalizeDistanceAndMerge(
                    std::vector< TDistanceAndWeight >& houses);

                //Calculates the weight of the houses that are
                // in the range [leftDistanceInclusive, leftDistanceInclusive + diameter].
                //
                // The actual station position would be leftDistanceInclusive + (diameter/2).
                static Weight StationCoverWeight(
                    const std::vector< TDistanceAndWeight >& houses,
                    const size_t leftDistanceInclusive,
                    const size_t diameter,
                    size_t& startIndex);

                static void FillStationCoverWeight(
                    const std::vector< TDistanceAndWeight >& houses,
                    const size_t diameter,
                    const size_t columns,
                    std::vector< Weight >& stationWeights);

                static Weight CalculateMatix(
                    const size_t diameter,
                    const size_t rows,
                    const size_t columns,
                    const std::vector< Weight >& stationWeights,
                    std::vector< Weight >& tempArray);

                static void BacktrackResult(
                    const Distance radius,
                    const size_t rows,
                    const size_t columns,
                    const size_t leftMostHouseDistance,
                    const std::vector< Weight >& stationWeights,
                    const std::vector< Weight >& tempArray,
                    std::vector< Distance >& chosenPlacements);
            };

            template <typename Distance,
                typename Weight>
                Weight
                Privet::Algorithms::Numbers::WeightedHousesOnRoad<Distance, Weight>
                ::Solve(
                    const std::vector< TDistanceAndWeight >& houses,
                    const Distance radius,
                    const size_t maximumStationCount,
                    std::vector< Distance >& chosenPlacements)
            {
                CheckInput(houses, radius, maximumStationCount);
                chosenPlacements.clear();

                std::vector< TDistanceAndWeight > housesWorkingCopy = houses;
                std::sort(housesWorkingCopy.begin(), housesWorkingCopy.end());

                const size_t diameter = AddUnsigned(size_t(radius), size_t(radius));

                {
                    Weight result1 = 0;

                    //TODO: p3. sorting is not required for this.
                    if (TryAllWithinDiameter(
                        housesWorkingCopy,
                        diameter,
                        chosenPlacements,
                        result1))
                    {
                        return result1;
                    }
                }
                //There are at least 2 houses here.

                const size_t leftMostHouseDistance = size_t(housesWorkingCopy[0].Distance);

                NormalizeDistanceAndMerge(housesWorkingCopy);

                const size_t housesSize = housesWorkingCopy.size();
                const size_t rightMostHouseDistance = size_t(housesWorkingCopy[housesSize - 1].Distance);

                const size_t one = 1;
                const size_t columns = AddUnsigned(rightMostHouseDistance, one);

                std::vector< Weight > stationWeights;
                FillStationCoverWeight(housesWorkingCopy, diameter, columns, stationWeights);

                //When there are more stations than houses.
                const size_t maximumStationCountNew = std::min(maximumStationCount, housesSize);
                const size_t rows = AddUnsigned(maximumStationCountNew, one);
                const auto bigSize = MultiplyUnsigned(rows, columns);

                //The matrix is [rows][columns].
                std::vector< Weight > tempArray(bigSize, Weight(0));

                const Weight result = CalculateMatix(
                    diameter,
                    rows,
                    columns,
                    stationWeights,
                    tempArray);

                BacktrackResult(
                    radius,
                    rows,
                    columns,
                    leftMostHouseDistance,
                    stationWeights,
                    tempArray,
                    chosenPlacements);

                return result;
            }

            template <typename Distance,
                typename Weight>
                void
                Privet::Algorithms::Numbers::WeightedHousesOnRoad<Distance, Weight>
                ::CheckInput(
                    const std::vector< TDistanceAndWeight >& houses,
                    const Distance radius,
                    const size_t maximumStationCount)
            {
                if (radius <= 0)
                {
                    throw std::runtime_error("The radius must be positive.");
                }

                if (maximumStationCount <= 0)
                {
                    throw std::runtime_error("The maximumStationCount must be positive.");
                }

                const size_t housesSize = houses.size();
                if (0 == housesSize)
                {
                    throw std::runtime_error("There must be at least one house.");
                }

                size_t index = 0;
                do
                {
                    const TDistanceAndWeight& house = houses[index];
                    house.Validate();
                } while ((++index) < housesSize);
            }

            template <typename Distance,
                typename Weight>
                bool
                Privet::Algorithms::Numbers::WeightedHousesOnRoad<Distance, Weight>
                ::TryAllWithinDiameter(
                    const std::vector< TDistanceAndWeight >& houses,
                    const size_t diameter,
                    std::vector< Distance >& chosenPlacements,
                    Weight& weight)
            {
                const size_t leftMostHouseDistance = size_t(houses[0].Distance);

                const size_t housesSize = houses.size();
                const size_t rightMostHouseDistance = size_t(houses[housesSize - 1].Distance);

                const bool allWithinDiameter = rightMostHouseDistance
                    //TODO: p4. add signed?
                    <= AddUnsigned(diameter, leftMostHouseDistance);
                if (allWithinDiameter)
                {
                    const size_t delta = rightMostHouseDistance - leftMostHouseDistance;
                    const size_t placement = leftMostHouseDistance + (delta >> 1);

                    chosenPlacements.push_back(Distance(placement));

                    weight = SumWeight(houses, 0, housesSize);
                }

                return allWithinDiameter;
            }

            template <typename Distance,
                typename Weight>
                Weight
                Privet::Algorithms::Numbers::WeightedHousesOnRoad<Distance, Weight>
                ::SumWeight(
                    const std::vector< TDistanceAndWeight >& houses,
                    const size_t initialIndexInclusive,
                    const size_t lastIndexExclusive)
            {
                Weight result = 0;
                size_t index = initialIndexInclusive;
                do
                {
                    result += houses[index].Weight;
                } while ((++index) < lastIndexExclusive);

                return result;
            }

            template <typename Distance,
                typename Weight>
                void
                Privet::Algorithms::Numbers::WeightedHousesOnRoad<Distance, Weight>
                ::NormalizeDistanceAndMerge(
                    std::vector< TDistanceAndWeight >& houses)
            {
                const Distance leftMostHouseDistance = houses[0].Distance;
                size_t housesSize = houses.size();
                size_t index = 0;

                do
                {
                    houses[index].Distance -= leftMostHouseDistance;

                    if (index > 0
                        && houses[index - 1].Distance == houses[index].Distance)
                    {//Merge.
                        houses[index - 1].Weight += houses[index].Weight;

                        //TODO: p2, performance. After sorting, do all operations in one step.
                        houses.erase(houses.begin() + index);

                        --housesSize;
                    }
                    else
                    {
                        ++index;
                    }
                } while (index < housesSize);
            }

            template <typename Distance,
                typename Weight>
                Weight
                Privet::Algorithms::Numbers::WeightedHousesOnRoad<Distance, Weight>
                ::StationCoverWeight(
                    //It has already been sorted by distance.
                    const std::vector< TDistanceAndWeight >& houses,
                    const size_t leftDistanceInclusive,
                    const size_t diameter,
                    size_t& startIndex)
            {
                const size_t rightDistanceInclusive = leftDistanceInclusive + diameter;

                const size_t housesSize = houses.size();
                Weight result = 0;

                bool hasStarted = false;
                for (size_t index = startIndex; index < housesSize; ++index)
                {
                    const TDistanceAndWeight& house = houses[index];

                    const bool isWithin = leftDistanceInclusive <= house.Distance
                        && house.Distance <= rightDistanceInclusive;

                    if (isWithin)
                    {
                        result += house.Weight;
                        hasStarted = true;
                    }
                    else if (hasStarted)
                    {//There cannot be more houses past rightDistanceInclusive.
                      //TODO: p2. Change startIndex when "distance" increases to skip initial houses.
                        break;
                    }
                }

                return result;
            }

            template <typename Distance,
                typename Weight>
                void
                Privet::Algorithms::Numbers::WeightedHousesOnRoad<Distance, Weight>
                ::FillStationCoverWeight(
                    const std::vector< TDistanceAndWeight >& houses,
                    const size_t diameter,
                    const size_t columns,
                    std::vector< Weight >& stationWeights)
            {
                stationWeights.reserve(columns);

                size_t startIndex = 0;
                for (size_t distance = 0; distance < columns; ++distance)
                {
                    const Weight weight = StationCoverWeight(
                        houses, distance, diameter, startIndex);

                    stationWeights.push_back(weight);
                }
            }

            template <typename Distance,
                typename Weight>
                Weight
                Privet::Algorithms::Numbers::WeightedHousesOnRoad<Distance, Weight>
                ::CalculateMatix(
                    const size_t diameter,
                    const size_t rows,
                    const size_t columns,
                    const std::vector< Weight >& stationWeights,
                    std::vector< Weight >& tempArray)
            {
                const size_t diameterPlusOne = diameter + 1;
                const size_t diameterPlusOnePlusColumns = diameterPlusOne + columns;

                const size_t maxRowShift = rows * columns;

                for (size_t rowShift = columns;
                    rowShift < maxRowShift;
                    rowShift += columns)//Go by "station count" from 1 to max.
                {
                    Weight maxWeight = 0;

                    for (size_t distance = 0; distance < columns; ++distance)
                    {
                        Weight newWeight = stationWeights[distance];
                        if (diameterPlusOne <= distance)
                        {
                            const size_t previousSmallerDistanceIndex =
                                rowShift + distance - diameterPlusOnePlusColumns;

                            newWeight += tempArray[previousSmallerDistanceIndex];
                        }

                        if (newWeight > maxWeight)
                        {
                            maxWeight = newWeight;
                        }

                        tempArray[rowShift + distance] = maxWeight;
                    }
                }

                const auto bigSize = MultiplyUnsigned(rows, columns);

                const Weight result = tempArray[bigSize - 1];
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
                Privet::Algorithms::Numbers::WeightedHousesOnRoad<Distance, Weight>
                ::BacktrackResult(
                    const Distance radius,
                    const size_t rows,
                    const size_t columns,
                    const size_t leftMostHouseDistance,
                    const std::vector< Weight >& stationWeights,
                    const std::vector< Weight >& tempArray,
                    std::vector< Distance >& chosenPlacements)
            {
                const size_t diameter = AddUnsigned(size_t(radius), size_t(radius));
                const size_t diameterPlusOne = diameter + 1;

                const auto bigSize = MultiplyUnsigned(rows, columns);
                const Weight result = tempArray[bigSize - 1];

                Weight remainingWeight = result;
                size_t distance = columns - 1, stationCount = rows - 1;
                size_t index = stationCount * columns + distance;

                for (;;)
                {
                    while (tempArray[index - columns] == tempArray[index])
                    {
                        --stationCount;
                        index -= columns;
                    }

                    while (tempArray[index - 1] == tempArray[index])
                    {
                        --distance;
                        --index;
                    }

                    const size_t position = leftMostHouseDistance + distance + radius;
                    chosenPlacements.push_back(Distance(position));

                    remainingWeight -= stationWeights[distance];
                    if (0 == remainingWeight || 1 == stationCount)
                    {
                        break;
                    }

                    --stationCount;
                    distance -= diameterPlusOne;

                    index -= columns + diameterPlusOne;
                }

                if (0 != remainingWeight)
                {
                    std::ostringstream ss;
                    ss << "Inner error: the remainingWeight (" << remainingWeight
                        << ") must be zero. result=" << result
                        << ".";
                    StreamUtilities::throw_exception(ss);
                }

                std::reverse(chosenPlacements.begin(), chosenPlacements.end());
            }
        }
    }
}