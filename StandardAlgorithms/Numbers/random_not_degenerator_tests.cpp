#include"random_not_degenerator_tests.h"
#include"../Utilities/test_utilities.h"
#include"prime_number_utility.h"
#include"primitive_root.h"
#include"random_not_degenerator.h"
#include<array>

namespace // small_prime_tests
{
    using int_t = std::uint32_t;
    using large_int_t = std::uint64_t;

    constexpr int_t prime_sam = 29;
    constexpr int_t generator1 = 2;

    static_assert(int_t{ 1 } < generator1 && generator1 < prime_sam);

    [[nodiscard]] constexpr auto small_prime_tests() -> bool
    {
        {
            constexpr auto smallest_root =
                Standard::Algorithms::Numbers::primitive_root_generator_slow<large_int_t>(prime_sam);

            static_assert(generator1 == smallest_root);
        }
        {
            constexpr auto good = Standard::Algorithms::Numbers::is_prime_simple(prime_sam);

            ::Standard::Algorithms::ert::are_equal(true, good, "small is prime");
        }

        Standard::Algorithms::Numbers::random_not_degenerator<prime_sam, generator1> degenerat{};
        std::vector<int_t> randoms;
        randoms.reserve(prime_sam);

        std::vector<bool> useds(prime_sam, false);
        useds[0] = true;

        for (std::size_t index{ 1 }; index < prime_sam; ++index)
        {
            const auto pseudo_random_value = degenerat();
            {
                const auto *const name = "pseudo-random value";

                Standard::Algorithms::require_positive(pseudo_random_value, name);
                Standard::Algorithms::require_greater(prime_sam, pseudo_random_value, name);
            }

            useds[pseudo_random_value] = true;
            randoms.push_back(pseudo_random_value);
        }

        if (const auto iter = std::find(useds.cbegin(), useds.cend(), false); iter != useds.cend()) [[unlikely]]
        {
            const auto bad_index = static_cast<std::size_t>(iter - useds.cbegin());

            auto err = "The index " + ::Standard::Algorithms::Utilities::zu_string(bad_index) +
                " must have been enumerated by the random_not_degenerator.";

            throw std::runtime_error(err);
        }

        const std::vector<int_t> expected_randoms{// NOLINTNEXTLINE
            14, 28, 27, 25, 21, 13, 26, 23, 17, 5, 10, 20, 11, 22, 15, // NOLINTNEXTLINE
            1, 2, 4, 8, 16, 3, 6, 12, 24, 19, 9, 18, 7
        };

        ::Standard::Algorithms::ert::are_equal(expected_randoms, randoms, "Produced pseudo-random numbers.");

        for (std::size_t index{ 1 }; index < prime_sam; ++index)
        {
            const auto pseudo_random_value = degenerat();
            const auto ind = index - 1U;
            const auto &expected_value = expected_randoms.at(ind);

            ::Standard::Algorithms::ert::are_equal(
                expected_value, pseudo_random_value, "Round two at " + ::Standard::Algorithms::Utilities::zu_string(ind));
        }

        return true;
    }
} // namespace

namespace // First not randoms
{
    constexpr auto top_size = 10U;

    [[nodiscard]] constexpr auto first_not_randoms() noexcept
    {
        constexpr std::array<int_t, top_size> randoms{// NOLINTNEXTLINE
            2147483645, 6, 4294967219, 864, 4294956923, // NOLINTNEXTLINE
            124416, 4293474299, 17915904, 4079976443, 2579890176
        };

        return randoms;
    }

    [[nodiscard]] constexpr auto first_test_base(const std::string &name, const auto &expected_randoms, auto gaulle)
        -> bool
    {
        assert(!name.empty());

        std::array<int_t, top_size> randoms{};
        Standard::Algorithms::Numbers::random_not_degenerator<> degenerat{};

        for (auto &rando : randoms)
        {
            rando = gaulle(degenerat);
        }

        ::Standard::Algorithms::ert::are_equal(expected_randoms, randoms, name);
        return true;
    }

    [[nodiscard]] constexpr auto first_not_randoms_tests() -> bool
    {
        return first_test_base("first_not_randoms_tests", first_not_randoms(),
            [](auto &gold_digger)
            {
                return gold_digger();
            });
    }

    [[nodiscard]] constexpr auto another_base_not_randoms_tests() -> bool
    {
        constexpr int_t another_base_prime = 1999;
        static_assert(Standard::Algorithms::Numbers::is_prime_simple(another_base_prime));

        auto expected_randoms = first_not_randoms();

        for (auto &dom : expected_randoms)
        {
            dom %= another_base_prime;
        }

        {
            constexpr int_t expected_front = 1923;
            ::Standard::Algorithms::ert::are_equal(expected_front, expected_randoms.front(), "expected front");
        }
        {
            constexpr int_t expected_last = 766;
            ::Standard::Algorithms::ert::are_equal(expected_last, expected_randoms.back(), "expected back");
        }

        return first_test_base("another_base_not_randoms_tests", expected_randoms,
            [](auto &partial_degenerat)
            {
                return partial_degenerat(another_base_prime);
            });
    }
} // namespace

void Standard::Algorithms::Numbers::Tests::random_not_degenerator_tests()
{
    static_assert(small_prime_tests());
    static_assert(first_not_randoms_tests());
    static_assert(another_base_not_randoms_tests());
}
