#include"digit_sum_tests.h"
#include"../Utilities/random.h"
#include"../Utilities/test_utilities.h"
#include"digit_sum.h"
#include"to_unsigned.h"

// Testing in debug.
// NOLINTNEXTLINE
std::int64_t Standard::Algorithms::Numbers::Inner::digit_sum2_rec_calls_Debug{};

namespace
{
    using int_t = std::int64_t;
    using random_t = Standard::Algorithms::Utilities::random_t<std::uint32_t>;

    constexpr auto small_mask = 0x1fFfU;

    [[nodiscard]] constexpr auto is_small(const int_t &numb) noexcept -> bool
    {
        const auto unsi = Standard::Algorithms::Numbers::to_unsigned(numb);
        const auto small = unsi & small_mask;
        return unsi == small;
    }

    // This cache can be filled once for all numbers.
    template<std::signed_integral int_t>
    [[nodiscard]] constexpr auto digit_sum_cache() -> std::vector<std::vector<int_t>>
    {
        // 19 digits in the largest std::int64 ~ 9E18.
        // max digit sum <= 19*9 < 180.

        // Better safe than sorry.
        constexpr auto cols = Standard::Algorithms::Numbers::max_number_digits + 2;

        return std::vector<std::vector<int_t>>(
            Standard::Algorithms::Numbers::digit_max * cols, std::vector<int_t>(cols, -1));
    }

    constexpr void test_digit_sum_1(std::vector<std::vector<int_t>> &cache, std::vector<char> &digits,
        const int_t &numb, const std::string &n_str, const std::vector<int_t> &prepared_sums,
        const int_t &expected = -1)
    {
        const auto fast = Standard::Algorithms::Numbers::digit_sum(prepared_sums, numb);

        if (int_t{} <= expected)
        {
            ::Standard::Algorithms::ert::are_equal(expected, fast, "digit_sum " + n_str);
        }

        {
            const auto actual = Standard::Algorithms::Numbers::digit_sum2<int_t>(cache, digits, numb);
            ::Standard::Algorithms::ert::are_equal(fast, actual, "digit_sum2 " + n_str);
        }

        if constexpr (::Standard::Algorithms::is_debug)
        {
            Standard::Algorithms::Numbers::Inner::digit_sum2_rec_calls_Debug = 0;

            const auto temp_debug = Standard::Algorithms::Numbers::digit_sum2<int_t>(cache, digits, numb);

            // NOLINTNEXTLINE NOLINT
            if (temp_debug) // Pacify the compiler.
            {
            }

            {
                constexpr auto greater1 =
                    Standard::Algorithms::Numbers::digit_ten * Standard::Algorithms::Numbers::max_number_digits;

                ::Standard::Algorithms::ert::greater(greater1,
                    Standard::Algorithms::Numbers::Inner::digit_sum2_rec_calls_Debug,
                    "second call, digit_sum2_rec_calls_Debug " + n_str);
            }
        }

        if (is_small(numb))
        {
            const auto slow = Standard::Algorithms::Numbers::digit_sum_slow(numb);
            ::Standard::Algorithms::ert::are_equal(fast, slow, "digit_sum_slow " + n_str);
        }
    }

    constexpr void test_digit_sum_range(const int_t &numb, const std::string &n_str,
        const std::vector<int_t> &prepared_sums, const int_t &tod, const std::string &tod_str)
    {
        ::Standard::Algorithms::ert::greater_or_equal(tod, numb, "to range");

        const auto fast = Standard::Algorithms::Numbers::digit_sum_range(prepared_sums, numb, tod);

        // todo(p4): digit_sum_range2

        if (is_small(numb))
        {
            const auto slow = Standard::Algorithms::Numbers::digit_sum_range_slow(numb, tod);
            ::Standard::Algorithms::ert::are_equal(fast, slow, "digit_sum_range_slow " + n_str + " ..  " + tod_str);
        }
    }

    [[nodiscard]] constexpr auto gen_number(random_t &rnd, const bool is_small1) -> int_t
    {
        if (is_small1)
        {
            auto numb1 = static_cast<int_t>(rnd() & small_mask);

            ::Standard::Algorithms::ert::greater_or_equal(numb1, int_t{}, "gen small number");

            return numb1;
        }

        constexpr auto del_sign_mask = 0x7fFfFfFfU;
        constexpr auto shift = 25U;

        const auto high = static_cast<std::uint64_t>(rnd() & del_sign_mask);
        const auto low = rnd() & del_sign_mask;

        auto numb = static_cast<int_t>((high << shift) | low);

        ::Standard::Algorithms::ert::greater_or_equal(numb, int_t{}, "gen number");

        return numb;
    }

    constexpr void test_digit_sum_large(const std::vector<int_t> &prepared_sums, std::vector<std::vector<int_t>> &cache,
        std::vector<char> &digits, random_t &rnd)
    {
        constexpr auto max_atts = 2;

        for (std::int32_t att{}; att < max_atts; ++att)
        {
            const auto is_small1 = att == 0;

            auto from = gen_number(rnd, is_small1);
            auto tod = gen_number(rnd, is_small1);

            if (tod < from)
            {
                std::swap(tod, from);
            }

            const auto n_str = std::to_string(from);
            const auto tod_str = std::to_string(tod);

            test_digit_sum_1(cache, digits, from, n_str, prepared_sums);

            test_digit_sum_range(from, n_str, prepared_sums, tod, tod_str);
        }
    }

    constexpr void test_digit_sum_specific(
        const std::vector<int_t> &prepared_sums, std::vector<std::vector<int_t>> &cache, std::vector<char> &digits)
    {
        const std::vector<std::pair<int_t, int_t>> tests{// NOLINTNEXTLINE
            { 0, 0 }, { 1, 1 }, { 2, 3 }, { 3, 6 }, { 9, 45 }, { 10, 46 },
            // NOLINTNEXTLINE
            { 11, 48 }, { 12, 51 }, { 13, 55 }, { 14, 60 }, { 15, 66 }, { 16, 73 },
            // NOLINTNEXTLINE
            { 99, 900 }, { 999, 13'500 }, { 9'999, 180'000 }, { 99'999, 2'250'000 },
            // NOLINTNEXTLINE
            { 999'999, 27'000'000 },
            // NOLINTNEXTLINE
            { 9'999'999, 315'000'000 },
            // NOLINTNEXTLINE
            { 99'999'999, 3'600'000'000 },
            // NOLINTNEXTLINE
            { 123'456'789, 4'366'712'385 },
            // NOLINTNEXTLINE
            { 999'999'999, 40'500'000'000 },
            // NOLINTNEXTLINE
            { 9'999'999'999, 450'000'000'000 },
            // NOLINTNEXTLINE
            { 99'999'999'999, 4'950'000'000'000 },
            // NOLINTNEXTLINE
            { 999'999'999'999, 54'000'000'000'000 },
            // NOLINTNEXTLINE
            { 9'999'999'999'999, 585'000'000'000'000 },
            // NOLINTNEXTLINE
            { 99'999'999'999'999, 6'300'000'000'000'000 },
            // NOLINTNEXTLINE
            { 999'999'999'999'999, 67'500'000'000'000'000 },
            // NOLINTNEXTLINE
            { 9'999'999'999'999'999, 720'000'000'000'000'000 },
            // NOLINTNEXTLINE
            { 10'000'000'000'000'000LL, 720'000'000'000'000'001LL },
            // NOLINTNEXTLINE
            { 99'999'999'999'999'990LL, 7'649'999'999'999'998'659LL },
            // NOLINTNEXTLINE
            { 99'999'999'999'999'999, 7'650'000'000'000'000'000 }
        };

        for (const auto &test : tests)
        {
            const auto &numb = test.first;
            const auto n_str = std::to_string(numb);

            const auto &expected = test.second;

            test_digit_sum_1(cache, digits, numb, n_str, prepared_sums, expected);
        }
    }
} // namespace

void Standard::Algorithms::Numbers::Tests::digit_sum_tests()
{
    const auto prepared_sums = prepare_digit_sum<int_t>();

    std::vector<char> digits{};
    random_t rnd{};
    auto cache = digit_sum_cache<int_t>();

    test_digit_sum_large(prepared_sums, cache, digits, rnd);

    test_digit_sum_specific(prepared_sums, cache, digits);

    test_digit_sum_large(prepared_sums, cache, digits, rnd);
}
