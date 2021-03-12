#include"bit_utilities_tests.h"
#include"../Utilities/test_utilities.h"
#include"bit_utilities.h"
#include<optional>

namespace
{
    using int_t = std::uint32_t;

    constexpr void lowest_bit_tests()
    {
        const std::initializer_list<std::pair<std::int32_t, std::int32_t>> tests{ { 0, 0 }, { -1, 1 }, { 1, 1 },
            { 2, 2 },
            // NOLINTNEXTLINE
            { 3, 1 }, { 4, 4 }, { 5, 1 }, { 6, 2 },
            // NOLINTNEXTLINE
            { 12, 4 }, { 100, 4 }, { 104, 8 } };

        for (const auto &test : tests)
        {
            const auto actual = Standard::Algorithms::Numbers::lowest_bit(test.first);

            ::Standard::Algorithms::ert::are_equal(test.second, actual, "lowest_bit " + std::to_string(test.first));
        }
    }

    [[nodiscard]] consteval auto has_zero_bit_tests() -> std::optional<std::int32_t>
    {
        const std::initializer_list<std::int32_t> haves{// NOLINTNEXTLINE
            1, 3, 5, 7, 63, 1'021
        };

        for (const auto &hav : haves)
        {
            if (!Standard::Algorithms::Numbers::has_zero_bit(hav))
            {
                return hav;
            }

            if (const auto negat = -hav; !Standard::Algorithms::Numbers::has_zero_bit(negat))
            {
                return negat;
            }

            if (const auto pferd = hav - 1; Standard::Algorithms::Numbers::has_zero_bit(pferd))
            {
                return pferd;
            }

            if (const auto fleisch = hav + 1; // Sad future for cattle.
                Standard::Algorithms::Numbers::has_zero_bit(fleisch))
            {
                return fleisch;
            }
        }

        return std::nullopt;
    }

    [[nodiscard]] constexpr auto build_pop_tests() -> std::vector<std::pair<int_t, int_t>>
    {
        std::vector<std::pair<int_t, int_t>> tests{ { 1U, 2U }, { 2U, 4U },
            // NOLINTNEXTLINE
            { 5U, 6U }, { 1U << 30U, 1U << 31U },
            // NOLINTNEXTLINE
            { (1U << 30U) | (1U << 29U) | (1U << 28U), (1U << 31U) | 3U },
            // NOLINTNEXTLINE
            { (1U << 31U) | 3U, (1U << 31U) | 5U } };

        const std::vector<int_t> sequentials{// NOLINTNEXTLINE
            0b1'1000'1111'1000, 0b1'1001'0000'1111,
            // NOLINTNEXTLINE
            0b1'1001'0001'0111, 0b1'1001'0001'1011,
            // NOLINTNEXTLINE
            0b1'1001'0001'1101, 0b1'1001'0001'1110,
            // NOLINTNEXTLINE
            0b1'1001'0010'0111
        };

        for (std::size_t index = 1; index < sequentials.size(); ++index)
        {
            tests.emplace_back(sequentials[index - 1U], sequentials[index]);
        }

        return tests;
    }

    constexpr void pop_tests()
    {
        const auto tests = build_pop_tests();

        for (const auto &test : tests)
        {
            constexpr int_t zero{};

            const auto &input = test.first;
            ::Standard::Algorithms::ert::not_equal(zero, input, "input");

            const auto &output = test.second;
            ::Standard::Algorithms::ert::not_equal(zero, output, "output");

            ::Standard::Algorithms::ert::not_equal(input, output, "In/out test values must be different.");

            {
                const auto actual = Standard::Algorithms::Numbers::next_greater_same_pop_count(input);

                ::Standard::Algorithms::ert::are_equal(
                    output, actual, "next_greater_same_pop_count " + std::to_string(input));
            }
            {
                const auto actual = Standard::Algorithms::Numbers::next_smaller_same_pop_count(output);

                ::Standard::Algorithms::ert::are_equal(
                    input, actual, "next_smaller_same_pop_count " + std::to_string(output));
            }
        }
    }

    constexpr void enum_mask_tests()
    {
        constexpr int_t source = 0b10'0110;

        std::vector<int_t> actual;

        Standard::Algorithms::Numbers::enumerate_all_subsets_of_mask<int_t>(source, actual);

        const std::vector<int_t> expected{// 0b100110, //3
            // 0b100100, 0b100010, 0b110, //2
            // 0b100, 0b100000, 0b10, //1
            //
            // NOLINTNEXTLINE
            0b10'0110, 0b10'0100, 0b10'0010, 0b10'0000,
            // NOLINTNEXTLINE
            0b110, 0b100, 0b10
        };

        ::Standard::Algorithms::ert::are_equal(expected, actual, "enumerate_all_subsets_of_mask");
    }
} // namespace

void Standard::Algorithms::Numbers::Tests::bit_utilities_tests()
{
    static_assert(!has_zero_bit_tests().has_value()); // todo(p3): print the violating value.

    lowest_bit_tests();
    pop_tests();
    enum_mask_tests();
}
