#pragma once
#include"../Utilities/require_utilities.h"
#include"gcd.h"
#include<vector>

namespace Standard::Algorithms::Numbers
{
    // Values must be small.
    // Time O(n*max_value).
    template<std::unsigned_integral long_int_t, std::unsigned_integral int_t>
    requires(sizeof(int_t) <= sizeof(long_int_t) && sizeof(int_t) <= sizeof(std::size_t))
    [[nodiscard]] constexpr auto max_gcd_pair_in_array(const std::vector<int_t> &values) -> int_t
    {
        const auto size = require_greater(values.size(), 1U, "size");

        const auto &max_val = static_cast<std::size_t>(*std::max_element(values.cbegin(), values.cend()));

        {
            constexpr auto greater1 = std::numeric_limits<std::size_t>::max() / 3U;

            require_greater(greater1, max_val, "max value");
        }

        // std::map<std::int32_t, std::int32_t> counts;
        std::vector<std::size_t> counts(max_val + 1U);

        for (std::size_t index{}; index < size; ++index)
        {
            const auto &item = values[index];
            const auto cas = static_cast<std::size_t>(item);

            ++counts.at(cas);
        }

        if (0U < counts[0])
        {
            return static_cast<int_t>(max_val);
        }

        for (auto index = max_val; 0U < index; --index)
        {
            auto ind_2 = static_cast<long_int_t>(index);
            std::int32_t small{};

            do
            {
                const auto &cnt = counts[ind_2];

                if (1U < cnt || (0U < cnt && 2 == ++small))
                {
                    return static_cast<int_t>(index);
                }
            } while ((ind_2 += index) <= max_val);
        }

        return {};
    }

    // Time O(n*n).
    template<std::unsigned_integral int_t>
    [[nodiscard]] constexpr auto max_gcd_pair_in_array_slow(const std::vector<int_t> &values) -> int_t
    {
        const auto size = require_greater(values.size(), 1U, "size");

        int_t result{};

        for (std::size_t index{}; index < size - 1U; ++index)
        {
            const auto &one = values[index];

            for (auto ind_2 = index + 1U; ind_2 < size; ++ind_2)
            {
                const auto &two = values[ind_2];

                const auto gcd1 = static_cast<int_t>(gcd_unsigned(one, two));

                result = std::max(result, gcd1);
            }
        }

        return result;
    }
} // namespace Standard::Algorithms::Numbers
