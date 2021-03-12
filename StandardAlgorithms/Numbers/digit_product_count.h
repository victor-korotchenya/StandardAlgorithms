#pragma once
#include <cassert>
#include <tuple>
#include <utility>
#include <vector>
#include <type_traits>
#include "digit_sum.h" // extract_digits

namespace
{
    // The prod might be changed.
    template <class number_t>
    short calc_skip_digits(number_t& prod)
    {
        // Always allow 0, 1.
        auto mask = ~3;
        for (auto d = 2; d <= 9 && prod > 1; ++d)
        {
            if (prod % d)
                continue;

            // Given prod=12, allow digits 0,1,2,3,4,6.
            mask &= ~(1 << d);
        }

        for (auto d = 2; d <= 9 && prod > 1; ++d)
        {
            while (!(prod % d))
                prod /= d;
        }

        return static_cast<short>(mask);
    }

    // [product, position, (use_any_digit, zero_leading)] = count
    //
    //n = 9, p = 5
    //(1, 0, 1) = 1
    //
    //n = 10, p = 5
    //(1, 0, 3) = 1
    //(1, 0, 0) = 0
    //(1, 1, 1) = 1
    //
    //n = 19, p = 5
    //(1, 0, 3) = 1
    //(1, 0, 0) = 1
    //(1, 1, 1) = 2 - cannot share 'buf' between calls, see n = 10.
    //
    //
    //n = 100, p = 5
    //(1, 0, 3) = 1
    //(1, 0, 2) = 1
    //(5, 0, 2) = 1
    //
    //(1, 1, 3) = 3
    //(1, 1, 0) = 0 - '00' from '100' as max digit is '0'.
    //
    //(1, 2, 1) = 3 - result.
    //
    // [product, position, (use_any_digit, zero_leading)] = count
    //
    // n=9876, p=10
    //(1, 0, 3) = 0
    //(1, 0, 2) = 0
    //(2, 0, 2) = 1
    //(5, 0, 2) = 1
    //
    //(1, 1, 3) = 2
    //(1, 1, 2) = 2
    //(4, 0, 2) = 0
    //(10, 0, 2) = 1
    //
    //(2, 1, 2) = 2
    //(5, 1, 2) = 2
    //(1, 2, 3) = 8
    //(1, 2, 2) = 6
    //(8, 0, 2) = 0
    //(4, 1, 2) = 0
    //(10, 1, 2) = 1
    //
    //(2, 2, 2) = 3
    //(5, 2, 2) = 3
    //(1, 3, 1) = 20
    //
    // [product, position, (use_any_digit, zero_leading)] = count
    template <class number_t, class map_t
#if _DEBUG
        , class p_t = std::pair<std::tuple<number_t, int, char>, number_t>
#endif
    >
        number_t digit_product_count_rec(
            const std::vector<char>& digits,
#if _DEBUG
            std::vector<p_t>& debug_inserted,
#endif
            map_t& buf,
            const number_t prod,
            const number_t prefix_prod,
            const int pos,
            // To skip digits that never add value.
            const short skip_digits,
            const bool use_any_digit = false,
            const bool zero_leading = true)
    {
        assert(prefix_prod > 0 && pos >= 0 && digits.size());

        const auto to_char = [](const bool a, const bool b) -> char
        {
            const auto c = static_cast<int>(a) << 1 | static_cast<int>(b);
            return static_cast<char>(c);
        };

        const auto digit_max = use_any_digit ? 9 : digits[pos];
        number_t count = 0;
        for (auto d = 0; d <= digit_max; d++)
        {
            // If (prod % 7 != 0), skip digit 7.
            if ((skip_digits >> d) & 1)
                continue;

            const auto zero_leading2 = zero_leading && !d;
            const number_t prefix_prod2 = zero_leading2 ? 1 : prefix_prod * d;
            assert(prefix_prod2 >= 0);

            number_t ad;
            if (!pos || // Lowest digit
                !prefix_prod2 ||// todo: p2. fix. Skip 0 products for now.
                prod < prefix_prod2) // Skip greater products.
            {// Add 1.
                ad = prefix_prod2 == prod;
            }
            else
            {
                const auto pos2 = pos - 1;
                const auto use_any_digit2 = use_any_digit || d != digits[pos];
                const auto key2 = std::make_tuple(prefix_prod2, pos2, to_char(use_any_digit2, zero_leading2));
                const auto it2 = buf.find(key2);
                if (it2 != buf.end())
                    ad = it2->second;
                else
                    ad = digit_product_count_rec<number_t, map_t>(digits,
#if _DEBUG
                        debug_inserted,
#endif
                        buf, prod, prefix_prod2, pos2, skip_digits, use_any_digit2, zero_leading2);
            }

            count += ad;
            assert(count >= 0 && ad >= 0);
        }

        const auto key = std::make_tuple(prefix_prod, pos, to_char(use_any_digit, zero_leading));
        assert(!buf.count(key));
#if _DEBUG
        //if (count)
        debug_inserted.push_back(std::make_pair(key, count));
#endif
        buf[key] = count;
        return count;
    }
}

namespace Privet
{
    namespace Algorithms
    {
        namespace Numbers
        {
            // From 0 to 100, there are 2 ints, having digit product 21: 37, 73.
            template <class number_t, class map_t>
            number_t digit_product_count(const number_t n, const number_t prod,
                std::vector<char>& digits,
                // 1510 of 14225 buf are != 0 for n = 1e18, p = 840000.
                map_t& buf)
            {
                static_assert(std::is_signed_v<number_t>);
                // todo: p2. fix 0 prod.
                assert(n >= 0 && prod > 0);

                if (n <= 0)
                    return !prod;

                auto left = prod;
                const auto skip_digits = calc_skip_digits<number_t>(left);
                if (left >= 11) // A prime number > 9 cannot be divided by a digit.
                    return 0;

                buf.clear();
                extract_digits(n, digits);

#if _DEBUG
                using p_t = std::pair<std::tuple<number_t, int, char>, number_t>;
                std::vector<p_t> debug_inserted;
#endif

                const auto pos = static_cast<int>(digits.size()) - 1;
                const auto r = digit_product_count_rec<number_t>(digits,
#if _DEBUG
                    debug_inserted,
#endif
                    buf, prod, 1, pos, skip_digits) - static_cast<number_t>(prod == 1);
                assert(r >= 0);
                return r;
            }

            // slow.
            template <class number_t>
            number_t digit_product_count_slow(const number_t n, const number_t prod)
            {
                static_assert(std::is_signed_v<number_t>);
                assert(n >= 0 && prod >= 0);

                number_t count = 0;
#if _DEBUG
                std::vector<number_t> debug_inserted;
#endif
                for (number_t i = 0; i <= n; ++i)
                {
                    number_t j = i, pr = 1;
                    do
                    {
                        const auto d = j % 10;
                        j /= 10;
                        pr *= d;
                    } while (j);

                    count += pr == prod;
#if _DEBUG
                    if (pr == prod)
                        debug_inserted.push_back(i);
#endif
                }

                return count;
            }
        }
    }
}