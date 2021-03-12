#include"arithmetic_tests.h"
#include"../Utilities/random.h"
#include"../Utilities/test_utilities.h"
#include"arithmetic.h"
#include"bit_utilities.h"
#include"default_modulus.h"
#include"factorials_cached.h"
#include"factoring_utilities.h"
#include"gcd.h"
#include"modulo_inverse.h"
#include<tuple>
#include<vector>

namespace
{
    constexpr auto modulo = Standard::Algorithms::Numbers::default_modulus;

    template<class t>
    struct triple final
    {
        t a{};
        t b{};
        t expected{};
    };

    template<class t>
    auto operator<< (std::ostream &str, const triple<t> &trip) -> std::ostream &
    {
        str << "A " << trip.a << ", B " << trip.b << ", Expected " << trip.expected;

        return str;
    }

    using unsigned_t = std::uint16_t;
    using signed_t = std::int16_t;

    using unsigned_triple_t = triple<unsigned_t>;
    using signed_triple_t = triple<signed_t>;

    template<class t>
    using add_function_t = t (*)(const t &one, const t &two);

    namespace
    {
        // todo(p3): remove globals and reinterpret_cast
        // NOLINTNEXTLINE
        bool IsSigned; // todo(p3): del global vars

        // NOLINTNEXTLINE
        bool ShallTestReverse;

        // NOLINTNEXTLINE
        void *add_function_p{};

        // NOLINTNEXTLINE
        std::int64_t ArgumentA;

        // NOLINTNEXTLINE
        std::int64_t ArgumentB;

        constexpr void test_modular_inverses_first_n()
        {
            const std::string name = "test_modular_inverses_till_";
            const std::array data{// NOLINTNEXTLINE
                3, 5, 7, 11, 13, 17, 97
            };

            std::vector<std::int32_t> expected{ 0, 1 };
            std::vector<std::int32_t> actual;

            for (const auto &size : data)
            {
                expected.resize(size + 1LL);

                for (auto index = 2; index < size; ++index)
                {
                    expected[index] = static_cast<std::int32_t>(
                        Standard::Algorithms::Numbers::modulo_inverse_prime<std::int64_t>(index, size));
                }

                actual.clear();

                Standard::Algorithms::Numbers::modular_inverses_first_n<std::int32_t, std::int64_t>(size, actual, size);
                ::Standard::Algorithms::ert::are_equal(expected, actual, name + std::to_string(size));
            }
        }
    } // namespace

    template<class t>
    constexpr void fail_run_impl()
    {
        // todo(p3): remove globals and reinterpret_cast
        // NOLINTNEXTLINE
        auto addFunction = reinterpret_cast<add_function_t<t>>(add_function_p);

        Standard::Algorithms::throw_if_null("add_function_p", addFunction);

        const auto one = static_cast<t>(ArgumentA);
        const auto two = static_cast<t>(ArgumentB);

        addFunction(one, two);
    }

    void fail_run()
    {
        if (IsSigned)
        {
            fail_run_impl<signed_t>();
        }
        else
        {
            fail_run_impl<unsigned_t>();
        }
    }

    template<class t>
    constexpr void success(
        const std::string &prefix_name, const std::vector<triple<t>> &test_cases, add_function_t<t> add_function)
    {
        Standard::Algorithms::throw_if_null("addFunction", add_function);

        for (const auto &test : test_cases)
        {
            auto str = ::Standard::Algorithms::Utilities::w_stream();
            str << prefix_name << "_" << test;

            const auto name = str.str();
            {
                const auto actual = add_function(test.a, test.b);
                ::Standard::Algorithms::ert::are_equal(test.expected, actual, name);
            }

            if (ShallTestReverse)
            {
                const auto actual = add_function(test.b, test.a);

                ::Standard::Algorithms::ert::are_equal(test.expected, actual, name + "_Reverse");
            }
        }
    }

    template<class t>
    constexpr void fail(const std::string &prefix_name, const std::vector<triple<t>> &test_cases,
        const std::string &bad_part1, add_function_t<t> add_function, const std::string &bad_part2)
    {
        Standard::Algorithms::throw_if_null("addFunction", add_function);

        for (const auto &test : test_cases)
        {
            auto str = ::Standard::Algorithms::Utilities::w_stream();
            str << prefix_name << "_" << test;

            const auto name = str.str();
            {
                auto str_2 = ::Standard::Algorithms::Utilities::w_stream();

                // todo(p3): Remove copy-paste.
                str_2 << bad_part1 << test.a << bad_part2 << test.b << " will produce an overflow.";
                const auto expected_message = str_2.str();

                ArgumentA = test.a;
                ArgumentB = test.b;

                ::Standard::Algorithms::ert::expect_exception<Standard::Algorithms::Numbers::arithmetic_exception>(
                    expected_message, fail_run, name);
            }

            if (ShallTestReverse)
            {
                auto str_2 = ::Standard::Algorithms::Utilities::w_stream();
                // todo(p3): Remove copy-paste.
                str_2 << bad_part1 << test.b << bad_part2 << test.a << " will produce an overflow.";

                const auto expected_message = str_2.str();
                ArgumentA = test.b;
                ArgumentB = test.a;

                ::Standard::Algorithms::ert::expect_exception<Standard::Algorithms::Numbers::arithmetic_exception>(
                    expected_message, fail_run, name);
            }
        }
    }

    void test_add_unsigned()
    {
        const std::string prefix_name = "add_unsigned";

        add_function_t<unsigned_t> add_function = Standard::Algorithms::Numbers::add_unsigned<unsigned_t>;

        // NOLINTNEXTLINE
        add_function_p = reinterpret_cast<void *>(add_function);
        IsSigned = false;

        constexpr auto maxx = std::numeric_limits<unsigned_t>::max();
        {
            std::vector<unsigned_triple_t> test_cases;
            test_cases.push_back({ 1, 3, 4 });
            test_cases.push_back({ 0, maxx, maxx });
            test_cases.push_back({ 0, 0, 0 });

            success<unsigned_t>(prefix_name, test_cases, add_function);
        }
        {
            std::vector<unsigned_triple_t> test_cases;
            test_cases.push_back({ maxx, maxx, maxx });

            fail<unsigned_t>(prefix_name, test_cases, "Adding ", add_function, " and ");
        }
    }

    void test_add_signed()
    {
        const std::string prefix_name = "add_signed";

        add_function_t<signed_t> add_function = Standard::Algorithms::Numbers::add_signed<signed_t>;

        // NOLINTNEXTLINE
        add_function_p = reinterpret_cast<void *>(add_function);
        IsSigned = true;

        constexpr auto minn = std::numeric_limits<signed_t>::min();
        constexpr auto maxx = std::numeric_limits<signed_t>::max();

        {
            std::vector<signed_triple_t> test_cases;
            test_cases.push_back({ 1, 3, 4 });
            test_cases.push_back({ 0, maxx, maxx });
            test_cases.push_back(
                // NOLINTNEXTLINE
                { 10, maxx - 10, maxx });

            test_cases.push_back({ maxx / 2, maxx - (maxx / 2), maxx });
            test_cases.push_back({ 0, 0, 0 });

            success<signed_t>(prefix_name, test_cases, add_function);
        }
        {
            std::vector<signed_triple_t> test_cases;
            test_cases.push_back({ minn, minn, 0 });
            test_cases.push_back({ maxx, maxx, 0 });
            test_cases.push_back({ 1, maxx, 0 });
            test_cases.push_back({ -1, minn, 0 });

            fail<signed_t>(prefix_name, test_cases, "Adding ", add_function, " and ");
        }
    }

    // todo(p3): void test_subtract_unsigned();
    void test_subtract_signed()
    {
        const std::string prefix_name = "subtract_signed";

        add_function_t<signed_t> add_function = Standard::Algorithms::Numbers::subtract_signed<signed_t>;

        // NOLINTNEXTLINE
        add_function_p = reinterpret_cast<void *>(add_function);
        IsSigned = true;

        constexpr auto maxx = std::numeric_limits<signed_t>::max();
        constexpr auto minn = std::numeric_limits<signed_t>::min();
        {
            std::vector<signed_triple_t> test_cases;
            test_cases.push_back({ 1, 3, -2 });
            test_cases.push_back({ 3, 1, 2 });
            test_cases.push_back({ 3, 0, 3 });
            test_cases.push_back({ 0, maxx, -maxx });
            test_cases.push_back({ maxx, 0, maxx });
            test_cases.push_back({ -maxx, -maxx, 0 });
            test_cases.push_back({ maxx, maxx, 0 });
            test_cases.push_back({ minn, minn, 0 });
            test_cases.push_back({ minn, 0, minn });

            test_cases.push_back(
                // NOLINTNEXTLINE
                { minn, -500, -32268 });

            test_cases.push_back(
                // NOLINTNEXTLINE
                { -10, maxx - 10, -maxx });

            test_cases.push_back({ 0, 0, 0 });
            test_cases.push_back({ -1, maxx, minn });
            test_cases.push_back({ -1, minn, maxx });

            success<signed_t>(prefix_name, test_cases, add_function);
        }
        {
            std::vector<signed_triple_t> test_cases;
            test_cases.push_back({ maxx, -maxx, 0 });
            test_cases.push_back({ 0, minn, 0 });
            test_cases.push_back({ -2, maxx, 0 });
            test_cases.push_back({ 1, minn, 0 });

            fail<signed_t>(prefix_name, test_cases, "Subtracting ", add_function, " minus ");
        }
    }

    constexpr void test_numbers_having_reminder()
    {
        using typ = std::uint32_t;
        constexpr typ divisor = 5;

        const auto test_cases = std::vector<std::tuple<typ, typ, typ>>{ std::make_tuple(0, 1, 0),
            std::make_tuple(0, 2, 0), std::make_tuple(0, 3, 0), std::make_tuple(0, 4, 0),
            // NOLINTNEXTLINE
            std::make_tuple(0, 5, 1),
            // NOLINTNEXTLINE
            std::make_tuple(0, 6, 1),
            // NOLINTNEXTLINE
            std::make_tuple(0, 9, 1),
            // NOLINTNEXTLINE
            std::make_tuple(0, 10, 2),
            // NOLINTNEXTLINE
            std::make_tuple(0, 11, 2), std::make_tuple(1, 1, 1), std::make_tuple(1, 2, 1), std::make_tuple(1, 3, 1),
            std::make_tuple(1, 4, 1),
            // NOLINTNEXTLINE
            std::make_tuple(1, 5, 1),
            // NOLINTNEXTLINE
            std::make_tuple(1, 6, 2),
            // NOLINTNEXTLINE
            std::make_tuple(1, 9, 2),
            // NOLINTNEXTLINE
            std::make_tuple(1, 10, 2),
            // NOLINTNEXTLINE
            std::make_tuple(1, 11, 3), std::make_tuple(2, 1, 0), std::make_tuple(2, 2, 1), std::make_tuple(2, 3, 1),
            std::make_tuple(2, 4, 1),
            // NOLINTNEXTLINE
            std::make_tuple(2, 5, 1),
            // NOLINTNEXTLINE
            std::make_tuple(2, 6, 1),
            // NOLINTNEXTLINE
            std::make_tuple(2, 7, 2),
            // NOLINTNEXTLINE
            std::make_tuple(2, 9, 2),
            // NOLINTNEXTLINE
            std::make_tuple(2, 11, 2),
            // NOLINTNEXTLINE
            std::make_tuple(2, 12, 3),
            // NOLINTNEXTLINE
            std::make_tuple(3, 1, 0), std::make_tuple(3, 2, 0), std::make_tuple(3, 3, 1), std::make_tuple(3, 4, 1),
            // NOLINTNEXTLINE
            std::make_tuple(3, 7, 1),
            // NOLINTNEXTLINE
            std::make_tuple(3, 8, 2),
            // NOLINTNEXTLINE
            std::make_tuple(3, 9, 2),
            // NOLINTNEXTLINE
            std::make_tuple(3, 12, 2),
            // NOLINTNEXTLINE
            std::make_tuple(3, 13, 3), std::make_tuple(4, 1, 0), std::make_tuple(4, 2, 0), std::make_tuple(4, 3, 0),
            std::make_tuple(4, 4, 1),
            // NOLINTNEXTLINE
            std::make_tuple(4, 5, 1),
            // NOLINTNEXTLINE
            std::make_tuple(4, 8, 1),
            // NOLINTNEXTLINE
            std::make_tuple(4, 9, 2),
            // NOLINTNEXTLINE
            std::make_tuple(4, 10, 2),
            // NOLINTNEXTLINE
            std::make_tuple(4, 13, 2),
            // NOLINTNEXTLINE
            std::make_tuple(4, 14, 3) };

        for (const auto &test : test_cases)
        {
            const auto &reminder = std::get<0>(test);
            const auto &numb = std::get<1>(test);

            const auto actual = Standard::Algorithms::Numbers::numbers_having_reminder<typ, divisor>(reminder, numb);

            const auto &expected = std::get<2>(test);
            ::Standard::Algorithms::ert::are_equal(expected, actual,
                "numbers_having_reminder(" + std::to_string(reminder) + ", " + std::to_string(numb) + ")");
        }
    }

    constexpr void test_gcd()
    {
        using int_t = std::uint64_t;
        using test_case = std::tuple<int_t, int_t, int_t>;

        const std::vector<test_case> tests{ std::make_tuple(0, 0, 0), std::make_tuple(0, 1, 1),
            // NOLINTNEXTLINE
            std::make_tuple(0, 10, 10), // NOLINTNEXTLINE
            std::make_tuple(10, 0, 10), std::make_tuple(1, 1, 1), std::make_tuple(1, 2, 1), std::make_tuple(2, 2, 2),
            std::make_tuple(2, 4, 2), // NOLINTNEXTLINE
            std::make_tuple(21, 14, 7), // NOLINTNEXTLINE
            std::make_tuple(11 * 13 * 7 * 19, 39 * 13 * 7 * 89, 13 * 7) };

        for (const auto &test : tests)
        {
            const auto &one = std::get<0>(test);
            const auto &two = std::get<1>(test);
            const auto &expected = std::get<2>(test);

            const auto name = "GCD(" + std::to_string(one) + ", " + std::to_string(two) + ")";
            {
                const auto actual = Standard::Algorithms::Numbers::gcd_unsigned(one, two);
                ::Standard::Algorithms::ert::are_equal(expected, actual, name);
            }
            {
                const auto actual = Standard::Algorithms::Numbers::gcd_unsigned(two, one);
                ::Standard::Algorithms::ert::are_equal(expected, actual, name + "_reverse");
            }

            const auto name2 = "2_" + name;
            const auto name3 = "3_" + name;
            {
                int_t first_coef{};
                int_t second_coeffic{};

                const auto actual = Standard::Algorithms::Numbers::gcd_extended(one, two, first_coef, second_coeffic);
                ::Standard::Algorithms::ert::are_equal(expected, actual, name2);

                if (0U != one && 0U != two)
                {
                    const auto actual3 = one * first_coef + two * second_coeffic;
                    ::Standard::Algorithms::ert::are_equal(expected, actual3, name3);

                    first_coef = second_coeffic = 0;

                    const auto gcd_actual =
                        Standard::Algorithms::Numbers::gcd_extended_simple(one, two, first_coef, second_coeffic);

                    const auto name3g = "3_gcde_" + name;
                    ::Standard::Algorithms::ert::are_equal(expected, gcd_actual, name3g);

                    const auto actual3g = one * first_coef + two * second_coeffic;
                    ::Standard::Algorithms::ert::are_equal(expected, actual3g, name3g + "_sum");
                }
            }
            {
                int_t first_coef{};
                int_t second_coeffic{};

                const auto actual = Standard::Algorithms::Numbers::gcd_extended(two, one,
                    // Swap numbers
                    second_coeffic, first_coef);

                ::Standard::Algorithms::ert::are_equal(expected, actual, name2 + "_reverse");

                if (0U != one && 0U != two)
                {
                    const auto actual3 = one * first_coef + two * second_coeffic;
                    ::Standard::Algorithms::ert::are_equal(expected, actual3, name3);

                    first_coef = second_coeffic = 0;

                    const auto gcd_actual =
                        Standard::Algorithms::Numbers::gcd_extended_simple(two, one, second_coeffic, first_coef);
                    const auto name3g = "3_gcde_rev_" + name;
                    ::Standard::Algorithms::ert::are_equal(expected, gcd_actual, name3g);

                    const auto actual3g = one * first_coef + two * second_coeffic;
                    ::Standard::Algorithms::ert::are_equal(expected, actual3g, name3g + "_sum");
                }
            }
        }
    }

    void test_modulo_inverse()
    {
        using int_t = std::int64_t;
        using test_case = std::tuple<int_t, int_t>;

        test_case random_test_case{};
        {
            const auto some_number = Standard::Algorithms::Utilities::random_t<std::int32_t>(1, modulo - 1)();

            const auto actual = Standard::Algorithms::Numbers::modulo_inverse<int_t>(some_number, modulo);

            random_test_case = std::make_tuple(some_number, actual);
        }

        const std::vector<test_case> tests{ std::make_tuple(1, 1),
            // NOLINTNEXTLINE
            std::make_tuple(2, 500'000'004), random_test_case };

        for (const auto &test : tests)
        {
            const auto &one = std::get<0>(test);
            const auto &expected = std::get<1>(test);
            const auto name = "modulo_inverse " + std::to_string(one);

            const auto actual = Standard::Algorithms::Numbers::modulo_inverse<int_t>(one, modulo);
            ::Standard::Algorithms::ert::are_equal(expected, actual, name);
            ::Standard::Algorithms::ert::greater_or_equal(actual, 0, name);
            ::Standard::Algorithms::ert::greater(modulo, actual, name);

            const auto prod = (one * actual) % modulo;
            ::Standard::Algorithms::ert::are_equal(1, prod, "prod_" + name);
        }
    }

    constexpr void test_find_first_byte()
    {
        using int_t = std::uint64_t;
        using pa_t = std::pair<int_t, std::uint32_t>;

        const std::vector<pa_t> tests = {// NOLINTNEXTLINE
            { 0x1234567890ABCDEF, 8 },
            // NOLINTNEXTLINE
            { 0, 7 },
            // NOLINTNEXTLINE
            { 0xFE2F30797E7F8081, 8 },
            // NOLINTNEXTLINE
            { 0xFE2F03797E7F8081, 5 },
            // NOLINTNEXTLINE
            { 0xC000, 7 }, // 49152
            // NOLINTNEXTLINE
            { 0xFFFF, 7 },
            // NOLINTNEXTLINE
            { 0x001029FFEE000001, 7 },
            // NOLINTNEXTLINE
            { 0x27E847F0FF303139, 8 },
            // NOLINTNEXTLINE
            { 0x2E2F30797E7F0410, 1 },
            // NOLINTNEXTLINE
            { 0xFFFFFFFFFFFFFFFF, 8 },
            // NOLINTNEXTLINE
            { 0xFF09FFFFFFFFFFFF, 6 },
            // NOLINTNEXTLINE
            { 0xFF0AFFFFFFFFFFFF, 8 },
            // NOLINTNEXTLINE
            { 0xFFFF07FFFFFFFFFF, 5 },
            // NOLINTNEXTLINE
            { 0xFFFF0700FFFFFF00, 5 },
            // NOLINTNEXTLINE
            { 0xFFFFFFFFFFFFFF04, 0 },
            // NOLINTNEXTLINE
            { 0xFFFFFFFFFFFF0304, 1 }
        };

        for (const auto &test : tests)
        {
            const auto actual = Standard::Algorithms::Numbers::find_first_byte(test.first);

            const auto name = "Test_find_first_byte(" + std::to_string(test.first);
            ::Standard::Algorithms::ert::are_equal(test.second, actual, name);
        }
    }

    constexpr void test_multi_byte_subtract()
    {
        using int_t = std::uint64_t;
        using pa_t = std::pair<int_t, int_t>;

        const std::vector<pa_t> tests = {// NOLINTNEXTLINE
            { 0, 0xD0D0D0D0D0D0D0D0 },
            // NOLINTNEXTLINE
            { 0x1234567890ABCDEF, 0xE2042648607B9DBF },
            // NOLINTNEXTLINE
            { 0x001029FFEE000001, 0xD0E0F9CFBED0D0D1 },
            // NOLINTNEXTLINE
            { 0x27E847F0FF303139, 0xF7B817C0CF000109 },
            // NOLINTNEXTLINE
            { 0x2E2F30797E7F8081, 0xFEFF00494E4F5051 }
        };

        for (const auto &test : tests)
        {
            constexpr int_t subtrahend = Standard::Algorithms::Numbers::lower_high_mask;

            auto actual = test.first;
            Standard::Algorithms::Numbers::multi_byte_subtract<subtrahend>(actual);

            const auto name = "multi_byte_subtract(" + std::to_string(test.first);
            ::Standard::Algorithms::ert::are_equal(test.second, actual, name);
        }
    }

    constexpr void test_powers_sum()
    {
        using int_t = std::uint64_t;

        constexpr auto mod = static_cast<std::uint32_t>(modulo);

        // base, power, expected.
        const auto tests = std::vector<std::tuple<int_t, int_t, int_t>>{
            std::make_tuple(0, 0, 0), std::make_tuple(0, 1, 0), std::make_tuple(0, 2, 0),
            // NOLINTNEXTLINE
            std::make_tuple(0, 235, 0), std::make_tuple(1, 0, 0), std::make_tuple(2, 0, 0), std::make_tuple(3, 0, 0),
            std::make_tuple(mod - 2, 0, 0), std::make_tuple(mod - 1, 0, 0), std::make_tuple(mod, 0, 0),
            std::make_tuple(mod + 1, 0, 0), std::make_tuple(mod + 2, 0, 0), std::make_tuple(mod + 3, 0, 0),
            std::make_tuple(1, 1, 1), std::make_tuple(1, 2, 2),
            // NOLINTNEXTLINE
            std::make_tuple(1, 5, 5),
            // NOLINTNEXTLINE
            std::make_tuple(1, mod + 1234, 1234), std::make_tuple(2, 1, 1), std::make_tuple(2, 2, 3),
            // NOLINTNEXTLINE
            std::make_tuple(2, 3, 7),
            // NOLINTNEXTLINE
            std::make_tuple(2, 4, 15),
            // NOLINTNEXTLINE
            std::make_tuple(2, 5, 31),
            // NOLINTNEXTLINE
            std::make_tuple(2, 6, 63),
            // NOLINTNEXTLINE
            std::make_tuple(2, 32, 294'967'267), // 4294967295
            std::make_tuple(3, 1, 1), // 1
            std::make_tuple(3, 2, 4), // 3
            // NOLINTNEXTLINE
            std::make_tuple(3, 3, 13), // 9
            // NOLINTNEXTLINE
            std::make_tuple(3, 4, 40), // 27
            // NOLINTNEXTLINE
            std::make_tuple(3, 5, 121), // 81
            // NOLINTNEXTLINE
            std::make_tuple(3, 6, 121 + 243), // 243
            // NOLINTNEXTLINE
            std::make_tuple(3, 32, 87'940'350), // Medium complexity to check.
        };

        for (const auto &test : tests)
        {
            const auto &base1 = std::get<0>(test);
            const auto &power1 = std::get<1>(test);

            const auto actual = Standard::Algorithms::Numbers::powers_sum_modulo<int_t, mod>(base1, power1);

            const auto name = "powers_sum_modulo(" + std::to_string(base1) + ", " + std::to_string(power1);
            const auto &expected = std::get<2>(test);

            ::Standard::Algorithms::ert::are_equal(expected, actual, name);
            ::Standard::Algorithms::ert::greater(mod, actual, name + " reminder");
        }
    }

    constexpr void test_factorials_cached()
    {
        using factorial_t = Standard::Algorithms::Numbers::factorials_cached<>;

        constexpr auto max_size = 10'000U;

        const std::vector<factorial_t> facts{ factorial_t(1),
            // NOLINTNEXTLINE
            factorial_t(5),
            // NOLINTNEXTLINE
            factorial_t(max_size) };

        // index, n, k, result.
        using t_t = std::tuple<std::uint32_t, std::uint32_t, std::uint32_t, std::uint32_t>;

        const std::vector<t_t> tests{ std::make_tuple(0, 0, 0, 1), std::make_tuple(0, 1, 0, 1),
            std::make_tuple(0, 1, 1, 1), std::make_tuple(1, 0, 0, 1), std::make_tuple(1, 1, 0, 1),
            std::make_tuple(1, 1, 1, 1), std::make_tuple(1, 3, 2, 3), std::make_tuple(1, 3, 1, 3),
            std::make_tuple(1, 3, 3, 1),
            // NOLINTNEXTLINE
            std::make_tuple(1, 5, 3, 10),
            // NOLINTNEXTLINE
            std::make_tuple(1, 5, 2, 10),
            // NOLINTNEXTLINE
            std::make_tuple(1, 5, 0, 1),
            // NOLINTNEXTLINE
            std::make_tuple(1, 5, 1, 5) };

        std::int32_t pos{};

        for (const auto &test : tests)
        {
            const auto &index = std::get<0>(test);
            const auto &nnn = std::get<1>(test);
            const auto &kkk = std::get<2>(test);
            const auto &fac = facts.at(index);
            const auto actual = fac.choose_modulo(nnn, kkk);

            const auto &expected = std::get<3>(test);
            ::Standard::Algorithms::ert::are_equal(expected, actual, "Test factorials_cached n_" + std::to_string(pos));

            ++pos;
        }

        for (auto ind_2 = 1; ind_2 <= 2; ++ind_2)
        {
            constexpr auto nnn = 5U;

            const auto name = std::to_string(ind_2);
            const auto &fac = facts.at(ind_2);

            {
                const auto actual = fac.factorial_modulo(nnn);

                constexpr auto n_factorial = 120U;

                ::Standard::Algorithms::ert::are_equal(n_factorial, actual, "factorial_modulo_" + name);
            }
            {
                const auto actual = fac.inverse_factorial_modulo(nnn);

                constexpr auto inverse = 808'333'339U;

                ::Standard::Algorithms::ert::are_equal(inverse, actual, "inverse_factorial_modulo_" + name);
            }
        }

        {
            const auto &fac_2 = facts.at(2);

            ::Standard::Algorithms::ert::are_equal(max_size, fac_2.size(), "max size");

            ::Standard::Algorithms::ert::are_equal( // NOLINTNEXTLINE
                531950728U, fac_2.factorial_modulo(max_size), "factorial_modulo");

            ::Standard::Algorithms::ert::are_equal( // NOLINTNEXTLINE
                556156297U, fac_2.inverse_factorial_modulo(max_size), "inverse_factorial_modulo");

            ::Standard::Algorithms::ert::are_equal( // NOLINTNEXTLINE
                8365757U, fac_2.factorial_modulo(1234), "factorial_modulo");

            ::Standard::Algorithms::ert::are_equal( // NOLINTNEXTLINE
                53320459U, fac_2.inverse_factorial_modulo(1234), "inverse_factorial_modulo");
        }
    }
} // namespace

void Standard::Algorithms::Numbers::Tests::arithmetic_tests()
{
    // todo(p3): split into several tests.

    ShallTestReverse = true;
    test_add_unsigned();
    test_add_signed();

    ShallTestReverse = false;
    test_subtract_signed();
    test_numbers_having_reminder();
    test_gcd();
    test_modulo_inverse();
    test_multi_byte_subtract();
    test_find_first_byte();
    test_powers_sum();
    test_factorials_cached();
    test_modular_inverses_first_n();
}
