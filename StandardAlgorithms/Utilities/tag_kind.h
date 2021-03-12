#pragma once
#include<cassert>
#include<cstdint>
#include<type_traits>
#include<utility>

namespace Standard::Algorithms::Utilities
{
    static_assert(std::is_unsigned_v<std::uint8_t>, "being a paranoid");

    enum class tag_kind : std::uint8_t
    {
        none,
        left,
        right,
        both,
    };

    [[nodiscard]] inline constexpr auto operator| (const tag_kind one, const tag_kind two) noexcept -> tag_kind
    {
        return static_cast<tag_kind>(std::to_underlying(one) | std::to_underlying(two));
    }

    inline constexpr auto operator|= (tag_kind &one, const tag_kind two) noexcept -> tag_kind
    {
        return one = (one | two);
    }

    [[nodiscard]] inline constexpr auto operator& (const tag_kind one, const tag_kind two) noexcept -> tag_kind
    {
        return static_cast<tag_kind>(std::to_underlying(one) & std::to_underlying(two));
    }

    [[nodiscard]] inline constexpr auto has_flag(const tag_kind value, const tag_kind flag) noexcept -> bool
    {
        assert(flag != tag_kind::none);

        return (value & flag) != tag_kind::none;
    }

    [[nodiscard]] inline constexpr auto operator- (const tag_kind value, const tag_kind to_remove) noexcept -> tag_kind
    {
        using uint_t = std::underlying_type_t<tag_kind>;

        static_assert(std::is_unsigned_v<uint_t> &&
            // todo(p1): Tilda of an unsigned type is paradoxically signed?
            std::is_signed_v<decltype(~uint_t{})>);

        assert(to_remove != tag_kind::none);

        auto first = std::to_underlying(value);
        auto second = std::to_underlying(to_remove);

        // Force the "nuts" to be unsigned.
        auto nuts = static_cast<uint_t>(~second);
        auto diff = static_cast<tag_kind>(first & nuts);
        return diff;
    }

    inline constexpr auto operator-= (tag_kind &value, const tag_kind to_remove) noexcept -> tag_kind
    {
        return value = (value - to_remove);
    }
} // namespace Standard::Algorithms::Utilities
