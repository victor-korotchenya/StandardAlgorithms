#pragma once
#include<algorithm>
#include<concepts>
#include<type_traits>
#include<utility>

namespace Standard::Algorithms
{
    template<class t>
    constexpr bool is_char_ptr =
        std::is_same_v<const char *, std::remove_cvref_t<t>> || std::is_same_v<char *, std::remove_cvref_t<t>>;
} // namespace Standard::Algorithms
