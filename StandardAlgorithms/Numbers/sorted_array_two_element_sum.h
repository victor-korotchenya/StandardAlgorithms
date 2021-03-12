#pragma once
#include"../Utilities/require_utilities.h"
#include<vector>

namespace Standard::Algorithms::Numbers
{
    // Given a sorted array and a "sum",
    // find two distinct indexes of elements,
    // which sum to "sum".
    // That is, given a number k, is there a number (sum-k) in the table?
    // If it is impossible, a pair of (-1) is returned.
    //
    // If the array were not sorted,
    // a hash table could be used to achieve average O(n) time.
    struct sorted_array_two_element_sum final
    {
        sorted_array_two_element_sum() = delete;

        using pair_t = std::pair<std::size_t, std::size_t>;

        // Time O(n).
        template<class int_t>
        [[nodiscard]] static constexpr auto fast(
            const std::vector<int_t> &sorted_data, const int_t &sum, std::size_t offset = 0) -> pair_t
        {
            if constexpr (::Standard::Algorithms::is_debug)
            {
                Standard::Algorithms::require_sorted(sorted_data, "sorted data");
            }

            const auto size = sorted_data.size();
            if (size < 2U)
            {
                return not_found_pair();
            }

            auto high = size - 1U;

            while (offset < high)
            {
                const auto actual = static_cast<int_t>(sorted_data.at(offset) + sorted_data.at(high));

                if (actual < sum)
                {
                    ++offset;
                }
                else if (sum < actual)
                {
                    assert(0U < high);

                    --high;
                }
                else
                {
                    assert(actual == sum);

                    return { offset, high };
                }
            }

            return not_found_pair();
        }

        // Slow time O(n*n).
        template<class int_t>
        [[nodiscard]] static constexpr auto slow(const std::vector<int_t> &sorted_data, const int_t &sum) -> pair_t
        {
            if constexpr (::Standard::Algorithms::is_debug)
            {
                Standard::Algorithms::require_sorted(sorted_data, "sorted data");
            }

            const auto size = sorted_data.size();
            if (size < 2U)
            {
                return not_found_pair();
            }

            for (std::size_t index1{}; index1 < size - 1U; index1++)
            {
                for (auto index2 = index1 + 1U; index2 < size; index2++)
                {
                    const auto actual = static_cast<int_t>(sorted_data[index1] + sorted_data[index2]);

                    if (sum == actual)
                    {
                        return { index1, index2 };
                    }
                }
            }

            return not_found_pair();
        }

        [[nodiscard]] inline static constexpr auto not_found_pair() noexcept -> pair_t
        {
            constexpr auto minus_one = 0U - static_cast<std::size_t>(1);
            return { minus_one, minus_one };
        }
    };
} // namespace Standard::Algorithms::Numbers
