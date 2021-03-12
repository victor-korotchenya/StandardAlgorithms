#pragma once
#include <stack>
#include "../Utilities/ExceptionUtilities.h"
#include "../Utilities/StreamUtilities.h"
#include "../Utilities/VectorUtilities.h"

namespace Privet
{
    namespace Algorithms
    {
        namespace Numbers
        {
            //Robot starts at (0,0) of (rows*columns) grid.
            //It can move one cell to {left, right, up, down}
            // when sum of digits of row and column indexes
            // is less or equal to than digitSumLimitInclusive.
            //
            //Input: positive integers: rows, columns, digitSumLimitInclusive.
            //Output: Number of cells reachable from (0,0).
            template <typename Number = unsigned int >
            class DigitSum_RobotInGrid final
            {
                DigitSum_RobotInGrid() = delete;

            public:

                static Number Slow(
                    const Number& rows,
                    const Number& columns,
                    const Number& digitSumLimitInclusive);

                static Number Fast(
                    const Number& rows,
                    const Number& columns,
                    const Number& digitSumLimitInclusive);

                //Sum of decimal digits of "value".
                static Number DigitSum(const Number& value);

                //Number of decimal digits.
                static Number DigitCount(const Number& positiveValue);

                //Maximum possible "digit sum".
                //E.g. for 10 it is 9, 20 - 10, 28 - 10, 29 - 11.
                static Number MaxDigitSum(const Number& positiveValue);

            private:

                static const char Empty = char(0);
                static const char Potential = char(1);
                static const char Counted = char(2);

                //Given n, returns 10^n.
                //E.g. for 1 returns 10, 2 - 100, 6 - 1000000.
                static Number Exp(const Number& positiveValue);

                static void CheckResult(
                    const Number& rows,
                    const Number& columns,
                    const Number& digitSumLimitInclusive,
                    const Number& result);
            };

            template <typename Number >
            Number DigitSum_RobotInGrid< Number >::Slow(
                const Number& rows, const Number& columns,
                const Number& digitSumLimitInclusive)
            {
                RequirePositive(rows, "rows");
                RequirePositive(columns, "columns");
                RequirePositive(digitSumLimitInclusive, "digitSumLimitInclusive");

                const Number zero = Number(0);
                const Number one = Number(1);
                const Number two = Number(2);

                //Add 2 along sides to avoid extra tests later.
                std::vector<std::vector<char>> data(rows + two, std::vector<char>(columns + two, Empty));

                Number row = zero;
                do
                {
                    const Number rowSum = DigitSum(row);
                    Number column = zero;
                    do
                    {
                        const Number columnSum = DigitSum(column);
                        if (rowSum + columnSum <= digitSumLimitInclusive)
                        {
                            data[row + one][column + one] = Potential;
                        }
                    } while (++column < columns);
                } while (++row < rows);

                using TPair = std::pair< Number, Number >;

                TPair candinates[4];
                const int candinatesSize = sizeof(candinates) / sizeof(TPair);

                std::stack< TPair > s;
                s.push(std::make_pair(one, one));
                data[one][one] = Counted;

                Number result = one;
                do
                {
                    const TPair& top = s.top();

                    candinates[0].first = top.first - one;
                    candinates[0].second = top.second;

                    candinates[1].first = top.first + one;
                    candinates[1].second = top.second;

                    candinates[2].first = top.first;
                    candinates[2].second = top.second - one;

                    candinates[3].first = top.first;
                    candinates[3].second = top.second + one;

                    s.pop();

                    for (int index = 0; index < candinatesSize; ++index)
                    {
                        const TPair& candinate = candinates[index];
                        if (Potential == data[candinate.first][candinate.second])
                        {//Enqueue only once.
                            data[candinate.first][candinate.second] = Counted;
                            s.push(std::make_pair(candinate.first, candinate.second));
                            ++result;
                        }
                    }
                } while (!s.empty());

                CheckResult(rows, columns, digitSumLimitInclusive, result);
                return result;
            }

            template <typename Number >
            Number DigitSum_RobotInGrid< Number >::Fast(
                const Number& rows, const Number& columns,
                const Number& digitSumLimitInclusive)
            {
                RequirePositive(rows, "rows");
                RequirePositive(columns, "columns");
                RequirePositive(digitSumLimitInclusive, "digitSumLimitInclusive");

                const Number rowsMaxSum = MaxDigitSum(rows);
                if (rowsMaxSum < digitSumLimitInclusive)
                {
                    const Number columnsMaxSum = rows == columns
                        ? rowsMaxSum : MaxDigitSum(columns);
                    if (rowsMaxSum + columnsMaxSum <= digitSumLimitInclusive)
                    {//Match everything.
                        const Number result = rows * columns;
                        return result;
                    }
                }

                const Number zero = Number(0);
                Number result = zero;

                //TODO: p3 finish fast...
                ++result;

                CheckResult(rows, columns, digitSumLimitInclusive, result);
                return result;
            }

            template <typename Number >
            Number DigitSum_RobotInGrid< Number >::MaxDigitSum(
                const Number& positiveValue)
            {
#ifdef _DEBUG
                RequirePositive(positiveValue, "MaxDigitSum");
#endif
                const Number ten = Number(10);
                if (positiveValue < ten)
                {
                    return positiveValue;
                }

                const Number one = Number(1);
                const Number digitCount = DigitCount(positiveValue);
                const Number oneAndZeros = Exp(digitCount - one);
                const Number leadDigit = positiveValue / oneAndZeros;
                const Number leadDigitAndNines = (leadDigit + one) * oneAndZeros - one;
                const bool areAllLowerNine = positiveValue == leadDigitAndNines;

                Number result = Number(9) * (digitCount - one) + leadDigit;
                if (!areAllLowerNine)
                {
                    result -= one;
                }

#ifdef _DEBUG
                RequirePositive(result, "MaxDigitSumResult");
#endif
                return result;
            }

            template <typename Number >
            Number DigitSum_RobotInGrid< Number >::Exp(const Number& positiveValue)
            {
#ifdef _DEBUG
                RequirePositive(positiveValue, "Exp");
#endif
                const Number zero = Number(0);
                const Number ten = Number(10);

                Number temp = positiveValue;
                Number result = ten;

                //TODO: p2 use faster * method.
                while (zero < --temp)
                {
                    result *= ten;
                }

#ifdef _DEBUG
                RequirePositive(positiveValue, "ExpResult");
#endif
                return result;
            }

            template <typename Number >
            Number DigitSum_RobotInGrid< Number >::DigitSum(
                const Number& value)
            {
                const Number zero = Number(0);
                const Number ten = Number(10);

                Number result = zero;
                Number temp = value;

                while (zero < temp)
                {
                    result += temp % ten;
                    temp = temp / ten;
                }

                return result;
            }

            template <typename Number >
            Number DigitSum_RobotInGrid< Number >::DigitCount(
                const Number& positiveValue)
            {
#ifdef _DEBUG
                RequirePositive(positiveValue, "DigitCount");
#endif
                const Number ten = Number(10);
                const Number one = Number(1);
                const Number zero = Number(0);

                Number result = one, temp = positiveValue / ten;

                while (zero < temp)
                {
                    temp = temp / ten;
                    ++result;
                }

#ifdef _DEBUG
                RequirePositive(positiveValue, "DigitCountResult");
#endif
                return result;
            }

            template <typename Number >
            void DigitSum_RobotInGrid< Number >::CheckResult(
                const Number& rows, const Number& columns,
                const Number& digitSumLimitInclusive,
                const Number& result)
            {
#ifdef _DEBUG
                RequirePositive(result, "CheckResult");
#endif
                const Number achievableResult = rows * columns;
                if (achievableResult < result)
                {
                    std::ostringstream ss;
                    ss << "Possible overflow: result (" << result << ") for input ("
                        << rows << ", "
                        << columns << ", "
                        << digitSumLimitInclusive
                        << ") must not exceed (rows * columns)="
                        << achievableResult << ".";
                    StreamUtilities::throw_exception(ss);
                }
            }
        }
    }
}