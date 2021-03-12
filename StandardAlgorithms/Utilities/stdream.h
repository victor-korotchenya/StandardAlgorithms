#pragma once
// "stdream.h"
#include"zu_string.h"
#include<cstddef>
#include<string>
#include<string_view>

namespace Standard::Algorithms
{
    // A constexpr string stream dream.
    // Note. std::ostream is not constexpr.
    struct stdream final
    {
        constexpr void reserve(const std::size_t capacity)
        {
            Buffer.reserve(capacity);
        }

        [[nodiscard]] constexpr auto str() const &noexcept -> const std::string &
        {
            return Buffer;
        }

        [[nodiscard]] constexpr auto buffer() &noexcept -> std::string &
        {
            return Buffer;
        }

private:
        std::string Buffer{};
    };

    inline constexpr auto operator<< (stdream &str, const char cha) -> stdream &
    {
        str.buffer() += cha;
        return str;
    }

    inline constexpr auto operator<< (stdream &str, const char *const rqa) -> stdream &
    {
        if (rqa != nullptr) [[likely]]
        {
            str.buffer() += rqa;
        }

        return str;
    }

    inline constexpr auto operator<< (stdream &str, const std::string_view &stri) -> stdream &
    {
        str.buffer() += stri;
        return str;
    }

    template<std::integral typ>
    inline constexpr auto operator<< (stdream &str, const typ &value) -> stdream &
    {
        {// todo(p3): avoid an unnecessary string allocation, use an array.
            const auto stri = Utilities::int_zu_string<typ>(value);
            str.buffer() += stri;
        }
        return str;
    }
} // namespace Standard::Algorithms
