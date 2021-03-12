#pragma once
#include<tuple>
#include "digit_sum.h" // extract_digits

namespace
{
    template <class number_t, class map_t>
    number_t digit_sum_no0_divisible_rec(
        map_t& m,
        const std::vector<char>& digits,
        const number_t sum,
        const number_t divisor,
        // Prefix digit remainder from the highest digit in 'digits.back()' to digit at 'pos'.
        // Note. 12 % 7 = ((1 % 7) * 10 + 2) % 7.
        const number_t prefix_remainder,
        // Prefix digit sum from the highest ...
        const int prefix_dig_sum,
        // Digit position in 'digits'.
        const int pos,
        const bool use_any_digit = false,
        const bool zero_leading = true)
    {
        assert(prefix_dig_sum >= 0 && prefix_dig_sum < 9 * max_number_digits && prefix_dig_sum <= sum && pos >= 0 && pos < static_cast<int>(digits.size()) && digits.size() && prefix_remainder >= 0 && prefix_remainder < divisor);

        const int digit_min = !zero_leading,
            digit_max = use_any_digit ? 9 : digits[pos];
        number_t count = 0;
        for (auto d = digit_min; d <= digit_max; ++d)
        {
            const auto prefix_remainder2 = (prefix_remainder * static_cast<number_t>(10) + d) % divisor;
            const auto prefix_dig_sum2 = prefix_dig_sum + d;
            assert(prefix_dig_sum2 >= 0 && prefix_remainder2 >= 0 && prefix_remainder2 < divisor);

            number_t ad;
            if (!pos || // Lowest digit
                sum < prefix_dig_sum2) // Skip greater sums.
                ad = !prefix_remainder2 && prefix_dig_sum2 == sum;
            else
            {
                const auto pos2 = pos - 1;
                const auto use_any_digit2 = use_any_digit || d != digits[pos];
                const auto zero_leading2 = zero_leading && !d;
                if (use_any_digit2)
                {
                    const auto key2 = std::make_tuple(prefix_dig_sum2, prefix_remainder2, pos2, char(use_any_digit2));
                    const auto it2 = m.find(key2);
                    if (it2 != m.end()) // Already computed.
                        ad = it2->second;
                    else
                        goto label_calc;
                }
                else
                {
                label_calc:
                    ad = digit_sum_no0_divisible_rec(m, digits, sum, divisor, prefix_remainder2, prefix_dig_sum2, pos2, use_any_digit2, zero_leading2);
                }
            }

            count += ad;
            assert(count >= 0 && ad >= 0);
        }

        assert(count >= 0);
        if (use_any_digit)
        {// Save.
            const auto key = std::make_tuple(prefix_dig_sum, prefix_remainder, pos,
                // 'zero_leading' doesn't have to be saved.
                char(use_any_digit));
            assert(!m.count(key));
            m[key] = count;
        }

        return count;
    }
}

namespace Privet
{
    namespace Algorithms
    {
        namespace Numbers
        {
            // Given positive integers n, sum, divisor,
            // count the numbers 1 <= x <= n:
            // - x has non 0 digits,
            // - digits sum(x) = sum,
            // - divisor divides x.
            //
            //Time depends linearly on divisor - not good.
            template <class number_t, class map_t>
            number_t digit_sum_no0_divisible(map_t& m,
                std::vector<char>& digits,
                const number_t n, const number_t sum, const number_t divisor)
            {
                static_assert(std::is_signed_v<number_t> && sizeof(int) <= sizeof(number_t));
                if (n <= 0 || n < divisor || sum <= 0 || divisor <= 0)
                    return 0;

                m.clear();
                extract_digits(n, digits);

                const auto pos = static_cast<int>(digits.size()) - 1;
                const auto r = digit_sum_no0_divisible_rec<number_t, map_t>(m, digits, sum, divisor, 0, 0, pos);
                return r;
            }

            template <class number_t>
            number_t digit_sum_no0_divisible_slow(const number_t n, const number_t sum, const number_t divisor)
            {
                assert(n > 0 && sum > 0 && divisor > 0);
                number_t count = 0;
                for (number_t i = 1; i <= n; ++i)
                {
                    if (i % divisor)
                        continue;

                    auto h = true;
                    number_t j = i, ds = 0;
                    do
                    {
                        const auto d = j % 10;
                        if (!d || ds + d > sum)
                        {
                            h = false;
                            break;
                        }

                        ds += d;
                        j /= 10;
                    } while (j);

                    if (sum == ds)
                        count += h;
                }

                return count;
            }
        }
    }
}