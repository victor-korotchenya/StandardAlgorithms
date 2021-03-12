#include"even_begin_odd_end_permute_tests.h"
#include"../Utilities/test_utilities.h"
#include"even_begin_odd_end_permute.h"
#include<array>

namespace // reverse_bits_tests
{
    constexpr void reverse_bits_test(
        const std::uint64_t value, const std::uint32_t bit_count, const std::uint64_t expected_reversed_value)
    {
        // NOLINTNEXTLINE
        constexpr std::uint64_t max_bits = 64;
        Standard::Algorithms::require_less_equal(bit_count, max_bits, "bit_count");

        {
            constexpr std::uint64_t one = 1;

            const auto max_value = -one + (bit_count < max_bits ? (one << bit_count) : 0U);
            Standard::Algorithms::require_positive(max_value, "max value");

            Standard::Algorithms::require_less_equal(value, max_value, "value vs max value");
        }
        {
            const auto actual = Standard::Algorithms::Numbers::reverse_bits(value, bit_count);

            ::Standard::Algorithms::ert::are_equal(expected_reversed_value, actual, "reverse_bits");
        }
        {
            const auto actual = Standard::Algorithms::Numbers::reverse_bits(expected_reversed_value, bit_count);

            ::Standard::Algorithms::ert::are_equal(value, actual, "reversed reverse_bits");
        }
    }

    [[nodiscard]] consteval auto reverse_bits_tests() -> bool
    {
        constexpr std::uint64_t one = 1;

        // NOLINTNEXTLINE
        reverse_bits_test(0B011'101, 6, 0B101'110); // NOLINTNEXTLINE
        reverse_bits_test(0, 1, 0); // NOLINTNEXTLINE
        reverse_bits_test(1, 1, 1); // NOLINTNEXTLINE
        reverse_bits_test(1, 2, 2); // NOLINTNEXTLINE
        reverse_bits_test(1, 3, 4); // NOLINTNEXTLINE
        reverse_bits_test(1, 4, 8); // NOLINTNEXTLINE
        reverse_bits_test(1, 5, 16); // NOLINTNEXTLINE
        reverse_bits_test(one, 64, one << 63U); // NOLINTNEXTLINE
        reverse_bits_test(3, 64, (one + 2LLU) << 62U); // NOLINTNEXTLINE
        reverse_bits_test(5, 64, (one + 4LLU) << 61U); // NOLINTNEXTLINE
        reverse_bits_test(11, 4, 13); // NOLINTNEXTLINE
        reverse_bits_test(9, 4, 9); // NOLINTNEXTLINE

        return true;
    }
} //  namespace

namespace // bit_reverse_permute_tests
{
    template<class coll_t>
    constexpr void bit_reverse_permute_test(const coll_t &expected, const std::string &name, coll_t &source)
    {
        assert(!name.empty());

        // No const here. NOLINTNEXTLINE
        std::span spa = source;

        Standard::Algorithms::Numbers::bit_reverse_permute(spa);
        ::Standard::Algorithms::ert::are_equal(expected, source, "bit_reverse_permute " + name);
    }

    [[nodiscard]] constexpr auto string_reverse_permute_tests() -> bool
    {
        using item_t = std::string;

        {
            const std::array expected{ item_t{ "milano" }, item_t{ "prato" }, item_t{ "palermo" }, item_t{ "torino" } };

            std::array source{ item_t{ "milano" }, item_t{ "palermo" }, item_t{ "prato" }, item_t{ "torino" } };

            bit_reverse_permute_test(expected, "4", source);
        }
        {
            const std::array expected{ item_t{ "milano" }, item_t{ "prato" }, item_t{ "palermo" } };

            std::array source{ item_t{ "milano" }, item_t{ "palermo" }, item_t{ "prato" } };

            bit_reverse_permute_test(expected, "3", source);
        }
        {
            const std::array expected{ item_t{ "milano" }, item_t{ "palermo" } };

            std::array source{ item_t{ "milano" }, item_t{ "palermo" } };

            bit_reverse_permute_test(expected, "2", source);
        }

        {
            const std::array expected{ item_t{ "milano" } };

            std::array source{ item_t{ "milano" } };

            bit_reverse_permute_test(expected, "1", source);
        }

        return true;
    }

    [[nodiscard]] constexpr auto number_reverse_permute_tests() -> bool
    {
        {
            const std::array expected{// NOLINTNEXTLINE
                -1000, 40, 20, 60, 10, 50, 30, 70
            };

            std::array source{// NOLINTNEXTLINE
                -1000, 10, 20, 30, 40, 50, 60, 70
            };

            bit_reverse_permute_test(expected, "8", source);
        }
        {
            const std::array expected{// NOLINTNEXTLINE
                -1000, 80, 40, 120, 20, 100, 60, 140, // NOLINTNEXTLINE
                10, 90, 50, 130, 30, 110, 70, 150
            };

            std::array source{// NOLINTNEXTLINE
                -1000, 10, 20, 30, 40, 50, 60, 70, // NOLINTNEXTLINE
                80, 90, 100, 110, 120, 130, 140, 150
            };

            bit_reverse_permute_test(expected, "16", source);
        }

        return true;
    }
} // namespace

void Standard::Algorithms::Numbers::Tests::even_begin_odd_end_permute_tests()
{
    static_assert(reverse_bits_tests());
    static_assert(string_reverse_permute_tests());
    static_assert(number_reverse_permute_tests());

    using int_t = std::int32_t;

    constexpr int_t size = 4;
    using array_t = std::array<int_t, size>;

    const array_t arr_orig{ 10, 20, 30, 40 };
    const array_t arr_orig_permuted = { 10, 30, 20, 40 };
    const array_t perm_orig = { 0, 2, 1, 3 };

    {
        auto arr = arr_orig;
        const std::span sir{ arr };

        auto perm = perm_orig;
        const std::span per{ perm };

        ::Standard::Algorithms::Numbers::apply_permutation(per, size, sir);
        ::Standard::Algorithms::ert::are_equal(arr, arr_orig_permuted, "apply_permutation");

        std::for_each(perm.begin(), perm.end(),
            [](auto &val)
            {
                val += size;
            });

        ::Standard::Algorithms::ert::are_equal(perm_orig, perm, "apply_permutation perm");
    }

    std::vector<bool> buffer;
    {
        auto arr = arr_orig;
        const std::span sir{ arr };

        const std::span per{ perm_orig };

        ::Standard::Algorithms::Numbers::apply_permutation2(per, size, sir, buffer);

        ::Standard::Algorithms::ert::are_equal(arr, arr_orig_permuted, "apply_permutation2");
    }
    {
        auto arr = arr_orig;
        const std::span ussr{ arr };

        ::Standard::Algorithms::Numbers::even_begin_odd_end_permute(ussr, buffer);

        ::Standard::Algorithms::ert::are_equal(arr, arr_orig_permuted, "even_begin_odd_end_permute");
    }
    {
        auto arr = arr_orig_permuted;
        const std::span madam{ arr };

        ::Standard::Algorithms::Numbers::even_begin_odd_end_permute_inverse(madam, buffer);

        ::Standard::Algorithms::ert::are_equal(arr, arr_orig, "even_begin_odd_end_permute_inverse");
    }
}
