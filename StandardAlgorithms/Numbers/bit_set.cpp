#include"bit_set.h"
#include"../Utilities/require_utilities.h"

namespace
{
    constexpr auto shift = 3U;
    constexpr auto mask = 7U;

    static_assert(sizeof(std::uint8_t) == 1);
} // namespace

[[nodiscard]] auto Standard::Algorithms::Numbers::size_bytes(const std::size_t bits) noexcept -> std::size_t
{
    const auto bytes = (bits >> shift) + ((bits & mask) != 0U ? 1U : 0U);

    return bytes;
}

Standard::Algorithms::Numbers::bit_set::bit_set(std::size_t bits)
    : Data(size_bytes(require_positive(bits, "bits")))
{
}

void Standard::Algorithms::Numbers::bit_set::set(std::size_t bit)
{
    assert(bit < max_bits());

    const auto high = bit >> shift;
    const auto low = bit & mask;

    auto &byt = Data.at(high);
    byt |= 1U << low;

    assert(byt != 0U);
}

[[nodiscard]] auto Standard::Algorithms::Numbers::bit_set::has(std::size_t bit) const -> bool
{
    assert(bit < max_bits());

    const auto high = bit >> shift;
    const auto low = bit & mask;

    const auto byt = static_cast<std::uint32_t>( // It is signed - must cast?
        Data.at(high));
    const auto value = (byt >> low) & 1U;

    return value != 0U;
}

[[nodiscard]] auto Standard::Algorithms::Numbers::bit_set::max_bits() const noexcept -> std::size_t
{
    const auto bits = Data.size() << shift;
    return bits;
}
