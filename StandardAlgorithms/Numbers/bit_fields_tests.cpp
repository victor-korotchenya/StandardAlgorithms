#include"bit_fields_tests.h"
#include<array>
#include<cstdint>

namespace
{
    // NOLINTNEXTLINE
    static_assert(sizeof(std::uint8_t) == 1 && sizeof(std::int32_t) == 4, "Basic test");

    struct str_5 final
    {
        std::int32_t beg{};
        std::uint8_t mid{};
        // 3 potentially lost bytes.
    };

    constexpr auto first_bytes = 5;
    constexpr auto first_lost_bytes = 3;

    struct str_8 final
    {// Join with no padding.
        [[no_unique_address]] str_5 strike{};
        [[no_unique_address]] std::array<std::uint8_t, first_lost_bytes> end{};
    };

    constexpr auto byte_bits = 8;

    struct str_8_bits final
    {// Expect no padding as well.
        std::int32_t beg : (byte_bits * sizeof(std::int32_t)){};
        std::uint8_t mid : byte_bits{};
        //
        std::uint8_t end1 : byte_bits{};
        std::uint8_t end2 : byte_bits{};
        std::uint8_t end3 : byte_bits{};
    };

    struct str_12 final
    {
        str_5 begin{};
        std::array<std::uint8_t, first_lost_bytes> end{};
        // 1 potentially lost byte.
    };
} // namespace

void Standard::Algorithms::Numbers::Tests::bit_fields_tests()
{
    constexpr auto packed_bytes = first_bytes + first_lost_bytes;

    static_assert(sizeof(str_5) == packed_bytes);

    static_assert(sizeof(str_8) == packed_bytes, "no_unique_address test.");

    static_assert(sizeof(str_8_bits) == packed_bytes, "bit fields test.");

    constexpr auto second_lost_bytes = 1;
    constexpr auto total_lost_bytes = first_lost_bytes + second_lost_bytes;

    static_assert(sizeof(str_12) == packed_bytes + total_lost_bytes, "Padding test.");
}
