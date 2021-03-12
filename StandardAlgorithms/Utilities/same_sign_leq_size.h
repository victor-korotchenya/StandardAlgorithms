#pragma once
#include<concepts>
#include<type_traits>
#include<utility>

namespace Standard::Algorithms
{
    template<std::integral int_t, std::integral long_int_t>
    constexpr bool same_sign_leq_size =
        std::is_signed_v<int_t> == std::is_signed_v<long_int_t> && sizeof(int_t) <= sizeof(long_int_t);
} // namespace Standard::Algorithms
