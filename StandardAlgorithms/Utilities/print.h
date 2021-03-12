#pragma once
// "print.h"
#include"is_debug.h"
#include<algorithm> // std::reverse
#include<array>
#include<cassert>
#include<concepts>
#include<cstddef>
#include<cstdint>
#include<limits>
#include<ostream>
#include<type_traits>
#include<utility>

#ifndef __clang__
#include<source_location>
#endif

namespace Standard::Algorithms::Utilities
{
    constexpr auto negative_sign = '-';
    constexpr auto max_int_logar = 3U;

    static_assert(1U == sizeof(std::int8_t));

    // The ending '\0' is not counted.
    [[nodiscard]] inline constexpr auto int_string_max_size(std::uint32_t logar = max_int_logar, bool is_signed1 = true)
        -> std::uint32_t
    {
        switch (logar)
        {
        case 0U:
            // 255
            // -127 // NOLINTNEXTLINE
            return is_signed1 ? 4U : 3U;
        case 1U:
            //  65,535
            // -32,767 // NOLINTNEXTLINE
            return is_signed1 ? 7U : 6U;
        case 2U:
            //  4,294,967,296
            // -2,147,483,647 // NOLINTNEXTLINE
            return is_signed1 ? 14U : 13U;
        case 3U:
            //  18,446,744,073,709,551,616
            // -9,223,372,036,854,775,807 // NOLINTNEXTLINE
            return 26U;
        default:
            assert(0);
            std::unreachable();
        }
    }

    template<std::integral typ>
    requires(sizeof(typ) <= sizeof(std::uint64_t))
    [[nodiscard]] constexpr auto typ_string_max_size() -> std::uint32_t
    {
        std::uint32_t logar{};

        if (sizeof(typ) <= sizeof(std::uint8_t))
        {
            logar = 0U;
        }
        else if (sizeof(typ) <= sizeof(std::uint16_t))
        {
            logar = 1U;
        }
        else if (sizeof(typ) <= sizeof(std::uint32_t))
        {
            logar = 2U;
        }
        else if (sizeof(typ) <= sizeof(std::uint64_t))
        {
            logar = 3U;
        }
        else
        {
            assert(0);
            std::unreachable();
        }

        constexpr auto is_signed1 = std::is_signed_v<typ>;

        auto size1 = int_string_max_size(logar, is_signed1);
        assert(0U < size1);
        return size1;
    }

    template<std::uint32_t max_size1>
    requires(0U < max_size1 && max_size1 < std::numeric_limits<std::uint16_t>::max() - 1U)
    struct small_buffer final
    {
        static constexpr auto max_size = max_size1;

        static_assert(1 == sizeof(std::int8_t));

        [[nodiscard]] constexpr auto size() const noexcept -> std::uint32_t
        {
            return Size;
        }

        [[nodiscard]] constexpr auto data() const &noexcept -> const std::array<std::int8_t, max_size> &
        {
            return Arr;
        }

        [[nodiscard]] constexpr auto data() &noexcept -> std::array<std::int8_t, max_size> &
        {
            return Arr;
        }

        // Return true in case of success.
        constexpr auto append(std::int8_t cha) noexcept -> bool
        {
            if (!(Size < max_size)) [[unlikely]]
            {
                assert(0);
                return false;
            }

            Arr[Size] = cha;
            ++Size;
            return true;
        }

private:
        // No initialization. // NOLINTNEXTLINE
        std::array<std::int8_t, max_size> Arr
#ifndef __clang__
            {}
#endif
        ;

        std::uint32_t Size{};
    };

    template<std::integral typ, class lam_t>
    requires(sizeof(typ) <= sizeof(std::uint64_t))
    constexpr void print(lam_t lam, typ value)
    {
        constexpr typ zero{};
        constexpr typ basis{ 10 };
        constexpr auto is_signed1 = std::is_signed_v<typ>;

        constexpr auto group_separ = ',';
        constexpr auto group_size = 3;
        constexpr auto max_size = typ_string_max_size<typ>();
        static_assert(0U < max_size);

        [[maybe_unused]] const auto is_negative_sign1 = value < zero;

        small_buffer<max_size> buf{};

        if constexpr (::Standard::Algorithms::is_debug)
        {
            assert(0U == buf.size());
        }

        for (auto groups = 0;;)
        {
            {
                auto digit = (value % basis);
                value /= basis;

                if constexpr (is_signed1)
                {
                    if (is_negative_sign1)
                    {
                        digit = -digit;
                    }
                }

                if constexpr (::Standard::Algorithms::is_debug)
                {
                    assert(!(digit < zero) && digit < basis);
                }

                constexpr auto digit_0 = '0';

                buf.append(static_cast<std::int8_t>(digit + digit_0));
            }

            if (zero == value)
            {
                break;
            }

            if (++groups < group_size)
            {
                continue;
            }

            groups = 0;
            buf.append(group_separ);
        }

        if constexpr (is_signed1)
        {
            if (is_negative_sign1)
            {
                buf.append(negative_sign);
            }
        }

        const auto size1 = buf.size();

        if constexpr (::Standard::Algorithms::is_debug)
        {
            assert(0U < size1 && size1 <= max_size);
        }

        auto &data = buf.data();
        // cppcheck-suppress [danglingTempReference]
        std::reverse(data.begin(), data.begin() + size1);

        for (std::uint32_t index{}; index < size1; ++index)
        {
            // cppcheck-suppress [danglingTempReference]
            const auto &byt = data[index];
            lam(byt);
        }
    }

    template<std::integral typ>
    requires(sizeof(typ) <= sizeof(std::uint64_t))
    auto print(std::ostream &str, const typ &value) -> std::ostream &
    {
        print<typ>(
            [&str](const auto &cha)
            {
                str << static_cast<char>(cha);
            },
            value);

        return str;
    }

#ifndef __clang__
    inline auto print(std::ostream &str, const std::source_location &loc = std::source_location::current())
        -> std::ostream &
    {
        str << "\nFunction '" << loc.function_name() << "' in file '" << loc.file_name() << "' at ";

        print(str, loc.line());
        str << ":";
        print(str, loc.column());
        str << "\n";

        return str;
    }
#endif
} // namespace Standard::Algorithms::Utilities
