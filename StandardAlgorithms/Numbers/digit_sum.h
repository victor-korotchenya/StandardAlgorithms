#pragma once
#include"../Utilities/require_utilities.h"
#include"digit_max.h"
#include"logarithm.h"
#include<vector>

namespace Standard::Algorithms::Numbers::Inner
{
    // n=123 return {1,2,3} with 3 as the lowest digit.
    template<class int_t, class tag_t>
    constexpr void extract_digits(int_t source, std::vector<tag_t> &destination)
    {
        constexpr int_t zero{};

        assert(!(source < zero));

        destination.clear();

        do
        {
            constexpr int_t ten = digit_ten;

            destination.push_back(static_cast<tag_t>(source % ten));

            source /= ten;
        } while (zero < source);
    }

    // Debug count calls.
    // NOLINTNEXTLINE
    [[maybe_unused]] extern std::int64_t digit_sum2_rec_calls_Debug;

    // Once the cache is filled in (10 * 18 * 200 * 2) = 72K steps,
    // fewer than 200 func calls are required to compute.
    template<std::signed_integral int_t>
    [[nodiscard]] constexpr auto digit_sum2_rec(
        // 905, lowest digit is 5 at position = 0, highest digit is 9 at position = 2.
        const std::vector<char> &digits, const int_t &digit_pos,
        // [ds][x] - ds is digit sum; 'x' counts digits 10**(x + 1), ds000..ds999, x=2 in 999.
        std::vector<std::vector<int_t>> &cache,
        // Prefix digit sum from the highest digit in 'digits.back()' to digit at 'digit_pos'.
        const int_t &prefix_dig_sum = {}, const bool use_any_digit = false) -> int_t
    {
        assert(int_t{} <= prefix_dig_sum && prefix_dig_sum < digit_max * max_number_digits && int_t{} <= digit_pos &&
            digit_pos < static_cast<int_t>(digits.size()));

        // n=901, ds = 0, digit_pos=2, use_any_digit = false.
        //  for d=0..8, r = (0..99) + .. + (800..899) =
        //                = [0][1]  + .. + [8][1] = 11700
        //                (all from the cache,
        //                 recursive call, use_any_digit = true as d != 9).
        //  Then for d=9, recursive call
        //    ds = 9, digit_pos=1, use_any_digit = false.
        //    for d=0..0, recursive call
        //      ds = 9, digit_pos=0, use_any_digit = false.
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
        if constexpr (::Standard::Algorithms::is_debug)
        {
            ++digit_sum2_rec_calls_Debug;
        }

        const auto top_digit = static_cast<int_t>(use_any_digit ? digit_max : digits[digit_pos]);

        int_t result{};

        for (int_t dig{}; dig <= top_digit; ++dig)
        {
            const auto prefix_dig_sum2 = static_cast<int_t>(prefix_dig_sum + dig);

            assert(int_t{} <= prefix_dig_sum2);

            int_t add{};

            if (const auto is_lowest_digit = digit_pos == int_t{}; is_lowest_digit)
            {
                add = prefix_dig_sum2;
            }
            else
            {
                const auto pos2 = digit_pos - 1;
                const auto use_any_digit2 = use_any_digit || dig != digits[digit_pos];

                if (const auto &val = cache[prefix_dig_sum2][pos2]; use_any_digit2 &&
                    // Already computed.
                    int_t{} <= val)
                {
                    add = val;
                }
                else
                {
                    add = digit_sum2_rec(digits, pos2, cache, prefix_dig_sum2, use_any_digit2);
                }
            }

            result += add;
            assert(int_t{} <= result && int_t{} <= add);
        }

        assert(int_t{} < result);

        if (use_any_digit)
        {
            auto &val = cache[prefix_dig_sum][digit_pos];

            assert(val == -1);

            val = result;
        }

        return result;
    }
} // namespace Standard::Algorithms::Numbers::Inner

namespace Standard::Algorithms::Numbers
{
    // Digit sums for numbers from 0 to [(power of 10) - 1].
    // 0) 0 ..9 sum to 45.
    // 1) 0 ..99 sum to 900 = 45*10 + 10*Sum(10 - 1).
    // 2) 0 ..999 sum to 13500.
    template<std::integral int_t>
    [[nodiscard]] constexpr auto prepare_digit_sum(int_t ten_digits = max_number_digits - 1) -> std::vector<int_t>
    {
        if (ten_digits <= int_t{})
        {
            ten_digits = 1;
        }
        else if (constexpr int_t top = max_number_digits - 1;
                 // Doing the best whatever is possible.
                 top < ten_digits)
        {
            ten_digits = top;
        }

        constexpr int_t nine_sum = 45;

        std::vector<int_t> res(ten_digits + 1LL);
        res.at(1) = nine_sum;

        auto coef = static_cast<int_t>(digit_ten * nine_sum);

        for (int_t index = 2; index <= ten_digits; ++index, coef *= digit_ten)
        {
            const auto &old = res.at(index - 1);

            assert(nine_sum < coef && nine_sum <= old && int_t{} == coef % digit_ten);

            auto &new_val = res.at(index);

            new_val = static_cast<int_t>(digit_ten * old + coef);

            assert(old <= new_val / digit_ten && int_t{} == new_val % digit_ten && old < new_val);
        }

        return res;
    }

    // Given 4, return sum of 1..4 which is 10.
    template<std::integral int_t>
    [[nodiscard]] constexpr auto consecutive_sum(const int_t &numb) -> int_t
    {
        assert(int_t{} <= numb);

        return static_cast<int_t>(numb * (numb + 1LL) >> 1U);
    }

    // From 0 to 12, the digit sum is (1+2+..+9) + (1+0) + (1+1) + (1+2) = 45+6=51.
    template<class int_t>
    [[nodiscard]] constexpr auto digit_sum(const std::vector<int_t> &prepared_sums, int_t numb) -> int_t
    {
        assert(!prepared_sums.empty() && int_t{} <= numb);

        if (numb <= digit_max)
        {
            return consecutive_sum(numb);
        }

        int_t sum{};

        do
        {
            assert(int_t{} <= numb);

            // Given numb = 678
            // log = 2, p10 = 100, high_digit = 6.
            const auto logatup = log10_slow<int_t>(numb);

            const auto &log = std::get<0>(logatup);
            const auto &p10 = std::get<1>(logatup);
            const auto &high_digit = std::get<2>(logatup);

            assert(int_t{} <= log);

            if (const auto size1 = static_cast<int_t>(prepared_sums.size()); !(log < size1)) [[unlikely]]
            {
                auto err = "Too large number " + std::to_string(numb) + " in digit_sum.";

                throw std::runtime_error(err);
            }

            if constexpr (::Standard::Algorithms::is_debug)
            {// left = 78
                const int_t left = -high_digit * p10 + numb;

                assert(int_t{} <= left && (int_t{} == numb || left < numb));
            }

            // 678 has regions {0..600, 601..678}
            //
            // Now, numb = 78, ensure convergence.
            numb -= high_digit * p10;
            assert(int_t{} <= numb);

            // 0..600 has Sum(6-1)*100 + 6*(1 + Sum(100 - 1)).
            sum += consecutive_sum(high_digit - 1) * p10 +
                high_digit *
                    (prepared_sums[log] +
                        1
                        // 601..678 - add the 6s.
                        + numb);

            assert(int_t{} <= sum);
        } while (int_t{} < numb);

        assert(int_t{} == numb);

        return sum;
    }

    // From 11 to 12, the digit sum is (1+1) + (1+2) = 5.
    template<class int_t>
    [[nodiscard]] constexpr auto digit_sum_range(
        const std::vector<int_t> &prepared_sums, const int_t &from, const int_t &to_inclusive) -> int_t
    {
        assert(int_t{} <= from);

        if (to_inclusive < from) [[unlikely]]
        {
            const auto err =
                "From " + std::to_string(from) + " cannot be greater than to " + std::to_string(to_inclusive);

            throw std::runtime_error(err);
        }

        const auto low = int_t{} < from ? digit_sum<int_t>(prepared_sums, static_cast<int_t>(from - 1)) : int_t{};

        const auto high = digit_sum<int_t>(prepared_sums, to_inclusive);

        auto res = static_cast<int_t>(high - low);

        assert(low <= high && int_t{} <= res);

        return res;
    }

    // See also digit_sum which is faster.
    template<std::signed_integral int_t>
    [[nodiscard]] constexpr auto digit_sum2(
        std::vector<std::vector<int_t>> &cache, std::vector<char> &digits, const int_t &numb) -> int_t
    {
        assert(int_t{} <= numb && digit_max * max_number_digits < cache.size());

        if (numb <= 1)
        {
            return numb;
        }

        Inner::extract_digits(numb, digits);
        const auto max_pos = static_cast<int_t>(digits.size()) - 1;

        auto res = Inner::digit_sum2_rec<int_t>(digits, max_pos, cache);

        assert(int_t{} <= res);

        return res;
    }

    // Sum of decimal digits of "numb".
    // E.g. 25 -> 7.
    template<std::integral int_t>
    [[nodiscard]] constexpr auto single_number_digit_sum_slow(int_t numb) -> int_t
    {
        int_t sum{};

        do
        {
            assert(int_t{} <= numb);

            constexpr int_t ten = digit_ten;

            const auto dig = numb % ten;
            numb /= ten;

            sum += dig;

            assert(int_t{} <= dig && dig < ten && int_t{} <= sum);
        } while (int_t{} < numb);

        return sum;
    }

    // 0..n, sum of decimal digits, slow.
    template<std::integral int_t>
    [[nodiscard]] constexpr auto digit_sum_slow(const int_t &numb) -> int_t
    {
        int_t sum{};

        for (int_t index = 1; index <= numb; ++index)
        {
            sum += single_number_digit_sum_slow<int_t>(index);
        }

        return sum;
    }

    // Slow.
    template<std::integral int_t>
    [[nodiscard]] constexpr auto digit_sum_range_slow(int_t from, const int_t &to_inclusive) -> int_t
    {
        assert(int_t{} <= from && from <= to_inclusive);

        int_t sum{};

        while (from <= to_inclusive)
        {
            sum += single_number_digit_sum_slow<int_t>(from);
            ++from;
        }

        return sum;
    }
} // namespace Standard::Algorithms::Numbers
