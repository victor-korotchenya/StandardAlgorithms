#pragma once
#include "Arithmetic.h" //abs_diff
#include "../Utilities/StreamUtilities.h"

namespace Privet
{
    namespace Algorithms
    {
        namespace Numbers
        {
            //The first line is non-decreasing, second: non-increasing.
            //There should be a point,
            // where the distance between lines in minimum.
            //Both beginning, ending are inclusive.
            //Binary search is used.
            template <typename number_x, typename incr_line_t,
                typename number_y = number_x,
                typename decr_line_t = incr_line_t>
                number_x CrossPointOTwofIncrDecrLines(
                    incr_line_t incr_line, decr_line_t decr_line,
                    number_x beginning, number_x ending)
            {
                if (ending < beginning)
                {
                    std::ostringstream ss;
                    ss << "Error: ending(" << ending
                        << ") < beginning(" << beginning << ").";
                    StreamUtilities::throw_exception(ss);
                }

                number_x result;
                number_y best_delta;
                {
                    const number_y incr_left = incr_line(beginning);
                    const number_y decr_left = decr_line(beginning);
                    if (beginning == ending || decr_left <= incr_left)
                    {
                        return beginning;
                    }

                    const number_y incr_right = incr_line(ending);
                    const number_y decr_right = decr_line(ending);
                    if (incr_right <= decr_right)
                    {
                        return ending;
                    }
                    //There must be a crossing point.

                    const number_y left_delta = abs_diff(incr_left, decr_left);
                    const number_y right_delta = abs_diff(incr_right, decr_right);

                    if (right_delta < left_delta)
                    {
                        result = ending;
                        best_delta = right_delta;
                    }
                    else
                    {
                        result = beginning;
                        best_delta = left_delta;
                    }
                }

                for (;;)
                {
                    const auto middle = static_cast<number_x>(beginning
                        + ((ending - beginning) >> 1));

                    const number_y v1 = incr_line(middle);
                    const number_y v2 = decr_line(middle);
                    if (v1 == v2)
                    {
                        return middle;
                    }

                    const number_y delta = abs_diff(v1, v2);
                    if (delta < best_delta)
                    {
                        result = middle;
                        best_delta = delta;
                    }

                    if (beginning == ending)
                    {
                        return result;
                    }

                    if (v1 < v2)
                    {
                        beginning = middle + 1;
                    }
                    else
                    {
                        if (middle == beginning)
                        {
                            return result;
                        }

                        ending = middle - 1;
                    }
                }
            }

            //slow
            template <typename number_x, typename incr_line_t,
                typename number_y = number_x,
                typename decr_line_t = incr_line_t>
                number_x CrossPointOTwofIncrDecrLines_slow(
                    incr_line_t incr_line, decr_line_t decr_line,
                    number_x beginning, number_x ending)
            {
                if (ending < beginning)
                {
                    std::ostringstream ss;
                    ss << "Error: ending(" << ending
                        << ") < beginning(" << beginning << ").";
                    StreamUtilities::throw_exception(ss);
                }

                number_x result = beginning;
                number_y best_delta = abs_diff(incr_line(beginning),
                    decr_line(beginning));

                while (++beginning <= ending)
                {
                    const number_y delta = abs_diff(incr_line(beginning),
                        decr_line(beginning));
                    if (delta < best_delta)
                    {
                        result = beginning;
                        best_delta = delta;
                    }
                }

                return result;
            }
        }
    }
}