#pragma once
#include"../Utilities/check_size.h"
#include"../Utilities/require_utilities.h"
#include<functional>
#include<stack>
#include<vector>

namespace Standard::Algorithms::Numbers
{
    // Given an array A, return max(indexMax - indexMin),
    // such that A[indexMin] < A[indexMax].
    // Return 0 when A is sorted in non-increasing order.
    template<class item_t,
        // To decrease memory use, please use the minimum possible type.
        std::integral int_t, class pair_t = std::pair<int_t, int_t>, class less_t = std::less<item_t>,
        class equal_t = std::equal_to<item_t>> // todo(p4): less is enough, del equal_to.
    struct max_index_difference_two_items final
    {
        constexpr explicit max_index_difference_two_items(less_t less1 = {}, equal_t equal2 = {})
            : Less(less1)
            , Equal(equal2)
        {
        }

        // Time O(n).
        [[nodiscard]] constexpr auto fast(const std::vector<item_t> &items) const -> pair_t
        {
            const auto size_1 = size_minus_one(items);

            if (Less(items.front(), items.back()))
            {// Quick shortcut.
                return { int_t{}, size_1 };
            }

            // Indexes of the right-most decreasing items e.g. {10, 7, 6, 2}.
            auto maximums = maximums_from_right(items);
            pair_t result{};
            int_t left{};

            for (;;)
            {
                while (left <= size_1 && Equal(items[left], items[maximums.top()]))
                {
                    if (++left < maximums.top())
                    {
                        continue;
                    }

                    // Maintain the invariant (left < right),
                    // where right is maximums.top().
                    maximums.pop();

                    if (maximums.empty())
                    {
                        return result;
                    }
                }

                const auto &right = maximums.top();

                assert(left < right && right <= size_1 && !Equal(items[left], items[right]));

                if (Less(items[left], items[right]))
                {
                    if (result.second - result.first < right - left)
                    {
                        result = { left, right };
                    }
                }
                else if (++left < right)
                {
                    continue;
                }

                // Go to the right by a potentially large step.
                maximums.pop();

                if (maximums.empty())
                {
                    return result;
                }
            }
        }

        // Time O(n*n).
        [[nodiscard]] constexpr auto slow(const std::vector<item_t> &items) const -> pair_t
        {
            const auto size_1 = size_minus_one(items);

            pair_t result{};
            int_t min_position{};

            do
            {
                auto max_position = static_cast<int_t>(min_position + 1);

                do
                {
                    if (result.second - result.first < max_position - min_position &&
                        Less(items[min_position], items[max_position]))
                    {
                        result = { min_position, max_position };
                    }
                } while (++max_position <= size_1);
            } while (++min_position < size_1);

            return result;
        }

private:
        [[nodiscard]] static constexpr auto size_minus_one(const std::vector<item_t> &items) -> int_t
        {
            const auto *const name = "items size";

            const auto size = items.size();

            {
                constexpr auto edge = 1U;

                require_greater(size, edge, name);
            }

            auto size_1 = static_cast<int_t>(
                Standard::Algorithms::Utilities::check_size<int_t>(name, size) - static_cast<int_t>(1));

            assert(int_t{} < size_1);

            return size_1;
        }

        [[nodiscard]] constexpr auto maximums_from_right(const std::vector<item_t> &items) const -> std::stack<int_t>
        {
            constexpr int_t zero{};

            auto imax = static_cast<int_t>(items.size() - 1U);
            auto index = static_cast<int_t>(items.size() - 2U);

            std::stack<int_t> maximums;
            maximums.push(imax);

            do
            {
                if (const auto &maxit = items[imax], &cur = items[index]; Less(maxit, cur))
                {
                    maximums.push(imax = index);
                }
            } while (zero < index--);

            return maximums;
        }

        less_t Less;
        equal_t Equal;
    };
} // namespace Standard::Algorithms::Numbers
