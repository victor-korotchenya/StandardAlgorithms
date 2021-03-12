#pragma once
#include <vector>
#include <algorithm>
#include "Arithmetic.h"
#include "../Geometry/point.h"

namespace Privet
{
    namespace Algorithms
    {
        namespace Numbers
        {
            template <typename Distance = unsigned int,
                typename Weight = unsigned long int,
                typename TPoint = Geometry::point2d <Distance>,
                typename TPointAndWeight = Geometry::PointAndWeight < TPoint, Weight >
            >
                class WeightedHousesOn2D final
            {
                WeightedHousesOn2D() = delete;

            public:

                //TODO: There is no check for overflows.
                static Weight Solve(
                    //It must have at least one house.
                    const std::vector< TPointAndWeight >& houses,
                    const Distance radius,
                    const size_t maximumStationCount,
                    //const Weight maximumAllowedTotalWeight,
                    std::vector< TPoint >& chosenPlacements);

            private:
                static void CheckInput(
                    const std::vector< TPointAndWeight >& houses,
                    const Distance radius,
                    const size_t maximumStationCount,
                    TPoint** left,
                    TPoint** right,
                    TPoint** top,
                    TPoint** bottom);

                static bool TryAllWithinDiameter(
                    const std::vector< TPointAndWeight >& houses,
                    const size_t diameter,
                    const TPoint* const left,
                    const TPoint* const right,
                    const TPoint* const top,
                    const TPoint* const bottom,
                    std::vector< TPoint >& chosenPlacements,
                    Weight& weight);

                static Weight SumWeight(
                    const std::vector< TPointAndWeight >& houses,
                    const size_t initialIndexInclusive,
                    const size_t lastIndexExclusive);
                /*
                //Subtracts left-most house distance from every house
                // to normalize the distances.
                //Also, if 2 or more houses are at the same distance, they are merged into one house.
                static void NormalizeDistanceAndMerge(
                  vector< TPointAndWeight > &houses);

                //Calculates the weight of the houses that are
                // in the range [leftDistanceInclusive, leftDistanceInclusive + diameter].
                //
                // The actual station position would be leftDistanceInclusive + (diameter/2).
                static Weight StationCoverWeight(
                  const vector< TPointAndWeight > &houses,
                  const size_t leftDistanceInclusive,
                  const size_t diameter,
                  size_t  &startIndex);

                static void FillStationCoverWeight(
                  const vector< TPointAndWeight > &houses,
                  const size_t diameter,
                  const size_t columns,
                  vector< Weight > &stationWeights);

                static Weight CalculateMatix(
                  const size_t diameter,
                  const size_t rows,
                  const size_t columns,
                  const vector< Weight > &stationWeights,
                  vector< Weight > &tempArray);

                static void BacktrackResult(
                  const Distance radius,
                  const size_t rows,
                  const size_t columns,
                  const size_t leftMostHouseDistance,
                  const vector< Weight > &stationWeights,
                  const vector< Weight > &tempArray,
                  vector< Distance > &chosenPlacements);*/
            };

            template <typename Distance,
                typename Weight,
                typename TPoint,
                typename TPointAndWeight
            >
                Weight
                Privet::Algorithms::Numbers
                ::WeightedHousesOn2D< Distance, Weight, TPoint, TPointAndWeight >
                ::Solve(
                    const std::vector< TPointAndWeight >& houses,
                    const Distance radius,
                    const size_t maximumStationCount,
                    std::vector< TPoint >& chosenPlacements)
            {
                const size_t diameter = AddUnsigned(size_t(radius), size_t(radius));
                chosenPlacements.clear();

                TPoint* left = nullptr;
                TPoint* right = nullptr;
                TPoint* top = nullptr;
                TPoint* bottom = nullptr;

                CheckInput(houses, radius, maximumStationCount,
                    &left, &right, &top, &bottom);

                Weight result1 = 0;

                //TODO: normalize.

                //TODO: paste.
                const size_t rightLeftDistance = size_t((right->X) - (left->X));
                const size_t topBottomDistance = size_t((top->Y) - (bottom->Y));

                if (TryAllWithinDiameter(
                    houses,
                    diameter,
                    left, right, top, bottom,
                    chosenPlacements,
                    result1))
                {
                    return result1;
                }

                if (0 == rightLeftDistance || 0 == topBottomDistance)
                {
                    throw std::runtime_error("TODO: call 1-d version.");
                }

                //There are at least 2 houses here.
                std::vector< TPointAndWeight > housesWorkingCopy = houses;
                std::sort(housesWorkingCopy.begin(), housesWorkingCopy.end());

                const size_t minSize = 6;
                const size_t rightLeftDistancePlus1 = std::max(minSize, AddUnsigned(rightLeftDistance, size_t(1)));
                const size_t topBottomDistancePlus1 = std::max(minSize, AddUnsigned(topBottomDistance, size_t(1)));

                const size_t rectangleSize = MultiplyUnsigned<size_t>(rightLeftDistancePlus1, topBottomDistancePlus1);

                //2D array.
                std::vector< Weight > stations(rectangleSize, Weight(0));
                {
                    for (size_t x = 0; x < topBottomDistancePlus1; ++x)
                    {
                        for (size_t y = 0; y < rightLeftDistancePlus1; ++y)
                        {
                            Weight w = 0;

                            //TODO: p2. Iterate through houses once only.
                            auto it = housesWorkingCopy.cbegin();
                            const auto end = housesWorkingCopy.cend();
                            do
                            {
                                const TPointAndWeight& pw = *it;

                                //const bool isWithinX = x <= (pw.Point.X + radius) &&pw.Point.X <= (x + radius);
                                //const bool isWithinY = y <= (pw.Point.Y + radius) &&pw.Point.Y <= (y + radius);
                                //
                                //(x, y) ->
                                //[x-2, y-2] + [x, y]
                                // x-2 <= p.x &&p.x <= x.
                                // x <= p.x + 2 &&p.x <= x.
                                const bool isWithinX = x <= (pw.Point.X + 2 * radius) && pw.Point.X <= (x + 0 * radius);
                                const bool isWithinY = y <= (pw.Point.Y + 2 * radius) && pw.Point.Y <= (y + 0 * radius);
                                const bool isWithin = isWithinX && isWithinY;
                                if (isWithin)
                                {
                                    w += pw.Weight;
                                }
                            } while (end != (++it));

                            const size_t stationIndex = (x * rightLeftDistancePlus1) + y;
                            stations[stationIndex] = w;
                        }
                    }
                }

                const size_t maximumStationCountPlus1 = AddUnsigned(maximumStationCount, size_t(1));
                const auto optSize = MultiplyUnsigned<size_t>(rectangleSize, maximumStationCountPlus1);

                //opt is 3D array, ah.m, good for you, and good for me!
                std::vector< Weight > opt;
                Weight result = 0;
                {
                    opt.resize(optSize, Weight(0));

                    for (size_t stationCount = 1; stationCount <= maximumStationCount; ++stationCount)
                    {
                        //Weight tempResult = 0;
                        //Weight tempResult2 = 0;

                        for (size_t x = 0; x < topBottomDistancePlus1; ++x)
                        {
                            Weight tempResultRow = 0;
                            for (size_t y = 0; y < rightLeftDistancePlus1; ++y)
                            {
                                const size_t stationIndex = (x * rightLeftDistancePlus1) + y;
                                const size_t oldBigIndex = stationIndex +
                                    (stationCount - 1) * topBottomDistancePlus1 * rightLeftDistancePlus1;
                                const size_t bigIndex = stationCount * topBottomDistancePlus1 * rightLeftDistancePlus1
                                    + stationIndex;

                                const Weight oldWeight = opt[oldBigIndex];

                                const Weight oldWeightUp = (0 == x) ? 0 : opt[bigIndex - rightLeftDistancePlus1];
                                const Weight oldWeightLeft = (0 == y) ? 0 : opt[bigIndex - 1];
                                const Weight oldWeightUpLeft = std::max(oldWeightUp, oldWeightLeft);

                                const Weight bestOldWeight = std::max(oldWeight, oldWeightUpLeft);

                                const Weight stationWeight = stations[stationIndex];
                                Weight newWeight = stationWeight;

                                const bool isBigX = 3 <= x;
                                const bool isBigY = 3 <= y;
                                if (isBigX)
                                {
                                    const size_t prevIndexX = oldBigIndex - 3 * rightLeftDistancePlus1;
                                    const Weight extraWeightX = opt[prevIndexX];

                                    if (isBigY)
                                    {//Both
                                      //All previous goes up.
                                        Weight maxExtraWeight = extraWeightX;

                                        //All previous goes left.
                                        const size_t prevIndexY = oldBigIndex - 3;
                                        const Weight extraWeightY = opt[prevIndexY];
                                        if (extraWeightY > maxExtraWeight)
                                        {
                                            maxExtraWeight = extraWeightY;
                                        }

                                        //Try to find the best split: some up (X) and remaining left (Y).
                                        for (size_t stationCountX = 1;
                                            stationCountX < stationCount - 1;
                                            ++stationCountX)
                                        {
                                            const size_t stationCountY = stationCount - stationCountX - 1;

                                            const size_t index2X = stationIndex +
                                                (stationCountX)*topBottomDistancePlus1 * rightLeftDistancePlus1
                                                - 3 * rightLeftDistancePlus1;

                                            const size_t index2Y = stationIndex +
                                                (stationCountY)*topBottomDistancePlus1 * rightLeftDistancePlus1
                                                - 3;

                                            const Weight weightX = opt[index2X];
                                            const Weight weightY = opt[index2Y];

                                            const size_t indexBothUpX = stationIndex +
                                                (stationCountX)*topBottomDistancePlus1 * rightLeftDistancePlus1
                                                - 3 * rightLeftDistancePlus1 - 3;

                                            const size_t indexBothUpY = stationIndex +
                                                (stationCountY)*topBottomDistancePlus1 * rightLeftDistancePlus1
                                                - 3 * rightLeftDistancePlus1 - 3;

                                            const Weight weight221 = opt[indexBothUpX];
                                            const Weight weight222 = opt[indexBothUpY];
                                            const Weight minWeightBoth = std::min(weight221, weight222);

                                            //TODO: p2. weightBoth is wrong.
                                            //
                                            //pppp ppp
                                            //pppp ppp
                                            //pppp ppp
                                            //pppp ppp
                                            //pppp ppp
                                            //
                                            //pppp 111
                                            //pppp 1X1
                                            //pppp 111
                                            //
                                            const Weight weightBoth = (1 == stationCountX || 1 == stationCountY)
                                                ? 0 : minWeightBoth;

                                            const Weight weightXY = weightX + weightY - weightBoth;
                                            if (weightXY > maxExtraWeight)
                                            {
                                                maxExtraWeight = weightXY;
                                            }
                                        }

                                        newWeight += maxExtraWeight;

                                        //const Weight extraWeightX = opt[prevIndexX];
                                        //newWeight += extraWeightX;
                                        //
                                        //
                                        //const size_t prevIndexY = oldBigIndex - 3;
                                        //const Weight extraWeightY = opt[prevIndexY];
                                        //newWeight += extraWeightY;
                                        //
                                        //const size_t prevIndexBoth = prevIndexX - 3;
                                        //const Weight extraWeightBoth = opt[prevIndexBoth];
                                        //newWeight -= extraWeightBoth;
                                    }
                                    else
                                    {//Only x.
                                        newWeight += extraWeightX;
                                    }
                                }
                                else if (isBigY)
                                {
                                    const size_t prevIndexY = oldBigIndex - 3;
                                    const Weight extraWeightY = opt[prevIndexY];
                                    newWeight += extraWeightY;
                                }

                                //                if (4 <= y)
                                //                {//Can go left for sure.
                                //                  const size_t oldBigIndexPrev = oldBigIndex - 3;
                                //                  const Weight extraWeight = opt[oldBigIndexPrev];
                                //                  newWeight += extraWeight;
                                //                }
                                //                else if (2 <= x)
                                //                {//Can go up and right
                                ////                  const size_t oldBigIndexPrev2 = oldBigIndex - rightLeftDistancePlus1 + 2;
                                //  //                newWeight += opt[oldBigIndexPrev2];
                                //                }

                                const Weight maxWeight = std::max(bestOldWeight, newWeight);
                                //if (newWeight > oldWeight)
                                //{
                                //  if (newWeight > tempResult)
                                //  {
                                //    tempResult = newWeight;
                                //  }
                                //}
                                //else if (oldWeight > tempResult)
                                //{
                                //  tempResult = oldWeight;
                                //}

                                if (maxWeight > tempResultRow)
                                {
                                    tempResultRow = maxWeight;
                                }

                                if (maxWeight > result)
                                {
                                    result = maxWeight;
                                }

                                opt[bigIndex] = //maxWeight
                                    tempResultRow
                                    //tempResult
                                    //result
                                    ;
                            }
                        }
                    }

                    //const Weight result = opt[optSize - 1];
                    return result;
                }
                /*
                const size_t leftMostHouseDistance = (size_t)(housesWorkingCopy[0].Distance);

                NormalizeDistanceAndMerge(housesWorkingCopy);

                const size_t housesSize = housesWorkingCopy.size();
                const size_t rightMostHouseDistance = (size_t)(housesWorkingCopy[housesSize - 1].Distance);

                const size_t one = 1;
                const size_t columns = AddUnsigned(rightMostHouseDistance, one);

                vector< Weight > stationWeights;
                FillStationCoverWeight(housesWorkingCopy, diameter, columns, stationWeights);

                //When there are more stations than houses.
                const size_t maximumStationCountNew = min(maximumStationCount, housesSize);
                const size_t rows = AddUnsigned(maximumStationCountNew, one);
                const size_t bigSize = MultiplyUnsigned(rows, columns);

                //The matrix is [rows][columns].
                vector< Weight > tempArray(bigSize, (Weight)0);

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

                return result;*/
            }

            template <typename Distance,
                typename Weight,
                typename TPoint,
                typename TPointAndWeight
            >
                void
                Privet::Algorithms::Numbers
                ::WeightedHousesOn2D< Distance, Weight, TPoint, TPointAndWeight >
                ::CheckInput(
                    const std::vector< TPointAndWeight >& houses,
                    const Distance radius,
                    const size_t maximumStationCount,
                    TPoint** pLeft,
                    TPoint** pRight,
                    TPoint** pTop,
                    TPoint** pBottom)
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

                TPoint* left, * right, * top, * bottom;

                left = right = top = bottom = const_cast<TPoint*>(&(houses[index].Point));

                do
                {
                    const TPointAndWeight& house = houses[index];
                    house.Validate();

                    if (house.Point.X < left->X)
                    {
                        left = const_cast<TPoint*>(&house.Point);
                    }
                    else if (right->X < house.Point.X)
                    {
                        right = const_cast<TPoint*>(&house.Point);
                    }

                    if (house.Point.Y < bottom->Y)
                    {
                        bottom = const_cast<TPoint*>(&house.Point);
                    }
                    else if (top->Y < house.Point.Y)
                    {
                        top = const_cast<TPoint*>(&house.Point);
                    }
                } while ((++index) < housesSize);

                *pLeft = left;
                *pRight = right;
                *pTop = top;
                *pBottom = bottom;
            }

            template <typename Distance,
                typename Weight,
                typename TPoint,
                typename TPointAndWeight
            >
                bool
                Privet::Algorithms::Numbers
                ::WeightedHousesOn2D< Distance, Weight, TPoint, TPointAndWeight >
                ::TryAllWithinDiameter(
                    const std::vector< TPointAndWeight >& houses,
                    const size_t diameter,
                    const TPoint* const left,
                    const TPoint* const right,
                    const TPoint* const top,
                    const TPoint* const bottom,
                    std::vector< TPoint >& chosenPlacements,
                    Weight& weight)
            {
                const Distance rightLeftDistance = (right->X) - (left->X);
                const Distance topBottomDistance = (top->Y) - (bottom->Y);

                const bool allWithinDiameter = rightLeftDistance <= diameter
                    && topBottomDistance <= diameter;
                if (allWithinDiameter)
                {
                    const TPoint point = TPoint(left->X + rightLeftDistance / 2,
                        bottom->Y + topBottomDistance / 2);

                    chosenPlacements.push_back(point);

                    const size_t housesSize = houses.size();
                    weight = SumWeight(houses, 0, housesSize);
                }

                return allWithinDiameter;
            }

            template <typename Distance,
                typename Weight,
                typename TPoint,
                typename TPointAndWeight
            >
                Weight
                Privet::Algorithms::Numbers
                ::WeightedHousesOn2D< Distance, Weight, TPoint, TPointAndWeight >
                ::SumWeight(
                    const std::vector< TPointAndWeight >& houses,
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

            /*template <typename Distance,
              typename Weight>
              void
              Privet::Algorithms::Numbers::WeightedHousesOn2D<Distance, Weight>
              ::NormalizeDistanceAndMerge(
              vector< TPointAndWeight > &houses)
            {
              const Distance leftMostHouseDistance = houses[0].Distance;
              size_t housesSize = houses.size();
              size_t index = 0;

              do
              {
                houses[index].Distance -= leftMostHouseDistance;

                if (index > 0
                  &&houses[index - 1].Distance == houses[index].Distance)
                {//Merge.
                  houses[index - 1].Weight += houses[index].Weight;

                  //TODO: p2, performance. After sorting, do all operations in one step.
                  houses.erase (houses.begin() + index);

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
              Privet::Algorithms::Numbers::WeightedHousesOn2D<Distance, Weight>
              ::StationCoverWeight(
              //It has already been sorted by distance.
              const vector< TPointAndWeight > &houses,
              const size_t leftDistanceInclusive,
              const size_t diameter,
              size_t  &startIndex)
            {
              const size_t rightDistanceInclusive = leftDistanceInclusive + diameter;

              const size_t housesSize = houses.size();
              Weight result = 0;

              bool hasStarted = false;
              for (size_t index = startIndex; index < housesSize; ++index)
              {
                const TPointAndWeight &house = houses[index];

                const bool isWithin = leftDistanceInclusive <= house.Distance
                  &&house.Distance <= rightDistanceInclusive;

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
              Privet::Algorithms::Numbers::WeightedHousesOn2D<Distance, Weight>
              ::FillStationCoverWeight(
              const vector< TPointAndWeight > &houses,
              const size_t diameter,
              const size_t columns,
              vector< Weight > &stationWeights)
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
              Privet::Algorithms::Numbers::WeightedHousesOn2D<Distance, Weight>
              ::CalculateMatix(
              const size_t diameter,
              const size_t rows,
              const size_t columns,
              const vector< Weight > &stationWeights,
              vector< Weight > &tempArray)
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

              const size_t bigSize = MultiplyUnsigned(rows, columns);

              const Weight result = tempArray[bigSize - 1];
              if (result <= 0)
              {
                ostringstream ss;
                ss << "Inner error. The resulting weight (" << result
                  << ") must be positive.";
                StreamUtilities::throw_exception(ss);
              }

              return result;
            }

            template <typename Distance,
              typename Weight>
              void
              Privet::Algorithms::Numbers::WeightedHousesOn2D<Distance, Weight>
              ::BacktrackResult(
              const Distance radius,
              const size_t rows,
              const size_t columns,
              const size_t leftMostHouseDistance,
              const vector< Weight > &stationWeights,
              const vector< Weight > &tempArray,
              vector< Distance > &chosenPlacements)
            {
              const size_t diameter = AddUnsigned(((size_t)radius), ((size_t)radius));
              const size_t diameterPlusOne = diameter + 1;

              const size_t bigSize = MultiplyUnsigned(rows, columns);
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
                chosenPlacements.push_back((Distance)position);

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
                ostringstream ss;
                ss << "Inner error: the remainingWeight (" << remainingWeight
                  << ") must be zero. result=" << result
                  << ".";
                StreamUtilities::throw_exception(ss);
              }

              reverse(chosenPlacements.begin(), chosenPlacements.end());
            }*/
        }
    }
}