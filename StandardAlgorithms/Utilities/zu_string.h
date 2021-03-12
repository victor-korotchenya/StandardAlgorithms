#pragma once
// "zu_string.h"
#include"is_integral_pure.h"
#include"print.h"
#include"w_stream.h"
#include<cstddef>
#include<cstdint>
#include<string>

namespace Standard::Algorithms::Utilities
{
    // It is slow - prefer to use print().
    // std::to_string is not constexpr - do it yourself.
    template<std::integral typ>
    [[nodiscard]] constexpr auto int_zu_string(const typ &value) -> std::string
    {
        std::string str;

        print<typ>(
            [&str](const auto &cha)
            {
                str += static_cast<char>(cha);
            },
            value);

        return str;
    }

    // It is slow - prefer to use print().
    // std::to_string is not constexpr - do it yourself.
    template<class typ>
    requires(std::is_arithmetic_v<std::remove_cvref_t<typ>>)
    [[nodiscard]] constexpr auto zu_string(const typ &value) -> std::string
    {
        if constexpr (is_integral_pure<typ>)
        {
            return int_zu_string<typ>(value);
        }

        auto str = w_stream();
        str << value;
        return str.str();
    }
} // namespace Standard::Algorithms::Utilities
