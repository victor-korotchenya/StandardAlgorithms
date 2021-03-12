#pragma once
#include <vector>

namespace Privet::Algorithms::Numbers
{
    // Given 0 < n <= 40, and a boolean array 'allowed' of size n+1,
    // count permutations of cost k modulo mod for each k in [0..n-1].
    // Cost of a permutation P of size n is the number of adjacent pairs:
    // 1) P[i - 1] divides P[i];
    // 2) allowed[P[i] / P[i - 1]] is true.
    // E.g. {1,2,4,  5,  3,6} has cost 3 provided allowed[2] = true.
    std::vector<int> adjacent_divide_permutation(const char* const allowed,
        const int n, const int mod, const bool is_debug = false);

    // slow.
    std::vector<int> adjacent_divide_permutation_slow(const char* const allowed,
        const int n, const int mod);
}