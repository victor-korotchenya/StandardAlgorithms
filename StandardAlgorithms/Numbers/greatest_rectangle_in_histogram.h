#pragma once
#include"../Utilities/same_sign_leq_size.h"
#include<concepts>
#include<exception>
#include<stack>
#include<stdexcept>
#include<vector>

namespace Standard::Algorithms::Numbers
{
    // A histogram consists of rectangles
    // each having unit width and non-negative height.
    // The question is to find a rectangle with the greatest area.
    //
    // See also "max_sub_matrix_in_2d_matrix.h".
    template<std::integral int_t, std::integral long_int_t>
    requires(same_sign_leq_size<int_t, long_int_t>)
    struct greatest_rectangle_in_histogram final
    {
        greatest_rectangle_in_histogram() = delete;

        [[nodiscard]] static constexpr auto compute_fast(const std::vector<int_t> &heights, std::size_t &start_index,
            int_t &start_height, std::size_t &start_width,
            // When true, then startIndex will
            // be calculated, and it might take O(N) time.
            const bool shall_compute_start_index = true) -> long_int_t
        {
            const auto size = heights.size();

            if (0U == size) [[unlikely]]
            {
                throw std::runtime_error("The 'heights' must have at least one element.");
            }

            start_index = 0;
            start_height = 0;
            start_width = 0;

            long_int_t result{};
            std::stack<std::size_t> sta;

            for (std::size_t index{}; index <= size; ++index)
            {
                while (!sta.empty() &&
                    ( // Empty the stack after processing all the items.
                        size == index
                        // The height is smaller than previous.
                        || heights[index] < heights[sta.top()]))
                {
                    const auto top = sta.top();
                    sta.pop();

                    const auto width = sta.empty() ? index : index - 1U - sta.top();

                    const auto temp = static_cast<long_int_t>(heights[top] * static_cast<long_int_t>(width));

                    if (!(result < temp))
                    {
                        continue;
                    }

                    result = temp;
                    start_index = top;
                    start_height = heights[top];
                    start_width = width;
                }

                if (size != index)
                {
                    sta.push(index);
                }
            }

            if (shall_compute_start_index && long_int_t{} < result)
            {
                fix_start_index(heights, start_height, start_index);
            }

            return result;
        }

        [[nodiscard]] static constexpr auto compute_slow(const std::vector<int_t> &heights, std::size_t &start_index,
            int_t &start_height, std::size_t &start_width, const bool shall_compute_start_index = true) -> long_int_t
        {
            const auto size = heights.size();
            if (0U == size) [[unlikely]]
            {
                throw std::runtime_error("The 'heights' must have at least one element.");
            }

            start_index = 0;
            start_height = 0;
            start_width = 0;

            long_int_t result{};

            for (std::size_t index{}; index < size; ++index)
            {
                const auto &current = heights[index];
                long_int_t width = 1;

                if (const auto can_go_left = 0U < index; can_go_left)
                {
                    auto go_left = index - 1U;

                    while (current <= heights[go_left])
                    {
                        ++width;

                        if (0U == go_left--)
                        {
                            break;
                        }
                    }
                }

                auto go_right = index + 1U;

                while (go_right < size && current <= heights[go_right])
                {
                    ++width;
                    ++go_right;
                }

                const auto temp = static_cast<long_int_t>(static_cast<long_int_t>(current) * width);

                if (result < temp)
                {
                    result = temp;
                    start_index = index;
                    start_height = heights[start_index];

                    start_width = static_cast<std::size_t>(width);
                }
            }

            if (shall_compute_start_index && long_int_t{} < result)
            {
                fix_start_index(heights, start_height, start_index);
            }

            return result;
        }

private:
        // Sometimes the "startIndex" must be moved left.
        inline static constexpr void fix_start_index(
            const std::vector<int_t> &heights, const int_t &start_height, std::size_t &start_index)
        {
            if constexpr (::Standard::Algorithms::is_debug)
            {
                if (int_t{} == start_height) [[unlikely]]
                {
                    throw std::runtime_error("0 == startHeight in FixStartIndex");
                }
            }

            while (0U < start_index && start_height <= heights[start_index - 1U])
            {
                --start_index;
            }
        }
    };
} // namespace Standard::Algorithms::Numbers
