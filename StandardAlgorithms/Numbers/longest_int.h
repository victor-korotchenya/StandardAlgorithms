#pragma once
#include<cassert>
#include<concepts>
#include<cstddef>
#include<cstdint>
#include<type_traits>

namespace Standard::Algorithms::Numbers
{
    template<std::integral int_t>
    requires(sizeof(int_t) <= sizeof(std::int64_t))
    using longest_int = std::conditional_t<std::is_signed_v<int_t>, std::int64_t, std::uint64_t>;
} // namespace Standard::Algorithms::Numbers
