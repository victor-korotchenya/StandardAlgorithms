#pragma once
#include"bit_utilities.h"
#include"digit_sum.h" // extract_digits
#include"shift.h"
#include<optional>

namespace Standard::Algorithms::Numbers::Inner
{
    [[nodiscard]] inline constexpr auto to_char(const bool high, const bool low) noexcept -> char
    {
        auto cha = static_cast<char>((high ? 2U : 0U) | (low ? 1U : 0U));
        return cha;
    }

    // Given prod=12, allow digits 0,1,2,3,4,6.
    // Given prod=22, allow none as 11 is a prime.
    template<class int_t>
    [[nodiscard]] constexpr auto calc_skip_digits(int_t prod) -> std::optional<std::int16_t>
    {
        assert(int_t{} < prod);

        constexpr int_t one = 1;

        // Always allow 0, 1.
        auto mask = 3U;

        for (auto dig = 2; dig <= digit_max && one < prod; ++dig)
        {
            if (const auto is_divisible = (prod % dig) == 0; is_divisible)
            {
                mask |= ::Standard::Algorithms::Numbers::shift_left(1U, dig);
            }
        }

        for (auto dig = 2; dig <= digit_prime_max && one < prod; ++dig)
        {
            while ((prod % dig) == 0)
            {
                prod /= dig;
            }
        }

        if (digit_prime_max < prod) // A greater prime number cannot be divided by a digit.
        {
            return std::nullopt;
        }

        auto banned = static_cast<std::int16_t>(~mask);
        return banned;
    }

    // [product, position, (use_any_digit, zero_leading)] = count
    //
    // n = 9, p = 5
    // (1, 0, 1) = 1
    //
    // n = 10, p = 5
    // (1, 0, 3) = 1
    // (1, 0, 0) = 0
    // (1, 1, 1) = 1
    //
    // n = 19, p = 5
    // (1, 0, 3) = 1
    // (1, 0, 0) = 1
    // (1, 1, 1) = 2 - cannot share 'buffer' between calls, see n = 10.
    //
    // n = 100, p = 5
    // (1, 0, 3) = 1
    // (1, 0, 2) = 1
    // (5, 0, 2) = 1
    //
    // (1, 1, 3) = 3
    // (1, 1, 0) = 0 - '00' from '100' as max digit is '0'.
    //
    // (1, 2, 1) = 3 - result.
    //
    // [product, position, (use_any_digit, zero_leading)] = count
    //
    // n=9876, p=10
    // (1, 0, 3) = 0
    // (1, 0, 2) = 0
    // (2, 0, 2) = 1
    // (5, 0, 2) = 1
    //
    // (1, 1, 3) = 2
    // (1, 1, 2) = 2
    // (4, 0, 2) = 0
    // (10, 0, 2) = 1
    //
    // (2, 1, 2) = 2
    // (5, 1, 2) = 2
    // (1, 2, 3) = 8
    // (1, 2, 2) = 6
    // (8, 0, 2) = 0
    // (4, 1, 2) = 0
    // (10, 1, 2) = 1
    //
    // (2, 2, 2) = 3
    // (5, 2, 2) = 3
    // (1, 3, 1) = 20
    //
    // [product, position, (use_any_digit, zero_leading)] = count
    template<std::signed_integral int_t, class map_t
#if _DEBUG
        ,
        class p_t = std::pair<std::tuple<int_t, std::int32_t, char>, int_t>
#endif
        >
    // todo(p3): decrease complexity.
    [[nodiscard]] constexpr auto digit_product_count_rec(
#if _DEBUG
        std::vector<p_t> &debug_inserted,
#endif
        const std::vector<char> &digits, const int_t &prod, map_t &buffer, const std::int32_t pos,
        const int_t &prefix_prod,
        // To skip digits that never add value.
        const std::int16_t skip_digits, const bool use_any_digit = false, const bool zero_leading = true) -> int_t
    {
        assert(int_t{} < prefix_prod && 0 <= pos && !digits.empty());

        const auto top_digit = use_any_digit ? digit_max : digits[pos];

        int_t count{};

        for (std::int32_t dig{}; dig <= top_digit; ++dig)
        {
            if (const auto shall_skip = has_zero_bit(shift_right(skip_digits, dig)); shall_skip)
            {
                continue;
            }

            const auto zero_leading2 = zero_leading && dig == 0;

            const auto prefix_prod2 = static_cast<int_t>(zero_leading2 ? 1 : prefix_prod * dig);

            assert(int_t{} <= prefix_prod2);

            int_t add{};

            if (0 == pos || // Lowest digit
                prod < prefix_prod2 || // Skip greater products.
                // todo(p3): fix. Skip 0 products for now.
                int_t{} == prefix_prod2)
            {
                add = prefix_prod2 == prod ? 1 : 0;
            }
            else
            {
                const auto pos2 = pos - 1;
                const auto use_any_digit2 = use_any_digit || dig != digits[pos];

                const auto key2 = std::make_tuple(prefix_prod2, pos2, to_char(use_any_digit2, zero_leading2));

                const auto it2 = buffer.find(key2);

                if (it2 != buffer.end())
                {
                    add = it2->second;
                }
                else
                {
                    add = digit_product_count_rec<int_t, map_t>(
#if _DEBUG
                        debug_inserted,
#endif
                        digits, prod, buffer, pos2, prefix_prod2, skip_digits, use_any_digit2, zero_leading2);
                }
            }

            count += add;

            assert(int_t{} <= count && int_t{} <= add);
        }

        const auto key = std::make_tuple(prefix_prod, pos, to_char(use_any_digit, zero_leading));

        assert(!buffer.contains(key));

#if _DEBUG
        debug_inserted.emplace_back(key, count);
#endif

        buffer[key] = count;

        return count;
    }
} // namespace Standard::Algorithms::Numbers::Inner

namespace Standard::Algorithms::Numbers
{
    // From 0 to 100, there are 2 ints, having digit product 21: 37, 73.
    template<std::signed_integral int_t, class map_t>
    [[nodiscard]] constexpr auto digit_product_count(const int_t &numb, std::vector<char> &digits, const int_t &prod,
        // 1510 of 14225 buffer are != 0 for n = 1e18, p = 840000.
        map_t &buffer) -> int_t
    {
        // todo(p3): fix 0 prod.
        assert(int_t{} <= numb && int_t{} < prod);

        if (numb <= int_t{})
        {
            return prod == int_t{} ? 1 : 0;
        }

        const auto skip_digits = Inner::calc_skip_digits<int_t>(prod);

        if (!skip_digits.has_value())
        {
            return {};
        }

        buffer.clear();
        Inner::extract_digits(numb, digits);

#if _DEBUG
        using p_t = std::pair<std::tuple<int_t, std::int32_t, char>, int_t>;

        std::vector<p_t> debug_inserted;
#endif

        const auto max_pos = static_cast<std::int32_t>(digits.size()) - 1;

        assert(0 <= max_pos);

        auto res = static_cast<int_t>(Inner::digit_product_count_rec<int_t>(
#if _DEBUG
                                          debug_inserted,
#endif
                                          digits, prod, buffer, max_pos, 1, skip_digits.value()) -
            static_cast<int_t>(prod == 1 ? 1 : 0));

        assert(int_t{} <= res);

        return res;
    }

    // Slow.
    template<std::signed_integral int_t>
    [[nodiscard]] constexpr auto digit_product_count_slow(const int_t &numb, const int_t &prod) -> int_t
    {
        assert(int_t{} <= numb && int_t{} <= prod);

        int_t count{};

#if _DEBUG
        std::vector<int_t> debug_inserted;
#endif

        for (int_t value{}; value <= numb; ++value)
        {
            int_t cur_prod = 1;

            {
                auto temp = value;

                do
                {
                    const auto dig = temp % digit_ten;
                    temp /= digit_ten;
                    cur_prod *= dig;
                } while (temp != int_t{});
            }

            count += cur_prod == prod ? 1 : 0;

#if _DEBUG
            if (cur_prod == prod)
            {
                debug_inserted.push_back(value);
            }
#endif
        }

        return count;
    }
} // namespace Standard::Algorithms::Numbers
