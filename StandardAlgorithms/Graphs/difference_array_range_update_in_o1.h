#pragma once
#include"../Utilities/require_utilities.h"
#include<vector>

namespace Standard::Algorithms::Trees
{
    // todo(p3): move to Numbers.

    // Given an array, and range update operations(left, rex, val)
    // 1) run each range update in O(1).
    // 2) restore the resulting array.
    template<class value_type1>
    struct difference_array_range_update_in_o1 final
    {
        using value_type = value_type1;

        constexpr explicit difference_array_range_update_in_o1(const std::vector<value_type> &source)
            : diffs(require_positive(source.size(), "source size"))
        {
            const auto one_less_size = count() - 1;

            diffs[0] = source[0];

            for (std::int32_t index{}; index < one_less_size; ++index)
            {
                diffs[index + 1] = source[index + 1] - source[index];
            }
        }

        [[nodiscard]] constexpr auto count() const noexcept -> std::int32_t
        {
            return static_cast<std::int32_t>(diffs.size());
        }

        constexpr void update(std::int32_t left, const value_type &value, std::int32_t rex)
        {
            require_less_equal(0, left, "left");
            require_greater(rex, left, "rex");

            const auto size = count();
            require_less_equal(rex, size, "rex");

            diffs[left] += value;

            if (rex < size)
            {
                diffs[rex] -= value;
            }
        }

        constexpr void restore(std::vector<value_type> &arr)
        {
            const auto size = count();
            arr.resize(size);
            arr[0] = diffs[0];

            for (auto index = 1; index < size; ++index)
            {
                arr[index] = diffs[index] + arr[index - 1];
            }
        }

private:
        std::vector<value_type> diffs;
    };

    template<class value_type1>
    struct difference_array_slow final
    {
        using value_type = value_type1;

        constexpr explicit difference_array_slow(const std::vector<value_type> &source)
            : data(source)
        {
            require_positive(source.size(), "source size");
        }

        [[nodiscard]] constexpr auto count() const noexcept -> std::int32_t
        {
            return static_cast<std::int32_t>(data.size());
        }

        constexpr void update(std::int32_t left, const value_type &value, std::int32_t rex)
        {
            require_less_equal(0, left, "left");
            require_greater(rex, left, "rex");
            require_less_equal(rex, count(), "rex");

            for (auto index = left; index < rex; ++index)
            {
                data[index] += value;
            }
        }

        constexpr void restore(std::vector<value_type> &arr)
        {
            arr = data;
        }

private:
        std::vector<value_type> data;
    };
} // namespace Standard::Algorithms::Trees
