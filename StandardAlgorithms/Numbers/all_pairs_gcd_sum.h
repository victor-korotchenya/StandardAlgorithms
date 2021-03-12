#pragma once
#include"../Utilities/same_sign_leq_size.h"
#include<algorithm>
#include<cassert>
#include<concepts>
#include<cstddef>
#include<cstdint>
#include<stdexcept>
#include<string>
#include<vector>

namespace Standard::Algorithms::Numbers
{
    // Given a non-negative integer array {0, 15, 20, 25 },
    // return sum(gcd(a[i], a[j}} for all valid (i,j).
    // Assuming values are in [0, 1e6].
    template<std::integral long_int_t, std::integral int_t>
    requires(same_sign_leq_size<int_t, long_int_t>)
    [[nodiscard]] constexpr auto all_pairs_gcd_sum(
        const std::vector<int_t> &arr, int_t (*gcd_func)(int_t, int_t), std::vector<long_int_t> &temp) -> long_int_t
    {
        // NOLINTNEXTLINE NOLINT
        if (gcd_func) // todo(p3): del unused param: int_t(*)(int_t, int_t)
        {
        }

        if (arr.empty())
        {
            return {};
        }

        // todo(p5): std::max_element for unsigned
        const auto [min_it, max_it] = std::minmax_element(arr.cbegin(), arr.cend());
        const auto &max_val = *max_it;

        if (constexpr auto upper = 1'000'000; upper < max_val) [[unlikely]]
        {
            auto error = "A value " + std::to_string(max_val) + " cannot exceed 1e6";
            throw std::invalid_argument(error);
        }

        if constexpr (std::is_signed_v<int_t>)
        {
            const auto &min_val = *min_it;

            if (min_val < int_t{}) [[unlikely]]
            {
                auto error = "A value " + std::to_string(min_val) + " cannot be negative.";
                throw std::invalid_argument(error);
            }
        }

        temp.assign(max_val + 1LL, long_int_t{});
        assert(!temp.empty());

        for (const auto &value : arr)
        {
            auto &cnt = temp[value];
            ++cnt;
            assert(long_int_t{} < cnt);
        }

        long_int_t total_sum{};

        for (long_int_t index = 1; index <= max_val; ++index)
        {
            total_sum += static_cast<long_int_t>(index * temp[index]);

            auto ind_2 = index;

            while ((ind_2 += index) <= max_val)
            {
                temp[index] += temp[ind_2];
            }
        }

        {
            const auto &zeros = temp[0];

            total_sum *= static_cast<long_int_t>(static_cast<long_int_t>(zeros) * 2);
        }

        for (auto index = max_val; int_t{} < index; --index)
        {
            temp[index] *= temp[index];

            auto ind_2 = index;

            while ((ind_2 += index) <= max_val)
            {
                temp[index] -= temp[ind_2];
            }

            assert(long_int_t{} <= temp[index]);

            total_sum += static_cast<long_int_t>(index * temp[index]);
        }

        return total_sum;
    }

    template<std::integral long_int_t, std::integral int_t>
    requires(same_sign_leq_size<int_t, long_int_t>)
    [[nodiscard]] constexpr auto all_pairs_gcd_sum_slow(const std::vector<int_t> &arr, int_t (*gcd_func)(int_t, int_t))
        -> long_int_t
    {
        assert(gcd_func);

        long_int_t total_sum{};

        for (const auto &one : arr)
        {
            for (const auto &two : arr)
            {
                total_sum += gcd_func(one, two);
            }
        }

        return total_sum;
    }
} // namespace Standard::Algorithms::Numbers
