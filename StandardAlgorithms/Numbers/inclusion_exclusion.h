#pragma once
#include"../Utilities/require_utilities.h"
#include"factoring_utilities.h"
#include"gcd.h"
#include<vector>

namespace Standard::Algorithms::Numbers
{
    // In the set theory, |A or B| = |A| + |B| - |A and B|.
    // Complementary, |A and B| = |A| + |B| - |A or B|.
    //
    // |A or B or C| = |A| + |B| + |C| +
    //  - |A and B| - |A and C| - |B and C| +
    //  + |A and B and C|.
    //
    // |A and B and C| = |A or B or C| +
    // - |A| - |B| - |C| +
    // + |A and B| + |A and C| + |B and C|.


    template<std::int32_t dim, std::signed_integral int_t>
    // NOLINTNEXTLINE
    requires(5 <= dim)
    constexpr void inclusion_exclusion(std::array<int_t, dim> &arr, // todo(p2): clone, rename & test?
        const int_t &mod)
    {
        constexpr int_t zero{};
        constexpr auto two = 2;

        require_positive(mod, "mod");

        for (auto index = dim / two; two <= index; --index)
        {
            auto &curr = arr[index];
            assert(zero <= curr && curr < mod);

            for (auto ind_2 = static_cast<std::int64_t>(index) * two; ind_2 < dim; ind_2 += index)
            {
                curr -= arr[ind_2];

                if (curr < zero)
                {
                    curr += mod;
                }

                assert(zero <= curr && curr < mod);
            }
        }
    }

    template<class data_t>
    [[nodiscard]] constexpr auto all_subsets(const std::vector<data_t> &data) -> std::vector<std::vector<data_t>>
    {
        {
            constexpr auto greater1 = 30U;

            require_greater(greater1, data.size(), "too large data size");
        }

        std::vector<std::vector<data_t>> sets(1);

        const auto size = static_cast<std::int32_t>(data.size());

        for (std::int32_t index{}; index < size; ++index)
        {
            const auto &datum = data[index];
            const auto size2 = static_cast<std::int32_t>(sets.size());

            // Add e.g. {2} to {{},{1}} producing {{2},{1,2}}.
            for (std::int32_t ind_2{}; ind_2 < size2; ++ind_2)
            {
                auto temp = sets[ind_2];
                temp.resize(temp.size() + 1U);
                temp.back() = datum;

                sets.push_back(std::move(temp));
            }
        }

        return sets;
    }

    // Count the integers, co-prime to divisor, in the interval [1, right].
    // Both divisor and right must be small.
    // Time complexity sqrt(divisor + right) due to Euler.
    template<std::signed_integral long_int_t, std::signed_integral int_t>
    requires(sizeof(int_t) <= sizeof(long_int_t))
    [[nodiscard]] constexpr auto coprimes_in_interval(
        const int_t &divisor, std::vector<int_t> &factors, const int_t &right) -> int_t
    {
        require_positive(divisor, "divisor");
        require_positive(right, "right");

        decompose_into_prime_divisors<long_int_t, int_t>(divisor, factors);

        {
            constexpr auto greater1 = 31U;

            require_greater(greater1, factors.size(), "factors size");
        }

        constexpr auto one = 1U;

        const auto size = factors.size();
        const auto edge_mask = one << size;

        auto ans = right; // Divisible counts will be subtracted/added.

        for (auto mask = one; mask < edge_mask; ++mask)
        {
            int_t prod = 1;

            auto paris = 0U;

            for (std::size_t index{}; index < size; ++index)
            {// todo(p3): for each set bit in mask - faster.
                if ((mask & (one << index)) != 0U)
                {
                    prod *= factors[index];
                    paris ^= one;

                    assert(int_t{} < prod);
                }
            }

            // Subtract/add divisibles.
            ans += right / prod * (paris != 0U ? -1 : 1);
        }

        return ans;
    }

    template<std::integral int_t>
    [[nodiscard]] constexpr auto coprimes_in_interval_slow(const std::pair<int_t, int_t> &divisor_right) -> int_t
    {
        constexpr int_t zero{};
        constexpr int_t one = 1;

        const auto &divisor = require_positive(divisor_right.first, "divisor");
        const auto &right = require_positive(divisor_right.second, "right");

        auto expected = zero;

        for (auto num = one; num < right; ++num)
        {
            const auto great = gcd_int(divisor, num);
            expected += great == one ? one : zero;
        }

        {// If the 'right' is sort of MAX_INT, handle an overflow neatly.
            const auto great = gcd_int(divisor, right);
            expected += great == one ? one : zero;
        }

        return expected;
    }
} // namespace Standard::Algorithms::Numbers
