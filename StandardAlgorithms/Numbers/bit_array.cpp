#include"bit_array.h"
#include"arithmetic.h"

namespace
{
    static_assert(1 < (CHAR_BIT));

    // A word has 64 bits.
    constexpr std::size_t bites_per_word = (CHAR_BIT) * sizeof(std::uint64_t);

    constexpr auto shift = 3U;
    constexpr auto mask = 7U;

    [[nodiscard]] constexpr auto calc_words(const std::size_t bit_array_size) -> std::size_t
    {
        {
            const auto *const name = "storage new size bits";
            Standard::Algorithms::require_positive(bit_array_size, name);

            constexpr std::uint32_t small_delta = (CHAR_BIT) * (CHAR_BIT);
            static_assert(4U < small_delta);

            constexpr auto edge_bits = std::numeric_limits<std::size_t>::max() - small_delta;

            Standard::Algorithms::require_greater(edge_bits, bit_array_size, name);
        }

        const auto bits = Standard::Algorithms::Numbers::pad_by_alignment(bit_array_size, bites_per_word);

        const auto words = bits / bites_per_word;

        if constexpr (::Standard::Algorithms::is_debug)
        {
            Standard::Algorithms::require_positive(words, "words");
        }

        return words;
    }

    [[nodiscard]] inline auto raw_byte(const std::vector<std::uint64_t> &data) noexcept -> std::uint8_t *
    {
        const auto *const const_raw =
            // Working with bytes.
            // NOLINTNEXTLINE
            reinterpret_cast<const std::uint8_t *>(data.data());

        auto *const raw =
            // No sense to make 2 functions.
            // NOLINTNEXTLINE
            const_cast<std::uint8_t *>(const_raw);

        assert(raw != nullptr);

        return raw;
    }

    [[maybe_unused]] constexpr void check_position_debug(
        const std::vector<std::uint64_t> &data, const std::size_t position)
    {
        const auto size_bits = data.size() * bites_per_word;

        if (position < size_bits)
        {
            return;
        }

        auto str = ::Standard::Algorithms::Utilities::w_stream();
        str << "sizeBits " << size_bits << " <= position " << position << " in CheckPosition_Debug.";

        Standard::Algorithms::throw_exception(str);
    }
} // namespace

Standard::Algorithms::Numbers::bit_array::bit_array(const std::size_t initial_size_bits)
{
    resize(0U < initial_size_bits ? initial_size_bits : 1U);
}

void Standard::Algorithms::Numbers::bit_array::resize(const std::size_t new_size_bits)
{
    const auto words = calc_words(new_size_bits);
    Data.resize(words);
}

// void bit_array::AssignAllBits(const std::uint8_t byteValue = {})
//{
//   const std::uint64_t newValue = convert_by_duplicating(byteValue);
//
//   Data.assign(Data.size(), newValue);
// }

void Standard::Algorithms::Numbers::bit_array::set_bit(const std::size_t position)
{
    if constexpr (::Standard::Algorithms::is_debug)
    {
        check_position_debug(Data, position);
    }

    // 0..7 =>  byte = 0 and bit = 0..7
    // 8..15 => byte = 1 and bit = 0..7

    const auto position_byte = position >> shift;
    const auto position_bit = position & mask;

    auto *const raw = raw_byte(Data);

    auto &old_byte =
        // Simpler code
        // NOLINTNEXTLINE
        raw[position_byte];

    const auto new_byte = static_cast<std::uint32_t>(old_byte) | (1U << position_bit);

    old_byte = static_cast<std::uint8_t>(new_byte);
}

[[nodiscard]] auto Standard::Algorithms::Numbers::bit_array::get_bit(const std::size_t position) const -> std::uint8_t
{
    if constexpr (::Standard::Algorithms::is_debug)
    {
        check_position_debug(Data, position);
    }

    const auto position_byte = position >> shift;
    const auto position_bit = static_cast<std::uint32_t>(position & mask);

    const auto *const raw = raw_byte(Data);
    const auto old_byte =
        // Simpler code
        // NOLINTNEXTLINE
        static_cast<std::uint32_t>(raw[position_byte]);

    auto result = static_cast<std::uint8_t>((old_byte >> position_bit) & 1U);

    return result;
}

[[nodiscard]] auto Standard::Algorithms::Numbers::bit_array::count_set_bits() const -> std::uint64_t
{
    std::uint64_t result{};

    for (const auto &datum : Data)
    {
        const auto cnt = static_cast<std::uint32_t>(pop_count(datum));

        result += cnt;
    }

    if constexpr (::Standard::Algorithms::is_debug)
    {
        if (const auto words = Data.size(); words * bites_per_word < result)
        {
            auto str = ::Standard::Algorithms::Utilities::w_stream();
            str << "count_set_bits returned " << result << " while there are " << (words * bites_per_word) << " bits.";

            throw_exception(str);
        }
    }

    return result;
}
