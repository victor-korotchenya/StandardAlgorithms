#pragma once
// "string_to_vector.h"
#include<algorithm>
#include<cassert>
#include<concepts>
#include<string>
#include<string_view>
#include<vector>

namespace Standard::Algorithms::Strings
{
    template<class string_t, class char_t = typename string_t::value_type, class vector_t = std::vector<char_t>,
        bool is_ung = std::is_same_v<std::remove_cvref_t<string_t>, std::remove_cvref_t<vector_t>>>
    [[nodiscard]] constexpr auto string_to_vector(const string_t &str) noexcept(is_ung) -> vector_t
    {
        if constexpr (is_ung)
        {
            return str;
        }

        const auto size = str.size();

        vector_t vec(size, char_t{});
        std::copy(str.cbegin(), str.cend(), vec.begin());

        return vec;
    }
} // namespace Standard::Algorithms::Strings
