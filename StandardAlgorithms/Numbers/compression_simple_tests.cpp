#include"compression_simple_tests.h"
#include"../Utilities/random.h"
#include"../Utilities/test_utilities.h"
#include"compression_simple.h"
#include<vector>

namespace
{
    using uint_t = std::uint32_t;

    constexpr std::size_t size_of_array = Standard::Algorithms::Numbers::bytes_as_7bits(sizeof(uint_t));

    using array_t = std::array<char, size_of_array>;
    using from_to_t = std::pair<uint_t, array_t>;

    constexpr char hero = 0x43;

    [[nodiscard]] consteval auto cha(const std::uint32_t val) -> char
    {
        return static_cast<char>(val);
    }

    // Only for the test data is it assumed that a hero might appear only in the tail (suffix).
    constexpr auto compute_expected_index(const from_to_t &test) -> std::size_t
    {
        const auto &buffer = test.second;
        const auto index = static_cast<std::size_t>(std::find(buffer.cbegin(), buffer.cend(), hero) - buffer.cbegin());

        const auto *const name = "expected index";
        ::Standard::Algorithms::ert::greater(index, 0U, name);
        ::Standard::Algorithms::ert::greater(size_of_array + 1U, index, name);

        return index;
    }

    constexpr void uint_7bit_test_compress(const from_to_t &test, const std::size_t expected_index)
    {
        array_t actual;
        std::fill(actual.begin(), actual.end(), hero);

        const auto &input = test.first;
        std::size_t index{};

        Standard::Algorithms::Numbers::uint_7bit_compress(input, actual, index
#if _DEBUG
            ,
            size_of_array
#endif
        );

        auto name = "uint_7bit_compress " + ::Standard::Algorithms::Utilities::zu_string(input);
        const auto &expected = test.second;
        ::Standard::Algorithms::ert::are_equal(expected, actual, name);

        name += " index";
        ::Standard::Algorithms::ert::are_equal(expected_index, index, name);
    }

    constexpr void uint_7bit_test_decompress(const from_to_t &test, const std::size_t expected_index)
    {
        const auto &buffer = test.second;
        std::size_t index{};

        const auto actual = Standard::Algorithms::Numbers::uint_7bit_decompress<uint_t>(buffer, index
#if _DEBUG
            ,
            size_of_array
#endif
        );

        const auto &expected = test.first;
        ::Standard::Algorithms::ert::are_equal(expected, actual, "uint_7bit_decompress");

        ::Standard::Algorithms::ert::are_equal(expected_index, index, "uint_7bit_decompress index");
    }

    constexpr void uint_7bit_compress_tests()
    {
        const std::vector<from_to_t> tests = {// 0x12E456A9 = 1 0010 1110 0100 0101 0110 1010 1001 =
            // 0x12E456A9 = 1 0010111 0010001 0101101 0101001 = // Split by 7 bits
            // 0x12E456A9 = 0x01 0x17 0x11 0x2D 0x29 // Split by 7 bits
            // 0x01 0x17 0x11 0x2D 0x29 -> (0x01)_(0x80 + 0x17)_(0x80 + 0x11)_(0x80 + 0x2D)_(0x80 + 0x29) =
            // 0x01 0x17 0x11 0x2D 0x29 -> (0x01)_(0x97)_______(0x91)_______(0xAD)______(0xA9) =
            // NOLINTNEXTLINE
            { 0x12E456A9, { cha(0xA9), cha(0xAD), cha(0x91), cha(0x97), 0x01 } },

            // 0x23456 = 10 0011 0100 0101 0110 =
            // 0x23456 = 1000 1101000 1010110 = // Split by 7 bits
            // 0x23456 = 0x08 0x68 0x56 // Split by 7 bits
            // 0x08 0x68 0x56 -> (0x08)_(0x80 + 0x68)_(0x80 + 0x56) =
            // 0x08 0x68 0x56 -> (0x08)_(0xE8)______(0xD6)
            // NOLINTNEXTLINE
            { 0x23456, { cha(0xD6), cha(0xE8), 0x08, hero, hero } },

            // Min value
            // NOLINTNEXTLINE
            { 0U, { 0U, hero, hero, hero, hero } },

            // NOLINTNEXTLINE
            { 1U, { 1U, hero, hero, hero, hero } },
            // NOLINTNEXTLINE
            { 17U, { 17U, hero, hero, hero, hero } },
            // NOLINTNEXTLINE
            { 126U, { 126U, hero, hero, hero, hero } },
            // NOLINTNEXTLINE
            { 127U, { 127U, hero, hero, hero, hero } },
            // NOLINTNEXTLINE
            { 0x80, { cha(0x80), 1, hero, hero, hero } }, // 0x80 = 1 0000000 -> 0x01 0x80
            // NOLINTNEXTLINE
            { 0xD5, { cha(0xD5), 1, hero, hero, hero } }, // 0xD5 = 1 1010101 = 0x01 0x55 -> 0x01 0xD5
            // NOLINTNEXTLINE
            { 0xFB, { cha(0xFb), 1, hero, hero, hero } }, // 0xFB =1 1111011 = 0x01 0x7b -> 0x01 0xFb
            // NOLINTNEXTLINE
            { 0xFF, { cha(0xFf), 1, hero, hero, hero } }, // 0xFF = 1 1111111 = 0x01 0x7F -> 0x01 0xFf
            // NOLINTNEXTLINE
            { 0x100, { cha(0x80), 2, hero, hero, hero } }, // 0x100 = 10 0000000 = 0x2 0000000 -> 0x02 0x80

            // NOLINTNEXTLINE
            { 0xFfFfFfFd, { cha(0xFd), cha(0x0Ff), cha(0x0Ff), cha(0x0Ff), cha(0x0F) } }, // Max value - 2
            // NOLINTNEXTLINE
            { 0xFfFfFfFe, { cha(0xFe), cha(0x0Ff), cha(0x0Ff), cha(0x0Ff), cha(0x0F) } }, // Max value - 1
            // NOLINTNEXTLINE
            { 0xFfFfFfFf, { cha(0xFf), cha(0x0Ff), cha(0x0Ff), cha(0x0Ff), cha(0x0F) } }
        }; // Max value

        for (const auto &test : tests)
        {
            const auto expected_index = compute_expected_index(test);
            uint_7bit_test_compress(test, expected_index);
            uint_7bit_test_decompress(test, expected_index);
        }
    }
} // namespace

void Standard::Algorithms::Numbers::Tests::compression_simple_tests()
{
    uint_7bit_compress_tests();
}
