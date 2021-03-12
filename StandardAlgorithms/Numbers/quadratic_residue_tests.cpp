#include"quadratic_residue_tests.h"
#include"../Utilities/test_utilities.h"
#include"quadratic_residue.h"
#include<array>
#include<unordered_map>

namespace
{
    using int_t = std::int32_t;
    using long_int_t = std::int64_t;

    constexpr int_t zero{};
    constexpr int_t one = 1;
    constexpr int_t two = 2;

    [[nodiscard]] auto precalc_roots(const int_t odd_prime) -> std::unordered_map<int_t, int_t>
    {
        {
            const auto test = zero < odd_prime && zero != (odd_prime % two);

            ::Standard::Algorithms::ert::are_equal(true, test, std::to_string(odd_prime));
        }

        std::unordered_map<int_t, int_t> roots;

        for (int_t index = 1; index < odd_prime; ++index)
        {
            const auto prod = static_cast<long_int_t>(index) * index;
            const auto remainder = static_cast<int_t>(prod % odd_prime);

            if (auto &value = roots[remainder]; value == zero)
            {
                value = index;
            }
        }

        return roots;
    }

    void euler_criterion_tests(const int_t odd_prime)
    {
        const auto roots = precalc_roots(odd_prime);
        const auto suffix = ", odd prime " + std::to_string(odd_prime);

        int_t yes{};
        int_t nos{};

        for (int_t index = 1; index < odd_prime; ++index)
        {
            const auto name = std::to_string(index) + suffix;
            const auto ite = roots.find(index);
            const auto expected = ite != roots.end();
            ++(expected ? yes : nos);
            {
                const auto actual = Standard::Algorithms::Numbers::euler_criterion<long_int_t>(index, odd_prime);

                ::Standard::Algorithms::ert::are_equal(expected, actual, "Euler's criterion " + name);
            }

            if (const auto root =
                    Standard::Algorithms::Numbers::find_quadratic_residue_modulo<long_int_t>(index, odd_prime);
                expected)
            {
                const auto root2 = static_cast<long_int_t>(odd_prime - root);
                Standard::Algorithms::require_between(one, root, odd_prime - one, "root");
                Standard::Algorithms::require_between(one, root2, odd_prime - one, "root2");

                const auto min_root = std::min(root, root2);

                ::Standard::Algorithms::ert::are_equal(ite->second, min_root, "find_quadratic_residue_modulo " + name);
            }
            else
            {
                ::Standard::Algorithms::ert::are_equal(-one, root, "find_quadratic_residue_modulo " + name);
            }

            {
                const auto legendary_nam = "legendre_symbol " + name;
                const auto legendre = Standard::Algorithms::Numbers::legendre_symbol<long_int_t>(index, odd_prime);
                {
                    const auto has_leg = legendre == one || legendre == -one;
                    ::Standard::Algorithms::ert::are_equal(true, has_leg, legendary_nam);
                }

                ::Standard::Algorithms::ert::are_equal(expected, zero < legendre, legendary_nam);
            }
            {
                const auto na_ja = "jacobi_symbol " + name;
                const auto jacobi = Standard::Algorithms::Numbers::jacobi_symbol<long_int_t>(index, odd_prime);
                {
                    const auto has_jac = jacobi == one || jacobi == -one;
                    ::Standard::Algorithms::ert::are_equal(true, has_jac, na_ja);
                }

                ::Standard::Algorithms::ert::are_equal(expected, zero < jacobi, na_ja);
            }
        }

        const auto half = odd_prime / two;

        ::Standard::Algorithms::ert::are_equal(half, yes, "euler_criterion yes must be half " + suffix);

        ::Standard::Algorithms::ert::are_equal(half, nos, "euler_criterion nos must be half " + suffix);
    }
} // namespace

void Standard::Algorithms::Numbers::Tests::quadratic_residue_tests()
{
    const std::array odd_primes{// NOLINTNEXTLINE
        3, 5, 7, 11, 13, 17, 19
    };

    for (const auto &prime : odd_primes)
    {
        euler_criterion_tests(prime);
    }
}
