#pragma once
#include<concepts>
#include<utility>

namespace Standard::Algorithms
{
    template<class pair_t>
    concept pair_like = requires(pair_t pair1) {
                            pair1.first;
                            pair1.second;
                        };
} // namespace Standard::Algorithms
