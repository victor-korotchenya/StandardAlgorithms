#include"primitive_root_tests.h"
#include"../Utilities/test_utilities.h"
#include"prime_number_utility.h"
#include"primitive_root.h"

namespace // primes
{
    using int_t = std::uint32_t;
    using large_int_t = std::uint64_t;

    using small_output_t = std::vector<int_t>;
    using small_test_case_t = std::pair<int_t, small_output_t>;

    [[nodiscard]] constexpr auto build_small_test_cases() -> std::vector<small_test_case_t>
    {
        return {// NOLINTNEXTLINE
            small_test_case_t{ 2, small_output_t{ 1 } }, // NOLINTNEXTLINE
            small_test_case_t{ 3, small_output_t{ 2 } }, // NOLINTNEXTLINE
            small_test_case_t{ 5, small_output_t{ 2, 3 } }, // NOLINTNEXTLINE
            small_test_case_t{ 7, small_output_t{ 3, 5 } }, // NOLINTNEXTLINE
            small_test_case_t{ 11, small_output_t{ 2, 6, 7, 8 } }, // NOLINTNEXTLINE
            small_test_case_t{ 13, small_output_t{ 2, 6, 7, 11 } }, // NOLINTNEXTLINE
            small_test_case_t{ 17, small_output_t{ 3, 5, 6, 7, 10, 11, 12, 14 } }, // NOLINTNEXTLINE
            small_test_case_t{ 19, small_output_t{ 2, 3, 10, 13, 14, 15 } }, // NOLINTNEXTLINE
            small_test_case_t{ 23, small_output_t{ 5, 7, 10, 11, 14, 15, 17, 19, 20, 21 } }, // NOLINTNEXTLINE
            small_test_case_t{ 29, small_output_t{ 2, 3, 8, 10, 11, 14, 15, 18, 19, 21, 26, 27 } }
        };
    }

    constexpr void check_is_generator(
        const int_t &base1, const std::string &name, const int_t &root1, const bool is_root_expected = false)
    {
        assert(!name.empty());

        const auto actual = Standard::Algorithms::Numbers::is_generator_slow<large_int_t>(base1, root1);

        ::Standard::Algorithms::ert::are_equal(
            is_root_expected, actual, name + " is_generator_slow " + ::Standard::Algorithms::Utilities::zu_string(root1));
    }

    [[nodiscard]] constexpr auto primes_tests() -> bool
    {
        const auto test_cases = build_small_test_cases();
        Standard::Algorithms::throw_if_empty("build_small_test_cases", test_cases);

        for (const auto &[prime, roots] : test_cases)
        {
            const auto name = "small test " + ::Standard::Algorithms::Utilities::zu_string(prime);
            Standard::Algorithms::throw_if_empty(name + " roots", roots);
            {
                const auto good = Standard::Algorithms::Numbers::is_prime_simple(prime);
                ::Standard::Algorithms::ert::are_equal(true, good, name + " is prime");
            }
            {
                const auto smallest_root =
                    Standard::Algorithms::Numbers::primitive_root_generator_slow<large_int_t>(prime);
                const auto &expected_root = roots[0];

                ::Standard::Algorithms::ert::are_equal(
                    expected_root, smallest_root, name + " primitive_root_generator_slow");
            }

            for (int_t index{}; index < prime; ++index)
            {
                const auto iter = std::find(roots.cbegin(), roots.cend(), index);
                const auto is_root_expected = iter != roots.cend();
                check_is_generator(prime, name, index, is_root_expected);
            }
        }

        return true;
    }
} // namespace

namespace // composites
{
    [[nodiscard]] constexpr auto build_composites() -> std::vector<int_t>
    {
        return {// NOLINTNEXTLINE
            4, 6, 8, 9, 10, 12, 14, 15, 16, 18, // NOLINTNEXTLINE
            20, 21, 21, 24, 25, 26, 27, 28, 30
        };
    }

    [[nodiscard]] constexpr auto composites_tests() -> bool
    {
        const auto test_cases = build_composites();
        Standard::Algorithms::throw_if_empty("build_composites", test_cases);

        for (const auto &composite : test_cases)
        {
            const auto name = "composite test " + ::Standard::Algorithms::Utilities::zu_string(composite);
            {
                const auto good = Standard::Algorithms::Numbers::is_prime_simple(composite);
                ::Standard::Algorithms::ert::are_equal(false, good, name + " is prime");
            }

            for (int_t index{}; index < composite; ++index)
            {
                check_is_generator(composite, name, index);
            }
        }

        return true;
    }
} // namespace

void Standard::Algorithms::Numbers::Tests::primitive_root_tests()
{
    static_assert(!Standard::Algorithms::Numbers::is_generator_slow<large_int_t>(0, 0));
    static_assert(Standard::Algorithms::Numbers::is_generator_slow<large_int_t>(1, 0), "A special 0 root case");

    static_assert(primes_tests());
    static_assert(composites_tests());
}
