#pragma once
#include"../Utilities/is_debug.h"
#include<algorithm>
#include<cassert>
#include<compare>
#include<cstddef>
#include<utility>
#include<vector>

namespace Standard::Algorithms::Numbers::Inner
{
    // It is expected to run faster than std::equal_range. todo(p3): Measure.
    // The element must belong to the array.
    [[nodiscard]] constexpr auto own_equal_positions(const auto &arr, const auto &element) noexcept
        -> std::pair<std::size_t, std::size_t>
    {
        if constexpr (::Standard::Algorithms::is_debug)
        {
            assert(std::is_sorted(arr.begin(), arr.end()));
        }

        const auto size = static_cast<std::size_t>(arr.size());
        assert(0 < size);
        // [[assume(0 < size)]];

        const auto *const begin = &(arr[0]);
        const auto *const end = begin + size;

        assert(begin <= &element && &element < end);
        // [[assume(begin <= &element && &element < end)]];

        auto left = &element - begin;

        for (auto step = left; 0 < step;)
        {
            while (step <= left && element == *(begin + left - step))
            {// Maximum 2 iterations.
                left -= step;
            }

            step = std::min(step / 2, left);
        }

        auto right_incl = (end - 1) - &element;
        assert(0 <= right_incl);

        for (auto step = right_incl; 0 < step;)
        {
            while (step <= right_incl && element == *(end - 1 - right_incl + step))
            {// Maximum 2 iterations.
                right_incl -= step;
            }

            step = std::min(step / 2, right_incl);
        }

        auto left_pos = static_cast<std::size_t>(left);
        auto right_pos = static_cast<std::size_t>(size - right_incl);

        return std::make_pair(left_pos, right_pos);
    }

    [[nodiscard]] constexpr auto equal_positions(const auto &arr, const auto &ele_ment) noexcept
        -> std::pair<std::size_t, std::size_t>
    {
        if constexpr (::Standard::Algorithms::is_debug)
        {
            assert(std::is_sorted(arr.begin(), arr.end()));
        }

        if (const auto *const ptr = &ele_ment; !arr.empty() && &(arr[0]) <= ptr && ptr < &(arr[0]) + arr.size())
        {
            return own_equal_positions(arr, ele_ment);
        }

        const auto begin = arr.begin();
        const auto end = arr.end();
        const auto [first, second] = std::equal_range(begin, end, ele_ment);

        const auto left = static_cast<std::size_t>(first - begin);
        const auto right = static_cast<std::size_t>(second - begin);

        assert(left <= right && right <= arr.size());

        return std::make_pair(left, right);
    }

    // todo(p4): Semper fi.
    [[nodiscard]] constexpr auto mid_element(const auto &arr_0, const std::pair<std::size_t, std::size_t> &takes,
        const auto &arr_1, const auto &gett) noexcept(!::Standard::Algorithms::is_debug) -> const auto &
    {
        const auto [take_0, take_1] = takes;

        const auto size_0 = static_cast<std::size_t>(arr_0.size());
        const auto size_1 = static_cast<std::size_t>(arr_1.size());

        assert(0 < size_0 && size_0 <= size_1 && take_0 <= size_0 && take_1 <= size_1);

        // [[assume(0 < size_0 && size_0 <= size_1 && take_0 <= size_0 && take_1 <= size_1)]];

        if (0 == take_0)
        {
            assert(0 < take_1);

            if (((size_0 + size_1) & 1U) != 0U)
            {// sizes: 2 + 3, or 5 + 10
                return gett(arr_1, take_1 - 1);
            }

            if (size_0 < size_1)
            {// sizes: 1 + 3, 2 + 4
                assert(take_1 < size_1);
                // [[assume(take_1 < size_1)]];

                return std::min(gett(arr_0, 0), gett(arr_1, take_1));
            }

            return gett(arr_0, 0);
        }

        if (0 == take_1)
        {
            assert(size_0 == size_1);
            return gett(arr_1, 0);
        }

        if (((size_0 + size_1) & 1U) != 0U)
        {
            const auto &median0 = gett(arr_0, take_0 - 1);
            const auto &median1 = gett(arr_1, take_1 - 1);
            return std::max(median0, median1);
        }

        assert(take_1 < size_1);
        // [[assume(take_1 < size_1)]];

        const auto &median1 = gett(arr_1, take_1);

        if (take_0 < size_0)
        {
            const auto &median0 = gett(arr_0, take_0);
            return std::min(median0, median1);
        }

        const auto &median0 = gett(arr_0, take_0 - 1);
        return std::max(median0, median1);
    }
} // namespace Standard::Algorithms::Numbers::Inner

namespace Standard::Algorithms::Numbers
{
    // Given a sorted array, find the median element positions.
    // Duplicates are allowed.
    // Time O(log(n+m)).
    // E.g. {10, 20, 30, 30, 50}, size = 5, mid = 2, element = 30, result = {2, 4}
    // {10, 20, 30, 30} -> {2, 4}
    // {10, 20, 30} -> {1, 2}
    // {10, 20} -> {1, 2}
    // {10} -> {0, 1}
    // An empty array {} produces {0, 0}.
    [[nodiscard]] constexpr auto sorted_array_median_element(const auto &arr) noexcept
        -> std::pair<std::size_t, std::size_t>
    {
        const auto size = static_cast<std::size_t>(arr.size());
        if (0ZU == size)
        {
            return {};
        }

        const auto mid_index = size >> 1U;
        const auto &median = arr[mid_index];

        auto res = Inner::equal_positions(arr, median);

        assert(res.first <= mid_index && res.first < res.second && res.second <= arr.size());

        return res;
    }

    // Given n=2 sorted arrays, find the median element positions in the virtually merged arrays.
    // The median in a non-empty array = array[array.size()/2].
    // Duplicates are allowed.
    // Time O(log(min(n, m))).
    // E.g. {10} and {20, 30, 30, 50} will return {{1, 1}, {1, 3}}.
    // {10} and {20} -> {{1, 1}, {0, 1}}.
    // {20} and {10} -> {{0, 1}, {1, 1}}.
    [[nodiscard]] constexpr auto sorted_arrays_median_element(const auto &arr_0, const auto &arr_1) noexcept(
        !::Standard::Algorithms::is_debug) -> std::pair<std::pair<std::size_t, std::size_t>,
        // (begin, end) in each array
        std::pair<std::size_t, std::size_t>>
    {
        const auto size_0 = static_cast<std::size_t>(arr_0.size());
        const auto size_1 = static_cast<std::size_t>(arr_1.size());

        if (size_1 < size_0)
        {
            auto rev = sorted_arrays_median_element(arr_1, arr_0);
            return std::make_pair(rev.second, rev.first);
        }

        if constexpr (::Standard::Algorithms::is_debug)
        {
            assert(std::is_sorted(arr_0.begin(), arr_0.end()) && std::is_sorted(arr_1.begin(), arr_1.end()));
        }

        if (arr_0.empty())
        {
            return std::make_pair(std::make_pair(0ZU, 0ZU), sorted_array_median_element(arr_1));
        }

        const auto gett = []<typename typ>(const typ &arr, const std::size_t ind) noexcept(
            !::Standard::Algorithms::is_debug) -> const auto &
        {
            assert(ind < static_cast<std::size_t>(arr.size()));

            if constexpr (constexpr bool has_at = requires(const typ &temp) { temp.at(0); };
                          ::Standard::Algorithms::is_debug && has_at)
            {
                return arr.at(ind);
            }

            return arr[ind];
        };

        // (5 + 14)/2 = 9 = index, but (5 + 14 + 1)/2 = 10 = half items to take.
        const auto half_count = (size_0 + 1U + size_1) >> 1U;

        std::size_t take_0{}; // How many items to take from the arrays in order
        std::size_t take_1{}; // to assemble the first half?

        for (std::size_t left{}, right = size_0;;)
        {// Binary search.
            take_0 = (right - left) / 2U + left;
            take_1 = half_count - take_0;

            assert(left <= right && right <= size_0 && take_0 <= half_count);

            if (0 < take_1 && take_0 < size_0)
            {
                const auto &ele_0 = gett(arr_0, take_0);
                const auto &ele_1 = gett(arr_1, take_1 - 1);

                if (ele_0 < ele_1)
                {
                    left = take_0 + 1;
                    continue;
                }
            }

            if (0 < take_0 && take_1 < size_1)
            {
                const auto &ele_0 = gett(arr_0, take_0 - 1);
                const auto &ele_1 = gett(arr_1, take_1);

                if (ele_1 < ele_0)
                {
                    right = take_0 - 1;
                    continue;
                }
            }

            break;
        }

        const auto &mid_ele_ment = Inner::mid_element(arr_0, std::make_pair(take_0, take_1), arr_1, gett);

        auto first = Inner::equal_positions(arr_0, mid_ele_ment);
        auto second = Inner::equal_positions(arr_1, mid_ele_ment);
        return std::make_pair(first, second);
    }

    // Slow time O(n+m).
    template<class item_t>
    [[nodiscard]] constexpr auto sorted_arrays_median_element_slow(const auto &arr_0, const auto &arr_1)
        -> std::pair<std::pair<std::size_t, std::size_t>, std::pair<std::size_t, std::size_t>>
    {
        const auto size_0 = static_cast<std::size_t>(arr_0.size());
        const auto size_1 = static_cast<std::size_t>(arr_1.size());

        if (0U == size_0 && 0U == size_1)
        {
            return {};
        }

        if (!std::is_sorted(arr_0.begin(), arr_0.end())) [[unlikely]]
        {
            throw std::runtime_error("0-th array must be sorted.");
        }

        if (!std::is_sorted(arr_1.begin(), arr_1.end())) [[unlikely]]
        {
            throw std::runtime_error("1-st array must be sorted.");
        }

        std::vector<item_t> burger;
        burger.reserve(size_0 + size_1);

        std::merge(arr_0.begin(), arr_0.end(), arr_1.begin(), arr_1.end(), std::back_inserter(burger));

        if (!std::is_sorted(burger.cbegin(), burger.cend())) [[unlikely]]
        {
            throw std::runtime_error("The merged array must be sorted.");
        }

        const auto mid_index = burger.size() >> 1U;
        const auto &median = burger.at(mid_index);

        auto first = Inner::equal_positions(arr_0, median);
        auto second = Inner::equal_positions(arr_1, median);

        return std::make_pair(first, second);
    }
} // namespace Standard::Algorithms::Numbers
