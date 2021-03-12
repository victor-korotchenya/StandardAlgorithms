#pragma once
#include <cmath>
#include <vector>
#include "Arithmetic.h"
#include "../Utilities/StreamUtilities.h"

namespace Privet::Algorithms::Numbers
{
    //Given a positive number n >= 3,
    // computes all pairs such that for each pair
    // the sum of consecutive numbers equal to n.
    //E.g. n=15, there are 3 pairs {1,5}, {4,6}, {7,8}:
    // 15 = 1+2+3+4+5 = 4+5+6 = 7+8.
    template <typename Number = size_t>
    class SumOfMin2ConsecutiveNumbers final
    {
        SumOfMin2ConsecutiveNumbers() = delete;

    public:
        using Pair = std::pair<Number, Number>;
        using Result = std::vector<Pair>;

        static Result Slow(const Number& n);

        //For numbers up to 10^9, it is actually slower.
        static Result OverOptimized(const Number& n);

    private:
        static bool CheckArgumentIsPowerOfTwo(const Number& n);

        static Number EstimateNumber(const Number& sum);
    };

    //Returns the sum of consecutive integers
    // from "fromNumber" to "number" inclusively.
    //
    // Sum 1...x is x(x+1)/2.
    template <typename Number>
    Number SumToN(const Number& fromNumber, const Number& toNumber)
    {
        if (fromNumber < Number(0))
        {//TODO: p3. Work with negative numbers.
            std::ostringstream ss;
            ss << "Error in SumToN: from(" << fromNumber
                << ") must be non-negative.";
            throw StreamUtilities::throw_exception(ss);
        }

        if (toNumber == fromNumber)
        {
            return toNumber;
        }

        if (fromNumber <= toNumber)
        {
            const Number one = Number(1);

            const Number sum2 = (toNumber & one)
                ? MultiplyUnsigned(AddUnsigned(toNumber, one) >> 1, toNumber)
                : MultiplyUnsigned(AddUnsigned(toNumber, one), toNumber >> 1);

            if (fromNumber <= one)
            {
                return sum2;
            }

            const Number sum1 = SumToN(one, fromNumber - one);
            const Number result = sum2 - sum1;
            return result;
        }

        std::ostringstream ss;
        ss << "Error in SumToN: from(" << fromNumber
            << ") cannot exceed toNumber(" << toNumber << ").";
        throw StreamUtilities::throw_exception(ss);
    }

    template <typename Number>
    typename SumOfMin2ConsecutiveNumbers<Number>::Result
        SumOfMin2ConsecutiveNumbers<Number>::Slow(const Number& n)
    {
        Result result;
        if (CheckArgumentIsPowerOfTwo(n))
        {
            return result;
        }

        Number low = 1;
        Number high = EstimateNumber(n);

        const Number lowMax = (n + Number(1)) >> 1;

        Number sum = SumToN(low, high);
        do
        {
            if (sum == n)
            {
#ifdef _DEBUG
                if (high <= low)
                {
                    std::ostringstream ss;
                    ss << "Error in Slow: high (" << high
                        << ") <= low(" << low
                        << ", n=" << n
                        << ".";
                    StreamUtilities::throw_exception(ss);
                }
#endif
                result.push_back(Pair(low, high));

                sum -= (low << 1) + Number(1);

                //The set size is decreased by at least 1.
                low += Number(2);
                ++high;

                sum += high;
            }
            else if (sum < n)
            {
                ++high;
                sum += high;
            }
            else
            {
                do
                {
                    sum -= low;
                    ++low;
                } while (n < sum);
            }
        } while (low < lowMax);

        return result;
    }

    template <typename Number>
    typename SumOfMin2ConsecutiveNumbers<Number>::Result
        SumOfMin2ConsecutiveNumbers<Number>::OverOptimized(const Number& n)
    {
        Result result;
        if (CheckArgumentIsPowerOfTwo(n))
        {
            return result;
        }

        Number low = 1;
        Number high = EstimateNumber(n);

        const Number lowMax = (n + Number(1)) >> 1;

        // Eventually, the low starts increasing by 1 or 2.
        bool shallGoFast = true;

        Number sum = SumToN(low, high);
        do
        {
#ifdef _DEBUG
            //if (n < 100)
            {
                const Number sumTemp = SumToN(low, high);
                if (sumTemp != sum)
                {
                    std::ostringstream ss;
                    ss << "Error in Fast: sumTemp(" << sumTemp
                        << ") != sum(" << sum
                        << "), low=" << low
                        << ", high=" << high
                        << ", n=" << n
                        << ".";
                    StreamUtilities::throw_exception(ss);
                }
            }
#endif
            if (sum == n)
            {
                result.push_back(Pair(low, high));

                sum -= (low << 1) + Number(1);

                //The set size is decreased by at least 1.
                low += Number(2);
                ++high;

                sum += high;
            }
            else if (sum < n)
            {
                //The upper limit increases by one.
                ++high;
                sum += high;
            }
            else
            {
                if (shallGoFast && low < (sum - n))
                {
                    //Let n = 4095 = Sum[1..90].
                    //Having subtracted(low + low + 1) and added new high,
                    // the sum becomes 4095 - 1 - 2 + 91 = 4183; low = 3, high = 91.
                    //So, the sum must go down by delta=88 by increasing the low from 3 to 14.
                    // Sum(1..2) = 3; Sum(1..3) = 6; Sum(1..4) = 10; Sum(1..13) = 91.
                    //Desired: delta = sum(1..x) - sum(1..low-1),
                    //sum(1..x) = delta + sum(1..low-1).
                    const Number sumLowMinusOne = SumToN(Number(0), low - Number(1));
                    const Number sumEstimated = (sum - n) + sumLowMinusOne;
                    const Number x = EstimateNumber(sumEstimated) + Number(1);

                    const Number deltaLimit = Number(100);
                    shallGoFast = deltaLimit + low < x;

                    if (low < x)
                    {
                        const Number sumX = SumToN(Number(1), x);
#ifdef _DEBUG
                        if (sumX <= sumLowMinusOne + x)
                        {
                            std::ostringstream ss;
                            ss << "Error in Fast: sumX(" << sumX
                                << ") <= sumLowMinusOne + x =" << (sumLowMinusOne + x)
                                << "), sumLowMinusOne=" << sumLowMinusOne
                                << "), x=" << x
                                << "), low=" << low
                                << ", high=" << high
                                << ", n=" << n
                                << ".";
                            StreamUtilities::throw_exception(ss);
                        }
#endif
                        sum -= sumX - sumLowMinusOne - x;
                        low = x;

                        continue;
                    }
                }

                do
                {
                    sum -= low;
                    ++low;
                } while (n < sum);
            }
        } while (low < lowMax);

        return result;
    }

    template <typename Number>
    bool SumOfMin2ConsecutiveNumbers<Number>::CheckArgumentIsPowerOfTwo(
        const Number& n)
    {
        const Number minValue = 3;
        if (n < minValue)
        {
            std::ostringstream ss;
            ss << "The number(" << n << ") must be at least " << minValue << ".";
            StreamUtilities::throw_exception(ss);
        }

        //Sum from m to n inclusively, where m < n.
        // Let h = m - 1, so n = h + k, where 1 < k; 0 <= h.
        //
        //Sum= n(n+1)/2 - h(h+1)/2 = 1/2*[(h + k)* (h + k + 1) - h(h + 1)] =
        // = 1/2*[(h*h + h*k + h) + (k*h + k*k + k) - h*h - h] =
        // = [2 * h + k + 1]*k/2.
        //If Sum = 2^x, then
        //  k must be k = 2^x, 0 < x;
        //  and (2 * h + k + 1) = 2^y.
        //It is impossible as (2 * h + k + 1) is an odd number.
        const bool result = IsPowerOfTwo(n);
        return result;
    }

    template <typename Number>
    Number SumOfMin2ConsecutiveNumbers<Number>
        ::EstimateNumber(const Number& sum)
    {
#ifdef _DEBUG
        if (sum < Number(1))
        {
            std::ostringstream ss;
            ss << "EstimateNumber(" << sum << ") must be at least 1.";
            StreamUtilities::throw_exception(ss);
        }
#endif
        // Sum 1...x is x(x+1)/2, then 0 = x*x + x - 2*s.
        //  D = sqrt(1 + 8*s).  x = (-1 + d)/2
        const Number discriminant = AddUnsigned<Number>(
            MultiplyUnsigned<Number>(Number(8), sum),
            Number(1));

        //At least 3 when sum=1.
        //At least 5 when sum=3.
        const Number discriminantRoot = Number(sqrt(discriminant));

        Number result = (discriminantRoot - Number(1)) >> 1;
        return result;
    }
}