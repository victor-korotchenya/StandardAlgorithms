#pragma once
#include"../Utilities/check_size.h"
#include"../Utilities/require_utilities.h"
#include<cmath> // std::abs
#include<vector>

namespace Standard::Algorithms::Numbers::Inner
{
    template<std::signed_integral int_t>
    [[nodiscard]] constexpr auto two_subs_mind_knap_check_size(const std::vector<int_t> &arr) -> int_t
    {
        const auto *const name = "array size";

        const auto size = Standard::Algorithms::Utilities::check_size<int_t>(name, arr.size());

        require_positive(size, name);

        int_t sum{};

        for (const auto &datum : arr)
        {
            require_non_negative(datum, "array item");

            const auto old = sum;
            sum += datum;

            if (sum < old) [[unlikely]]
            {
                throw std::overflow_error("array sum is overflown.");
            }
        }

        {
            constexpr auto maxi = std::numeric_limits<int_t>::max() - 1;

            static_assert(int_t{} < maxi);

            require_greater(maxi, sum, "total sum");
        }

        return size;
    }

    template<std::signed_integral int_t>
    constexpr void two_subs_mind_knap_slow_helper(int_t &min_sum, std::vector<bool> &negative_signs, int_t ind,
        const std::vector<int_t> &arr, std::vector<bool> &temp, const int_t &cur_sum = {})
    {
        if (ind == int_t{})
        {
            auto cand = static_cast<int_t>(std::abs(cur_sum));

            if (!(cand < min_sum))
            {
                return;
            }

            min_sum = cand;
            copy(temp.cbegin(), temp.cend(), negative_signs.begin());

            return;
        }

        --ind;

        const auto &item = arr.at(ind);
        temp.at(ind) = false;

        two_subs_mind_knap_slow_helper<int_t>(
            min_sum, negative_signs, ind, arr, temp, static_cast<int_t>(cur_sum + item));

        temp.at(ind) = true;

        two_subs_mind_knap_slow_helper<int_t>(
            min_sum, negative_signs, ind, arr, temp, static_cast<int_t>(cur_sum - item));
    }
} // namespace Standard::Algorithms::Numbers::Inner

namespace Standard::Algorithms::Numbers
{
    // An array A of non-negative numbers should be split into 2 disjoint subsets B, C
    // s.t. A = B+C, and |sum(B) - sum(C)| is minimum.
    // Pseudo-polynomial time.
    template<std::signed_integral int_t>
    [[nodiscard]] constexpr auto two_subsets_min_diff_knapsack(
        const std::vector<int_t> &arr, std::vector<bool> &negative_signs) -> int_t
    {
        const auto size = Inner::two_subs_mind_knap_check_size<int_t>(arr);

        constexpr int_t zero{};
        constexpr int_t one = 1;
        constexpr int_t minus_one = -one;

        static_assert(minus_one < zero);

        std::vector<std::vector<int_t>> buffers(size + one);
        buffers.at(0).assign(one, zero);

        int_t cur_sum{};

        for (int_t index{}; index < size; ++index)
        {
            const auto &item = arr[index];
            const auto &prevs = buffers[index];

            auto &currs = buffers[index + one];
            currs.assign(cur_sum + item + one, minus_one);

            for (int_t cand{}; cand <= cur_sum; ++cand)
            {
                if (const auto unreachable1 = prevs.at(cand) < zero; unreachable1)
                {// prevs.at(cand) == minus_one
                    continue;
                }

                {
                    const auto diff = std::abs(cand - item);
                    currs.at(diff) = cand;
                }
                {
                    const auto added = cand + item;
                    currs.at(added) = cand;
                }
            }

            cur_sum += item;
        }

        int_t rem{};
        {
            const auto &currs = buffers.at(arr.size());

            while (currs.at(rem) < zero)
            {
                ++rem;

                assert(rem <= cur_sum);
            }
        }

        negative_signs.resize(size);

        const auto min_sum = rem;

        for (auto index = size - one; zero <= index; --index)
        {
            const auto &item = arr[index];
            const auto absrem = std::abs(rem);
            const auto added = std::abs(rem + item);
            [[maybe_unused]] const auto diff = std::abs(rem - item);

            const auto &cur = buffers.at(index + one).at(absrem);
            assert(added == cur || diff == cur);

            const auto sign1 = added != cur;
            negative_signs[index] = sign1;
            rem += static_cast<int_t>(item * (sign1 ? minus_one : one));
        }

        assert(rem == zero);

        return min_sum;
    }

    template<std::signed_integral int_t>
    [[nodiscard]] constexpr auto two_subsets_min_diff_knapsack_slow(
        const std::vector<int_t> &arr, std::vector<bool> &negative_signs) -> int_t
    {
        const auto size = Inner::two_subs_mind_knap_check_size<int_t>(arr);

        auto min_sum = std::numeric_limits<int_t>::max();
        negative_signs.assign(size, false);

        auto temp = negative_signs;

        Inner::two_subs_mind_knap_slow_helper<int_t>(min_sum, negative_signs, size, arr, temp);

        return min_sum;
    }
} // namespace Standard::Algorithms::Numbers
