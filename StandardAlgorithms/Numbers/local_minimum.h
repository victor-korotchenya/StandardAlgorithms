#pragma once
#include<cassert>
#include<concepts>
#include<cstddef>
#include<numeric> // std::midpoint
#include<vector>

namespace Standard::Algorithms::Numbers
{
    // Given an array of distinct comparable values, find a local minimum index.
    // A local minimum is less than its direct neighbors.
    // Time O(log(n)).
    template<class item_t>
    [[nodiscard]] constexpr auto local_minimum(const std::vector<item_t> &items) noexcept -> std::size_t
    {
        const auto size = items.size();

        if (size <= 1U || items[0] < items[1])
        {
            return 0U;
        }

        assert(items[1] < items[0]);

        if (items.back() < items[size - 2U])
        {
            return size - 1U;
        }

        assert(3U <= size && items[size - 2U] < items.back());

        std::size_t left = 1;
        auto right = size - 2U;

        while (left <= right)
        {
            const auto mid = std::midpoint(left, right);

            assert(0U < mid);

            const auto &prev = items[mid - 1U];
            const auto &cur = items[mid];

            if (cur < prev)
            {
                const auto &next = items[mid + 1U];

                if (cur < next)
                {
                    return mid;
                }

                assert(next < cur);

                left = mid + 1U;
            }
            else
            {
                assert(prev < cur);

                right = mid - 1U;
            }
        }

        assert(0); // Must never happen - wrong input.

        return items.size();
    }

    // Slow time O(n).
    template<class item_t>
    [[nodiscard]] constexpr auto local_minimum_slow(const std::vector<item_t> &items) noexcept -> std::size_t
    {
        auto size = items.size();

        if (size <= 1U || items[0] < items[1])
        {
            return 0U;
        }

        --size;

        if (items.back() < items[size - 1U])
        {
            return size;
        }

        assert(2U <= size);

        for (std::size_t index = 1; index < size; ++index)
        {
            const auto &prev = items[index - 1U];
            const auto &cur = items[index];
            const auto &next = items[index + 1U];

            if (cur < prev && cur < next)
            {
                return index;
            }
        }

        assert(0); // Must never happen - wrong input.

        return items.size();
    }
} // namespace Standard::Algorithms::Numbers
