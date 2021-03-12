#pragma once
#include <vector>
#include <sstream>
#include "../Numbers/Arithmetic.h"
#include "../Geometry/point.h"
#include "SegmentedRegression.h"

namespace Privet::Algorithms::Geometry
{
    template <typename Coordinate = float,
        typename TPoint = Geometry::point2d< Coordinate >,
        typename Number = double>
        class SegmentedRegressionOld final
    {
        SegmentedRegressionOld() = delete;

    public:
        using TCoordinate = Coordinate;

        //The 'points' must be sorted in the strictly increasing order of X-coordinate.
        static Number Solve(
            //E.g. numeric_limits< double >::infinity()
            const Number& infinity,
            const std::vector< TPoint >& points,
            //The "segmentCost' must be non-negative
            const Number segmentCost,
            std::vector< SegmentResult< Number > >& segments);

    private:
        static void CheckPoints(
            const std::vector< TPoint >& points);

        static void ComputeCoefficients(
            const Number& infinity,
            const std::vector< TPoint >& points,
            std::vector< std::vector< SegmentInfo< Number > > >& infos);

        static Number FindOptimal(
            const size_t size,
            const Number segmentCost,
            const std::vector< std::vector< SegmentInfo< Number > > >& infos,
            SegmentedRegressionSession< Number >& session);

        static void RestoreSolution(
            const size_t size,
            const std::vector< std::vector< SegmentInfo< Number > > >& infos,
            const SegmentedRegressionSession< Number >& session,
            std::vector< SegmentResult< Number > >& segments);
    };

    template <typename Coordinate,
        typename TPoint,
        typename Number >
        Number SegmentedRegressionOld< Coordinate, TPoint, Number >::Solve(
            const Number& infinity,
            const std::vector< TPoint >& points,
            const Number segmentCost,
            std::vector< SegmentResult < Number > >& segments)
    {
        const size_t sizeMin = 2;
        const size_t size = points.size();
        if (size < sizeMin)
        {
            std::ostringstream ss;
            ss << "There must be at least " << sizeMin <<
                " points, now having " << size << ".";
            StreamUtilities::throw_exception(ss);
        }

        const Number zero = static_cast<Number>(0);
        if (segmentCost < zero)
        {
            std::ostringstream ss;
            ss << "The segment cost (" << segmentCost << ") must be non-negative.";
            StreamUtilities::throw_exception(ss);
        }

        CheckPoints(points);

        SegmentedRegressionSession< Number > session;
        session.Initialize(size);

        std::vector<std::vector<SegmentInfo<Number>>> infos(size, std::vector<SegmentInfo<Number>>(size));

        ComputeCoefficients(infinity, points, infos);

        const Number result = FindOptimal(size, segmentCost, infos, session);

        RestoreSolution(size, infos, session, segments);

        return result;
    }

    template <typename Coordinate,
        typename TPoint,
        typename Number >
        void SegmentedRegressionOld< Coordinate, TPoint, Number >::CheckPoints(
            const std::vector< TPoint >& points)
    {
        const size_t size = points.size();

        Coordinate last = points[0].X;
        const size_t skipFirst = 1;

        for (size_t i = skipFirst; i < size; ++i)
        {
            const TPoint& point = points[i];

            if (points[i].X <= last)
            {
                std::ostringstream ss;
                ss << "Point (" << point <<
                    ") at [" << i <<
                    "] must have X-coordinate, greater than previous value=" << last << ".";
                StreamUtilities::throw_exception(ss);
            }

            last = points[i].X;
        }
    }

    template <typename Coordinate,
        typename TPoint,
        typename Number >
        void SegmentedRegressionOld< Coordinate, TPoint, Number >::ComputeCoefficients(
            const Number& infinity,
            const std::vector< TPoint >& points,
            std::vector< std::vector< SegmentInfo< Number > > >& infos)
    {
        const Number zero = static_cast<Number>(0);
        const size_t size = points.size();

        const size_t one = 1;
        const size_t sizePlusOne = AddUnsigned(one, size);

        std::vector< Number > totalSumX, totalSumY, totalSumXX, totalSumXY;
        totalSumX.resize(sizePlusOne);
        totalSumY.resize(sizePlusOne);
        totalSumXX.resize(sizePlusOne);
        totalSumXY.resize(sizePlusOne);

        totalSumX[0] = zero;
        totalSumY[0] = zero;
        totalSumXY[0] = zero;
        totalSumXX[0] = zero;

        for (size_t i = 0; i < size; ++i)
        {
            const TPoint& point = points[i];

            totalSumX[i + 1] = totalSumX[i] + point.X;
            totalSumY[i + 1] = totalSumY[i] + point.Y;
            totalSumXX[i + 1] = totalSumXX[i] + point.X * point.X;
            totalSumXY[i + 1] = totalSumXY[i] + point.X * point.Y;

            std::vector< SegmentInfo< Number > >& info = infos[i];

            for (size_t j = 0; j <= i; ++j)
            {
                //Use "Number" type because it is to be cast to Number.
                const Number segmentLength = static_cast<Number>(i + 1 - j);
                const Number sumX = totalSumX[i + 1] - totalSumX[j];
                const Number sumY = totalSumY[i + 1] - totalSumY[j];
                const Number sumXX = totalSumXX[i + 1] - totalSumXX[j];
                const Number sumXY = totalSumXY[i + 1] - totalSumXY[j];

                const Number numerator = segmentLength * sumXY - sumX * sumY;
                if (zero == numerator)
                {
                    info[j].Slope = zero;
                }
                else
                {
                    const Number denominator = segmentLength * sumXX - sumX * sumX;
                    info[j].Slope = zero == denominator ? infinity : numerator / denominator;
                }

                info[j].Intercept = (sumY - info[j].Slope * sumX) / segmentLength;

                Number errorTemp = zero;

                for (size_t k = j; k <= i; ++k)
                {
                    const TPoint& pointK = points[k];

                    const Number tmp = pointK.Y
                        - info[j].Slope * pointK.X
                        - info[j].Intercept;
                    errorTemp += tmp * tmp;
                }

                info[j].Error = errorTemp;
            }
        }
    }

    template <typename Coordinate,
        typename TPoint,
        typename Number >
        Number SegmentedRegressionOld< Coordinate, TPoint, Number >::FindOptimal(
            const size_t size,
            const Number segmentCost,
            const std::vector< std::vector< SegmentInfo< Number > > >& infos,
            SegmentedRegressionSession< Number >& session)
    {
        std::vector< Number >& optimals = session.Optimals;
        std::vector< size_t >& optimalIndexes = session.OptimalIndexes;

        optimals[0] = static_cast<Number>(0);

        size_t i = 0;
        do
        {
            const std::vector< SegmentInfo< Number > >& info = infos[i];

            size_t bestIndex = 0;
            Number minValue = info[0].Error;

            size_t j = 1;
            while (j <= i)
            {
                const Number value = info[j].Error + optimals[j];
                if (value < minValue)
                {
                    bestIndex = j;
                    minValue = value;
                }

                ++j;
            }

            optimalIndexes[i] = bestIndex;
            optimals[++i] = minValue + segmentCost;
        } while (i < size);

        const Number result = optimals[size];
        return result;
    }

    template <typename Coordinate,
        typename TPoint,
        typename Number >
        void SegmentedRegressionOld< Coordinate, TPoint, Number >::RestoreSolution(
            const size_t size,
            const std::vector< std::vector< SegmentInfo< Number > > >& infos,
            const SegmentedRegressionSession< Number >& session,
            std::vector< SegmentResult< Number > >& segments)
    {
        segments.clear();

        size_t current = session.OptimalIndexes[size - 1];
        size_t next = size - 1;

        for (;;)
        {
#ifdef _DEBUG
            if (0 == next)
            {
                throw std::runtime_error("0 == next");
            }

            if (next <= current)
            {
                std::ostringstream ss;
                ss << "next(" << next << ") <= current(" << current << ").";
                StreamUtilities::throw_exception(ss);
            }
#endif
            const SegmentInfo< Number > info = infos[next][current];

            segments.push_back(SegmentResult< Number >(
                SegmentInfo< Number >(
                    info.Slope,
                    info.Intercept,
                    info.Error),
                current, next));

            if (0 == current)
            {
                break;
            }

            next = current - 1;
            current = session.OptimalIndexes[next];
        }

        reverse(segments.begin(), segments.end());
    }
}