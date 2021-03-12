#pragma once
#include"../Utilities/require_utilities.h"
#include"../Utilities/same_sign_leq_size.h"
#include"digit_max.h"

namespace Standard::Algorithms::Numbers::Inner
{
    template<std::integral int_t, int_t modulus>
    requires(int_t{} < modulus)
    constexpr void dsu_check_input(const char digit1, const int_t &numb, const char digit2)
    {
        require_positive(digit1, "digit1");
        require_positive(numb, "numb");
        require_positive(digit2, "digit2");

        require_less_equal(digit1, digit_max, "digit1");
        require_less_equal(digit2, digit_max, "digit2");
    }

    template<std::integral int_t>
    [[nodiscard]] constexpr auto has_only_ones(int_t numb) -> int_t
    {
        assert(int_t{} < numb); // Is there anyone home?

        constexpr int_t zero{};
        constexpr int_t one = 1;
        constexpr int_t ten = 10;

        do
        {
            if (const auto rem = numb % ten; rem != one)
            {
                return zero;
            }

            numb /= ten;
        } while (numb != zero);

        return one;
    }

    // 1) Perfect division.
    //  dig_sum = 4888_812, digits = {4, 8}, delta = 4.
    //       4888_840, dif = 40-12 = 28 = 4*x, x = 7.
    //    Next digit sum is 4888_812 + 4*7 = 4888_840
    // 2) 1 greater.
    //  dig_sum = 4888_182, digits = {4, 8}, delta = 4.
    //       4888_400, dif = 400-182 = 218 = 4*x, x = round_up(218.0/4) = 55
    //    Next dig_sum is 4888_182 + 4*55 = 4888_402
    template<class long_int_t, class int_t>
    requires(same_sign_leq_size<int_t, long_int_t>)
    [[nodiscard]] constexpr auto largest_different_digit_ratio(const char digit1, long_int_t dig_sum, const char digit2)
        -> int_t
    {
        const auto delta = static_cast<long_int_t>(digit2 - digit1);

        assert(0 < digit1 && digit1 < digit2 && long_int_t{} < delta);

        long_int_t low{};
        long_int_t cur{};
        long_int_t next{};
        long_int_t p10 = 1;

        do
        {
            assert(long_int_t{} < dig_sum && long_int_t{} < p10);

            constexpr long_int_t ten = 10;

            const auto dig = static_cast<long_int_t>(dig_sum % ten);
            dig_sum /= ten;

            low += p10 * dig;
            assert(long_int_t{} <= low && long_int_t{} <= dig_sum);

            if (dig == static_cast<long_int_t>(digit1) || dig == static_cast<long_int_t>(digit2))
            {
                p10 *= ten;
                continue;
            }

            cur = low;

            const auto dig2 = dig < static_cast<long_int_t>(digit1) ? digit1
                : dig < static_cast<long_int_t>(digit2)             ? digit2
                                                                    : static_cast<char>(ten);

            next = static_cast<long_int_t>(p10 * static_cast<long_int_t>(dig2));

            assert(dig < static_cast<long_int_t>(dig2) && long_int_t{} < static_cast<long_int_t>(dig2) &&
                static_cast<long_int_t>(dig2) <= ten && low < next);

            p10 *= ten;
        } while (dig_sum != long_int_t{});

        assert(long_int_t{} <= cur && cur < next);

        auto rem = static_cast<long_int_t>((next - cur) % delta);

        auto ratio = static_cast<int_t>( // todo(p3): overflow.
            (next - cur) / delta + (rem != long_int_t{} ? 1 : 0));

        if (ratio == int_t{})
        {
            ratio = 1;
        }

        assert(int_t{} < ratio);

        return ratio;
    }

    // n=1, return 1
    // n=2, 10
    // n=3, 100
    // 10**(n - 1)
    template<class int_t>
    [[nodiscard]] constexpr auto power10_slow(const int_t &pow) -> int_t
    {
        assert(int_t{} <= pow);

        int_t res = 1;

        for (int_t index{}; index < pow; ++index)
        {
            constexpr auto base1 = 10;

            res *= base1;

            assert(int_t{} < res);
        }

        return res;
    }

    template<class int_t>
    [[nodiscard]] constexpr auto has_given_digits_slow(
        int_t numb, const char digit1, int_t &digit_sum1, const char digit2) -> bool
    {
        assert(int_t{} < numb);

        digit_sum1 = int_t{};

        do
        {
            constexpr int_t ten = 10;

            const auto dig = static_cast<int_t>(numb % ten);

            if (dig != static_cast<int_t>(digit1) && dig != static_cast<int_t>(digit2))
            {
                return false;
            }

            numb /= ten;
            digit_sum1 += dig;
        } while (numb != int_t{});

        return true;
    }
} // namespace Standard::Algorithms::Numbers::Inner

namespace Standard::Algorithms::Numbers
{
    // Given positive integers n, digit1, digit2,
    // count such numbers x of length n that only digits digit1 and digit2 are in:
    // - x,
    // - digits sum(x).
    template<class long_int_t, class int_t, class factorials_t, int_t modulus>
    requires(same_sign_leq_size<int_t, long_int_t> && int_t{} < modulus)
    [[nodiscard]] constexpr auto digit_and_sum_of_ab(
        const factorials_t &factorials, char digit1, const int_t &numb, char digit2) -> int_t
    {
        Inner::dsu_check_input<int_t, modulus>(digit1, numb, digit2);

        if (digit1 == digit2)
        {
            auto has = Inner::has_only_ones<int_t>(numb);
            return has;
        }

        if (digit2 < digit1)
        {
            std::swap(digit2, digit1);
        }

        const auto delta = static_cast<long_int_t>(digit2 - digit1);
        assert(digit1 < digit2 && long_int_t{} < delta);

        // digit sum = digit1 * (n - x) + digit2 * x,  where x = [0..n]
        //           = digit1 * n + (digit2 - digit1) * x

        const auto dig_sum_min = static_cast<long_int_t>(numb * static_cast<long_int_t>(digit1));

        const auto dig_sum_max = static_cast<long_int_t>(numb * static_cast<long_int_t>(digit2));

        assert(long_int_t{} < dig_sum_min && dig_sum_min < dig_sum_max);

        auto dig_sum = dig_sum_min;
        long_int_t count{};
        int_t mmm{};

        do
        {
            assert(long_int_t{} < dig_sum);

            long_int_t dig_sum2{};

            if (!Inner::has_given_digits_slow<long_int_t>(dig_sum, digit1, dig_sum2, digit2))
            {
                // todo(p3): even faster skip.
                const auto ratio = Inner::largest_different_digit_ratio<long_int_t, int_t>(digit1, dig_sum, digit2);

                const auto larger_step = static_cast<long_int_t>(static_cast<long_int_t>(ratio) * delta);

                assert(long_int_t{} < larger_step);

                dig_sum += larger_step;
                mmm += ratio;

                assert(long_int_t{} < dig_sum && int_t{} < mmm);

                continue;
            }

            const auto ncm = factorials.choose_modulo(numb, mmm);
            assert(long_int_t{} <= ncm && ncm < modulus);

            count += ncm;

            if (!(count < modulus))
            {
                count -= modulus;
            }

            dig_sum += delta;
            ++mmm;

            assert(long_int_t{} <= count && long_int_t{} < dig_sum && int_t{} < mmm);
        } while (dig_sum <= dig_sum_max);

        return static_cast<int_t>(count % modulus);
    }

    // Slow still. Enumerate all digit sums.
    template<class long_int_t, class int_t, class factorials_t, int_t modulus>
    requires(same_sign_leq_size<int_t, long_int_t>)
    [[nodiscard]] constexpr auto digit_and_sum_of_ab_slow_still(
        const factorials_t &factorials, char digit1, const int_t &numb, char digit2) -> int_t
    {
        Inner::dsu_check_input<int_t, modulus>(digit1, numb, digit2);

        if (digit2 == digit1)
        {
            const auto dig_sum =
                static_cast<long_int_t>(static_cast<long_int_t>(numb) * static_cast<long_int_t>(digit1));

            long_int_t dig_sum2{};

            auto has =
                static_cast<long_int_t>(Inner::has_given_digits_slow<long_int_t>(dig_sum, digit1, dig_sum2, digit1));

            return has;
        }

        if (digit2 < digit1)
        {
            std::swap(digit2, digit1);
        }

        assert(digit1 < digit2);

        const auto dig_sum_min =
            static_cast<long_int_t>(static_cast<long_int_t>(numb) * static_cast<long_int_t>(digit1));

        const auto dig_sum_max =
            static_cast<long_int_t>(static_cast<long_int_t>(numb) * static_cast<long_int_t>(digit2));

        const auto delta = static_cast<long_int_t>(digit2 - digit1);

        assert(long_int_t{} < dig_sum_min && dig_sum_min < dig_sum_max && long_int_t{} < delta);

        long_int_t count{};
        int_t mmm{};
        auto dig_sum = dig_sum_min;

        for (; dig_sum <= dig_sum_max; dig_sum += delta, ++mmm)
        {
            assert(long_int_t{} < dig_sum && int_t{} <= mmm && mmm <= numb);

            long_int_t dig_sum2{};

            if (!Inner::has_given_digits_slow<long_int_t>(dig_sum, digit1, dig_sum2, digit2))
            {
                continue;
            }

            const auto ncm = factorials.choose_modulo(numb, mmm);
            assert(long_int_t{} <= ncm && ncm < modulus);

            count += ncm;

            if (!(count < modulus))
            {
                count -= modulus;
            }

            assert(long_int_t{} <= count && count < modulus);
        }

        assert(dig_sum == dig_sum_max + delta && mmm == numb + 1);

        return static_cast<int_t>(count);
    }

    // Slow. Enumerate all numbers.
    template<class int_t, int_t modulus>
    [[nodiscard]] constexpr auto digit_and_sum_of_ab_slow(const char digit1, const int_t &numb, const char digit2)
        -> int_t
    {
        Inner::dsu_check_input<int_t, modulus>(digit1, numb, digit2);

        constexpr int_t ten = 10;
        {
            constexpr int_t largest = ten + 1;

            require_greater(largest, numb, "numb");
        }

        const auto initial = static_cast<int_t>(Inner::power10_slow<int_t>(numb - 1));

        const auto edge = static_cast<int_t>(initial * ten);
        assert(int_t{} < initial && initial < edge);

        std::uint64_t count{};

        for (auto number = initial; number < edge; ++number)
        {
            int_t dig_sum{};

            if (!Inner::has_given_digits_slow<int_t>(number, digit1, dig_sum, digit2))
            {
                continue;
            }

            int_t dig_sum2{};

            const auto has2 = Inner::has_given_digits_slow<int_t>(dig_sum, digit1, dig_sum2, digit2);

            count += has2 ? 1 : 0;
        }

        auto res = static_cast<int_t>(count % static_cast<std::uint64_t>(modulus));

        return res;
    }
} // namespace Standard::Algorithms::Numbers
