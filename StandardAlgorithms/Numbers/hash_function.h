#pragma once
#include<concepts>
#include<cstddef>

namespace Standard::Algorithms::Numbers
{
    template<class hash_func_t, class key_t>
    concept hash_function = requires(hash_func_t func, const key_t &key)
    // Avoid 70 chars long prefixes.
    // NOLINTNEXTLINE
    {
        {
            func(key)
            } noexcept -> std::convertible_to<std::size_t>;
    };
} // namespace Standard::Algorithms::Numbers
