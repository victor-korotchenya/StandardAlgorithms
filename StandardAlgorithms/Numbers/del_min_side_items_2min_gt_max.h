#pragma once
#include"../Utilities/same_sign_leq_size.h"
#include<cassert>
#include<cstddef>
#include<cstdint>
#include<set>
#include<type_traits>
#include<vector>

namespace Standard::Algorithms::Numbers
{
    // Given n>=0 integers,
    // remove a minimum number of items from the sides
    // so that (max < 2*min) in the remaining sub-array.
    // A single positive number satisfy the above condition.
    // Return number of deleted on the left, and right.
    // Time O(n*log(n)), space O(n).
    template<class long_int_t, class int_t>
    requires(same_sign_leq_size<int_t, long_int_t>)
    [[nodiscard]] constexpr auto del_min_side_items_2min_gt_max(const std::vector<int_t> &arr)
        -> std::pair<std::size_t, std::size_t>
    {
        const auto size = arr.size();

        std::size_t left_del{};
        auto right_del = size;

        std::multiset<int_t> mus;

        for (std::size_t left{}, right{}; right < size; ++right)
        {
            const auto &datum = arr[right];

            if (!(int_t{} < datum))
            {// Twice a negative number can never be greater
                // than self or any larger number.
                mus.clear();
                left = right + 1U;

                continue;
            }

            mus.insert(datum);

            constexpr long_int_t two = 2;

            auto mini = static_cast<long_int_t>(*mus.cbegin());
            auto maxi = static_cast<long_int_t>(*mus.crbegin());

            assert(mini < mini * two && maxi < maxi * two); // no overflow
            assert(!(maxi < mini)); // sorting problem

            while (mini * two <= maxi)
            {
                assert(left < right && 2U <= mus.size());

                {
                    const auto &min_value = arr[left];

                    auto iter = mus.find(min_value);
                    assert(iter != mus.end());

                    mus.erase(iter);
                    ++left;
                }

                mini = *mus.cbegin();

                // Given {1,9,3}, after deleting 9, must update the maximum to 3.
                maxi = *mus.crbegin();
            }

            if (const auto ri_del_cand = size - 1U - right; left + ri_del_cand < left_del + right_del)
            {
                left_del = left;
                right_del = ri_del_cand;
            }
        }

        return { left_del, right_del };
    }

    // Slow time O(n*n).
    template<class long_int_t, class int_t>
    requires(same_sign_leq_size<int_t, long_int_t>)
    [[nodiscard]] constexpr auto del_min_side_items_2min_gt_max_slow(const std::vector<int_t> &arr)
        -> std::pair<std::size_t, std::size_t>
    {
        const auto size = arr.size();
        if (size == 0U)
        {
            return {};
        }

        std::size_t left_pos{};
        std::size_t right_pos{};

        for (std::size_t index{}; index < size; ++index)
        {
            const auto &arr_index = arr[index];

            if (!(int_t{} < arr_index))
            {
                continue;
            }

            constexpr long_int_t two = 2;

            auto mini = static_cast<long_int_t>(arr_index);
            auto maxi = mini;
            auto ind_2 = index;

            do
            {
                const auto &datum = arr[ind_2];

                if (datum < mini)
                {
                    mini = datum;
                }
                else if (maxi < datum)
                {
                    maxi = datum;
                }

                assert(mini <= int_t{} || mini < mini * two); // no overflow
            } while (maxi < static_cast<long_int_t>(mini) * two && ++ind_2 < size);

            assert(index < ind_2);

            if (right_pos - left_pos < ind_2 - index)
            {
                left_pos = index;
                right_pos = ind_2;
            }
        }

        return { left_pos, size - right_pos };
    }
} // namespace Standard::Algorithms::Numbers
