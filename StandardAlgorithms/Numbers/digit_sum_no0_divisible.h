#pragma once
#include"digit_sum.h"

namespace Standard::Algorithms::Numbers::Inner
{
    template<class int_t, class map_t>
    struct dsnd_context final
    {
        const int_t &sum;
        const int_t &divisor;
        const std::vector<char> &digits;
        map_t &map1;
    };

    template<class int_t, class long_int_t>
    constexpr void dsnd_check_input(const int_t &numb, const long_int_t &sum, const int_t &divisor)
    {
        require_positive(numb, "numb");
        require_positive(sum, "sum");
        require_positive(divisor, "divisor");
    }

    template<std::signed_integral int_t, class map_t>
    [[nodiscard]] constexpr auto digit_sum_no0_divisible_rec(dsnd_context<int_t, map_t> &context,
        const std::int32_t digit_pos,
        // Prefix digit remainder from the highest digit in 'digits.back()' to digit at 'digit_pos'.
        // 12 % 7 = ((1 % 7) * 10 + 2) % 7.
        const int_t &prefix_remainder = 0,
        // Prefix digit sum from the highest ..
        const std::int32_t prefix_dig_sum = 0, const bool use_any_digit = false, const bool zero_leading = true)
        -> int_t
    {
        assert(0 <= prefix_dig_sum && prefix_dig_sum < digit_max * max_number_digits && prefix_dig_sum <= context.sum);

        assert(0 <= digit_pos && digit_pos < static_cast<std::int32_t>(context.digits.size()) &&
            0 <= prefix_remainder && prefix_remainder < context.divisor);

        const auto digit_min = zero_leading ? 0 : 1;
        const auto top_digit = use_any_digit ? digit_max : context.digits.at(digit_pos);

        int_t count{};

        for (auto dig = digit_min; dig <= top_digit; ++dig)
        {
            assert(0 <= count);

            constexpr int_t ten = digit_ten;

            const auto prefix_remainder2 = static_cast<int_t>((prefix_remainder * ten + dig) % context.divisor);

            const auto prefix_dig_sum2 = static_cast<int_t>(prefix_dig_sum + dig);

            assert(0 <= prefix_dig_sum2 && 0 <= prefix_remainder2 && prefix_remainder2 < context.divisor);

            if (digit_pos == 0 || // Lowest digit
                context.sum < prefix_dig_sum2) // Skip greater sums.
            {
                count += 0 == prefix_remainder2 && prefix_dig_sum2 == context.sum ? 1 : 0;

                continue;
            }

            const auto digit_pos2 = digit_pos - 1;

            const auto use_any_digit2 = use_any_digit || dig != context.digits.at(digit_pos);

            const auto zero_leading2 = zero_leading && 0 == dig;

            const auto key2 = std::make_tuple(prefix_dig_sum2, prefix_remainder2, digit_pos2);

            const auto it2 = context.map1.find(key2);

            const auto is_already_computed = use_any_digit2 && it2 != context.map1.end();

            const auto add = is_already_computed ? it2->second
                                                 : digit_sum_no0_divisible_rec(context, digit_pos2, prefix_remainder2,
                                                       prefix_dig_sum2, use_any_digit2, zero_leading2);

            count += add;
        }

        assert(0 <= count);

        if (use_any_digit)
        {
            const auto key = std::make_tuple(prefix_dig_sum,
                // 'zero_leading' doesn't have to be saved.
                prefix_remainder, digit_pos);

            assert(!context.map1.contains(key));

            context.map1[key] = count;
        }

        return count;
    }
} // namespace Standard::Algorithms::Numbers::Inner

namespace Standard::Algorithms::Numbers
{
    // Given positive integers n, sum, divisor,
    // count the numbers 1 <= x <= n:
    // - divisor divides x;
    // - x has only non-zero digits;
    // - digits sum(x) = sum.
    //
    // Time depends linearly on divisor - not good.
    template<std::signed_integral int_t, class map_t>
    requires(sizeof(std::int32_t) <= sizeof(int_t))
    [[nodiscard]] constexpr auto digit_sum_no0_divisible(
        const int_t &numb, map_t &map1, const int_t &sum, std::vector<char> &digits, const int_t &divisor) -> int_t
    {
        Inner::dsnd_check_input(numb, sum, divisor);

        if (numb < divisor)
        {
            return {};
        }

        map1.clear();
        Inner::extract_digits(numb, digits);

        assert(!digits.empty());

        Inner::dsnd_context<int_t, map_t> context{ sum, divisor, digits, map1 };

        const auto digit_pos = static_cast<std::int32_t>(digits.size()) - 1;

        auto res = Inner::digit_sum_no0_divisible_rec<int_t, map_t>(context, digit_pos);

        return res;
    }

    template<std::integral int_t>
    [[nodiscard]] constexpr auto digit_sum_no0_divisible_slow(const int_t &numb, const int_t &sum, const int_t &divisor)
        -> int_t
    {
        Inner::dsnd_check_input(numb, sum, divisor);

        int_t count{};

        for (int_t index = 1; index <= numb; ++index)
        {
            if ((index % divisor) != int_t{})
            {
                continue;
            }

            int_t dsu{};
            auto ind_2 = index;
            auto has = true;

            do
            {
                constexpr int_t ten = digit_ten;

                const auto dig = ind_2 % ten;

                if (dig == int_t{} || sum < dsu + dig)
                {
                    has = false;
                    break;
                }

                dsu += dig;
                ind_2 /= ten;
            } while (int_t{} < ind_2);

            count += (has && sum == dsu) ? 1 : 0;
        }

        return count;
    }
} // namespace Standard::Algorithms::Numbers
