#pragma once
#include "../Utilities/StreamUtilities.h"

//#ifndef isinf
//#define isinf(x) (!_finite(x))
//#endif

#include "../Numbers/Arithmetic.h"
#include "../Geometry/point.h"

namespace Privet::Algorithms::Geometry
{
    template <typename Number = double>
    struct SegmentInfo final
    {
        Number Slope;

        //Y-intercept - the point where the line crosses the Y axis (when x=0).
        Number Intercept;

        Number Error;

        SegmentInfo(
            Number slope = static_cast<Number>(0),
            Number intercept = static_cast<Number>(0),
            Number error = static_cast<Number>(0))
            :
            Slope(slope), Intercept(intercept), Error(error)
        {
        }

        inline bool operator == (SegmentInfo< Number > const& b) const
        {
            //Use NumberComparer in order to compare doubles with epsilon.
            const bool result = Slope == b.Slope && Intercept == b.Intercept
                && Error == b.Error;
            return result;
        }

        friend std::ostream& operator <<
            (std::ostream& str, SegmentInfo< Number > const& b)
        {
            const auto prec = StreamUtilities::GetMaxDoublePrecision();

            str << std::setprecision(prec)
                << "Slope=" << b.Slope << ", Intercept=" << b.Intercept
                << ", Error=" << b.Error;
            return str;
        }
    };

    template <typename Number = double>
    struct SegmentResult final
    {
        SegmentInfo< Number > Info;

        //First point number, zero based, from the points array.
        size_t Point1;

        //Last point number.
        size_t Point2;

        SegmentResult(
            SegmentInfo< Number > info = SegmentInfo< Number >(),
            size_t point1 = 0, size_t point2 = 0)
            :
            Info(info),
            Point1(point1), Point2(point2)
        {
        }

        inline bool operator == (SegmentResult< Number > const& b) const
        {
            const bool result = Info == b.Info
                && Point1 == b.Point1 && Point2 == b.Point2;
            return result;
        }

        friend std::ostream& operator <<
            (std::ostream& str, SegmentResult< Number > const& b)
        {
            str
                << b.Info
                << ", Points=(" << b.Point1 << ", " << b.Point2 << ")";
            return str;
        }
    };

    template <typename Number = double>
    struct SegmentedRegressionSession final
    {
        using AbsoluteValueFunction = Number(*)(const Number&);

        Number Infinity{};

        AbsoluteValueFunction AbsoluteValue{};

        //These sums store 0 at [0].
        std::vector< Number > X, Y, XX, XY, YY;

        //It does not store 0 at [0].
        std::vector< size_t > OptimalIndexes;

        //Stores 0 at [0].
        std::vector< Number > Optimals;

        void inline Initialize(const size_t size)
        {
            const size_t one = 1;
            const size_t sizePlusOne = AddUnsigned(one, size);

            X.resize(sizePlusOne);
            Y.resize(sizePlusOne);
            XX.resize(sizePlusOne);
            XY.resize(sizePlusOne);
            YY.resize(sizePlusOne);

            const Number zero = static_cast<Number>(0);

            X[0] = zero;
            Y[0] = zero;
            XX[0] = zero;
            XY[0] = zero;
            YY[0] = zero;

            OptimalIndexes.resize(size);

            Optimals.resize(sizePlusOne);
            Optimals[0] = zero;
        }
    };

    template <typename Coordinate = float,
        typename TPoint = Geometry::point2d< Coordinate >,
        //"Number" must have not smaller precision than "Coordinate" does.
        typename Number = double
    >
        class SegmentedRegression final
    {
        SegmentedRegression() = delete;

    public:
        using TCoordinate = Coordinate;

        using AbsoluteValueFunction = typename SegmentedRegressionSession< Number >::AbsoluteValueFunction;

        //The 'points' must be sorted in the strictly increasing order of X-coordinate.
        static Number Solve(
            //E.g. numeric_limits< double >::infinity()
            const Number& infinity,
            //E.g. abs
            const AbsoluteValueFunction absoluteValueFunction,
            const std::vector< TPoint >& points,
            //The "segmentCost' must be non-negative
            const Number segmentCost,
            std::vector< SegmentResult< Number > >& segments);

    private:

        static Number FindOptimal(
            const std::vector< TPoint >& points,
            const Number segmentCost,
            SegmentedRegressionSession< Number >& session);

        static void RestoreSolution(
            const size_t size,
            const SegmentedRegressionSession< Number >& session,
            std::vector< SegmentResult< Number > >& segments);

        inline static void FindMinimum(
            const SegmentedRegressionSession< Number >& session,
            const size_t next,
            size_t& bestIndex,
            Number& minValue);

        inline static void CheckPoint(
            const TPoint& point,
            const size_t index,
            Coordinate& last);

        inline static void ComputeSums(
            const TPoint& point,
            const size_t i,
            SegmentedRegressionSession< Number >& session);

        inline static void ComputeSegment(
            const SegmentedRegressionSession< Number >& session,
            const size_t current, const size_t next,
            SegmentInfo< Number >& segmentInfo);
    };

    template <typename Coordinate,
        typename TPoint,
        typename Number >
        Number SegmentedRegression< Coordinate, TPoint, Number >::Solve(
            const Number& infinity,
            const AbsoluteValueFunction absoluteValueFunction,
            const std::vector< TPoint >& points,
            const Number segmentCost,
            std::vector< SegmentResult < Number > >& segments)
    {
        if (nullptr == absoluteValueFunction)
        {
            throw std::runtime_error("nullptr == absoluteValueFunction");
        }

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

        SegmentedRegressionSession< Number > session;
        session.Infinity = infinity;
        session.AbsoluteValue = absoluteValueFunction;
        session.Initialize(size);

        const Number result = FindOptimal(points, segmentCost, session);

        RestoreSolution(size, session, segments);

        return result;
    }

    template <typename Coordinate,
        typename TPoint,
        typename Number >
        Number SegmentedRegression< Coordinate, TPoint, Number >::FindOptimal(
            const std::vector< TPoint >& points,
            const Number segmentCost,
            SegmentedRegressionSession< Number >& session)
    {
        const size_t size = points.size();
        std::vector< Number >& optimals = session.Optimals;
        std::vector< size_t >& optimalIndexes = session.OptimalIndexes;

        Coordinate last = points[0].X;

        size_t next = 0;

        SegmentInfo< Number > segmentInfo;
        do
        {
            const TPoint& point = points[next];
            if (next)
            {
                CheckPoint(point, next, last);
            }

            ComputeSums(point, next, session);

            size_t bestIndex;
            Number minValue;

            FindMinimum(
                session,
                next,
                bestIndex, minValue);

            optimalIndexes[next] = bestIndex;
            optimals[++next] = minValue + segmentCost;
        } while (next < size);

        const Number result = optimals[size];
        return result;
    }

    template <typename Coordinate,
        typename TPoint,
        typename Number >
        void SegmentedRegression< Coordinate, TPoint, Number >::RestoreSolution(
            const size_t size,
            const SegmentedRegressionSession< Number >& session,
            std::vector< SegmentResult< Number > >& segments)
    {
        const std::vector< size_t >& optimalIndexes = session.OptimalIndexes;

        segments.clear();

        size_t current = optimalIndexes[size - 1];
        size_t next = size - 1;

        SegmentInfo< Number > segmentInfo;

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
            ComputeSegment(
                session,
                current, next,
                segmentInfo);

            segments.push_back(SegmentResult< Number >(
                segmentInfo, current, next));

            if (0 == current)
            {
                break;
            }

            next = current - 1;
            current = optimalIndexes[next];
        }

        reverse(segments.begin(), segments.end());
    }

    template <typename Coordinate,
        typename TPoint,
        typename Number >
        void SegmentedRegression< Coordinate, TPoint, Number >::FindMinimum(
            const SegmentedRegressionSession< Number >& session,
            const size_t next,
            size_t& bestIndex,
            Number& minValue)
    {
        const std::vector< Number >& optimals = session.Optimals;

        SegmentInfo< Number > segmentInfo;

        ComputeSegment(
            session,
            0, next,
            segmentInfo);

        bestIndex = 0;
        minValue = segmentInfo.Error;

        size_t current = 1;
        while (current <= next)
        {
            if (optimals[current] < minValue)//Since error is non-negative, it should be faster.
            {//TODO: p2. check with profiler whether "if" makes sense.
                ComputeSegment(
                    session,
                    current, next,
                    segmentInfo);

                const Number value = segmentInfo.Error + optimals[current];
                if (value < minValue)
                {
                    bestIndex = current;
                    minValue = value;
                }
            }

            ++current;
        }
    }

    template <typename Coordinate,
        typename TPoint,
        typename Number >
        void SegmentedRegression< Coordinate, TPoint, Number >::CheckPoint(
            const TPoint& point,
            const size_t index,
            Coordinate& last)
    {
        if (point.X <= last)
        {
            std::ostringstream ss;
            ss << "Point (" << point <<
                ") at [" << index <<
                "] must have X-coordinate, greater than previous value=" << last << ".";
            StreamUtilities::throw_exception(ss);
        }

        last = point.X;
    }

    template <typename Coordinate,
        typename TPoint,
        typename Number >
        void SegmentedRegression< Coordinate, TPoint, Number >::ComputeSums(
            const TPoint& point,
            const size_t i,
            SegmentedRegressionSession< Number >& session)
    {
        //"Number" can have better precision.
        const Number& x = point.X;
        const Number& y = point.Y;

        session.X[i + 1] = session.X[i] + x;
        session.Y[i + 1] = session.Y[i] + y;
        session.XX[i + 1] = session.XX[i] + x * x;
        session.XY[i + 1] = session.XY[i] + x * y;
        session.YY[i + 1] = session.YY[i] + y * y;
    }

    template <typename Coordinate,
        typename TPoint,
        typename Number >
        void SegmentedRegression< Coordinate, TPoint, Number >::ComputeSegment(
            const SegmentedRegressionSession< Number >& session,
            const size_t current, const size_t next,
            SegmentInfo< Number >& segmentInfo)
    {
        const Number zero = static_cast<Number>(0);

        //Sums from (current+1) to (next+1) inclusive.
        const Number sumX = session.X[next + 1] - session.X[current];
        const Number sumY = session.Y[next + 1] - session.Y[current];
        const Number sumXX = session.XX[next + 1] - session.XX[current];
        const Number sumXY = session.XY[next + 1] - session.XY[current];
        const Number sumYY = session.YY[next + 1] - session.YY[current];

        //Use "Number" type because it is to be cast to Number.
        const Number segmentLength = static_cast<Number>(next + 1 - current);

        const Number numerator = segmentLength * sumXY - sumX * sumY;

        Number slope;
        if (zero == numerator)
        {
            slope = zero;
        }
        else
        {
            const Number denominator = segmentLength * sumXX - sumX * sumX;
            slope = zero == denominator ? session.Infinity : numerator / denominator;
        }

        segmentInfo.Slope = slope;

        const Number interceptNominator = sumY - slope * sumX;
        const Number intercept = interceptNominator / segmentLength;
        segmentInfo.Intercept = intercept;

        //For all k=[current..next]
        //Take the sum of squares of these for k-th point at (X[k], Y[k]):
        //  Slope[next, current] * X[k] + Intercept[next, current] - Y[k].

        //The expanded sum (A == slope, B== Intercept) is:
        //  A*A*X*X + B*B + Y*Y
        //     + 2AXB - 2AXY - 2BY.
        //
        // These sums are required:
        // X*X, Y*Y, X, X*Y, Y

        const Number two = static_cast<Number>(2);

        const Number rawError = slope * slope * (sumXX)
            +intercept * interceptNominator
            + sumYY
            + two
            * (slope * (intercept * sumX - sumXY)
                - intercept * sumY
                );

        //Sometimes the error can be negative due to rounding.
        segmentInfo.Error = session.AbsoluteValue(rawError);
    }
}