#include"prime_number_utility_tests.h"
#include"../Utilities/definitiv_operator_feed.h"
#include"../Utilities/random.h"
#include"../Utilities/test_utilities.h"
#include"eratosthenes_sieve_cache.h"
#include"factoring_utilities.h"
#include"prime_number_utility.h"
#include<array>
#include<cstdlib> // std::exit
#include<iostream>

namespace
{
    using int_t = std::int64_t;

    constexpr auto n_min = 1;
    constexpr auto n_max = 10'007;
    constexpr auto prime_count = 1'230ZU;

    static_assert(Standard::Algorithms::Numbers::is_prime_simple(n_max));

    constexpr int_t wonderful_root67 = 67;
    constexpr int_t first_carmichael_number = 561;
    static_assert(1 == wonderful_root67 * wonderful_root67 % first_carmichael_number);

    [[nodiscard]] constexpr auto into_odd_and_shifts_tests() -> bool
    {
        using num_odds_shits = std::array<int_t, 3>;

        constexpr std::array tests{
            // NOLINTNEXTLINE
            num_odds_shits{ 56, 7, 3 }, // NOLINTNEXTLINE
            num_odds_shits{ 28, 7, 2 }, // NOLINTNEXTLINE
            num_odds_shits{ 14, 7, 1 }, // NOLINTNEXTLINE
            num_odds_shits{ 7, 7, 0 }, // NOLINTNEXTLINE
            num_odds_shits{ 2, 1, 1 }, // NOLINTNEXTLINE
            num_odds_shits{ 1, 1, 0 } // NOLINTNEXTLINE
        };

        for (const auto &[num, odds, shits] : tests)
        {
            const auto actual = Standard::Algorithms::Numbers::into_odd_and_shifts<int_t>(num);

            const std::pair<int_t, int_t> expected{ odds, shits };

            ::Standard::Algorithms::ert::are_equal(expected, actual, "into_odd_and_shifts");
        }

        return true;
    }

    [[nodiscard]] constexpr auto carmichael_number_tests() -> bool
    {
        constexpr int_t prim3 = 3;
        constexpr int_t prim11 = 11;
        constexpr int_t prim17 = 17;

        static_assert(first_carmichael_number == prim3 * prim11 * prim17);

        static_assert(!Standard::Algorithms::Numbers::is_pseudo_prime(first_carmichael_number, prim3) &&
                Standard::Algorithms::Numbers::is_pseudo_prime(first_carmichael_number, wonderful_root67),
            "Euler was right.");

        static_assert(
            !Standard::Algorithms::Numbers::naive_pseudo_prime(first_carmichael_number), "Euler was right again.");

        static_assert(
            !Standard::Algorithms::Numbers::is_prime_simple(first_carmichael_number), "Euler was right again and over.");

        return true;
    }

    constexpr void run_miller_rabin_test(const int_t &prime_cand,
        Standard::Algorithms::Utilities::definitiv_operator_feed<int_t> &feeder, const bool expected = true)
    {
        const auto max_attempts = std::max<int_t>(static_cast<int_t>(feeder.size()), 1);

        feeder.reset();

        const auto actual = Standard::Algorithms::Numbers::miller_rabin_test<int_t>(prime_cand, feeder, max_attempts);

        ::Standard::Algorithms::ert::are_equal(expected, actual, "miller_rabin_test");
    }

    constexpr void run_miller_rabin_test(
        const int_t &prime_cand, const bool expected = true, std::vector<int_t> &&randoms = {})
    {
        Standard::Algorithms::Utilities::definitiv_operator_feed<int_t> feeder(std::move(randoms));

        run_miller_rabin_test(prime_cand, feeder, expected);
    }

    [[maybe_unused]] void print_pseudos_and_exit(const int_t &prime_cand)
    {
        std::cout << "\nPrint pseudos(" << prime_cand << "):\n";

        for (int_t pseudo{ 2 }; pseudo + 1 < prime_cand; ++pseudo)
        {
            try
            {
                run_miller_rabin_test(prime_cand, true, std::vector<int_t>{ pseudo });
            }
            catch (...)
            {
                continue;
            }

            std::cout << pseudo << ", ";
        }

        // NOLINTNEXTLINE
        std::exit(0);
    }

    [[nodiscard]] constexpr auto mrt_tests() -> bool
    {
        run_miller_rabin_test(2);
        run_miller_rabin_test(3);

        {
            constexpr std::array even_composites{// NOLINTNEXTLINE
                4, 6, 8, 10, 12, 14, 16, 32
            };

            for (const auto &num : even_composites)
            {
                run_miller_rabin_test(num, false);
            }
        }

        run_miller_rabin_test( // NOLINTNEXTLINE
            5, true, std::vector<int_t>{ 2 });

        run_miller_rabin_test( // NOLINTNEXTLINE
            5, true, std::vector<int_t>{ 2, 3 });

        run_miller_rabin_test( // NOLINTNEXTLINE
            34 * 11, false, std::vector<int_t>{ 2 });

        run_miller_rabin_test( // NOLINTNEXTLINE
            first_carmichael_number, false, std::vector<int_t>{ 2 });
        run_miller_rabin_test( // NOLINTNEXTLINE
            first_carmichael_number, false, std::vector<int_t>{ 3 });
        run_miller_rabin_test(first_carmichael_number, false, std::vector<int_t>{ wonderful_root67 });

        // Pseudo-prime, okay!
        // 103 -> 511 -> 256 -> 460 -> 103
        // 101 -> 103
        // 305 -> 460
        // 458 -> 511 == 7*73
        run_miller_rabin_test(first_carmichael_number, true,
            std::vector<int_t>{// NOLINTNEXTLINE
                50, 101, 103, 256, 305, 458, 460, 511 });


        // print_pseudos_and_exit(561);

        return true;
    }

    constexpr void run_polland_rho_test(const int_t &composite)
    {
        constexpr int_t one{ 1 };
        Standard::Algorithms::require_greater(composite, one, "composite");

        const auto [divisor, steps] = Standard::Algorithms::Numbers::run_polland_rho<int_t>(composite);

        {
            const auto *const name = "run_polland_rho steps";
            Standard::Algorithms::require_positive(steps, name);

            const auto allowed_steps_max =
                Standard::Algorithms::Numbers::int_sqrt(static_cast<std::uint64_t>(composite)) + 10LLU;

            Standard::Algorithms::require_less_equal(steps, allowed_steps_max, name);
        }
        {
            const auto *const name = "run_polland_rho divisor";
            Standard::Algorithms::require_greater(divisor, one, name);
            Standard::Algorithms::require_greater(composite, divisor, name);
        }

        constexpr int_t zero{};

        const auto quot = static_cast<int_t>(composite / divisor);

        if (const auto remainder = static_cast<int_t>(composite % divisor); zero != remainder) [[unlikely]]
        {
            auto err = "Composite " + std::to_string(composite) + " is not divisible by a factor " +
                std::to_string(divisor) + ", run_polland_rho got a remainder: " + std::to_string(remainder) + ".";

            throw std::runtime_error(err);
        }

        const auto prod = static_cast<int_t>(quot * divisor);
        if (prod != composite) [[unlikely]]
        {
            auto err = "Composite " + std::to_string(composite) + " * factor // factor, where factor is " +
                std::to_string(divisor) + ", run_polland_rho is " + std::to_string(prod) + ".";

            throw std::runtime_error(err);
        }
    }

    [[nodiscard]] constexpr auto polland_rho_tests() -> bool
    {
        {
            constexpr int_t prim11 = 11;
            constexpr int_t prim19 = 19;
            constexpr int_t composit = prim11 * prim19;

            run_polland_rho_test(composit);
        }
        {
            constexpr int_t prim5 = 5;

            {
                constexpr int_t compost = prim5 * prim5;
                run_polland_rho_test(compost);
            }
            {
                constexpr int_t compost = prim5 * prim5 * prim5;

                run_polland_rho_test(compost);
            }
            {
                constexpr int_t compost = prim5 * prim5 * prim5 * prim5 * prim5;

                run_polland_rho_test(compost);
            }
        }

        constexpr int_t prim997 = 997;
        constexpr int_t prima_10k = 10'007;
        {
            constexpr int_t composit = prim997 * prim997;

            run_polland_rho_test(composit);
        }
        {
            constexpr int_t composit = prim997 * prima_10k;

            run_polland_rho_test(composit);
        }
        {
            constexpr int_t composit = prima_10k * prima_10k;

            run_polland_rho_test(composit);
        }
        {
            constexpr int_t prim41 = 41;
            constexpr int_t composit = prim41 * prim997 * prima_10k;

            run_polland_rho_test(composit);
        }

        return true;
    }

    [[nodiscard]] constexpr auto check_primes_ptr(const auto *const ptr) -> const std::vector<std::int32_t> &
    {
        Standard::Algorithms::throw_if_null("eratosthenes_sieve_cache ptr", ptr);

        const auto &prime_numbers = ptr->first;

        Standard::Algorithms::require_less_equal(
            prime_count, prime_numbers.size(), "Might be re-used by other threads. eratosthenes_sieve_cache size");

        return prime_numbers;
    }

    constexpr void fill_randoms(const int_t &prime_candidate, auto &randoms)
    {
        while (randoms.back() + 2LL < prime_candidate)
        {
            randoms.push_back(randoms.back() + 1);
        }
    }

    void more_mr_tests()
    {
        const auto s_ptr = Standard::Algorithms::Numbers::eratosthenes_sieve_cache(n_max);
        const auto &prime_numbers = check_primes_ptr(s_ptr.get());

        constexpr std::int32_t max_mrt_number =
            // n_max; // n_max*n_max is too slow, but it has been checked anyway.
            23;

        static_assert(2 < max_mrt_number && max_mrt_number <= n_max);

        std::vector<int_t> randoms;
        // Ensure no reference is invalidated.
        randoms.reserve(prime_numbers.back());
        randoms.push_back(2);

        Standard::Algorithms::Utilities::definitiv_operator_feed<int_t> feeder(std::as_const(randoms));

        for (int_t last{ 1 }; const auto &prime : prime_numbers)
        {
            if (max_mrt_number < prime)
            {
                break;
            }

            while (++last < prime)
            {
                fill_randoms(last, randoms);

                {
                    constexpr auto is_prim = false;
                    run_miller_rabin_test(last, feeder, is_prim);
                }

                run_polland_rho_test(last);
            }

            assert(last == prime);
            fill_randoms(prime, randoms);

            ::Standard::Algorithms::ert::are_equal(
                randoms.size(), feeder.size(), "should have used a ctor with a reference, feeder size");

            run_miller_rabin_test(prime, feeder);
        }
    }

    void factorial_prime_max_power_tests()
    {
        const auto s_ptr = Standard::Algorithms::Numbers::eratosthenes_sieve_cache(n_max);
        const auto &prime_numbers = check_primes_ptr(s_ptr.get());

        const auto size_minus_one = static_cast<std::int32_t>(prime_numbers.size()) - 1;

        Standard::Algorithms::require_positive(size_minus_one, "size - 1");

        Standard::Algorithms::Utilities::random_t<std::int32_t> rnd(n_min, n_max);

        constexpr auto max_att = 2;

        for (std::int32_t att{}; att < max_att; ++att)
        {
            const auto is_zero = att == 0;

            std::int32_t nnn{};
            std::int32_t prime{};
            std::int32_t expected{};

            if (is_zero)
            {
                // NOLINTNEXTLINE
                nnn = 1'000, prime = 5, expected = 249;
            }
            else
            {
                nnn = rnd();

                const auto index = rnd(0, size_minus_one);
                prime = prime_numbers.at(index);
            }

            Standard::Algorithms::require_less_equal(Standard::Algorithms::Numbers::min_prime, prime, "prime");

            const auto name = "factorial_prime_max_power(" + std::to_string(nnn) + "," + std::to_string(prime) + ")";

            const auto fast = Standard::Algorithms::Numbers::factorial_prime_max_power(nnn, prime);

            if (is_zero)
            {
                ::Standard::Algorithms::ert::are_equal(expected, fast, name);
            }

            const auto slow = Standard::Algorithms::Numbers::factorial_prime_max_power_slow<int_t>(nnn, prime);

            ::Standard::Algorithms::ert::are_equal(fast, slow, name + " slow");
        }
    }
} // namespace

void Standard::Algorithms::Numbers::Tests::prime_number_utility_tests()
{
    static_assert(into_odd_and_shifts_tests());
    static_assert(carmichael_number_tests());
    static_assert(mrt_tests());
    static_assert(polland_rho_tests());

    more_mr_tests();
    factorial_prime_max_power_tests();
}
