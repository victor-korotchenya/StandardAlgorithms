#pragma once
#include"../Utilities/is_debug.h"
#include<array>
#include<bit>
#include<cassert>
#include<concepts>
#include<cstddef>
#include<cstdint>

namespace Standard::Algorithms::Numbers
{
    // todo(p3): more zip e.g. strings, bool[].

    // Encode every 7_bits as (extra + 7_bits) where extra is 0x80 unless the highest 7_bits.
    // The order of bytes is reverted to have less code.
    // Watch out for buffer overflow!
    // Good for small numbers, faster writing.
    template<std::unsigned_integral uint_t, std::size_t array_size>
    requires(0U < array_size && std::endian::native == std::endian::little)
    constexpr void uint_7bit_compress(uint_t value, std::array<char, array_size> &buffer, std::size_t &index
#if _DEBUG
        ,
        const std::size_t max_index = array_size
#endif
    )
    {
#if _DEBUG
        assert(0U < max_index && max_index <= array_size);
#endif

        for (;;)
        {
            constexpr auto bits7 = 7U;
            constexpr uint_t zero{};
            constexpr uint_t low = 0x7F;
            constexpr uint_t high = 0x80;

            const auto compressed = (value < high ? zero : high) | (value & low);

#if _DEBUG
            {
                constexpr auto max_uchar = 255U;

                assert(index < max_index && compressed <= max_uchar);
            }
#endif

            buffer.at(index) = static_cast<char>(compressed);
            ++index;

            value >>= bits7;

            if (value == zero)
            {
                return;
            }
        }
    }

    // Return the number of bytes.
    inline constexpr auto bytes_as_7bits(std::size_t bytes) -> std::size_t
    {
        assert(0U < bytes && bytes < (bytes << 3U) && bytes == ((bytes << 3U) >> 3U));

        constexpr auto code = 7U;

        const auto bits = bytes << 3U;
        const auto bytes_7 = bits / code + ((bits % code) != 0U ? 1U : 0U);

        assert(bytes < bytes_7);

        return bytes_7;
    }

    template<std::unsigned_integral uint_t, std::size_t array_size>
    requires(0U < array_size && std::endian::native == std::endian::little)
    constexpr auto uint_7bit_decompress(const std::array<char, array_size> &buffer, std::size_t &index
#if _DEBUG
        ,
        const std::size_t max_index = array_size
#endif
        ) -> uint_t
    {
#if _DEBUG
        assert(0U < max_index && max_index <= array_size);
#endif

        std::uint32_t shift{}; // todo(p3): to favor read: remove the shift, normalize the byte order in compress().

        for (uint_t value{};;)
        {
            constexpr auto bits7 = 7U;
            constexpr uint_t low = 0x7F;
            constexpr uint_t high = 0x80;

#if _DEBUG
            assert(index < max_index);
#endif

            const auto &letter = static_cast<std::uint8_t>(buffer.at(index));
            const auto done = (letter & high) == 0U;
            const auto add = letter & low;

            const auto new_value = (add << shift) | value;
            assert(value <= new_value);

            value = new_value;
            ++index;

            if (done)
            {
                return value;
            }

            shift += bits7;

            if constexpr (::Standard::Algorithms::is_debug)
            {
                static constexpr auto max_shift_bits = bits7 * bytes_as_7bits(sizeof(uint_t));

                static_assert(0U < max_shift_bits);

                assert(0U < shift && shift < max_shift_bits);
            }
        }
    }
} // namespace Standard::Algorithms::Numbers
