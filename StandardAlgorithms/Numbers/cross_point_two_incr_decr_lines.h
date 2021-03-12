#pragma once
#include"arithmetic.h"

namespace Standard::Algorithms::Numbers
{
    // The first line is non-decreasing, second: non-increasing.
    // There should be a point,
    // where the distance between lines in minimum.
    // Both beginning, ending are inclusive.
    // The binary search is used.
    template<class number_y, class number_x, class incr_line_t, class decr_line_t>
    [[nodiscard]] constexpr auto cross_point_two_incr_decr_lines(
        number_x beginning, incr_line_t incr_line, decr_line_t decr_line, number_x ending) -> number_x
    {
        if (ending == beginning)
        {
            return beginning;
        }

        if (ending < beginning)
        {
            auto str = ::Standard::Algorithms::Utilities::w_stream();
            str << "Error: ending " << ending << " < beginning " << beginning << ".";

            throw_exception(str);
        }

        number_x result{};
        number_y best_delta{};

        {
            const auto incr_left = static_cast<number_y>(incr_line(beginning));
            const auto decr_left = static_cast<number_y>(decr_line(beginning));

            if (!(incr_left < decr_left))
            {
                return beginning;
            }

            const auto incr_right = static_cast<number_y>(incr_line(ending));
            const auto decr_right = static_cast<number_y>(decr_line(ending));

            if (!(decr_right < incr_right))
            {
                return ending;
            }

            // There must be a crossing point.
            const auto left_delta = static_cast<number_y>(abs_diff(incr_left, decr_left));
            const auto right_delta = static_cast<number_y>(abs_diff(incr_right, decr_right));

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
            const auto middle = static_cast<number_x>(beginning + ((ending - beginning) >> 1U));

            const auto inc = static_cast<number_y>(incr_line(middle));
            const auto dec = static_cast<number_y>(decr_line(middle));

            if (inc == dec)
            {
                return middle;
            }

            const auto delta = static_cast<number_y>(abs_diff(inc, dec));

            if (delta < best_delta)
            {
                result = middle;
                best_delta = delta;
            }

            if (beginning == ending)
            {
                return result;
            }

            if (inc < dec)
            {
                beginning = static_cast<number_x>(middle + 1);
            }
            else
            {
                if (middle == beginning)
                {
                    return result;
                }

                ending = static_cast<number_x>(middle - 1);
            }
        }
    }

    // Slow.
    template<class number_y, class number_x, class incr_line_t, class decr_line_t>
    [[nodiscard]] constexpr auto cross_point_two_incr_decr_lines_slow(
        number_x beginning, incr_line_t incr_line, decr_line_t decr_line, number_x ending) -> number_x
    {
        if (ending < beginning)
        {
            auto str = ::Standard::Algorithms::Utilities::w_stream();
            str << "Error: ending(" << ending << ") < beginning(" << beginning << ").";
            throw_exception(str);
        }

        auto result = beginning;

        auto best_delta = static_cast<number_y>(abs_diff(incr_line(beginning), decr_line(beginning)));

        while (++beginning <= ending)
        {
            const auto delta = static_cast<number_y>(abs_diff(incr_line(beginning), decr_line(beginning)));

            if (delta < best_delta)
            {
                result = beginning;
                best_delta = delta;
            }
        }

        return result;
    }
} // namespace Standard::Algorithms::Numbers
