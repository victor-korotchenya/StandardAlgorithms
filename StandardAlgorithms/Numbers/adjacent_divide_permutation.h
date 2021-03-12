#pragma once
#include<cstdint>
#include<vector>

namespace Standard::Algorithms::Numbers
{
    // Given 0 < size <= 40, and a boolean array 'allowed' of size size+1,
    // count permutations of cost k modulo mod for each k in [0..size-1].
    // Cost of a permutation P of size size is the number of adjacent pairs:
    // 1) P[i - 1] divides P[i];
    // 2) allowed[P[i] / P[i - 1]] is true.
    // E.g. {1,2,4,  5,  3,6} has cost 3 provided allowed[2] = true.
    [[nodiscard]] auto adjacent_divide_permutation(std::int32_t size, const std::vector<char> &allowed,
        std::int32_t mod, bool is_debug = false) -> std::vector<std::int32_t>;

    // Slow.
    [[nodiscard]] auto adjacent_divide_permutation_slow(
        std::int32_t size, const std::vector<char> &allowed, std::int32_t mod) -> std::vector<std::int32_t>;
} // namespace Standard::Algorithms::Numbers
