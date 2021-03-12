#pragma once
#include"../Utilities/require_utilities.h"
#include"../Utilities/same_sign_leq_size.h"
#include<unordered_map>
#include<vector>

namespace Standard::Algorithms::Numbers::Inner
{
    template<std::integral long_int_t, std::integral int_t>
    requires(same_sign_leq_size<int_t, long_int_t>)
    [[nodiscard]] constexpr auto restore_subsequence(const std::int32_t len_max, const std::vector<int_t> &arr,
        std::int32_t pos_max, const long_int_t &dif) -> std::vector<int_t>
    {
        assert(2 <= len_max && len_max - 1 <= pos_max);

        require_less_equal(len_max, static_cast<std::int32_t>(arr.size()), "length max");

        std::vector<int_t> result;
        result.reserve(len_max);

        result.push_back(arr.at(pos_max));

        while (0 <= --pos_max)
        {
            const auto &item = arr[pos_max];

            if (result.back() - dif == item)
            {
                result.push_back(item);
            }
        }

        assert(static_cast<std::int32_t>(result.size()) == len_max);

        std::reverse(result.begin(), result.end());

        return result;
    }
} // namespace Standard::Algorithms::Numbers::Inner

namespace Standard::Algorithms::Numbers
{
    constexpr auto longest_ari_progr_max_size = 10'000U;

    // Find a longest arithmetic progression subsequence in a sorted array.
    // An arithmetic progression is a sequence a0, a0 + dif, a0 + 2*dif, a0 + 3*dif, a0 + 4*dif, ..
    // where dif might be negative, 0, or positive.
    // 0, 1, or 2 numbers always form a progression.
    // 3 or more numbers must satisfy for all i (0 < i < n-1):
    // a[i+1] - a[i] == a[i] - a[i-1],
    // or:
    // a[i+1] + a[i-1] == 2*a[i].

    // todo(p2): finish? Time O(n**2), space O(n).
    /*
    template<std::integral long_int_t, std::integral int_t>
    requires(same_sign_leq_size<int_t, long_int_t>)
    [[nodiscard]]  constexpr auto longest_arithmetic_progression_in_sorted(const std::vector<int_t>& arr) ->
    std::vector<int_t>
    {
        require_less_equal(arr.size(), longest_ari_progr_max_size, "array size");

        const auto size = static_cast<std::int32_t>(arr.size());
        if (size <= 2)
        {
            return arr;
        }

        assert(arr[0] <= arr[1] && arr[1] <= arr[2] && arr[2] <= arr.back() && arr[size - 2] <= arr.back());

        auto len_max = 2;
        auto pos_prev = 0;
        auto pos_max = 1;

        for (std::int32_t index{}; index < size - 1; ++index)
        {// Try zero difference
            const auto old = index;

            while (index < size - 1 && arr[index] == arr[index + 1])
            {
                ++index;
            }

            const auto cand = index - old + 1;
            if (len_max < cand)
            {
                len_max = cand, pos_prev = index - 1, pos_max = index;
            }
        }

        if (const auto cand = 2LL * static_cast<std::int64_t>(len_max);
            size < cand) // Must be strictly less. Consider {0,0,1,2}. Here |{0,0}| = 2 < 3 == {0,1,2}.
        {
            constexpr long_int_t dif_max{};
            auto result1 = Inner::restore_subsequence<long_int_t, int_t>(len_max, arr, pos_max, dif_max);

            return result1;
        }

        // ?????

        const auto dif_max = static_cast<long_int_t>(static_cast<long_int_t>(arr[pos_max]) - arr[pos_prev]);

        auto next_item = arr[pos_max] + dif_max;

        for (auto pos = pos_max + 1; pos < size; ++pos)
        {// Ensure that pos_max is actually the maximum position for the given difference.
            const auto &item = arr[pos];
            if (item == next_item)
            {
                pos_max = pos;
                next_item += dif_max;
            }
        }

        auto result = Inner::restore_subsequence<long_int_t, int_t>(len_max, arr, pos_max, dif_max);

        return result;
    }*/

    // Slow time, space O(n**2).
    template<std::integral long_int_t, std::integral int_t>
    requires(same_sign_leq_size<int_t, long_int_t>)
    [[nodiscard]] constexpr auto longest_arithmetic_progression_in_sorted_slow2(const std::vector<int_t> &arr)
        -> std::vector<int_t>
    {
        require_less_equal(arr.size(), longest_ari_progr_max_size, "array size");

        const auto size = static_cast<std::int32_t>(arr.size());
        if (size <= 2)
        {
            return arr;
        }

        assert(arr[0] <= arr[1] && arr[1] <= arr[2] && arr[2] <= arr.back() && arr[size - 2] <= arr.back());

        auto len_max = 2;
        auto pos_max = 1;
        auto dif_max = static_cast<long_int_t>(static_cast<long_int_t>(arr[1]) - arr[0]);

        // Given an index,
        std::vector<
            // and a difference -> max length.
            std::unordered_map<long_int_t, std::int32_t>>
            buffers(size);

        for (auto ind_2 = 1; ind_2 < size; ++ind_2)
        {
            const auto &item_2 = arr[ind_2];

            assert(arr[ind_2 - 1] <= item_2);

            auto &buf_cur = buffers[ind_2];

            for (auto index = ind_2 - 1; 0 <= index; --index)
            {
                const auto &item = arr[index];

                const auto dif = static_cast<long_int_t>(static_cast<long_int_t>(item_2) - item);

                auto &cur_length = buf_cur[dif];

                const auto &bupr = buffers[index];
                const auto iter = bupr.find(dif);

                if (iter == bupr.end())
                {// When index = 0, ind_2 = 2, arr {10, 10, 10},
                    // cur_length is 3.
                    if (cur_length == 0)
                    {
                        cur_length = 2;
                    }

                    continue;
                }

                const auto &prev_max_length = iter->second;
                assert(2 <= prev_max_length && prev_max_length < size);

                const auto cand = prev_max_length + 1;

                if (cand <= cur_length)
                {
                    continue;
                }

                cur_length = cand;

                if (len_max < cur_length)
                {
                    len_max = cur_length, pos_max = ind_2, dif_max = dif;
                }
            }
        }

        auto result = Inner::restore_subsequence<long_int_t, int_t>(len_max, arr, pos_max, dif_max);
        return result;
    }

    // If each item stores maximum lengths for all possible differences,
    // then maybe two items could make +1 longer progression.
    // 2, x, 5 // {2,5} item 5 has difference 3, max length 2
    // 2, x, 5, y, 8 // {2,5,8} 8 has {dif 6, len 2}, {dif 3, len 3}
    // 2, x, 5, y, 8, z, 11 // {2,5,8,11} 11 has {dif 3, len 4}
    // O(n*n) space.
    // It is 30 per cent slower than longest_arithmetic_progression_in_sorted_slow2.
    template<std::integral long_int_t, std::integral int_t>
    requires(same_sign_leq_size<int_t, long_int_t>)
    [[nodiscard]] constexpr auto longest_arithmetic_progression_in_sorted_other(const std::vector<int_t> &arr)
        -> std::vector<int_t>
    {
        require_less_equal(arr.size(), longest_ari_progr_max_size, "array size");

        const auto size = static_cast<std::int32_t>(arr.size());
        if (size <= 2)
        {
            return arr;
        }

        assert(arr[0] <= arr[1] && arr[1] <= arr[2] && arr[2] <= arr.back() && arr[size - 2] <= arr.back());

        auto dif_max = static_cast<long_int_t>(static_cast<long_int_t>(arr[1]) - arr[0]);
        auto len_max = 2;
        auto pos_max = 1;

        // Given a difference,
        std::unordered_map<long_int_t,
            // starting at index -> max length of a subsequence.
            std::unordered_map<std::int32_t, std::int32_t>>
            buffers;

        for (std::int32_t index{}; index < size - 1; ++index)
        {
            for (auto ind_2 = index + 1; ind_2 < size; ++ind_2)
            {
                const auto dif = static_cast<long_int_t>(static_cast<long_int_t>(arr[ind_2]) - arr[index]);

                auto &buf = buffers[dif];
                auto &cur_length = buf[ind_2];

                // Try a previous item at the index.
                const auto iter = buf.find(index);
                if (iter == buf.end())
                {
                    assert(0 == cur_length || 2 == cur_length);

                    cur_length = 2;
                    continue;
                }

                const auto &prev_max_length = iter->second;

                assert(
                    0 < index && 2 <= prev_max_length && prev_max_length < size && cur_length <= prev_max_length + 1);

                cur_length = prev_max_length + 1;

                if (len_max < cur_length)
                {
                    len_max = cur_length, pos_max = ind_2, dif_max = dif;
                }
            }
        }

        auto result = Inner::restore_subsequence<long_int_t, int_t>(len_max, arr, pos_max, dif_max);
        return result;
    }

    // Slow time O(n**3).
    template<std::integral long_int_t, std::integral int_t>
    requires(same_sign_leq_size<int_t, long_int_t>)
    [[nodiscard]] constexpr auto longest_arithmetic_progression_in_sorted_slow(const std::vector<int_t> &arr)
        -> std::vector<int_t>
    {
        require_less_equal(arr.size(), longest_ari_progr_max_size, "array size");

        const auto size = static_cast<std::int32_t>(arr.size());
        if (size <= 2)
        {
            return arr;
        }

        assert(arr[0] <= arr[1] && arr[1] <= arr[2] && arr[2] <= arr.back() && arr[size - 2] <= arr.back());

        std::vector<int_t> result{ arr[0], arr[1] };
        std::vector<int_t> buffer;

        for (std::int32_t index{}; index < size - 2; ++index)
        {// There must be at least 3 items.
            for (auto ind_2 = index + 1; ind_2 < size - 1; ++ind_2)
            {
                buffer.assign(1, arr[index]);
                buffer.push_back(arr[ind_2]);

                const auto dif = static_cast<long_int_t>(static_cast<long_int_t>(arr[ind_2]) - arr[index]);

                for (auto ind_three = ind_2 + 1; ind_three < size; ++ind_three)
                {
                    const auto &item = arr[ind_three];
                    if (buffer.back() + dif == item)
                    {
                        buffer.push_back(item);
                    }
                }

                if (result.size() < buffer.size())
                {
                    std::swap(result, buffer);
                }
            }
        }

        return result;
    }
} // namespace Standard::Algorithms::Numbers
