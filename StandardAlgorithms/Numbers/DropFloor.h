#pragma once

#include "Arithmetic.h"
#include "../Utilities/StreamUtilities.h"

namespace Privet
{
    namespace Algorithms
    {
        namespace Numbers
        {
            template <typename Number>
            static Number covered_floors(
                const Number drops,
                const Number balls,
                const Number floors_limit)
            {
                Number sum = 0;
                Number addings = drops;

                for (Number i = 1; i <= balls; ++i)
                {
                    sum += addings;
                    if (floors_limit <= sum)
                    {//Stop to avoid an overflow.
                        return floors_limit;
                    }

                    addings *= drops - i;
                    addings /= (i + 1);
                }

                return sum;
            }

            template <typename Number>
            Number drop_floor(
                const Number balls,
                const Number floors,
                //To avoid an overflow, it must be small e.g. 100K.
                const Number floors_limit)
            {
                if (floors_limit < floors)
                {
                    std::ostringstream ss;
                    ss << "Too small floors_limit=" << floors_limit
                        << " < floors=" << floors << ".";
                    StreamUtilities::throw_exception(ss);
                }

                if (balls <= 1 || floors <= 1)
                    return floors;

                const Number binaryResult = static_cast<Number>(
                    1 + MostSignificantBit(static_cast<unsigned long>(floors)));

                if (binaryResult <= balls)
                    //There are enough balls to use a binary search.
                    return binaryResult;

                Number left = binaryResult, right = floors - 1;
                assert(left <= right);
                do
                {
                    const auto mid = left + ((right - left) >> 1);
                    const auto canCoverFloors = covered_floors<Number>(
                        mid, balls, floors_limit);

                    const auto isOk = floors <= canCoverFloors;
                    if (isOk)
                        right = mid - 1;
                    else
                        left = mid + 1;
                } while (left <= right);

                return right + 1;
            }
        }
    }
}