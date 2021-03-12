#pragma once
// "iota_vector.h"
#include<algorithm>
#include<cassert>
#include<concepts>
#include<cstddef>
#include<numeric> // iota
#include<utility>
#include<vector>

namespace Standard::Algorithms::Utilities
{
    template<class t>
    [[nodiscard]] [[gnu::pure]] constexpr auto iota_vector(const std::size_t size, t initial_value = {})
        -> std::vector<t>
    {
        std::vector<t> vec(size);
        std::iota(vec.begin(), vec.end(), initial_value);

        return vec;
    }

    template<class t>
    [[nodiscard]] [[gnu::pure]] constexpr auto iota_vector_reversed(const std::size_t size, t initial_value = {})
        -> std::vector<t>
    {
        std::vector<t> vec(size);
        std::iota(vec.rbegin(), vec.rend(), initial_value);

        return vec;
    }
} // namespace Standard::Algorithms::Utilities
