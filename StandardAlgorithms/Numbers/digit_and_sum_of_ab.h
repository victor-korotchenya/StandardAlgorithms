#pragma once
#include<cassert>

namespace
{
    template <class number_t>
    number_t has_only_ones(number_t n)
    {
        assert(n > 0);
        do
        {
            const auto d = n % 10;
            if (d != 1)
                return 0;
            n /= 10;
        } while (n);

        return 1;
    }

    // 1) Perfect division.
    //  ds = 4888_812, digits = {4, 8}, delta = 4.
    //       4888_840, dif = 40-12 = 28 = 4*x, x = 7.
    //    Next digit sum is 4888_812 + 4*7 = 4888_840
    // 2) 1 greater.
    //  ds = 4888_182, digits = {4, 8}, delta = 4.
    //       4888_400, dif = 400-182 = 218 = 4*x, x = round_up(218.0/4) = 55
    //    Next ds is 4888_182 + 4*55 = 4888_402
    template <class number_t, class number_t2>
    number_t largest_different_digit_ratio(number_t2 ds, const char digit1, const char digit2)
    {
        static_assert(sizeof(number_t) <= sizeof(number_t2));

        const auto delta = number_t2(digit2 - digit1);
        assert(ds > 0 && digit1 > 0 && digit1 < digit2&& digit2 <= 9 && delta > 0 && delta < 9);

        number_t2 p10 = 1, low = 0, cur = 0, next = 0;
        do
        {
            assert(ds > 0 && p10 > 0);

            const number_t2 d = ds % 10;
            ds /= 10;
            low += p10 * d;
            assert(low >= 0 && ds >= 0);

            if (d == digit1 || d == digit2)
            {
                p10 *= 10;
                continue;
            }

            cur = low;

            const auto d2 = d < digit1 ? digit1
                : d < digit2 ? digit2
                : char(10);

            next = p10 * number_t2(d2);
            assert(d < d2&& d2 > 0 && d2 <= 10 && low < next);
            p10 *= 10;
        } while (ds);

        assert(cur >= 0 && next > 0 && cur < next);

        number_t2 rem = (next - cur) % delta,
            ratio = (next - cur) / delta + (rem != 0);
        if (!ratio)
            ratio = 1;

        assert(ratio > 0);
        // todo: p3. overflow.
        return static_cast<number_t>(ratio);
    }

    // n=1, return 1
    // n=2, 10
    // n=3, 100
    // 10**(n - 1)
    template <class number_t>
    number_t power10_slow(const number_t n)
    {
        assert(n >= 0);
        number_t p = 1;
        for (number_t i = 0; i < n; ++i)
        {
            p *= 10;
            assert(p > 0);
        }
        return p;
    }

    template <class number_t>
    bool check_digits_slow(number_t a, number_t& digit_sum, const char digit1, const char digit2)
    {
        assert(a > 0);
        digit_sum = 0;
        do
        {
            const auto d = a % 10;
            if (d != static_cast<number_t>(digit1) && d != static_cast<number_t>(digit2))
                return false;

            digit_sum += d;
            a /= 10;
        } while (a);
        return true;
    }
}

namespace Privet
{
    namespace Algorithms
    {
        namespace Numbers
        {
            // Given positive integers n, digit1, digit2,
            // count such numbers x of length n that only digits digit1 and digit2 are in:
            // - x,
            // - digits sum(x).
            template <class number_t, class number_t2, class factorials_t, number_t modulus = 1000 * 1000 * 1000 + 7>
            number_t digit_and_sum_of_ab(
                const factorials_t& factorials,
                const number_t n, char digit1, char digit2)
            {
                static_assert(sizeof(number_t) <= sizeof(number_t2) && modulus > 0);

                if (n <= 0 || digit1 <= 0 || digit2 <= 0 || digit1 > 9 || digit2 > 9)
                {
                    assert(0);
                    return 0;
                }

                if (digit1 == digit2)
                {
                    const auto s = has_only_ones<number_t>(n);
                    return static_cast<number_t>(s);
                }

                if (digit2 < digit1)
                    std::swap(digit2, digit1);

                const auto delta = number_t2(digit2 - digit1);
                assert(digit1 < digit2&& delta > 0 && delta < 9);

                // digit sum = digit1 * (n - x) + digit2 * x,  where x = [0..n]
                //           = digit1 * n + (digit2 - digit1) * x

                const number_t2 ds_min = static_cast<number_t2>(n) * static_cast<number_t2>(digit1),
                    ds_max = static_cast<number_t2>(n) * static_cast<number_t2>(digit2);
                assert(ds_min > 0 && ds_max > 0 && ds_min < ds_max);

                number_t2 count = 0, ds = ds_min;
                number_t m = 0;
                do
                {
                    assert(ds > 0);
                    number_t2 ds2;
                    if (!check_digits_slow<number_t2>(ds, ds2, digit1, digit2))
                    {
                        // todo: p3. even faster skip.
                        const auto ratio = largest_different_digit_ratio<number_t, number_t2>(ds, digit1, digit2);

                        const auto larger_step = number_t2(ratio) * delta;
                        assert(larger_step > 0);

                        ds += larger_step;
                        m += ratio;
                        assert(ds > 0 && m > 0);
                        continue;
                    }

                    const auto nm = factorials.choose_modulo(n, m);
                    assert(nm >= 0 && nm < modulus);

                    count += nm;
                    ds += delta;
                    ++m;
                    assert(count >= 0 && ds > 0 && m > 0);
                } while (ds <= ds_max);

                return static_cast<number_t>(count % modulus);
            }

            // Slow. Enumerate all numbers.
            template <class number_t, number_t modulus = 1000 * 1000 * 1000 + 7>
            number_t digit_and_sum_of_ab_slow(
                const number_t n, const char digit1, const char digit2)
            {
                static_assert(modulus > 0);
                assert(n <= 10);
                if (n <= 0 || digit1 <= 0 || digit2 <= 0 || digit1 > 9 || digit2 > 9)
                {
                    assert(0);
                    return 0;
                }

                const number_t initial = power10_slow<number_t>(n - 1),
                    edge = initial * number_t(10);
                assert(initial > 0 && edge > 0 && initial < edge);

                int64_t count = 0;
                for (auto number = initial; number < edge; ++number)
                {
                    number_t ds;
                    if (!check_digits_slow<number_t>(number, ds, digit1, digit2))
                        continue;

                    number_t ds2;
                    const auto h2 = check_digits_slow<number_t>(ds, ds2, digit1, digit2);
                    count += h2;
                }
                return static_cast<number_t>(count % static_cast<int64_t>(modulus));
            }

            // Slow still. Enumerate all digit sums.
            template <class number_t, class number_t2, class factorials_t, number_t modulus = 1000 * 1000 * 1000 + 7>
            number_t digit_and_sum_of_ab_slow_still(
                const factorials_t& factorials,
                const number_t n, char digit1, char digit2)
            {
                static_assert(sizeof(number_t) <= sizeof(number_t2) && modulus > 0);
                if (n <= 0 || digit1 <= 0 || digit2 <= 0 || digit1 > 9 || digit2 > 9)
                {
                    assert(0);
                    return 0;
                }

                if (digit2 == digit1)
                {
                    const number_t2 ds = number_t2(n) * number_t2(digit1);
                    number_t2 ds2;
                    const auto h = check_digits_slow<number_t2>(ds, ds2, digit1, digit1);
                    return number_t(h);
                }

                if (digit2 < digit1)
                    std::swap(digit2, digit1);
                assert(digit1 < digit2);

                const number_t2 ds_min = n * number_t2(digit1),
                    ds_max = n * number_t2(digit2),
                    delta = number_t2(digit2 - digit1);
                assert(ds_min > 0 && ds_max > 0 && ds_min < ds_max&& delta > 0 && delta < 9);

                number_t2 count = 0;
                number_t m = 0;
                auto ds = ds_min;
                for (; ds <= ds_max; ds += delta, ++m)
                {
                    assert(count >= 0 && ds > 0 && m >= 0 && m <= n);

                    number_t2 ds2;
                    if (!check_digits_slow<number_t2>(ds, ds2, digit1, digit2))
                        continue;

                    const auto nm = factorials.choose_modulo(n, m);
                    assert(nm >= 0 && nm < modulus);

                    count += nm;
                    assert(count >= 0);
                }

                assert(ds == ds_max + delta && m == n + 1);
                return static_cast<number_t>(count % modulus);
            }
        }
    }
}