#pragma once
#include"arithmetic.h"
#include<vector>

namespace Standard::Algorithms::Numbers
{
    // Given an array, compute the number of inversions,
    // where an inversion is a pair (x, y) of two array items having (y < x).
    //
    // An array {1, 234, 56} has 1 inversion pair: (234, 56).
    struct array_inversion_count final
    {
        array_inversion_count() = delete;

        // Slow time O(n*n).
        template<class int_t>
        [[nodiscard]] static constexpr auto slow(const std::vector<int_t> &data) noexcept -> std::uint64_t
        {
            const auto size = data.size();
            if (size <= 1U)
            {
                return {};
            }

            std::uint64_t inv_cnt{};
            std::size_t index{};

            do
            {
                auto index_2 = index + 1U;

                do
                {
                    inv_cnt += data[index_2] < data[index] ? 1U : 0U;
                } while (++index_2 < size);
            } while (++index < size - 1U);

            return inv_cnt;
        }

        // The data must be unique, distinct, without repetition or duplicates.
        // Invalid input sample. {10, 10, 20} has no inversions, but, since the first 2 items repeat,
        // the incorrectly calculated result is (0-0) + (1-0) + (2-1) = 2.
        template<std::integral int_t, class bst_t>
        [[nodiscard]] static constexpr auto unique_data_via_bst(const std::vector<int_t> &data) -> std::uint64_t
        {
            const auto size = data.size();
            if (size <= 1U)
            {
                return {};
            }

            std::uint64_t inv_cnt{};
            bst_t tree{};

            for (std::size_t index{}; index < size; ++index)
            {
                const auto &item = data[index];
                const auto rank1 = tree.lower_bound_rank(item).second;
                assert(rank1 <= index);

                // todo(p3): use an insert hint.
                tree.insert(item);

                const auto delta = index - rank1;
                inv_cnt += delta;
            }

            return inv_cnt;
        }

        // Using merge sort, the array will be sorted, and thus modified!
        // O(n*log(n)) time and O(n) space.
        template<class int_t>
        [[nodiscard]] static constexpr auto fast_using_merge_sort(std::vector<int_t> &data) -> std::uint64_t
        {
            const auto size = data.size();
            if (size <= 1U)
            {
                return {};
            }

            std::uint64_t inv_cnt{};

            // Size|maxSubsetSize = 2|1,
            // 3|2, 4|2,
            // 5|4, 6|4, 7|4, 8|4,
            // 9|8, 10|8, 11|8, 12|8.
            const auto max_subset_size =
                static_cast<std::size_t>(round_to_greater_power_of_two(static_cast<std::uint64_t>(size)) >> 1U);

            std::size_t subset_size = 1;

            std::vector<int_t> buffer(size); // todo(p3): size/2 RAM.

            do
            {
                inv_cnt += process_subsets<int_t>(data, subset_size, buffer);

                subset_size <<= 1U;
            } while (subset_size <= max_subset_size);

            return inv_cnt;
        }

private:
        template<class int_t>
        [[nodiscard]] static constexpr auto process_subsets(
            std::vector<int_t> &data, const std::size_t subset_size, std::vector<int_t> &buffer) -> std::uint64_t
        {
            std::uint64_t inv_cnt{};

            const auto size = data.size();
            assert(0U < subset_size && subset_size < size);

            const auto two_subsets = subset_size << 1U;
            std::size_t start{};

            do
            {
                const auto mid = start + subset_size - 1U;
                const auto stop = std::min(start + two_subsets, size);

                assert(start <= mid && mid + 1U < stop);

                if (const auto is_inversion = data[mid + 1U] < data[mid]; is_inversion)
                {
                    inv_cnt += merge_impl<int_t>(data, mid, { start, stop }, buffer);
                }
                else
                {
                    if constexpr (::Standard::Algorithms::is_debug)
                    {// Skip the already sorted array merge.
                        assert(std::is_sorted(data.cbegin() + start, data.cbegin() + stop));
                    }
                }

                start += two_subsets;
            } while (start < size - subset_size);

            return inv_cnt;
        }

        // Subset1 [start, mid].
        // Subset2 [mid + 1, stop).
        template<class int_t>
        [[nodiscard]] static constexpr auto merge_impl(std::vector<int_t> &data, const std::size_t mid,
            const std::pair<std::size_t, std::size_t> &start_stop, std::vector<int_t> &buffer) -> std::uint64_t
        {
            const auto &start = start_stop.first;
            const auto &stop = start_stop.second;

            std::uint64_t inv_cnt{};

            auto end1 = mid;
            std::size_t begin_2{};
            auto index = stop - 1U;
            auto end2 = index;

            for (;;)
            {
                assert(mid < end2 && index < stop);

                if (data[end2] < data[end1])
                {
                    inv_cnt += end2 - mid;
                    buffer[index] = data[end1];

                    if (end1 == start)
                    {
                        for (std::size_t rem{}; rem < end2 - mid; ++rem)
                        {// remainder 2.
                            data[start + rem] = data[rem + mid + 1U];
                        }

                        begin_2 = start + end2 - mid;
                        break;
                    }

                    --end1;
                }
                else
                {
                    buffer[index] = data[end2];

                    if (--end2 == mid)
                    {// Skip copying the remainder 1.
                        begin_2 = end1 + 1U;
                        break;
                    }
                }

                --index;
            }

            std::copy(buffer.cbegin() + index, buffer.cbegin() + stop, data.begin() + begin_2);

            return inv_cnt;
        }
    };
} // namespace Standard::Algorithms::Numbers
