#pragma once
#include <utility>
#include "../Utilities/StreamUtilities.h"
#include "../Utilities/ExceptionUtilities.h"
#include "../Utilities/VectorUtilities.h"

namespace Privet
{
    namespace Algorithms
    {
        namespace Numbers
        {
            //Given a set of n>0 coins, each having >0 value,
            // you make the first step meaning either left-most or right-most coin is opened;
            // Then the enemy makes his step; and so on until there are coins.
            //The aim is to maximize your total value.
            template <typename Number>
            class CoinLineMaxSum final
            {
                CoinLineMaxSum() = delete;

            public:
                //It is slow.
                static Number MinGuaranteedValue(const std::vector<Number>& values);

                static Number MinGuaranteedValue_Fast(const std::vector<Number>& values);

            private:
                using TBuffer = std::vector<std::vector<std::pair<Number, Number>>>;

                static void ValidateAndSetDiagonal(
                    const std::vector<Number>& values,
                    TBuffer& buffer);
            };

            template <typename Number>
            Number CoinLineMaxSum<Number>::MinGuaranteedValue(const std::vector<Number>& values)
            {
                ThrowIfEmpty(values, "values");

                const auto size = values.size();
                TBuffer buffer(size);

                //5,  8,  2       given, sum=15
                // 5/8  2/8       our min/max
                //   8/7          result=7

                // a/b              c/d
                //   min(b,d) / sum - min(b,d)

                //8,  2,  1       given, sum=11
                // 2/8  1/2       our min/max
                //   2/9          result=9
                //
                //5,  8,  2,  1   given, sum=16
                // 5/8  2/8  1/2  our min/max
                //   8/7  2/9
                //     7/9        they = (sum-result)=7, result = max(5+2, 1+8)=9.
                //
                //5,  8,  2,  1,   4   given, sum=20
                // 5/8  2/8  1/2  1/4  our min/max
                //   8/7  2/9  2/5
                //     7/9  5/10
                //      9/11           they = (sum-result)=9, result = max(5+5, 4+7)=11.
                ValidateAndSetDiagonal(values, buffer);

                Number sum = values[0];

                for (size_t delta = 1; delta < size; ++delta)
                {
                    sum += values[delta];

                    const size_t newSize = size - delta;
                    buffer[delta].resize(newSize);

                    for (size_t startIndex = 0; startIndex < newSize; ++startIndex)
                    {
                        const Number left = values[startIndex] + buffer[delta - 1][startIndex + 1].first;
                        const Number right = values[startIndex + delta] + buffer[delta - 1][startIndex].first;

                        const Number tMax = std::max(left, right);

                        const Number regionSum = buffer[delta - 1][startIndex + 1].first
                            + buffer[delta - 1][startIndex + 1].second;

                        const Number tMin = values[startIndex]
                            + regionSum
                            - tMax;

                        buffer[delta][startIndex] = std::make_pair(tMin, tMax);
                    }
                }

                const Number result = sum - buffer[size - 1][0].first;
                const Number resultTwo = buffer[size - 1][0].second;
                if (resultTwo != result)
                {
                    std::ostringstream ss;
                    ss << "There might have been an overflow: resultTwo(" << resultTwo
                        << ") != result (" << result << ") for size=" << size << ".";
                    StreamUtilities::throw_exception(ss);
                }

                return result;
            }

            template <typename Number>
            Number CoinLineMaxSum<Number>::MinGuaranteedValue_Fast(const std::vector<Number>& values)
            {
                ThrowIfEmpty(values, "values");

                const auto size = values.size();
                const auto isOdd = 0 != (size & size_t(1));

                {
                    TBuffer bufferDel(size);
                    //TODO: p4. only validate.
                    ValidateAndSetDiagonal(values, bufferDel);
                }

                if (1 == size)
                    return values[0];

                //Assuming the player 2 plays efficiently,
                //the buffer[x][y] is the first player's maximum for coins from x to y.
                std::vector<std::vector<Number>> buffer(size - 1, std::vector<Number>(size));

                Number sum = values.back();

                for (size_t i = 0; i < size - 1; ++i)
                {//delta = 1.
                    buffer[i][i + 1] = std::max(values[i], values[i + 1]);
                    sum += values[i];
                }

                for (size_t delta = 3; delta <= size; delta += 2)
                {
                    for (size_t x = 0, y = delta; y < size; ++x, ++y)
                    {
                        //[x][y - 2]                  [x][y]
                        //            [x + 1][y - 1]
                        //                            [x + 2][y]
                        const Number bottom = values[x] + std::min(
                            //Enemy chooses the next after our choice.
                            buffer[x + 2][y],
                            // ...          before y.
                            buffer[x + 1][y - 1]);

                        const Number left = values[y] + std::min(
                            //Enemy chooses the next after x.
                            buffer[x + 1][y - 1],
                            // ...          before our choice.
                            buffer[x][y - 2]);

                        buffer[x][y] = std::max(
                            //We select either the x.
                            bottom,
                            // or the y.
                            left);
                    }
                }

                if (isOdd)
                {
                    const Number& enemyValue1 = buffer[1][size - 1];
                    const Number& enemyValue2 = buffer[0][size - 2];

                    const Number result = std::max(sum - enemyValue1, sum - enemyValue2);
                    return result;
                }

                const auto& result = buffer[0][size - 1];
                return result;
            }

            template <typename Number>
            void CoinLineMaxSum<Number>::ValidateAndSetDiagonal(
                const std::vector<Number>& values,
                TBuffer& buffer)
            {
                const size_t size = values.size();
                buffer[0].resize(size);

                for (size_t i = 0; i < size; ++i)
                {
                    if (values[i] <= Number(0))
                    {
                        std::ostringstream ss;
                        ss << "Error: item(" << values[i]
                            << ") at index=" << i << " must be positive.";
                        StreamUtilities::throw_exception(ss);
                    }

                    //TODO: p2. avoid copying.
                    buffer[0][i] = std::make_pair(0, values[i]);
                }
            }
        }
    }
}