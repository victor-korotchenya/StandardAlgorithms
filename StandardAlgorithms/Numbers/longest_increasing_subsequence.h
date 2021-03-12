#pragma once
#include<cassert>
#include<cstddef>
#include<functional>
#include<limits>
#include<stdexcept>
#include<string>
#include<utility>
#include<vector>

namespace Standard::Algorithms::Numbers
{
    // It can be used to find the longest bitonic subsequence
    // which first increases then decreases e.g. 10,20,30,40,35,25,15,10,2.
    // Just find max(LIS[i] + LIS_reversed_arr[i] - 1) i=0..n-1.
    template<class item_t, class container_t = std::vector<item_t>, class less_t = std::less<item_t>>
    struct longest_increasing_subsequence final
    {
        longest_increasing_subsequence() = delete;

        static constexpr std::size_t one = 1;

        static constexpr auto max_size = std::numeric_limits<std::size_t>::max() - 4U;

        // Time O(n*log(n)).
        [[nodiscard]] static constexpr auto fast(const container_t &source, less_t comparer = {})
            -> std::vector<std::size_t>
        {
            const auto size = source.size();

            if (size <= 1U)
            {
                return std::vector<std::size_t>(size, 0U);
            }

            if (max_size < size) [[unlikely]]
            {
                throw std::runtime_error("Too large string size.");
            }

            std::vector<std::size_t> previous(size);

            // The "increasingSubsequence" actually starts at index 1.
            std::vector<std::size_t> increasing_subsequence(size + one);

            const auto result_length = calculate_fast(previous, source, increasing_subsequence, comparer);

            assert(0U < result_length && result_length <= size);

            const auto initial_index = increasing_subsequence.at(result_length);

            reconstruct_result_fast(initial_index, previous, result_length,
                // Let's reuse no longer used memory.
                increasing_subsequence);

            return increasing_subsequence;
        }

        // Time O(n*n).
        [[nodiscard]] static constexpr auto slow(const container_t &source, less_t comparer = {})
            -> std::vector<std::size_t>
        {
            if (source.empty())
            {
                return {};
            }

            std::vector<std::size_t> lengths(source.size());
            const auto result_length = calculate_slow(source, lengths, comparer);
            auto result = reconstruct_result_slow(lengths, result_length);

            return result;
        }

private:
        using matrix_pair_slow = std::pair<std::vector<std::size_t>, std::size_t>;

        [[nodiscard]] static constexpr auto calculate_slow(
            const container_t &source, std::vector<std::size_t> &lengths, less_t comparer) -> std::size_t
        {
            const auto size = source.size();

            std::size_t result_length{};

            for (std::size_t index{}; index < size; ++index)
            {
                auto &maxi = lengths[index];
                maxi = one; // The item by itself.

                for (std::size_t ind_2{}; ind_2 < index; ++ind_2)
                {
                    if (comparer(source[ind_2], source[index]))
                    {
                        const auto new_length = one + lengths[ind_2];
                        if (maxi < new_length)
                        {
                            maxi = new_length;
                        }
                    }
                }

                if (result_length < maxi)
                {
                    result_length = maxi;
                }
            }

            return result_length;
        }

        [[nodiscard]] static constexpr auto reconstruct_result_slow(
            const std::vector<std::size_t> &lengths, std::size_t result_length) -> std::vector<std::size_t>
        {
            assert(0U < result_length);

            std::vector<std::size_t> result(result_length);

            // Start from the end.
            auto index = lengths.size() - one;

            do
            {
                while (result_length != lengths.at(index))
                {
                    assert(0U < index);

                    --index;
                }

                --result_length;
                result.at(result_length) = index;
            } while (0U < result_length);

            return result;
        }

        [[nodiscard]] static constexpr auto calculate_fast(std::vector<std::size_t> &previous,
            const container_t &source, std::vector<std::size_t> &increasing_subsequence, less_t comparer) -> std::size_t
        {
            const auto size = source.size();

            auto result_length = one;

            for (auto index = one; index < size; ++index)
            {
                // Binary search for the largest positive "searchIndex",
                // such that 1 <= searchIndex <= resultLength
                // and source[increasingSubsequence[searchIndex - 1]] < source[index]
                const auto search_index =
                    find_largest_previous(source, result_length, increasing_subsequence, index, comparer);

                // Now, the searchIndex is 1 greater
                // than the length of the longest prefix at source[index].
                // The predecessor of source[index] is
                // the last index of the subsequence of length searchIndex - 1.
                previous[index] = increasing_subsequence[search_index - one];
                increasing_subsequence[search_index] = index;

                if (result_length < search_index)
                {
                    result_length = search_index;
                }
            }

            return result_length;
        }

        // Time O(log(n)).
        [[nodiscard]] static constexpr auto find_largest_previous(const container_t &source, std::size_t high,
            const std::vector<std::size_t> &increasing_subsequence, const std::size_t index, less_t comparer)
            -> std::size_t
        {
            assert(0U < high && high < source.size());
            assert(0U < index && index < source.size());

            const auto &cur_item = source[index];

            {// Quickly try the previous item.
                const auto &middle = high;
                const auto &previous_index = increasing_subsequence[middle];

                assert(previous_index < index);

                const auto &prev_item = source[previous_index];

                if (comparer(prev_item, cur_item))
                {
                    return middle + one;
                }
            }

            --high; // Pen-ult is the best hope.

            auto low = one;

            while (low <= high)
            {
                const auto middle = low + ((high - low) >> 1U);
                const auto &previous_index = increasing_subsequence[middle];

                const auto &prev_item = source[previous_index];

                if (comparer(prev_item, cur_item))
                {// Found a sub-sequence ending with a smaller value.
                    low = middle + one;

                    assert(0U < low);
                }
                else
                {
                    high = middle - one;
                }
            }

            assert(0U < low);

            return low;
        }

        static constexpr void reconstruct_result_fast(std::size_t index, const std::vector<std::size_t> &previous,
            std::size_t length, std::vector<std::size_t> &result)
        {
            constexpr auto some_non_zero_symbol = 0U - one;

            assert(0U < length && length <= previous.size() && 0U == previous.at(0));

            result.assign(length, some_non_zero_symbol);

            do
            {
                --length;
                result.at(length) = index;

                index = previous.at(index);
            } while (0U < length);
        }
    };
} // namespace Standard::Algorithms::Numbers
