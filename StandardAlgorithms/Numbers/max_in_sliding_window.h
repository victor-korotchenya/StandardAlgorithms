#pragma once
#include <functional>
#include <vector>
#include <deque>
#include "../Utilities/ExceptionUtilities.h"

namespace
{
    // Remove items, that <= current, as they cannot be maximum.
    template <typename t, typename less_eq_t>
    void remove_not_greater_before(const std::vector<t>& data,
        const t& current,
        std::deque<size_t>& indexes,
        less_eq_t less_eq)
    {
        while (!indexes.empty() &&
            less_eq(data[indexes.back()], current))
        {
            indexes.pop_back();
        }
    }
}

namespace Privet::Algorithms::Numbers
{
    //Given an array and "window width",
    //returns the array of max value for each window.
    template <typename t, typename less_eq_t = std::less_equal<t>>
    void max_in_sliding_window(
        const std::vector<t>& data,
        const size_t width,
        std::vector<t>& window_maximums,
        less_eq_t less_eq = {})
    {
        RequirePositive(width, "width");

        const auto size = data.size();
        RequireNotGreater(width, size, "Width cannot exceed the data size");

        window_maximums.resize(size - width + 1);

        std::deque<size_t> indexes;
        size_t i = 0;
        do
        {
            // Leave only greater values: 9, 7, 6, 4 == current.
            remove_not_greater_before<t, less_eq_t>(data, data[i], indexes, less_eq);
            indexes.push_back(i);
        } while (++i < width);

        while (i < size)
        {
            //Now, data[indexDeque.front()] is the max element in the queue.
            window_maximums[i - width] = data[indexes.front()];

            // No longer in the window - remove.
            if (indexes.size() && indexes.front() <= i - width)
                indexes.pop_front();

            remove_not_greater_before<t, less_eq_t>(data, data[i], indexes, less_eq);

            indexes.push_back(i);
            ++i;
        }

        window_maximums.back() = data[indexes.front()];
    }
}