#pragma once
#include <cassert>
#include <stdexcept>
#include <vector>
#include <string>
#include <type_traits>

namespace
{
    constexpr auto max_number_digits = 20;

    // n=123 return {1,2,3} with 3 as the lowest digit.
    template <class number_t, class number_t2>
    void extract_digits(number_t source, std::vector<number_t2>& target)
    {
        assert(source > 0);

        target.clear();
        while (source > 0)
        {
            const auto d = static_cast<number_t2>(source % 10);
            target.push_back(d);
            source /= 10;
        }
    }

#if _DEBUG
    int64_t digit_sum2_rec_calls;
#endif

    // Once the cache is filled in (10 * 18 * 200 * 2) = 72K steps,
    // fewer than 200 func calls are required to compute.
    template <class number_t, class number_t2>
    number_t digit_sum2_rec(
        // [ds][x] - ds is digit sum; 'x' counts digits 10**(x + 1), ds000..ds999, x=2 in 999.
        std::vector<std::vector<number_t>>& cache,
        // 905, lowest digit is 5 at pos = 0, highest digit is 9 at pos = 2.
        const std::vector<char>& digits,
        // Prefix digit sum from the highest digit in 'digits.back()' to digit at 'pos'.
        const number_t2 prefix_dig_sum,
        // Digit position in 'digits'.
        const number_t2 pos,
        const bool use_any_digit = false)
    {
        static_assert(sizeof number_t2 <= sizeof number_t);
        assert(prefix_dig_sum >= 0 && prefix_dig_sum < 9 * max_number_digits && pos >= 0 && pos < static_cast<int>(digits.size()));

        // n=901, ds = 0, pos=2, use_any_digit = false.
        //  for d=0..8, r = (0..99) + ... + (800..899) =
        //                = [0][1]  + ... + [8][1] = 11700
        //                (all from the cache,
        //                 recursive call, use_any_digit = true as d != 9).
        //  Then for d=9, recursive call
        //    ds = 9, pos=1, use_any_digit = false.
        //    for d=0..0, recursive call
        //      ds = 9, pos=0, use_any_digit = false.
        //        for d=0..1
        //          ds2 = 9, +9.
        //          ds2 = 10, +10.
        //      return 19.
        //    return 19.
        //  +19, return 11719.

        // Cache values, sum of:
        // [0][0]  =  45, 0..9
        // [1][0]  =  55, 10..19
        // [2][0]  =  65, 20..29
        // [9][0]  = 135, 90..99. Check 90 + 45 = 135.
        // [26][0] = 305, 260..269. Check 26*10 + 45 = 305, where 26 is the digit sum.
        //
        // [0][1] = 900, 0..99. Hi and low, (0..9)*10 + (0..9)*10 = 900.
        // [1][1] = 1000. 100..199
        // [2][1] = 1100.
        // [130][1] = 13900 = 130*100 + 900.
        //
        // [0][2] = 13500, 0..999
        // [1][2] = 14500, 1000..1999
        // [2][2] = 15500
        //
        // [0][3] = 180000, 0..9999
        // [1][3] = 190000, 10000..19999
        // [2][3] = 200000
        //
        // [0][4] = 2250000
        // [1][4] = 2350000
        // [2][4] = 2450000
        //
        // [0][15] = 720000000000000000
        // [1][15] = 730000000000000000
        // [2][15] = 740000000000000000
#if _DEBUG
        ++digit_sum2_rec_calls;
#endif
        const number_t2 digit_max = use_any_digit ? 9 : digits[pos];
        number_t result = 0;
        for (number_t2 d = 0; d <= digit_max; ++d)
        {
            const auto prefix_dig_sum2 = prefix_dig_sum + d;
            assert(prefix_dig_sum2 >= 0);

            number_t ad;
            if (!pos) // Lowest digit
                ad = prefix_dig_sum2;
            else
            {
                const auto pos2 = pos - 1;
                const auto use_any_digit2 = use_any_digit || d != digits[pos];
                if (use_any_digit2)
                {
                    const auto& val = cache[prefix_dig_sum2][pos2];
                    if (val >= 0)
                    {// Already computed.
                        ad = val;
                    }
                    else
                        goto label_calc;
                }
                else
                {
                label_calc:
                    ad = digit_sum2_rec(cache, digits, prefix_dig_sum2, pos2, use_any_digit2);
                }
            }

            result += ad;
            assert(result >= 0 && ad >= 0);
        }

        assert(result > 0);
        if (use_any_digit)
        {// Save.
            auto& val = cache[prefix_dig_sum][pos];
            assert(val == -1);
            val = result;
        }

        return result;
    }
}

namespace Privet
{
    namespace Algorithms
    {
        namespace Numbers
        {
            // 1000..9999, return 3,
            // 100..999, return 2,
            // 10..99, return 1,
            // 0..9, return 0.
            template <class number_t>
            number_t log10_slow(number_t n, number_t& high_digit, number_t& p10)
            {
                assert(n >= 0);

                number_t r = 0;
                p10 = 1;
                while (n >= 10)
                {
                    n /= 10;
                    ++r;
                    p10 *= 10;
                }
                high_digit = n;
                assert(high_digit >= 0 && high_digit <= 9 && p10 > 0);
                return r;
            }

            // Digit sums for numbers from 0 to [(power of 10) - 1].
            // 0) 0 ..9 sum to 45.
            // 1) 0 ..99 sum to 900 = 45*10 + 10*Sum(10 - 1).
            // 2) 0 ..999 sum to 13500.
            template <class number_t>
            std::vector<number_t> prepare_digit_sum(number_t ten_digits = 19)
            {
                if (ten_digits <= 0)
                    ten_digits = 1;
                else if (ten_digits > 19)
                    ten_digits = 19;

                std::vector<number_t> rs(ten_digits + 1);
                rs[0] = 0;
                rs[1] = 45;

                for (number_t i = 2, tp = 10; i <= ten_digits; ++i, tp *= 10)
                {
                    rs[i] = 45 * tp + 10 * rs[i - 1];
                    assert(rs[i] > 0);
                }

                return rs;
            }

            template <class number_t>
            number_t small_digit_sum(const number_t n)
            {
                assert(n >= 0 && n <= 9);
                return n * (n + 1) >> 1;
            }

            // From 0 to 12, the digit sum is (1+2+..+9) + (1+0) + (1+1) + (1+2) = 45+6=51.
            template <class number_t>
            number_t digit_sum(const std::vector<number_t>& prepared_sums, const number_t val)
            {
                assert(prepared_sums.size() && val >= 0);
                if (val <= 9)
                    return small_digit_sum(val);

                number_t sum = 0, n = val;
                do
                {
                    // n = 678
                    assert(n >= 0);

                    // high_digit = 6, p10 = 100, log = 2
                    number_t high_digit, p10;
                    const auto log = log10_slow(n, high_digit, p10);
                    assert(log >= 0 && log < static_cast<number_t>(prepared_sums.size()));
#if _DEBUG
                    {// left = 78
                        const number_t left = -high_digit * p10 + n;
                        assert(left >= 0 && (!n || left < n));
                    }
#endif
                    // 678 has regions {0..600, 601..678}

                    // n = 78, ensure convergence.
                    n -= high_digit * p10;
                    assert(n >= 0);

                    //0..600 has Sum(6-1)*100 + 6*(1 + Sum(100 - 1)).
                    sum += small_digit_sum(high_digit - 1) * p10
                        + high_digit * (prepared_sums[log] + 1
                            //601..678 - add the 6s.
                            + n);
                    assert(sum >= 0);
                } while (n);
                return sum;
            }

            // From 11 to 12, the digit sum is (1+1) + (1+2) = 5.
            template <class number_t>
            number_t digit_sum_range(const std::vector<number_t>& prepared_sums,
                const number_t from, const number_t to_inclusive)
            {
                assert(from >= 0 && from <= to_inclusive);
                if (from > to_inclusive)
                {
                    const auto err = "From " + std::to_string(from) + " cannot be greater than to " + std::to_string(to_inclusive);
                    throw std::runtime_error(err);
                }

                const number_t hi = digit_sum(prepared_sums, to_inclusive),
                    low = from > 0 ? digit_sum(prepared_sums, from - 1) : 0,
                    r = hi - low;
                assert(r >= 0);
                return r;
            }

            // This can be done once for all numbers.
            // See digit_sum2.
            template <class number_t>
            std::vector<std::vector<number_t>> digit_sum2_cache()
            {
                static_assert(std::is_signed_v<number_t>);

                // 20 digits in largest int64 ~ 18e18.
                // max digit sum is 20*9 = 180.
                std::vector<std::vector<number_t>> r(9 * max_number_digits,
                    std::vector<number_t>(max_number_digits, -1));
                return r;
            }

            // See also digit_sum which is faster.
            template <class number_t, class number_t2>
            number_t digit_sum2(std::vector<std::vector<number_t>>& cache,
                std::vector<char>& digits,
                const number_t n)
            {
                static_assert(std::is_signed_v<number_t> && std::is_signed_v<number_t2> && sizeof number_t2 <= sizeof number_t);

                assert(n >= 0 && cache.size() == 9 * max_number_digits);
                if (n <= 1)
                    return n;

                extract_digits(n, digits);
                const auto pos = static_cast<number_t2>(digits.size()) - 1;
                const auto r = digit_sum2_rec<number_t, number_t2>(cache, digits, 0, pos);
                return r;
            }

            // For a single number.
            template <class number_t>
            number_t number_digit_sum_slow(const number_t n)
            {
                number_t sum = 0, t = n;
                do
                {
                    assert(t >= 0);
                    const auto d = t % 10;
                    assert(d >= 0 && d <= 9);
                    sum += d;
                    t /= 10;
                } while (t);

                return sum;
            }

            // 0..n, sum of decimal digits, slow.
            template <class number_t>
            number_t digit_sum_slow(const number_t n)
            {
                number_t sum = 0;
                for (number_t i = 1; i <= n; ++i)
                {
                    sum += number_digit_sum_slow(i);
                }

                return sum;
            }

            // slow.
            template <class number_t>
            number_t digit_sum_range_slow(const number_t from, const number_t to_inclusive)
            {
                assert(from >= 0 && from <= to_inclusive);

                number_t sum = 0;
                for (number_t i = from; i <= to_inclusive; ++i)
                {
                    sum += number_digit_sum_slow(i);
                }

                return sum;
            }
        }
    }
}
