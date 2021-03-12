#pragma once
#include"../Utilities/require_utilities.h"
#include<deque>
#include<functional>
#include<vector>

namespace Standard::Algorithms::Numbers::Inner
{
    // Remove items, that <= current, as they cannot be maximum.
    template<class item_t, class less_eq_t>
    constexpr void remove_not_greater_before(
        const std::vector<item_t> &data, const item_t &current, std::deque<std::size_t> &indexes, less_eq_t less_eq)
    {
        while (!indexes.empty())
        {
            if (const auto &back = data.at(indexes.back()); !less_eq(back, current))
            {
                break;
            }

            indexes.pop_back();
        }
    }
} // namespace Standard::Algorithms::Numbers::Inner

namespace Standard::Algorithms::Numbers
{
    // Given an array and "window width",
    // return the array of max value for each window.
    template<class item_t, class less_eq_t = std::less_equal<item_t>>
    constexpr void max_in_sliding_window(const std::vector<item_t> &data, const std::size_t width,
        std::vector<item_t> &window_maximums, less_eq_t less_eq = {})
    {
        require_positive(width, "width");

        const auto size = data.size();
        require_less_equal(width, size, "width");

        window_maximums.resize(size - width + 1U);

        std::deque<std::size_t> indexes;
        std::size_t index{};

        do
        {
            // Leave only greater values: 9, 7, 6, 4 == current.
            Inner::remove_not_greater_before<item_t, less_eq_t>(data, data[index], indexes, less_eq);
            indexes.push_back(index);
        } while (++index < width);

        while (index < size)
        {
            // data[indexDeque.front()] is the max element in the queue.
            window_maximums[index - width] = data[indexes.at(0)];

            // No longer in the window - remove.
            if (indexes.front() <= index - width)
            {
                indexes.pop_front();
            }

            Inner::remove_not_greater_before<item_t, less_eq_t>(data, data[index], indexes, less_eq);

            indexes.push_back(index);
            ++index;
        }

        window_maximums.back() = data.at(indexes.at(0));
    }
} // namespace Standard::Algorithms::Numbers
