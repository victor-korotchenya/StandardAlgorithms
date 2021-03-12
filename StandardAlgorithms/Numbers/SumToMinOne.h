#pragma once

#include <vector>
#include <algorithm>
#include "SumItem.h"

namespace Privet
{
    namespace Algorithms
    {
        namespace Numbers
        {
            //TODO: p1. Finish.
            template <typename Integer = unsigned long long int>
            class SumToMinOne final
            {
                SumToMinOne() = delete;

                using TSumItem = SumItem< Integer >;
                using TSubSequence = std::vector< size_t >;

            public:

                //There is no check for overflows.
                static Integer Solve(
                    const std::vector< TSumItem >& items,
                    std::vector< TSubSequence >& chosenPartitions);

            private:
                static void CheckInput(const std::vector< TSumItem >& items);

                static Integer LowerBound(
                    const std::vector< TSumItem >& items);

                static Integer UpperBound(
                    const std::vector< TSumItem >& items);

                //static void ComputeMatrix(
                //  const vector< TSumItem > &items,
                //  const Weight upperBoundWeight,
                //  vector< Integer > &maxSumMatrix);
                //
                //static Integer BacktrackResult(
                //  const vector< TSumItem > &items,
                //  const size_t upperBoundWeight,
                //  const vector< Integer > &maxSumMatrix,
                //  vector< size_t > &chosenIndexes);
            };

            template <typename Integer>
            Integer
                Privet::Algorithms::Numbers::SumToMinOne<Integer>
                ::Solve(
                    const std::vector< TSumItem >& items,
                    std::vector< TSubSequence >& chosenPartitions)
            {
                CheckInput(items);
                chosenPartitions.clear();

                std::vector< TSumItem > itemsWorkingCopy = items;

                std::sort(itemsWorkingCopy.begin(), itemsWorkingCopy.end());

                const Integer lowerBound = LowerBound(itemsWorkingCopy);
                const Integer upperBound = UpperBound(itemsWorkingCopy);

                if (lowerBound == upperBound)
                {
                    return lowerBound;
                }

                std::vector<std::vector<TSumItem>> possibleValue1Sequences;

                const size_t size = itemsWorkingCopy.size();
                std::vector< size_t > bigNumber(size, 0);

                for (;;)
                {
                    bigNumber[size - 1]++;

                    double d = 0;

                    if (1 <= d)
                    {
                        {
                            std::vector< TSumItem > sequence;
                            possibleValue1Sequences.push_back(sequence);
                        }

                        //vector< TSumItem > &sequence = possibleValue1Sequences[
                        //  possibleValue1Sequences.size() - 1];
                    }

                    break;
                }

                Integer result = 0;

                /*
                {
                  size_t index = 0;

                  do
                  {
                    TSumItem &item = itemsWorkingCopy[index];
                    if (item.Value <= item.Count)
                    {
                      const Integer toAdd = item.Count / item.Value;
                      const Integer reminder = item.Count % item.Value;

                      result += toAdd;

                      if (0 == reminder)
                      {//Precise match.
                        //TODO: p2. performance.
                        itemsWorkingCopy.erase (itemsWorkingCopy.begin() + index);

                        continue;
                      }

                      item.Count = reminder;
                    }

                    ++index;
                  } while (index < itemsWorkingCopy.size());
                }

                const size_t size = itemsWorkingCopy.size();
                if (size > 1)
                {
                  size_t index = 0;

                  double d = 0;
                  do
                  {
                    const TSumItem &item = itemsWorkingCopy[index];
                    const double addon = item.Count / ((double)(item.Value));
                    d += addon;
                    if (1.0 <= d)
                    {
                      ++result;
                      d = 0;
                    }
                  } while ((++index) < size);
                }*/

                ////itemsSize - rows, upperBoundWeight - columns.
                //vector< Integer > maxSumMatrix;
                //ComputeMatrix(items, upperBoundWeight, maxSumMatrix);

                //const Integer result = BacktrackResult(
                //  items, (size_t)upperBoundWeight, maxSumMatrix, chosenIndexes);
                return result;
            }

            template <typename Integer>
            void
                Privet::Algorithms::Numbers::SumToMinOne<Integer>
                ::CheckInput(const std::vector< TSumItem >& items)
            {
                const size_t itemsSize = items.size();
                if (0 == itemsSize)
                {
                    throw std::runtime_error("There must be at least one item.");
                }

                for (size_t i = 0; i < itemsSize; ++i)
                {
                    items[i].Validate();
                }
            }

            template <typename Integer>
            Integer
                Privet::Algorithms::Numbers::SumToMinOne<Integer>
                ::LowerBound(const std::vector< TSumItem >& items)
            {
                Integer result = 0;

                const size_t size = items.size();
                size_t index = 0;

                double d = 0;

                do
                {
                    const TSumItem& item = items[index];

                    const Integer toAdd = item.Count / item.Value;
                    const Integer reminder = item.Count % item.Value;

                    result += toAdd;

                    const double addon = reminder / static_cast<double>(item.Value);
                    d += addon;
                    if (1.0 <= d)
                    {
                        ++result;
                        d = 0;
                    }
                } while ((++index) < size);

                return result;
            }

            template <typename Integer>
            Integer
                Privet::Algorithms::Numbers::SumToMinOne<Integer>
                ::UpperBound(const std::vector< TSumItem >& items)
            {
                const size_t itemsSize = items.size();

                double sum = 0;

                for (size_t i = 0; i < itemsSize; ++i)
                {
                    sum += items[i].Count / double(items[i].Value);
                }

                Integer result = Integer(sum);
                return result;
            }
            /*
                  template <typename Integer,
                    typename Weight>
                    void
                    Privet::Algorithms::Numbers::SumToMinOne<Integer, Weight>
                    ::ComputeMatrix(
                    const vector< TSumItem > &items,
                    const Weight upperBoundWeight,
                    //itemsSize - rows, upperBoundWeight - columns.
                    vector< Integer > &maxSumMatrix)
                  {
                    const size_t itemsSize = items.size();
                    const size_t one = 1;
                    const size_t itemsSize_PlusOne = AddUnsigned(one, itemsSize);
                    const size_t upperBoundWeight_PlusOne = AddUnsigned(one, (size_t)upperBoundWeight);

                    const size_t bigSize = MultiplyUnsigned(
                      itemsSize_PlusOne,
                      upperBoundWeight_PlusOne,
                      (SIZE_MAX));

                    maxSumMatrix.resize(bigSize, 0);

                    for (size_t row = 0; row < itemsSize; ++row)
                    {
                      for (size_t column = 1; column < upperBoundWeight_PlusOne; ++column)
                      {
                        const size_t leftIndex = (row * upperBoundWeight_PlusOne) + column;
                        const size_t upLeftIndex = leftIndex - items[row].Weight;

                        const bool shallUpdate = items[row].Weight <= column
                          &&(maxSumMatrix[leftIndex] < items[row].Integer + maxSumMatrix[upLeftIndex]);

                        const size_t index = leftIndex + upperBoundWeight_PlusOne;

                        maxSumMatrix[index] = shallUpdate
                          ? items[row].Integer + maxSumMatrix[upLeftIndex]
                        : maxSumMatrix[leftIndex];
                      }
                    }
                  }

                  template <typename Integer,
                    typename Weight>
                    Integer
                    Privet::Algorithms::Numbers::SumToMinOne<Integer, Weight>
                    ::BacktrackResult(
                    const vector< TSumItem > &items,
                    const size_t upperBoundWeight,
                    //itemsSize - rows, upperBoundWeight - columns.
                    const vector< Integer > &maxSumMatrix,
                    vector< size_t > &chosenIndexes)
                  {
                    const size_t matrixSize = maxSumMatrix.size();
                    if (0 == matrixSize)
                    {
                      throw exception("Inner error. The matrixSize is zero.");
                    }

                    const Integer result = maxSumMatrix[matrixSize - 1];
                    if (result <= 0)
                    {
                      ostringstream ss;
                      ss << "Inner error. The resulting value (" << result
                        << ") must be positive.";
                      StreamUtilities::throw_exception(ss);
                    }

                    const size_t one = 1;
                    const size_t upperBoundWeight_PlusOne = AddUnsigned(one, (size_t)upperBoundWeight);
                    const size_t itemsSize = items.size();

                    size_t row = itemsSize - one;
                    size_t column = upperBoundWeight;
                    Integer rest = result;

                    for(;;)
                    {
                      const size_t leftIndex = (row * upperBoundWeight_PlusOne) + column;
                      const size_t upLeftIndex = leftIndex - items[row].Weight;

                      const size_t index = leftIndex + upperBoundWeight_PlusOne;

                      const bool shallInclude = items[row].Weight <= column
                        &&(maxSumMatrix[index] == items[row].Integer + maxSumMatrix[upLeftIndex]);

                      if (shallInclude)
                      {
                        chosenIndexes.push_back(row);

                        rest -= items[row].Integer;

                        if (0 == rest)
                        {
                          break;
                        }

                        if (column <= items[row].Weight)
                        {
                          ostringstream ss;
                          ss << "Inner error. Column (" << column
                            << ") must be greater than weight (" << items[row].Weight << ") at row"
                            << row
                            << ", rest=" << rest
                            << ", result=" << result
                            << ".";
                          StreamUtilities::throw_exception(ss);
                        }

                        column -= items[row].Weight;
                      }

                      if (0 == row)
                      {
                        ostringstream ss;
                        ss << "Inner error. Reached 0-th row whereas the remaining value is "
                          << rest
                          << ", result=" << result
                          << ".";
                        StreamUtilities::throw_exception(ss);
                      }

                      //Try next item.
                      --row;
                    }

                    reverse(chosenIndexes.begin(), chosenIndexes.end());

                    if (rest > 0)
                    {
                      ostringstream ss;
                      ss << "Inner error. The remaining value (" << rest
                        << ") must be zero. result=" << result << ".";
                      StreamUtilities::throw_exception(ss);
                    }

                    return result;
                  }*/
        }
    }
}