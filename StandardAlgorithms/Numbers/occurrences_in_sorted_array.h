#pragma once
#include"../Utilities/require_utilities.h"
#include"binary_search.h"
#include<numeric> // std::midpoint
#include<vector>

namespace Standard::Algorithms::Numbers
{
    // Given a sorted array A, and an element,
    // return the two indexes of the first and last occurrences of the element.
    // If the element occurs once, both indexes are equal to the index of the element.
    // If the element does not occur, both indexes are equal to -1.
    struct occurrences_in_sorted_array final
    {
        occurrences_in_sorted_array() = delete;

        using pair_t = std::pair<std::size_t, std::size_t>;

        static constexpr std::size_t minus_one = 0ZU - static_cast<std::size_t>(1);

        [[nodiscard]] inline static constexpr auto not_found_pair() noexcept -> pair_t
        {
            return { minus_one, minus_one };
        }

        // Time O(log(n)).
        template<class element_t>
        [[nodiscard]] static constexpr auto find_occurrences(
            const std::vector<element_t> &data, const element_t &element) -> pair_t
        {
            if constexpr (::Standard::Algorithms::is_debug)
            {
                Standard::Algorithms::require_sorted(data, "data");
            }

            const auto size = data.size();

            std::size_t low{};
            std::size_t middle{};
            auto high = size - 1U;

            if (0U == size || !find_helper(data, element, low, middle, high))
            {
                return not_found_pair();
            }

            auto best_low = middle;
            left_most(low, data, element, best_low);

            auto &best_high = middle;
            right_most(best_high, data, element, high);

            assert(best_low <= best_high && best_high < size);

            return { best_low, best_high };
        }

        // Slow time O(n).
        template<class element_t>
        [[nodiscard]] static constexpr auto find_occurrences_slow(
            const std::vector<element_t> &data, const element_t &element) -> pair_t
        {
            if constexpr (::Standard::Algorithms::is_debug)
            {
                Standard::Algorithms::require_sorted(data, "data");
            }

            const auto size = data.size();
            if (0U == size)
            {
                return not_found_pair();
            }

            assert(data.data() != nullptr);

            const auto spa = std::span(data.data(), data.size());
            const auto *const piter = binary_search(spa, element);

            if (piter == nullptr)
            {
                return not_found_pair();
            }

            auto low = static_cast<std::size_t>(piter - data.data());
            auto high = low;

            if constexpr (::Standard::Algorithms::is_debug)
            {
                if (!(low < size))
                {
                    auto str = ::Standard::Algorithms::Utilities::w_stream();
                    str << "Error: low(" << low << ") must be less than size(" << size << ").";

                    throw_exception(str);
                }
            }

            while (0U < low && element == data[low - 1U])
            {
                --low;
            }

            while (high + 1U < size && element == data[high + 1U])
            {
                ++high;
            }

            assert(low <= high && high < size);

            return { low, high };
        }

private:
        template<class element_t>
        [[nodiscard]] static constexpr auto find_helper(const std::vector<element_t> &data, const element_t &element,
            std::size_t &low, std::size_t &middle, std::size_t &high) -> bool
        {
            do
            {
                // [[assume(!(high < left) && high < data.size())]];
                assert(!(high < low) && high < data.size());

                middle = std::midpoint(low, high);

                const auto &candy = data[middle];

                if (element == candy)
                {
                    return true;
                }

                // todo(p3): 3-way compare
                if (element < candy)
                {
                    if (0U == middle)
                    {
                        return false;
                    }

                    high = middle - 1U;
                }
                else
                {
                    low = middle + 1U;
                }
            } while (low <= high);

            return false;
        }

        template<class element_t>
        static constexpr void left_most(
            std::size_t low, const std::vector<element_t> &data, const element_t &element, std::size_t &best)
        {
            assert(low <= best && best < data.size() && element == data[best]);

            while (low < best && element == data[best - 1U])
            {// Can go left?
                auto high = best - 1U;

                [[maybe_unused]] const auto flag = find_helper(data, element, low, best, high);

                assert(best < data.size() && element == data[best]);
                assert(flag);
            }
        }

        template<class element_t>
        static constexpr void right_most(
            std::size_t &best, const std::vector<element_t> &data, const element_t &element, std::size_t high)
        {
            assert(best <= high && high < data.size() && element == data[best]);

            while (best < high && element == data[best + 1U])
            {// Can go right?
                auto low = best + 1U;

                [[maybe_unused]] const auto flag = find_helper(data, element, low, best, high);

                assert(best < data.size() && element == data[best]);
                assert(flag);
            }
        }
    };
} // namespace Standard::Algorithms::Numbers
