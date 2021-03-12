#include"is_little_endian.h"
#include"is_debug.h"
#include<bit> // std::bit_cast
#include<cassert>
#include<cstdint>

namespace
{
    static_assert(0xBaAbCeEc == std::byteswap<std::uint32_t>(0xEcCeAbBa));
    static_assert(0xaCeDa == std::byteswap<std::uint32_t>(0xDaCe0a00), "0x000aCeDa");
}

[[nodiscard]] auto Standard::Algorithms::Utilities::is_little_endian() noexcept -> bool
{
    const std::uint16_t value = 1;

    const auto *const ptr = std::bit_cast<const std::uint8_t *>(&value);
    const auto &low = *ptr;
    auto result = 0U != low;

    if constexpr (::Standard::Algorithms::is_debug)
    {
        [[maybe_unused]] constexpr auto is_little = std::endian::native == std::endian::little;

        assert(is_little == result);
    }

    return result;
}
