#include"hash_utilities_tests.h"
#include"../Utilities/test_utilities.h"
#include"hash_utilities.h"
#include<initializer_list>

namespace
{
    using int_t = std::uint64_t;
    using floating_t = Standard::Algorithms::floating_point_type;

    constexpr floating_t coefficient = 0.371945695781478359467985779459737138246939;

    // todo(p3): make consteval when Clang becomes C++23 compliant (std::modf).
    [[nodiscard]] constexpr auto mult_hash_tests_impl(
        const std::string &prefix, const int_t &size, const auto &expected_beauties) -> bool
    {
        assert(!prefix.empty());

        Standard::Algorithms::require_positive(size, prefix + " hash size");
        Standard::Algorithms::require_positive(expected_beauties.size(), prefix + " expected beauties size");

        const auto prename = prefix + " multiplication_method_hash ";

        int_t pre_hash{};

        for (const auto &expected : expected_beauties)
        {
            const auto name = prename + std::to_string(pre_hash);
            Standard::Algorithms::require_greater(size, expected, name + " expected");

            {
                const auto masha = Standard::Algorithms::Numbers::multiplication_method_hash<int_t, floating_t>(
                    pre_hash, coefficient, size);

                ::Standard::Algorithms::ert::are_equal(expected, masha, name);
            }

            ++pre_hash;
        }

        return true;
    }

    // todo(p3): make consteval when Clang becomes C++23 compliant (std::modf).
    [[nodiscard]] constexpr auto multiplication_method_hash_tests() -> bool
    {
        {
            constexpr int_t pow2_size = 1U << 3U;

            // Each row look pretty random - only 1 collision.
            // The last row even has no collisions!
            // However, there are many parasitic repetitions between some consecutive rows.
            const std::initializer_list<int_t> pow2_beauties{
                // NOLINTNEXTLINE
                0, 2, 5, 0, 3, 6, 1, 4, // NOLINTNEXTLINE
                7, 2, 5, 0, 3, 6, 1, 4, // NOLINTNEXTLINE
                7, 2, 5, 0, 3, 6, 1, 4, // NOLINTNEXTLINE
                7, 2, 5, 0, 3, 6, 1, 4, // NOLINTNEXTLINE
                7, 2, 5, 0, 3, 6, 1, 4, // NOLINTNEXTLINE
                7, 1, 4, 7, 2, 5, 0, 3, // NOLINTNEXTLINE
                6, 1, 4, 7, 2, 5, 0, 3 // NOLINTNEXTLINE
            };

            if (!mult_hash_tests_impl("power two", pow2_size, pow2_beauties))
            {
                return false;
            }
        }
        {
            constexpr int_t prime_size = 17U;

            const std::initializer_list<int_t> prime_beauties{
                // NOLINTNEXTLINE
                0, 6, 12, 1, 8, 14, 3, 10, // NOLINTNEXTLINE
                16, 5, 12, 1, 7, 14, 3, 9, // NOLINTNEXTLINE
                16, 5, 11, 1, 7, 13, 3, 9, // NOLINTNEXTLINE
                15, 5, 11, 0, 7, 13, 2, 9, // NOLINTNEXTLINE
                15, 4, 10, 0, 6, 12, 2, 8, // NOLINTNEXTLINE
                14, 4, 10, 16, 6, 12, 1, 8, // NOLINTNEXTLINE
                14, 3, 10, 16, 5, 12, 1, 7 // NOLINTNEXTLINE
            };

            if (!mult_hash_tests_impl("prime", prime_size, prime_beauties))
            {
                return false;
            }
        }

        return true;
    }
} // namespace

void Standard::Algorithms::Numbers::Tests::hash_utilities_tests()
{
    // todo(p3):  static_assert(multiplication_method_hash_tests());

    ::Standard::Algorithms::ert::are_equal(true, multiplication_method_hash_tests(), "multiplication_method_hash_tests");
}
