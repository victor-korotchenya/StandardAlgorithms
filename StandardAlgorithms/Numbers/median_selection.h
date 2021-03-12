#pragma once
#include"sort.h"

namespace Standard::Algorithms::Numbers::Inner
{
    constexpr std::size_t partition_number = 5;

    // Linear time O(n).
    template<class item_t, class less_t, std::size_t extent = std::dynamic_extent>
    [[nodiscard]] constexpr auto partite(std::span<item_t, extent> spa, less_t less1)
        // todo(p4): noexcept()
        -> std::size_t
    {
        const auto size = spa.size();
        assert(0U < size && spa.data() != nullptr);

        const auto partitions = size / partition_number + ((size % partition_number) != 0U ? 1U : 0U);

        assert(0U < partitions);

        std::size_t left{};
        auto right = partition_number - static_cast<std::size_t>(1);

        for (std::size_t part{}; part < partitions; ++part, left += partition_number, right += partition_number)
        {
            right = std::min(right, spa.size() - 1U);

            const auto diff = right - left;
            assert(left <= right && diff < partition_number);

            const auto median_index = diff >> 1U;
            {
                auto sub = spa.subspan(left, diff + 1U);

                insertion_sort<item_t, less_t, extent>(sub, less1);
            }

            // Move the median to the array beginning.
            std::swap(spa[part], spa[median_index]);
        }

        return partitions;
    }

    // Time O(n).
    template<class item_t, class less_t = std::less<item_t>, std::size_t extent = std::dynamic_extent>
    [[nodiscard]] constexpr auto count_less_equal(const std::span<item_t, extent> spa, const item_t &median,
        less_t less1 = {}) noexcept(noexcept(std::declval<less_t>()(std::declval<item_t>(), std::declval<item_t>())))
        -> std::pair<std::size_t, std::size_t>
    {
        std::size_t lesser_size{};
        std::size_t equal_size{};

        for (const auto &cur : spa)
        {
            if (less1(cur, median))
            {
                assert(lesser_size < spa.size());

                ++lesser_size;
            }
            else if (const auto are_equal = !less1(median, cur); are_equal)
            {// (cur == median)
                assert(equal_size < spa.size());

                ++equal_size;
            }
        }

        return { lesser_size, equal_size };
    }

    // Time O(n).
    template<class item_t, class less_t, std::size_t extent = std::dynamic_extent>
    constexpr void smaller_to_beginning(
        const std::size_t lesser_size, std::span<item_t, extent> spa, const std::size_t median_index, less_t less1)
    {
        assert(0U < lesser_size && lesser_size < spa.size() && median_index < spa.size());

        std::size_t smaller_index{};
        {
            const auto &medium = spa[median_index];

            while (less1(spa[smaller_index], medium))
            {
                if (lesser_size == ++smaller_index)
                {
                    return;
                }
            }
        }

        const auto medium = spa[median_index];

        for (;;)
        {
            if (auto &back = spa.back(); less1(back, medium))
            {
                std::swap(spa[smaller_index], back);

                if (lesser_size == ++smaller_index)
                {
                    return;
                }

                while (less1(spa[smaller_index], medium))
                {
                    if (lesser_size == ++smaller_index)
                    {
                        return;
                    }
                }
            }

            assert(1U < spa.size());

            spa = spa.subspan(0U, spa.size() - 1U);
        }
    }

    // Time O(n).
    template<class item_t, class less_t, std::size_t extent = std::dynamic_extent>
    constexpr void greater_to_ending(
        const std::size_t greater_size, std::span<item_t, extent> spa, const std::size_t median_index, less_t less1)
    {
        assert(0U < greater_size && greater_size < spa.size());

        const auto greater_size_last_index = spa.size() - greater_size;

        auto greater_index = spa.size() - 1U;
        {
            const auto &medium = spa[median_index];

            while (less1(medium, spa[greater_index]))
            {
                if (greater_size_last_index == greater_index--)
                {
                    return;
                }
            }
        }

        const auto medium = spa[median_index];
        std::size_t left_index{};

        for (;;)
        {
            if (auto &left_item = spa[left_index]; less1(medium, left_item))
            {
                std::swap(spa[greater_index], left_item);

                if (greater_size_last_index == greater_index)
                {
                    return;
                }

                assert(0U < greater_index);

                while (less1(medium, spa[--greater_index]))
                {
                    if (greater_size_last_index == greater_index)
                    {
                        return;
                    }

                    assert(0U < greater_index);
                }
            }

            ++left_index;
            assert(left_index < spa.size());
        }
    }
} // namespace Standard::Algorithms::Numbers::Inner

namespace Standard::Algorithms::Numbers
{
    // todo(p4): partial_sort_copy

    template<class item_t, class less_t = std::less<item_t>, std::size_t extent = std::dynamic_extent>
    struct median_selection final
    {
        median_selection() = delete;

        // 5 partitions; linear running time O(n).
        // See also "quick_select_kth_smallest".
        // Note. Median selection can be used when sort is not required e.g. fractional knapsack in time O(n).
        [[nodiscard]] static constexpr auto select_order_statistic(
            std::span<item_t, extent> spa, const std::size_t zero_based_statistic_order, less_t less1 = {}) -> item_t &
        {
            throw_if_null("data in select_order_statistic", spa.data());

            const auto size = require_positive(spa.size(), "data size in select_order_statistic");

            require_greater(size, zero_based_statistic_order, "zero-based statistic order");

            const auto selection_index = select_impl(spa, zero_based_statistic_order, less1);

            assert(selection_index < size);

            auto &result = spa[selection_index];
            return result;
        }

private:
        [[nodiscard]] static constexpr auto select_impl(
            std::span<item_t, extent> spa, std::size_t zero_based_statistic_order, less_t less1
#if _DEBUG
            ,
            const std::int32_t stack_depth = {}
#endif
            ) -> std::size_t
        {
#if _DEBUG
            {
                // log(10**65) / log(5) < 93.
                static constexpr auto stack_max_size = 100;

                require_greater(stack_max_size, stack_depth, "stack depth in median_selection");
            }
#endif

            // To have fewer recursive calls.
            std::size_t shift{};

            for (;;)
            {
                assert(spa.data() != nullptr && zero_based_statistic_order < spa.size());

                if (spa.size() <= Inner::partition_number)
                {
                    insertion_sort<item_t, less_t, extent>(spa, less1);

                    auto result = shift + zero_based_statistic_order;
                    return result;
                }

                const auto partitions = Inner::partite<item_t, less_t, extent>(spa, less1);

                auto sparta = spa.subspan(0U, partitions);

                // There should be no more than Log(n)/Log(Inner::partition_number) + 1 recursive calls.
                const auto median_of_medians_index = select_impl(sparta, partitions >> 1U, less1
#if _DEBUG
                    ,
                    stack_depth + 1
#endif
                );

                assert(median_of_medians_index < spa.size());

                // todo(p4): join count partitions + smaller_to_beginning + greater_to_ending.

                const auto &median = spa[median_of_medians_index];

                const auto [lesser_size, equal_size] =
                    Inner::count_less_equal<item_t, less_t, extent>(spa, median, less1);

                assert(lesser_size < spa.size() && 0U < equal_size && equal_size <= spa.size());

                if (zero_based_statistic_order < lesser_size)
                {
                    Inner::smaller_to_beginning<item_t, less_t, extent>(
                        lesser_size, spa, median_of_medians_index, less1);

                    spa = spa.subspan(0U, lesser_size);
                    continue;
                }

                const auto new_start = lesser_size + equal_size;

                if (zero_based_statistic_order < new_start)
                {
                    return shift + median_of_medians_index;
                }

                const auto greater_size = spa.size() - new_start;

                assert(0U < greater_size && greater_size < spa.size());

                Inner::greater_to_ending<item_t, less_t, extent>(greater_size, spa, median_of_medians_index, less1);

                shift += new_start;
                zero_based_statistic_order -= new_start;
                spa = spa.subspan(new_start, spa.size() - new_start);
            }
        }
    };

    // median_selection_large_external_file.h
    // A large file of n records cannot fit in RAM: can read into the memory only m records, m << n.
    // Need to find k min records, k < m.
    // If a file could fit in the main memory, time O(n).
    // Use a heap, time O(n*log(k)).
    // todo(p3): Use 2 arrays of size m/2, read in O(2n/m) blocks, after block is read, leave only k min records, time
    // O(n).
    //
    // todo(p3): Find k-th statistic in B bytes disk file using sqrt(B) memory.
} // namespace Standard::Algorithms::Numbers
