#pragma once
#include<cassert>
#include<concepts>
#include<cstddef>
#include<cstdint>
#include<string>
#include<type_traits>
#include<utility>

namespace Standard::Algorithms::Numbers
{
    // See also std::make_signed, std::make_unsigned,
    // using make_unsigned_t = typename std::make_unsigned<t>::type;
    template<std::unsigned_integral int_t>
    [[nodiscard]] constexpr auto to_unsigned(int_t value) noexcept
    {
        return value;
    }

    template<std::signed_integral int_t>
    [[nodiscard]] constexpr auto to_signed(int_t value) noexcept
    {
        return value;
    }

    template<std::unsigned_integral int_t>
    requires(std::is_same_v<std::remove_cvref_t<int_t>, int_t>) // todo(p3): del?
    [[nodiscard]] constexpr auto to_signed(int_t value) noexcept
    {
        if constexpr (std::is_same_v<int_t, std::uint8_t>)
        {
            return static_cast<std::int8_t>(value);
        }
        else if constexpr (std::is_same_v<int_t, std::uint16_t>)
        {
            return static_cast<std::int16_t>(value);
        }
        else if constexpr (std::is_same_v<int_t, std::uint32_t>)
        {
            return static_cast<std::int32_t>(value);
        }
        else if constexpr (std::is_same_v<int_t, std::uint64_t>)
        {
            return static_cast<std::int64_t>(value);
        }
        // else if constexpr (std::is_same_v<int_t, std::uint128_t>)
        //{
        //     return static_cast<std::int128_t>(value);
        // }
        else
        {
            []<bool flag = false>()
            {
                // static_assert(flag, ("Unsupported type " + std::string(typeid(int_t).name())).c_str());
                static_assert(flag, "Unsupported type");
            }
            ();
        }
    }

    template<std::signed_integral int_t>
    requires(std::is_same_v<std::remove_cvref_t<int_t>, int_t>) // todo(p3): del?
    [[nodiscard]] constexpr auto to_unsigned(int_t value) noexcept
    {
        if constexpr (std::is_same_v<int_t, std::int8_t>)
        {
            return static_cast<std::uint8_t>(value);
        }
        else if constexpr (std::is_same_v<int_t, std::int16_t>)
        {
            return static_cast<std::uint16_t>(value);
        }
        else if constexpr (std::is_same_v<int_t, std::int32_t>)
        {
            return static_cast<std::uint32_t>(value);
        }
        else if constexpr (std::is_same_v<int_t, std::int64_t>)
        {
            return static_cast<std::uint64_t>(value);
        }
        // else if constexpr (std::is_same_v<int_t, std::int128_t>)
        //{
        //     return static_cast<std::uint128_t>(value);
        // }
        else
        {
            []<bool flag = false>()
            {
                // static_assert(flag, ("Unsupported type " + std::string(typeid(int_t).name())).c_str());
                static_assert(flag, "Unsupported type");
            }
            ();
        }
    }
} // namespace Standard::Algorithms::Numbers
