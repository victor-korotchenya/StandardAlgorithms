#pragma once
#include"sorted_array_two_element_sum.h"
#include<tuple>

namespace Standard::Algorithms::Numbers
{
    // Given a sorted array and a "sum",
    // find three distinct indexes of elements summing to "sum".
    // If it is impossible, a 3-tuple of (-1) is returned.
    struct sorted_array_three_element_sum final
    {
        sorted_array_three_element_sum() = delete;

        using tuple_t = std::tuple<std::size_t, std::size_t, std::size_t>;

        // Time O(n*n).
        template<class int_t>
        [[nodiscard]] static constexpr auto fast(const std::vector<int_t> &sorted_data, const int_t &sum) -> tuple_t
        {
            if constexpr (::Standard::Algorithms::is_debug)
            {
                Standard::Algorithms::require_sorted(sorted_data, "sorted data");
            }

            const auto size = sorted_data.size();
            if (size < 3U)
            {
                return not_found_tuple();
            }

            constexpr auto not_found_pair = sorted_array_two_element_sum::not_found_pair();

            for (std::size_t offset{}; offset < size - 2U; ++offset)
            {
                const auto delta = static_cast<int_t>(sum - sorted_data[offset]);

                const auto sum2 = sorted_array_two_element_sum::fast(sorted_data, delta, offset + 1U);

                if (not_found_pair != sum2)
                {
                    return { offset, sum2.first, sum2.second };
                }
            }

            return not_found_tuple();
        }

        // Slow time O(n*n*n).
        template<class int_t>
        [[nodiscard]] static constexpr auto slow(const std::vector<int_t> &sorted_data, const int_t &sum) -> tuple_t
        {
            if constexpr (::Standard::Algorithms::is_debug)
            {
                Standard::Algorithms::require_sorted(sorted_data, "sorted data");
            }

            const auto size = sorted_data.size();
            if (size < 3U)
            {
                return not_found_tuple();
            }

            for (std::size_t index1{}; index1 < size - 2U; ++index1)
            {
                for (auto index2 = index1 + 1U; index2 < size - 1U; ++index2)
                {
                    for (auto index3 = index2 + 1U; index3 < size; ++index3)
                    {
                        const auto actual =
                            static_cast<int_t>(sorted_data[index1] + sorted_data[index2] + sorted_data[index3]);

                        if (sum == actual)
                        {
                            return { index1, index2, index3 };
                        }
                    }
                }
            }

            return not_found_tuple();
        }

private:
        [[nodiscard]] static constexpr auto not_found_tuple() noexcept -> tuple_t
        {
            constexpr auto minus_one = 0U - static_cast<std::size_t>(1);
            return { minus_one, minus_one, minus_one };
        }
    };
} // namespace Standard::Algorithms::Numbers
