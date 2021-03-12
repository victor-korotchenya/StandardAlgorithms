#pragma once

#ifdef _OPENMP
#include <omp.h>
#endif

#include <utility> // pair

#ifdef _DEBUG
#include "Arithmetic.h"
#endif

#include "Sort.h" // Min3
#include "../Geometry/rectangle2d.h"
#include "../Geometry/point.h"
#include "../Utilities/StreamUtilities.h"
#include "GreatestRectangleInHistogram.h"
#include "../Utilities/VectorUtilities.h"

namespace Privet::Algorithms::Numbers
{
    //2D boolean matrix data[m][n], not necessary squared, is given.
    //1) Find max square sub-matrix having all true values.
    //2) Find max area sub-matrix of all true values.
    template <typename TSize = unsigned int,
        typename TSizeSquared = size_t,
        typename TPoint = Geometry::point2d< TSize >,
        typename TRectangle = Geometry::rectangle2d< TSize, TSizeSquared, TPoint > >
        class MaxSubMatrixIn2DMatrix final
    {
        MaxSubMatrixIn2DMatrix() = delete;

    public:

        //TODO: p2. It is incorrect.
        static TRectangle MaxArea(
            const std::vector< std::vector< bool > >& data);

        static TRectangle MaxAreaMethod2(
            const std::vector< std::vector< bool > >& data);

        //The returned matrix will be squared.
        static TRectangle MaxSquare(
            const std::vector< std::vector< bool > >& data);

        //Slow method.
        //
        //The returned matrix contains the maximum available sum.
        //The "TNumber" is to be signed, or it would be just the whole matrix.
        template <typename TNumber, typename TNumberSquared >
        static TNumberSquared MaxSum(
            const std::vector< std::vector< TNumber > >& data,
            TRectangle& resultRectangle);

        //The returned matrix contains the maximum available sum.
        //The "TNumber" is to be signed, or it would be just the whole matrix.
        template <typename TNumber, typename TNumberSquared >
        static TNumberSquared MaxSumFast(
            const std::vector< std::vector< TNumber > >& data,
            TRectangle& resultRectangle);

    private:

        template <typename TNumberSquared >
        static TNumberSquared ComputeMaxSumFast(
            const std::vector< std::vector< TNumberSquared > >& sums,
            TRectangle& resultRectangle);
    };

    template <typename TSize, typename TSizeSquared,
        typename TPoint, typename TRectangle >
        TRectangle MaxSubMatrixIn2DMatrix< TSize, TSizeSquared, TPoint, TRectangle >::MaxArea(
            const std::vector< std::vector< bool > >& data)
    {
        //This code is wrong.

        const size_t sizeRaw = data.size();
        if (0 == sizeRaw)
        {
            throw std::runtime_error("0 == size");
        }

        VectorUtilities::IsMatrix< bool, std::vector< bool > >(data);

        const TSize size = TSize(sizeRaw);
        if (size != sizeRaw)
        {
            std::ostringstream ss;
            ss << "Overflow: size (" << sizeRaw << ") as TSize is " << size << ".";
            StreamUtilities::throw_exception(ss);
        }

        const size_t rowSizeRaw = data[0].size();
        const TSize rowSize = TSize(rowSizeRaw);
        if (rowSize != rowSizeRaw)
        {
            std::ostringstream ss;
            ss << "Overflow: row size (" << rowSizeRaw << ") as TSize is "
                << rowSize << ".";
            StreamUtilities::throw_exception(ss);
        }

        const TSize rowSizePlusOne = rowSize + TSize(1);
        if (rowSizePlusOne <= rowSize)
        {
            std::ostringstream ss;
            ss << "Overflow: rowSizePlusOne (" << rowSizePlusOne <<
                ") must be greater than rowSize (" << rowSize << ").";
            StreamUtilities::throw_exception(ss);
        }

        //One can go left (first) or go up (second).
        using TLeftUpPair = std::pair< TRectangle, TRectangle >;
        const TLeftUpPair zeroPair = TLeftUpPair(TRectangle(), TRectangle());

        //Set zero-sized rectangle.
        TRectangle result = TRectangle();

        TSizeSquared maxArea = TSizeSquared(0);

        //Index starts with 1.
        std::vector< TLeftUpPair > previousMatrix, currentMatrix;

        //Row 0 and column 0 will be zeroes.
        previousMatrix.resize(rowSizePlusOne);
        currentMatrix.resize(rowSizePlusOne);

        for (TSize row = TSize(0); row < size; ++row)
        {
            const std::vector< bool >& dataRow = data[row];

            for (TSize column = TSize(0); column < rowSize; ++column)
            {
                if (dataRow[column])
                {
                    const TRectangle& leftRectangle = currentMatrix[column].first;
                    const TRectangle& upRectangle = previousMatrix[column + 1].second;

                    const TSize leftHeight = leftRectangle.Height();
                    const TSize leftWidth = leftRectangle.Width();

                    const TSize upHeight = upRectangle.Height();
                    const TSize upWidth = upRectangle.Width();

#ifdef _DEBUG
                    if (TSize(0) == leftHeight && TSize(0) != leftWidth
                        || TSize(0) != leftHeight && TSize(0) == leftWidth)
                    {
                        std::ostringstream ss;
                        ss << "Both must be 0, but leftHeight=" << leftHeight
                            << ", leftWidth=" << leftWidth << ".";
                        StreamUtilities::throw_exception(ss);
                    }

                    if (TSize(0) == upHeight && TSize(0) != upWidth
                        || TSize(0) != upHeight && TSize(0) == upWidth)
                    {
                        std::ostringstream ss;
                        ss << "Both must be 0, but upHeight=" << upHeight
                            << ", upWidth=" << upWidth << ".";
                        StreamUtilities::throw_exception(ss);
                    }
#endif
                    //Go left.
                    const TSize height1 = TSize(0) == leftHeight
                        ? upHeight + 1
                        : min(leftHeight, upHeight + 1);
                    const TSize width1 = leftWidth + 1;
                    const TSizeSquared tempResult1 = TSizeSquared(height1) * TSizeSquared(width1);

                    const TPoint lastPoint = TPoint(row + 1, column + 1);

                    const TRectangle bestLeft = TRectangle(
                        TPoint(row + 1 - height1, column + 1 - width1),
                        lastPoint);

                    if (maxArea < tempResult1)
                    {
                        result = bestLeft;
                        maxArea = tempResult1;
                    }

                    //Go up.
                    const TSize height2 = upHeight + 1;
                    const TSize width2 = TSize(0) == upWidth
                        ? leftWidth + 1
                        : min(upWidth, leftWidth + 1);
                    const TSizeSquared tempResult2 = TSizeSquared(height2) * TSizeSquared(width2);

                    const TRectangle bestUp = TRectangle(
                        TPoint(row + 1 - height2, column + 1 - width2),
                        lastPoint);

                    if (maxArea < tempResult2)
                    {
                        result = bestUp;
                        maxArea = tempResult2;
                    }

                    currentMatrix[column + 1] = TLeftUpPair(bestLeft, bestUp);
                }
                else
                {
                    currentMatrix[column + 1] = zeroPair;
                }
            }

            swap(previousMatrix, currentMatrix);
        }

        return result;
    }

    template <typename TSize, typename TSizeSquared,
        typename TPoint, typename TRectangle >
        TRectangle MaxSubMatrixIn2DMatrix< TSize, TSizeSquared, TPoint, TRectangle >::
        MaxAreaMethod2(
            const std::vector< std::vector< bool > >& data)
    {
        const size_t size = data.size();
        if (0 == size)
        {
            throw std::runtime_error("0 == size");
        }

        const size_t rowSize = data[0].size();

        VectorUtilities::IsMatrix< bool, std::vector< bool > >(data);

        TRectangle result = TRectangle();
        TSizeSquared maxArea = TSizeSquared(0);

        std::vector< TSize > current(rowSize, TSize(0));

        for (size_t row = 0; row < size; ++row)
        {
            const std::vector< bool >& dataRow = data[row];

            for (size_t column = 0; column < rowSize; ++column)
            {
                current[column] = dataRow[column] ? (current[column] + 1) : 0;
            }

            size_t startIndex = 0;
            TSize startHeigth = 0;
            size_t startWidth = 0;

            const bool shallComputeStartIndex = true;

            const TSizeSquared tempArea = GreatestRectangleInHistogram< TSize, TSizeSquared >
                ::ComputeFast(current, startIndex, startHeigth, startWidth,
                    shallComputeStartIndex);

            if (maxArea < tempArea)
            {
                const TPoint point1 = TPoint(TSize(row + 1 - startHeigth), TSize(startIndex));
                const TPoint point2 = TPoint(TSize(row + 1), TSize(startIndex + startWidth));

                result = TRectangle(point1, point2);
                maxArea = tempArea;
            }
        }

        return result;
    }

    template <typename TSize, typename TSizeSquared,
        typename TPoint, typename TRectangle >
        TRectangle MaxSubMatrixIn2DMatrix< TSize, TSizeSquared, TPoint, TRectangle >::MaxSquare(
            const std::vector< std::vector< bool > >& data)
    {
        const size_t sizeRaw = data.size();
        if (0 == sizeRaw)
        {
            throw std::runtime_error("0 == size");
        }

        VectorUtilities::IsMatrix< bool, std::vector< bool > >(data);

        const TSize size = TSize(sizeRaw);
        if (size != sizeRaw)
        {
            std::ostringstream ss;
            ss << "Overflow: size (" << sizeRaw << ") as TSize is " << size << ".";
            StreamUtilities::throw_exception(ss);
        }

        const size_t rowSizeRaw = data[0].size();
        const TSize rowSize = TSize(rowSizeRaw);
        if (rowSize != rowSizeRaw)
        {
            std::ostringstream ss;
            ss << "Overflow: row size (" << rowSizeRaw << ") as TSize is "
                << rowSize << ".";
            StreamUtilities::throw_exception(ss);
        }

        const TSize rowSizePlusOne = rowSize + TSize(1);
        if (rowSizePlusOne <= rowSize)
        {
            std::ostringstream ss;
            ss << "Overflow: rowSizePlusOne (" << rowSizePlusOne <<
                ") must be greater than rowSize (" << rowSize << ").";
            StreamUtilities::throw_exception(ss);
        }

        //Set zero-sized rectangle.
        TRectangle result = TRectangle();
        TSize maxSide = TSize(0);

        //Index starts with 1.
        std::vector< TSize > previousMatrix, currentMatrix;

        //Row 0 and column 0 will be zeros.
        previousMatrix.resize(rowSizePlusOne, TSize(0));
        currentMatrix.resize(rowSizePlusOne, TSize(0));

        for (TSize row = TSize(0); row < size; ++row)
        {
            const std::vector< bool >& dataRow = data[row];

            for (TSize column = TSize(0); column < rowSize; ++column)
            {
                if (dataRow[column])
                {
                    const TSize& upLeft = previousMatrix[column];
                    const TSize& up = previousMatrix[column + 1ll];
                    const TSize& left = currentMatrix[column];
                    const TSize side = Min3(upLeft, up, left) + 1;

                    if (maxSide < side)
                    {
                        result = TRectangle(
                            TPoint(row + 1 - side, column + 1 - side),
                            TPoint(row + 1, column + 1));
                        maxSide = side;
                    }

                    currentMatrix[column + 1ll] = side;
                }
                else
                {
                    currentMatrix[column + 1ll] = 0;
                }
            }

            swap(previousMatrix, currentMatrix);
        }

        return result;
    }

    template <typename TSize, typename TSizeSquared,
        typename TPoint, typename TRectangle >
        template <typename TNumber, typename TNumberSquared >
    TNumberSquared MaxSubMatrixIn2DMatrix< TSize, TSizeSquared, TPoint, TRectangle >
        ::MaxSum(const std::vector< std::vector< TNumber > >& data, TRectangle& resultRectangle)
    {
        static_assert(sizeof(TNumber) <= sizeof(TNumberSquared));

#ifdef _DEBUG
        AssertSigned< TNumber >("MaxSum");
#endif
        const size_t size = data.size();
        if (0 == size)
        {
            throw std::runtime_error("0 == size");
        }

        VectorUtilities::IsMatrix< bool, std::vector< TNumber > >(data);

        const size_t rowSize = data[0].size();

        resultRectangle = TRectangle();
        TNumberSquared result = TNumberSquared(0);

        //The left top point.
        for (size_t startRow = 0; startRow < size; ++startRow)
        {
            for (size_t startColumn = 0; startColumn < rowSize; ++startColumn)
            {
                //The right buttom point.
                for (size_t stopRow = startRow; stopRow < size; ++stopRow)
                {
                    for (size_t stopColumn = startColumn; stopColumn < rowSize; ++stopColumn)
                    {
                        TNumberSquared temp = TNumberSquared(0);

                        //Calc result sum.
                        for (size_t i = startRow; i <= stopRow; ++i)
                        {
                            for (size_t j = startColumn; j <= stopColumn; ++j)
                            {
                                temp += data[i][j];
                            }
                        }

                        if (result < temp)
                        {
                            resultRectangle = TRectangle(
                                TPoint(TSize(startRow), TSize(startColumn)),
                                TPoint(TSize(stopRow + 1), TSize(stopColumn + 1)));

                            result = temp;
                        }
                    }
                }
            }
        }

        return result;
    }

    template <typename TSize, typename TSizeSquared,
        typename TPoint, typename TRectangle >
        template <typename TNumber, typename TNumberSquared >
    TNumberSquared MaxSubMatrixIn2DMatrix< TSize, TSizeSquared, TPoint, TRectangle >
        ::MaxSumFast(const std::vector< std::vector< TNumber > >& data, TRectangle& resultRectangle)
    {
        static_assert(sizeof(TNumber) <= sizeof(TNumberSquared),
            "sizeof(TNumber) <= sizeof(TNumberSquared)");

#ifdef _DEBUG
        AssertSigned< TNumber >("MaxSum");
#endif
        const size_t size = data.size();
        if (0 == size)
        {
            throw std::runtime_error("0 == size");
        }

        VectorUtilities::IsMatrix< bool, std::vector< TNumber > >(data);

        //The first row consists entirely from zeros.
        std::vector< std::vector< TNumberSquared > > sums;

        //TODO: p3. Having 100*2 matrix, it would be 50 times faster to sum by rows.
        const bool includeFirstZeroRow = true;
        VectorUtilities::SumVerticalMatrices(data, sums, includeFirstZeroRow);

        const TNumberSquared result = ComputeMaxSumFast< TNumberSquared >(
            sums, resultRectangle);
        return result;
    }

    template <typename TSize, typename TSizeSquared,
        typename TPoint, typename TRectangle >
        template <typename TNumberSquared >
    TNumberSquared MaxSubMatrixIn2DMatrix< TSize, TSizeSquared, TPoint, TRectangle >
        ::ComputeMaxSumFast(
            const std::vector< std::vector< TNumberSquared > >& sums,
            TRectangle& resultRectangle)
    {
        //OMP by VC cannot handle unsigned ints, phew.
        using OMPSignedNumber = long long int;

        //In the sums, the first row has zeros only.
        const OMPSignedNumber sizeRaw = OMPSignedNumber(sums.size());
        if (sizeRaw <= 1)
        {
            throw std::runtime_error("The sums must not be empty.");
        }

        const OMPSignedNumber size = sizeRaw - 1;

        const OMPSignedNumber rowSize = OMPSignedNumber(sums[0].size());

        resultRectangle = TRectangle();
        TNumberSquared result = TNumberSquared(0);

        OMPSignedNumber x1 = 0, y1 = 0, x2 = 0, y2 = 0;

        //Sharing "resultRectangle", having template type, results in wrong results sometimes!
        //Solution: just use simple numbers as x1, x2.
        //
        //Wrong pragma:
        //#pragma omp parallel for default(none) shared(result, resultRectangle, size, rowSize, sums, TRectangle, TPoint, TSize)
        //
        //Valid one:
#pragma omp parallel for default(none) shared(result, resultRectangle, size, rowSize, sums, x1, y1, x2, y2)
        for (OMPSignedNumber rowBegin = 0; rowBegin < size; ++rowBegin)
        {
            const std::vector< TNumberSquared >& previousRow = sums[rowBegin];

            for (OMPSignedNumber rowEnd = rowBegin + 1; rowEnd <= size; ++rowEnd)
            {
                const std::vector< TNumberSquared >& endRow = sums[rowEnd];

                TNumberSquared currentSum = 0;
                OMPSignedNumber minColumn = 0;

                for (OMPSignedNumber column = 0; column < rowSize; ++column)
                {
                    currentSum += endRow[column] - previousRow[column];

                    if (currentSum <= 0)
                    {//If got negative sum, then nullify it.
                        currentSum = 0;
                        minColumn = column + 1;
                        continue;
                    }

                    if (result < currentSum)
                    {
#pragma omp critical
                        {
                            if (result < currentSum)
                            {
                                //Do not uncomment!
                                //
                                //resultRectangle = TRectangle(
                                //  TPoint(TSize(rowBegin), TSize(minColumn)),
                                //  TPoint(TSize(rowEnd), TSize(column + 1)));
                                //
                                // #pragma omp flush(resultRectangle) - this is wrong.

                                x1 = rowBegin;
                                x2 = rowEnd;

                                y1 = minColumn;
                                y2 = column + 1;

                                result = currentSum;
                            }
                        }
                    }
                }
            }
        }

        resultRectangle = TRectangle(
            TPoint(TSize(x1), TSize(y1)),
            TPoint(TSize(x2), TSize(y2)));

        return result;
    }
}