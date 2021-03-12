#pragma once
#include<string>
#include<vector>

namespace Standard::Algorithms::Strings
{
    // Given a string, count..

    // todo(p2). Suffix tree, time O(n).

    // todo(p4). Compressed trie, time O(n*n).

    // Lower bound because the hashes of different strings might match - rolling hash idea.
    // Time O(n*n).
    [[nodiscard]] auto count_distinct_substrings_of_length(
        const std::string &str, const std::vector<std::int32_t> &lengths) -> std::vector<std::int32_t>;

    // Time O(n**3).
    [[nodiscard]] auto count_distinct_substrings_of_length_slow(
        const std::string &str, const std::vector<std::int32_t> &lengths) -> std::vector<std::int32_t>;
} // namespace Standard::Algorithms::Strings
