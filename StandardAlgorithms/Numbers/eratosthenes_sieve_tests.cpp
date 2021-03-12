#include"eratosthenes_sieve_tests.h"
#include"../Utilities/random.h"
#include"../Utilities/test_utilities.h"
#include"eratosthenes_sieve.h"
#include"factoring_utilities.h"
#include"prime_number_utility.h"

namespace
{
    // NOLINTNEXTLINE
    constexpr auto max_size = 2 * 3 * 5 * 7 * 11 + 4; // 2310 + 4

    static_assert(Standard::Algorithms::Numbers::min_prime < max_size);

    template<std::integral long_int_t, std::integral int_t, class pair_t>
    requires(Standard::Algorithms::same_sign_leq_size<int_t, long_int_t>)
    constexpr void test_factoring(std::vector<pair_t> &factor_powers, const std::vector<int_t> &min_prime_factors,
        std::vector<int_t> &temp, const int_t &num, std::vector<pair_t> &factor_powers2, std::vector<int_t> &temp2)
    {
        factor_powers.clear();

        Standard::Algorithms::Numbers::decompose_eratosthenes_sieve_factoring(min_prime_factors, factor_powers, num);

        Standard::Algorithms::require_positive(factor_powers.size(), "factor powers size");

        factor_powers2.clear();

        Standard::Algorithms::Numbers::decompose_into_divisors_with_powers<long_int_t, int_t, pair_t>(
            num, factor_powers2);

        ::Standard::Algorithms::ert::are_equal(
            factor_powers, factor_powers2, "decompose_eratosthenes_sieve_factoring " + std::to_string(num));

        temp.resize(factor_powers2.size());

        std::transform(
            factor_powers.cbegin(), factor_powers.cend(), temp.begin(),
            [] [[nodiscard]] (const pair_t &par) noexcept -> const auto & { return par.first; });

        Standard::Algorithms::Numbers::decompose_into_prime_divisors<long_int_t, int_t>(num, temp2);

        ::Standard::Algorithms::ert::are_equal(temp, temp2, "decompose_into_prime_divisors " + std::to_string(num));
    }

    constexpr void test_decompose_into_divisors_with_powers()
    {
        using int_t = std::int64_t;
        using pair_t = std::pair<int_t, std::int32_t>;

        constexpr auto tumba = static_cast<int_t>(4) *
            // NOLINTNEXTLINE
            625 * 25 * 243 * 243 * 243;

        const std::vector<pair_t> expected{ { 2, 2 },
            // NOLINTNEXTLINE
            { 3, 15 },
            // NOLINTNEXTLINE
            { 5, 6 } };

        std::vector<pair_t> components;
        Standard::Algorithms::Numbers::decompose_into_divisors_with_powers<int_t, int_t>(tumba, components);

        const auto *const func_name = static_cast<const char *>(__FUNCTION__);
        ::Standard::Algorithms::ert::are_equal(expected, components, func_name);

        components.clear();

        Standard::Algorithms::Numbers::decompose_into_divisors_with_powers_slow<int_t>(tumba, components);

        ::Standard::Algorithms::ert::are_equal(expected, components, func_name + std::string("_slow"));
    }
} // namespace

void Standard::Algorithms::Numbers::Tests::eratosthenes_sieve_tests()
{
    using int_t = std::int64_t;
    using long_int_t = std::int64_t;
    using pair_t = std::pair<int_t, int_t>;

    Standard::Algorithms::Utilities::random_t<std::int32_t> rnd(Standard::Algorithms::Numbers::min_prime, max_size);

    const auto size = rnd();
    const auto prime_factors = Standard::Algorithms::Numbers::eratosthenes_sieve<long_int_t, int_t>(size);

    const auto &primes = prime_factors.first;

    const auto flags_slow = Standard::Algorithms::Numbers::eratosthenes_sieve_slow<long_int_t, int_t>(size);

    const auto prefix = "eratosthenes_sieve " + std::to_string(size) + " ";

    std::vector<pair_t> factor_powers;
    std::vector<pair_t> factor_powers2;
    std::vector<int_t> temp;
    std::vector<int_t> temp2;

    constexpr auto max_attempts = 1;

    for (std::int32_t att{}; att < max_attempts; ++att)
    {
        const auto num = rnd(Standard::Algorithms::Numbers::min_prime, size);

        const auto expected = is_prime_simple(num);
        const auto name = prefix + std::to_string(num);
        {
            const auto actual = prime_factors.second.at(num) == num;
            ::Standard::Algorithms::ert::are_equal(expected, actual, name);
        }
        {
            const auto iter = std::lower_bound(primes.cbegin(), primes.cend(), num);
            const auto actual_2 = iter != primes.cend() && *iter == num;

            ::Standard::Algorithms::ert::are_equal(expected, actual_2, "lower " + name);
        }

        test_factoring<long_int_t, int_t>(factor_powers, prime_factors.second, temp, num, factor_powers2, temp2);

        const auto slow = flags_slow.at(num);

        ::Standard::Algorithms::ert::are_equal(expected, slow, "slow " + name);
    }
}

void Standard::Algorithms::Numbers::Tests::prime_number_count_tests()
{
    test_decompose_into_divisors_with_powers();

    using int_t = std::int64_t;
    using long_int_t = int_t;

    Standard::Algorithms::Utilities::random_t<std::int32_t> rnd(Standard::Algorithms::Numbers::min_prime, max_size);
    std::unordered_map<int_t, int_t> cache;

    constexpr auto max_attempts = 2;

    for (std::int32_t att{}; att < max_attempts; ++att)
    {
        const auto num = rnd();
        const auto name = std::to_string(num);

        const auto simple = Standard::Algorithms::Numbers::prime_number_count_simple<long_int_t, int_t>(num);

        const auto &expected = simple; // todo(p3): faster.
        // ::Standard::Algorithms::ert::are_equal(expected, simple, "prime_number_count_simple " + name);

        const auto slow = Standard::Algorithms::Numbers::prime_number_count_slow<long_int_t, int_t>(cache, num);

        ::Standard::Algorithms::ert::are_equal(expected, slow, "prime_number_count_slow " + name);
    }
}
