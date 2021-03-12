#pragma once
#include <cassert>
#include <type_traits>
#include <vector>
#include "../Utilities/ExceptionUtilities.h"
#include <complex>

namespace
{
    template <typename int_t>
    void check_non_negative(const std::vector<int_t>& arr)
    {
        static_assert(std::is_signed_v<int_t>);
        RequireGreater(arr.size(), 0, "array size");

        int_t sum{};
        for (const auto& v : arr)
        {
            RequireNonNegative(v, "array item");
            if (sum + v < sum)
                throw std::overflow_error("Numbers sum is overflown.");
            sum += v;
        }
    }

    template<typename int_t>
    void slow_helper(const std::vector<int_t>& arr,
        std::vector<bool>& negative_signs,
        std::vector<bool>& temp,
        int_t& min_sum,
        const int_t cur_sum,
        const int_t ind)
    {
        static_assert(std::is_signed_v<int_t>);
        if (ind == int_t(0))
        {
            const auto m = static_cast<int_t>(abs(cur_sum));
            if (m >= min_sum)
                return;

            min_sum = m;
            copy(temp.begin(), temp.end(), negative_signs.begin());
            return;
        }

        const auto& v = arr[ind - 1];
        temp[ind - 1] = false;
        slow_helper(arr,
            negative_signs,
            temp,
            min_sum,
            static_cast<int_t>(cur_sum + v),
            static_cast<int_t>(ind - 1));

        temp[ind - 1] = true;
        slow_helper(arr,
            negative_signs,
            temp,
            min_sum,
            static_cast<int_t>(cur_sum - v),
            static_cast<int_t>(ind - 1));
    }
}

namespace Privet
{
    namespace Algorithms
    {
        namespace Numbers
        {
            // An array A of non-negatibe numbers should be split into 2 disjoint subsets B, C
            // s.t. A = B+C, and |sum(B) - sum(C)| is minimum.
            template<typename int_t>
            int_t two_subsets_min_diff_knapsack_slow(const std::vector<int_t>& arr,
                std::vector<bool>& negative_signs)
            {
                check_non_negative<int_t>(arr);

                int_t min_sum = std::numeric_limits<int_t>::max();
                negative_signs.assign(arr.size(), false);

                auto negative_signs_temp = negative_signs;

                slow_helper(arr,
                    negative_signs,
                    negative_signs_temp,
                    min_sum,
                    static_cast<int_t>(0),
                    static_cast<int_t>(arr.size()));

                return min_sum;
            }

            template<typename int_t>
            int_t two_subsets_min_diff_knapsack(const std::vector<int_t>& arr,
                std::vector<std::vector<int_t>>& temp,
                std::vector<bool>& negative_signs)
            {
                check_non_negative<int_t>(arr);

                temp.resize(2 + arr.size());

                int_t cur_sum{};
                temp[1].assign(1, cur_sum);

                for (int_t i = 0; i < static_cast<int_t>(arr.size()); cur_sum += arr[i], ++i)
                {
                    const auto& prev = temp[i + 1];
                    const auto& v = arr[i];
                    auto& cur = temp[i + 2];
                    cur.assign(1 + cur_sum + v, static_cast<int_t>(-1));

                    for (int_t s = 0; s <= cur_sum; ++s)
                    {
                        if (prev[s] < static_cast<int_t>(0))
                            continue;

                        const auto d = abs(s - v);
                        const auto pl = s + v;
                        cur[d] = s;
                        cur[pl] = s;
                    }
                }

                int_t rem = 0;
                {
                    const auto& cur = temp[arr.size() + 1];
                    while (cur[rem] < static_cast<int_t>(0))
                    {
                        ++rem;
                        assert(rem <= cur_sum);
                    }
                }

                negative_signs.resize(arr.size());
                const auto min_sum = rem;
                for (int_t i = static_cast<int_t>(arr.size() - 1); i >= 0; --i)
                {
                    const auto& v = arr[i];
                    const auto arem = abs(rem);
                    const auto& cur = temp[i + 2][arem];
                    const auto pl = abs(rem + v);

                    const auto d = abs(rem - v);
                    assert(pl == cur || d == cur);

                    const auto sign = pl != cur;
                    negative_signs[i] = sign;
                    rem += v * (sign ? -1 : 1);
                }

                assert(rem == 0);
                return min_sum;
            }
        }
    }
}