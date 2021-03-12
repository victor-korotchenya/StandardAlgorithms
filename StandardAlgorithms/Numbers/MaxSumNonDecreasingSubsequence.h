#pragma once

#include <vector>
#include <tuple>
#include "../Utilities/ExceptionUtilities.h"
#include "../Utilities/VectorUtilities.h"
#include "BinarySearch.h"

namespace Privet::Algorithms::Numbers
{
    //Given an array "values" of n>0 positive integers,
    //returns the array "result" of n integers,
    //such that the total sum of numbers in the "result" is maximized,
    // and for each index "i" in the range [0, n-1]
    //  the following constraint holds: 0 <= result[i] <= values[i];
    // and all positive numbers in the "result" form non-decreasing subsequence.
    //
    //For example, given { 3, 5 }, the result will be { 3, 5 }
    //    as the result is non-decreasing, and cannot be beaten when everything is included.
    // { 5, 3 } returns { 3, 3 } because (3+3) is greater than (5+0).
    //    Note, that { 5, 3 } is not a valid result because (5 <= 3) does not hold.
    //    Also, { 5, 5 } is invalid as the second 5 must not exceed 3.
    // { 5, 2 } returns { 5, 0 } because (2+2) is smaller than (5+0).
    // { 5, 2, 7 } returns { 5, 0, 7 } because (5+0+7) is greater than (2+2+7) for { 2,2,7 }.
    //
    // Here is a longer example: { 40, 20, 30, 10, 20, 50, 30, 100, 20 },
    // and the result: { 20, 20, 20, 0, 20, 30, 30, 100, 0 });
    template <typename Number = unsigned int,
        //To avoid overflows.
        typename LongNumber = unsigned long long int>
        class MaxSumNonDecreasingSubsequence final
    {
        MaxSumNonDecreasingSubsequence() = delete;

        static_assert(sizeof(Number) <= sizeof(LongNumber),
            "LongNumber must not be smaller than Number.");

        //Max_sum_value, previous_row, previous column.
        using record_t = std::tuple<LongNumber, size_t, size_t>;
        static constexpr int ValueId = 0;
        static constexpr int RowId = 1;
        static constexpr int ColumnId = 2;

        using matrix_t = std::vector<std::vector<record_t>>;

        //matrix, best_sum, best_row(index), best_column(height).
        using computed_tuple = std::tuple<matrix_t, LongNumber, size_t, size_t>;

    public:

        //Time complexity is O(n*n*n).
        static std::vector<Number> Compute(const std::vector<Number>& values)
        {
            ThrowIfEmpty(values, "values");
            RequireAllPositive<Number>(values, "values");

            const auto size = values.size();
            if (1 == size)
                return values;

            auto uniqueValues = values;
            VectorUtilities::sort_remove_duplicates(uniqueValues);

            const auto computed = ComputeMatrix(values, uniqueValues);
            const auto result = RestoreResult(size, uniqueValues, computed);
            return result;
        }

    private:
        //Time complexity is O(log(n)).
        template <typename T>
        static size_t GetIndex(const std::vector<T>& sortedValues, const T& value)
        {
            const auto size = sortedValues.size();
            const auto data = sortedValues.data();
            const auto endInclusive = data + size - 1;

            const auto found = binary_search_inclusive(data, endInclusive, value);

            const size_t index = found - data;
            if (!found || size <= index)
            {
                std::ostringstream ss;
                ss << "Error in GetIndex: size (" << size
                    << ") <= index (" << index << ")"
                    << ", value = " << value << ".";
                StreamUtilities::throw_exception(ss);
            }

            return index;
        }

        //A cell in the "matrix" will contain the maximum total sum value for the given value and height,
        // and the pointer to one of the best previous cells
        // which is to be used to backtrack the resulting array.
        static computed_tuple ComputeMatrix(
            const std::vector<Number>& values,
            const std::vector<Number>& uniqueValues)
        {
            const auto size = values.size();

            std::vector<std::vector<record_t>> matrix(size);
            LongNumber bestSum = 0;
            size_t bestIndex = 0, bestHeight = 0;

            for (size_t index = 0; index < size; ++index)
            {
                const size_t valueIndex = GetIndex(uniqueValues, values[index]);
                const auto rowResult = ComputeRow(uniqueValues, index, valueIndex, matrix);
                if (bestSum < std::get<ValueId>(rowResult))
                {
                    bestSum = std::get<ValueId>(rowResult);
                    bestIndex = std::get<RowId>(rowResult);
                    bestHeight = std::get<ColumnId>(rowResult);
                }
            }

            return computed_tuple(std::move(matrix), bestSum, bestIndex, bestHeight);
        }

        static record_t ComputeRow(
            const std::vector<Number>& uniqueValues,
            const size_t index,
            const size_t valueIndex,
            std::vector<std::vector<record_t>>& matrix)
        {
            const auto uniqueValuesSize = uniqueValues.size();

            std::vector<record_t>& row = matrix[index];
            row.resize(uniqueValues.size());

            LongNumber bestSum = 0;
            size_t bestIndex = 0, bestHeight = 0;

            for (size_t height = 0; height < uniqueValuesSize; ++height)
            {
                const auto isWithin = height <= valueIndex;
                const LongNumber currentValue = uniqueValues[isWithin ? height : valueIndex];
                if (0 == index)
                {//No previous value.
                    row[height] = record_t(currentValue, 0, 0);
                }
                else
                {//Previous value exists.
                    const std::vector<record_t>& previousRow = matrix[index - 1];
                    const auto previousValue = std::get<ValueId>(previousRow[height]);

                    row[height] = isWithin
                        ? record_t(
                            //TODO: p3. There is no overflow check.
                            previousValue + currentValue, index - 1, height)
                        : previousRow[height];
                    if (isWithin && 0 < height)
                        TrySmallerHeight(currentValue, index, height, previousRow, row);
                }

                if (bestSum < std::get<ValueId>(row[height]))
                {
                    bestSum = std::get<ValueId>(row[height]);
                    bestIndex = index;
                    bestHeight = height;
                }
            }

            return record_t(bestSum, bestIndex, bestHeight);
        }

        static void TrySmallerHeight(
            const LongNumber& currentValue,
            const size_t index,
            const size_t height,
            const std::vector<record_t>& previousRow,
            std::vector<record_t>& row)
        {
            auto height2 = height - 1;
            for (;;)
            {
                const auto temp = //TODO: p3. There is no overflow check.
                    std::get<ValueId>(previousRow[height2]) + currentValue;
                if (std::get<ValueId>(row[height]) < temp)
                {
                    row[height] = record_t(temp,
                        index - 1,
                        height2);
                }

                if (0 == height2--)
                    break;
            }
        }

        static std::vector<Number> RestoreResult(
            const size_t size,
            const std::vector<Number>& uniqueValues,
            const computed_tuple& computed)
        {
            std::vector<Number> result(size);

            const std::vector<std::vector<record_t>>& matrix = std::get<0>(computed);
            LongNumber reminder = std::get<1>(computed);
            //It has been computed where is the first best matrix cell.
            size_t index = std::get<2>(computed);
            size_t height = std::get<3>(computed);

            for (;;)
            {
                result[index] = uniqueValues[height];
#ifdef _DEBUG
                if (reminder < uniqueValues[height])
                {
                    std::ostringstream ss;
                    ss << "Error in RestoreResult: reminder("
                        << reminder << ") < "
                        << uniqueValues[height] << "=uniqueValues[" << height << "].";
                    StreamUtilities::throw_exception(ss);
                }
#endif
                reminder -= uniqueValues[height];
                if (LongNumber(0) == reminder)
                    break;

                MoveToPreviousRowColumn(matrix[index][height], index, height);
                MoveToEdgeValue(matrix, reminder, index, height);
            }

            if (reminder)
            {
                std::ostringstream ss;
                ss << "The reminder(" << reminder << ") must be zero.";
                StreamUtilities::throw_exception(ss);
            }

            return result;
        }

        static void MoveToPreviousRowColumn(
            const record_t& record,
            size_t& index,
            size_t& height)
        {
            const size_t newIndex = std::get<RowId>(record);
            const size_t newHeight = std::get<ColumnId>(record);
            if (index <= newIndex)
            {//The index must decrease.
                std::ostringstream ss;
                ss << "Error in MoveToPreviousRowColumn: index(" << index
                    << ") <= newIndex(" << newIndex << ").";
                StreamUtilities::throw_exception(ss);
            }

            if (height < newHeight)
            {
                std::ostringstream ss;
                ss << "Error in MoveToPreviousRowColumn: height(" << height
                    << ") < newHeight(" << newHeight << ").";
                StreamUtilities::throw_exception(ss);
            }

            index = newIndex;
            height = newHeight;
        }

        //Find the reminder cell having the lowest row/column.
        static void MoveToEdgeValue(
            const std::vector<std::vector<record_t>>& matrix,
            const LongNumber& reminder,
            size_t& index,
            size_t& height)
        {
            for (;;)
            {
                auto shallStop = true;

                while (0 < index && std::get<ValueId>(matrix[index - 1][height]) == reminder)
                {
                    --index;
                    shallStop = false;
                }

                while (0 < height && std::get<ValueId>(matrix[index][height - 1]) == reminder)
                {
                    --height;
                    shallStop = false;
                }

                if (shallStop)
                    break;
            }
        }
    };
}