#pragma once
// "suffix_array.h"
#include<cstddef>
#include<cstdint>
#include<string>
#include<string_view>
#include<vector>

namespace Standard::Algorithms::Strings
{
    // Suffix array sa, where sa[i] is the starting index of the i-th smallest non-empty suffix.
    // The whole word index is 0. Only for small strings of size <= 2E9.
    // Time O(n).
    //
    // Start index, non-empty suffixes of "banana":
    // 0 banana
    // 1 anana
    // 2 nana
    // 3 ana
    // 4 na
    // 5 a
    //
    // Suffix array sa, and sorted suffixes of "banana":
    // 5 a
    // 3 ana
    // 1 anana
    // 0 banana
    // 4 na
    // 2 nana
    //
    // Thus, the suffix array sa of "banana" is { 5, 3, 1, 0, 4, 2 };
    // the longest common prefix LCP { 0, 1, 3, 0, 0, 2 } with previous:
    // 5 a, LCP = 0, here is no previous suffix
    // 3 ana,  1
    // 1 anana,  3 == |ana|, where ana = max_length_prefix(ana, anana)
    // 0 banana,  0
    // 4 na,  0
    // 2 nana,  2.
    //
    // The inverted sa array, called ranks, helps calculating LCP easily.
    // "banana" ranks { 3, 2, 5, 1, 4, 0 }. E.g. sa[2] = 1 for "anana", so ranks[1] = 2.
    // sa, rank, suffix, previous suffix, LCP:
    // 0, 3, banana, anana, LCP = 0
    // 1, 2, anana, ana,  3
    // 2, 5, nana, na,  2
    // 3, 1, ana, a,  1
    // 4, 4, na, banana,  0
    // 5, 0, a, "",  0.
    void suffix_array_other(
        std::string_view str, std::vector<std::int32_t> &sar, std::vector<std::int32_t> *lcp = nullptr);

    // Initially, use the char codes for substrings of size 1.
    // Compute the new ranks using char compression, where min rank is 0.
    // Until the size n is reached, double the substring size:
    // - Combine a pair of 2 substring ranks. When no right part, let second = -1.
    // - Sort the pairs using O(n) sort. Compute the new ranks.
    // - Stop if all the ranks are unique (permutation).
    // Time O(n*log(n)*log(n)); can be decreased to O(n*log(n)).
    [[nodiscard]] auto suffix_array_slow_still(const std::string &str) -> std::vector<std::int32_t>;

    // Slow time O(n**3); average time O(n*n*log(n)).
    [[nodiscard]] auto suffix_array_slow(const std::string &str, std::vector<std::int32_t> *plcp = nullptr)
        -> std::vector<std::int32_t>;

    // Given a large constant text
    // and many usually small patterns,
    // slowly find where each pattern appears
    // in the text in O(|pattern| * log(|text|)).
    [[nodiscard]] auto suffix_array_search_slow(const std::string &text,
        const std::vector<std::int32_t> &suffix_array_of_text, const std::string &pattern) -> std::int32_t;
} // namespace Standard::Algorithms::Strings
