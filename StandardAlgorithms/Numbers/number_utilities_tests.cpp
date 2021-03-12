#include"number_utilities_tests.h"
#include"../Utilities/ert.h"
#include"../Utilities/random.h"
#include"modulo_inverse.h"
#include"number_utilities.h"
#include"power_root.h"
#include"sum_min_two_consecutive_numbers.h" // sum_to_n
#include"totient.h"
#include<array>
#include<tuple>

namespace
{
    constexpr void sum_test()
    {
        using tuple_t = std::tuple<std::size_t, std::size_t, std::size_t>;

        const std::vector<tuple_t> tests{ { 0, 0, 0 }, // NOLINTNEXTLINE
            { 0, 1, 1 }, { 0, 2, 3 }, { 0, 9, 45 }, // NOLINTNEXTLINE
            { 0, 10, 55 }, { 1, 10, 55 }, { 2, 10, 54 }, // NOLINTNEXTLINE
            { 3, 10, 52 }, { 4, 10, 49 }, { 5, 10, 45 } };

        for (const auto &[from, tod, expected] : tests)
        {
            const auto actual = Standard::Algorithms::Numbers::sum_to_n(from, tod);

            const auto name = "sum_to_n(" + std::to_string(from) + ", " + std::to_string(tod);

            ::Standard::Algorithms::ert::are_equal(expected, actual, name);
        }
    }

    constexpr void modulo_inverse_prime_tests()
    {
        using number_expected_t = std::pair<std::int32_t, std::int32_t>;

        constexpr auto prime_number = 11;

        const std::vector<number_expected_t> tests{ std::make_pair(1, 1), // NOLINTNEXTLINE
            std::make_pair(2, 6), std::make_pair(3, 4), // NOLINTNEXTLINE
            std::make_pair(4, 3), std::make_pair(5, 9), // NOLINTNEXTLINE
            std::make_pair(6, 2), std::make_pair(7, 8), // NOLINTNEXTLINE
            std::make_pair(8, 7), std::make_pair(9, 5), // NOLINTNEXTLINE
            std::make_pair(10, 10), // NOLINTNEXTLINE
            std::make_pair(prime_number * 123'456 + 7, 8) };

        std::vector<std::int32_t> prime_factors;

        for (const auto &[number, expected] : tests)
        {
            const auto text_num = std::to_string(number);
            {
                const auto actual =
                    Standard::Algorithms::Numbers::modulo_inverse_prime<std::int64_t>(number, prime_number);

                const auto name = "modulo_inverse_prime(" + text_num;
                ::Standard::Algorithms::ert::are_equal(expected, actual, name);
            }
            {
                const auto actual = Standard::Algorithms::Numbers::modulo_inverse_slow_but_checked<std::int64_t,
                    std::int32_t, prime_number>(number, prime_factors);

                const auto name = "modulo_inverse_slow_but_checked(" + text_num;
                ::Standard::Algorithms::ert::are_equal(expected, actual, name);
            }
            {
                const auto actual = Standard::Algorithms::Numbers::modulo_inverse<std::int64_t>(number, prime_number);

                const auto name = "modulo_inverse(" + text_num;
                ::Standard::Algorithms::ert::are_equal(expected, actual, name);
            }
        }
    }

    constexpr void euler_phi_totient_tests()
    {
        using number_expected_t = std::pair<std::int32_t, std::int32_t>;

        const std::vector<number_expected_t> tests{ std::make_pair(1, 1), std::make_pair(2, 1), // NOLINTNEXTLINE
            std::make_pair(3, 2), std::make_pair(4, 2), // NOLINTNEXTLINE
            std::make_pair(36, 12), { 12, 4 }, // NOLINTNEXTLINE
            std::make_pair(50, 20), std::make_pair(51, 32), // NOLINTNEXTLINE
            std::make_pair(52, 24), std::make_pair(53, 52), // NOLINTNEXTLINE
            std::make_pair(54, 18), std::make_pair(55, 40), // NOLINTNEXTLINE
            std::make_pair(56, 24) };

        const auto n_max = std::max_element(tests.cbegin(), tests.cend())->first;

        const auto euler_phi_totients = Standard::Algorithms::Numbers::euler_phi_totient_all<std::int64_t>(n_max);

        std::vector<std::int32_t> prime_factors;

        for (const auto &[number, expected] : tests)
        {
            const auto name = "euler_phi_totient(" + std::to_string(number);
            {
                const auto actual = Standard::Algorithms::Numbers::euler_phi_totient<std::int64_t>(number, prime_factors);

                ::Standard::Algorithms::ert::are_equal(expected, actual, name);
            }
            {
                const auto &actual = euler_phi_totients.at(number);

                ::Standard::Algorithms::ert::are_equal(expected, actual, name + " all");
            }
        }
    }

    [[nodiscard]] constexpr auto sa_int_sqrt_tests() -> bool
    {
        using num_sqrt = std::array<std::int32_t, 2>;

        constexpr std::array tests{
            // NOLINTNEXTLINE
            num_sqrt{ 0, 0 }, num_sqrt{ 1, 1 }, // NOLINTNEXTLINE
            num_sqrt{ 2, 1 }, num_sqrt{ 3, 1 }, // NOLINTNEXTLINE
            num_sqrt{ 4, 2 }, num_sqrt{ 5, 2 }, // NOLINTNEXTLINE
            num_sqrt{ 6, 2 }, num_sqrt{ 7, 2 }, // NOLINTNEXTLINE
            num_sqrt{ 8, 2 }, num_sqrt{ 9, 3 }, // NOLINTNEXTLINE
            num_sqrt{ 14, 3 }, num_sqrt{ 15, 3 }, // NOLINTNEXTLINE
            num_sqrt{ 16, 4 }, num_sqrt{ 17, 4 }, // NOLINTNEXTLINE
            num_sqrt{ 100, 10 }, num_sqrt{ 10'000, 100 }, // NOLINTNEXTLINE
        };

        for (const auto &[num, expected_root] : tests)
        {
            const auto actual = Standard::Algorithms::Numbers::int_sqrt(num);

            ::Standard::Algorithms::ert::are_equal(expected_root, actual, "int_sqrt");
        }

        return true;
    }

    void int_sqrt_tests()
    {
        constexpr auto lower = 0;
        constexpr auto upper = 1'000'000'000'000'000'000LL;

        Standard::Algorithms::Utilities::random_t<std::int64_t> rnd(lower, upper);

        const std::vector<std::int64_t> data{// NOLINTNEXTLINE
            999'999'824'000'007'648LL, static_cast<std::int64_t>(rnd())
        };

        for (const auto &datum : data)
        {
            const auto fast = Standard::Algorithms::Numbers::int_sqrt(datum);
            const auto slow = Standard::Algorithms::Numbers::int_sqrt_slow(datum);

            const auto oka = !(datum < 0) && !(fast < 0) && fast == slow && fast <= datum && fast * fast <= datum &&
                datum < (fast + 1LL) * (fast + 1LL);

            ::Standard::Algorithms::ert::are_equal(true, oka,
                "int_sqrt(" + std::to_string(datum) + "), fast " + std::to_string(fast) + ", slow " +
                    std::to_string(slow));
        }
    }

    constexpr void mobius_tests()
    {
        constexpr auto size = 20;

        const auto mob = Standard::Algorithms::Numbers::mobius(size);

        const std::vector<std::int8_t> expected{
            0, 1, -1, -1, 0, // 4
            -1, 1, -1, 0, 0, 1, // 10
            -1, 0, -1, 1, 1, 0, -1, 0, -1, 0, // 20
        };

        ::Standard::Algorithms::ert::are_equal(expected, mob, "Mobius");
    }

    void all_divisors_test()
    {
        std::vector<std::int32_t> factors;
        std::vector<std::int32_t> factors_2;
        std::unordered_set<std::int32_t> set_2;

        constexpr auto max_size = 64;

        for (auto size = 1; size <= max_size; ++size)
        {
            Standard::Algorithms::Numbers::all_divisors(size, factors);
            std::sort(factors.begin(), factors.end());

            Standard::Algorithms::Numbers::all_divisors_slow(size, set_2);
            factors_2.clear();
            factors_2.insert(factors_2.begin(), set_2.begin(), set_2.end());
            std::sort(factors_2.begin(), factors_2.end());

            ::Standard::Algorithms::ert::are_equal(factors, factors_2, "all_divisors(" + std::to_string(size));
        }
    }
} // namespace

void Standard::Algorithms::Numbers::Tests::number_utilities_tests()
{
    static_assert(sa_int_sqrt_tests());
    int_sqrt_tests();

    sum_test();
    modulo_inverse_prime_tests();
    euler_phi_totient_tests();
    mobius_tests();
    all_divisors_test();
}
