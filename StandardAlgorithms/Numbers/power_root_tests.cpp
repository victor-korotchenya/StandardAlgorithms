#include"power_root_tests.h"
#include"../Utilities/stringer.h"
#include"../Utilities/test_utilities.h"
#include"power_root.h"
#include<array>
#include<numbers>
#include<tuple>

namespace
{
    // 5 ** 27
    constexpr std::int64_t five_to_27th = 7'450'580'596'923'828'125LL;
    static_assert(0 < five_to_27th);

    template<std::integral int_t>
#ifndef __clang__
    [[nodiscard]] constexpr
#endif
        auto
        int_power_tests() -> bool
    {
        static_assert(int_t{ 1 } == Standard::Algorithms::Numbers::int_power<int_t, int_t>(2, 0));

        static_assert(int_t{ 2 } == Standard::Algorithms::Numbers::int_power<int_t, int_t>(2, 1));

        static_assert(int_t{ 4 } == Standard::Algorithms::Numbers::int_power<int_t, int_t>(2, 2));

        // NOLINTNEXTLINE
        static_assert(int_t{ 8 } == Standard::Algorithms::Numbers::int_power<int_t, int_t>(2, 3));

        constexpr int_t max_value{ 10 };
        constexpr int_t max_exponent{ 5 };

        Standard::Algorithms::Utilities::stringer stinger;

        for (int_t value{}; value <= max_value; ++value)
        {
            stinger.initialize("int_power " + ::Standard::Algorithms::Utilities::zu_string(value) + ", ");

            int_t expected = int_t{} < value ? 1 : 0;

            for (int_t exponent{}; exponent <= max_exponent; ++exponent)
            {
                stinger.reset_append(::Standard::Algorithms::Utilities::zu_string(exponent));

                const auto actual = Standard::Algorithms::Numbers::int_power<int_t, int_t>(value, exponent);

                ::Standard::Algorithms::ert::are_equal(expected, actual, stinger);

                expected *= value;
            }
        }

        return true;
    }

    template<std::integral int_t>
#ifndef __clang__
    [[nodiscard]] constexpr
#endif
        auto
        sqrt_tests() -> bool
    {
        using floating_t = Standard::Algorithms::floating_point_type;

        constexpr int_t max_value{ 19 };

        Standard::Algorithms::Utilities::stringer stinger;

        for (int_t value{}; value <= max_value; ++value)
        {
            stinger.initialize(::Standard::Algorithms::Utilities::zu_string(value));

            const auto doub = static_cast<floating_t>(value);
            const auto expected = std::sqrt(doub);
            {
                stinger.reset_append(" sqrt_newton");

                const auto actual = Standard::Algorithms::Numbers::sqrt_newton<floating_t>(doub);

                ::Standard::Algorithms::ert::are_equal_with_epsilon(expected, actual, stinger);
            }
            {
                stinger.reset_append(" root_newton");

                constexpr int_t exponent{ 2 };

                const auto actual = Standard::Algorithms::Numbers::root_newton<floating_t, int_t>(doub, exponent);

                ::Standard::Algorithms::ert::are_equal_with_epsilon(expected, actual, stinger);
            }

            const auto int_expected = static_cast<int_t>(expected);
            {
                stinger.reset_append(" int_sqrt");

                const auto actual = Standard::Algorithms::Numbers::int_sqrt<int_t>(value);

                ::Standard::Algorithms::ert::are_equal(int_expected, actual, stinger);
            }
            {
                stinger.reset_append(" int_sqrt_slow");

                const auto actual = Standard::Algorithms::Numbers::int_sqrt_slow<int_t>(value);

                ::Standard::Algorithms::ert::are_equal(int_expected, actual, stinger);
            }
        }

        return true;
    }

    template<std::integral int_t, std::floating_point floating_t = Standard::Algorithms::floating_point_type>
    requires(sizeof(std::int64_t) <= sizeof(int_t))
#ifndef __clang__
    [[nodiscard]] constexpr
#endif
        auto root_newton_tests() -> bool
    {
        {
            constexpr auto expected = std::numbers::sqrt2;

            const auto actual = Standard::Algorithms::Numbers::root_newton<floating_t, int_t>(2.0, 2);

            ::Standard::Algorithms::ert::are_equal_with_epsilon(expected, actual, "sqrt2");
        }
        {
            constexpr auto expected = std::numbers::sqrt3;

            const auto actual = Standard::Algorithms::Numbers::root_newton<floating_t, int_t>(3.0, 2);

            ::Standard::Algorithms::ert::are_equal_with_epsilon(expected, actual, "sqrt3");
        }

        constexpr floating_t one{ 1 };

        constexpr floating_t int_ten{ 10 };
        constexpr int_t max_value{ 11 };
        constexpr int_t max_exponent{ 12 };

        Standard::Algorithms::Utilities::stringer stinger;

        for (int_t value{}, val_10 = 1; value <= max_value; ++value, val_10 *= int_ten)
        {
            stinger.initialize("root_newton " + ::Standard::Algorithms::Utilities::zu_string(value) + ", ");

            const auto doub = static_cast<floating_t>(value);
            const auto doub_10 = static_cast<floating_t>(val_10);

            for (int_t exponent = 1, exp_10 = 1; exponent <= max_exponent; ++exponent, exp_10 *= int_ten)
            {
                {
                    stinger.reset_append(::Standard::Algorithms::Utilities::zu_string(exponent));

                    const auto revex = one / static_cast<floating_t>(exponent);
                    const auto expected = std::pow(doub, revex);

                    const auto actual = Standard::Algorithms::Numbers::root_newton<floating_t, int_t>(doub, exponent);

                    ::Standard::Algorithms::ert::are_equal_with_epsilon(expected, actual, stinger);
                }
                if (int_t{ 1 } < value)
                {
                    stinger.append(" rev");

                    const auto rev_base = one / static_cast<floating_t>(doub);
                    const auto revex = one / static_cast<floating_t>(exponent);
                    const auto expected = std::pow(rev_base, revex);

                    const auto actual = Standard::Algorithms::Numbers::root_newton<floating_t, int_t>(rev_base, exponent);

                    ::Standard::Algorithms::ert::are_equal_with_epsilon(expected, actual, stinger);
                }
                {
                    stinger.reset_append("tens " + ::Standard::Algorithms::Utilities::zu_string(exp_10));

                    const auto revex = one / static_cast<floating_t>(exp_10);
                    const auto expected = std::pow(doub_10, revex);

                    const auto actual = Standard::Algorithms::Numbers::root_newton<floating_t, int_t>(doub_10, exp_10);

                    ::Standard::Algorithms::ert::are_equal_with_epsilon(expected, actual, stinger);
                }
                {
                    stinger.append(" rev");

                    const auto rev_base = one / static_cast<floating_t>(doub_10);
                    const auto revex = one / static_cast<floating_t>(exp_10);
                    const auto expected = std::pow(rev_base, revex);

                    const auto actual = Standard::Algorithms::Numbers::root_newton<floating_t, int_t>(rev_base, exp_10);

                    ::Standard::Algorithms::ert::are_equal_with_epsilon(expected, actual, stinger);
                }
            }
        }

        return true;
    }

    template<std::integral int_t>
    requires(sizeof(int_t) == sizeof(std::uint64_t))
#ifndef __clang__
    [[nodiscard]] constexpr
#endif
        auto power_overflow_tests() -> bool
    {
        using test_t = std::tuple<int_t, int_t, int_t>;

        constexpr auto max_val = std::numeric_limits<int_t>::max();
        constexpr int_t pow63 = std::is_signed_v<int_t> ? max_val : (1LLU << 63LLU);

        static_assert(int_t{} < pow63);

        constexpr std::array tests{
            // NOLINTNEXTLINE
            test_t{ 1, 0, 1 }, test_t{ 1, 1, 1 }, // NOLINTNEXTLINE
            test_t{ 1, 2, 1 }, test_t{ 1, 500, 1 }, // NOLINTNEXTLINE
            test_t{ 1, max_val, 1 }, test_t{ 2, 0, 1 }, // NOLINTNEXTLINE
            test_t{ 2, 1, 2 }, test_t{ 2, 2, 4 }, test_t{ 2, 3, 8 }, // NOLINTNEXTLINE
            test_t{ 2, 10, 1024 }, test_t{ 2, 62, 1LLU << 62LLU }, // NOLINTNEXTLINE
            test_t{ 2, 63, pow63 }, test_t{ 2, 64, max_val }, test_t{ 2, 500, max_val }, // NOLINTNEXTLINE
            test_t{ 2, max_val, max_val }, test_t{ max_val, max_val, max_val }, // NOLINTNEXTLINE
            test_t{ 5, 3, 125 }, test_t{ 5, 28, max_val }, // NOLINTNEXTLINE
            test_t{ 5, 27, five_to_27th }, // NOLINTNEXTLINE
        };

        for (const auto &[base, exp, expected] : tests)
        {
            Standard::Algorithms::require_positive(base, "base in power_with_overflow_check");

            const auto actual = Standard::Algorithms::Numbers::power_with_overflow_check<int_t, max_val>(base, exp);

            ::Standard::Algorithms::ert::are_equal(expected, actual,
                "power_with_overflow_check " + ::Standard::Algorithms::Utilities::zu_string(base) + " " +
                    ::Standard::Algorithms::Utilities::zu_string(exp));
        }

        return true;
    }

    template<std::integral int_t>
    requires(sizeof(int_t) == sizeof(std::uint64_t))
#ifndef __clang__
    [[nodiscard]] constexpr
#endif
        auto int_root_binary_tests() -> bool
    {
        using test_t = std::tuple<int_t, int_t, int_t>;

        constexpr auto max_val = std::numeric_limits<int_t>::max();

        constexpr int_t p62 = 1LLU << 62LLU;
        static_assert(int_t{} < p62);

        constexpr std::array tests{
            // NOLINTNEXTLINE
            test_t{ 1, 1, 1 }, test_t{ 2, 1, 2 }, test_t{ 500, 1, 500 },
            test_t{ max_val, 1, max_val }, // NOLINTNEXTLINE
            test_t{ five_to_27th, 27, 5 }, test_t{ five_to_27th + 1, 27, 5 }, // NOLINTNEXTLINE
            test_t{ five_to_27th - 1, 27, 4 }, test_t{ five_to_27th, 27 + 1, 4 }, // NOLINTNEXTLINE
            test_t{ p62, 62, 2 }, test_t{ p62 - 1, 62, 1 }, // NOLINTNEXTLINE
            test_t{ p62, 63, 1 }, test_t{ p62 + 1, 62, 2 }, // NOLINTNEXTLINE
            test_t{ p62, 64, 1 }, test_t{ p62, 500, 1 }, // NOLINTNEXTLINE
            test_t{ p62 / 3 * 2, 62, 1 }, test_t{ p62 / 2, 62, 1 }, // NOLINTNEXTLINE
            test_t{ max_val, 64, 1 }, test_t{ max_val, max_val, 1 }, // NOLINTNEXTLINE
            // pow(2**64 - 2, 1/63) = 2.02212
            test_t{ max_val - 1, 63, std::is_signed_v<int_t> ? 1 : 2 }, // NOLINTNEXTLINE
            // todo(p4): fix overflow check. pow(2**64 - 1, 1/63) = 2.02212
            // todo(p4): fix: test_t{ max_val, 63, std::is_signed_v<int_t> ?1:2 }, // NOLINTNEXTLINE
        };

        for (const auto &[base, exp, expected] : tests)
        {
            Standard::Algorithms::require_positive(base, "base in int_root_binary");
            Standard::Algorithms::require_positive(exp, "exp in int_root_binary");

            const auto actual = Standard::Algorithms::Numbers::int_root_binary_slow<int_t, max_val>(base, exp);

            ::Standard::Algorithms::ert::are_equal(expected, actual,
                "int_root_binary_slow " + ::Standard::Algorithms::Utilities::zu_string(base) + " " +
                    ::Standard::Algorithms::Utilities::zu_string(exp));
        }

        return true;
    }
} // namespace

void Standard::Algorithms::Numbers::Tests::power_root_tests()
{
#ifndef __clang__
    static_assert(int_power_tests<std::uint64_t>());
    static_assert(int_power_tests<std::int64_t>());

    static_assert(root_newton_tests<std::uint64_t>());
    static_assert(root_newton_tests<std::int64_t>());

    static_assert(sqrt_tests<std::uint64_t>());
    static_assert(sqrt_tests<std::int64_t>());

    static_assert(power_overflow_tests<std::uint64_t>());
    static_assert(power_overflow_tests<std::int64_t>());

    static_assert(int_root_binary_tests<std::uint64_t>());
    static_assert(int_root_binary_tests<std::int64_t>());
#else
    int_power_tests<std::uint64_t>();
    int_power_tests<std::int64_t>();

    root_newton_tests<std::uint64_t>();
    root_newton_tests<std::int64_t>();

    sqrt_tests<std::uint64_t>();
    sqrt_tests<std::int64_t>();

    power_overflow_tests<std::uint64_t>();
    power_overflow_tests<std::int64_t>();

    int_root_binary_tests<std::uint64_t>();
    int_root_binary_tests<std::int64_t>();
#endif
}
