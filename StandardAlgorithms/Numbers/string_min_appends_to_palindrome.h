#pragma once
#include"../Strings/string_utilities.h" // is_palindrome
#include"../Utilities/check_size.h"
#include<unordered_map>

// todo(p4): to strings.

namespace Standard::Algorithms::Numbers::Inner
{
    // A string "abbaca" (of length 6) reverted is "acabba".
    // The automaton goes to the initial 0-th position on any mismatch:
    // [0] = {{'a' *, 1}}  // from 0 following 'a' move to 1.
    // [1] = {{'a', 1}, {'c' *, 2}}
    // [2] = {{'a' *, 3}}  // previous position temporarily becomes 1.
    // [3] = {{'a', 1}, {'b' *, 4}, {'c', 2}}
    // [4] = {{'a', 1}, {'b' *, 5}}
    // [5] = {{'a' *, 6}}  // previous also gets 1.
    //
    // Matching the original string "abbaca":
    // Start from the position 0, on 'a' move to 1.
    // 1, 'b' -> 0
    // 0, 'b' -> 0
    // 0, 'a' -> 1
    // 1, 'c' -> 2
    // 2, 'a' -> 3
    template<std::integral int_t, class string_t, class char_t>
    [[nodiscard]] constexpr auto build_string_match_reverted_automaton(const string_t &str, const int_t &size)
        -> std::vector<std::unordered_map<char_t, int_t>>
    {
        constexpr int_t zero{};
        constexpr int_t one = 1;

        assert(zero < size);

        // Build a DFA, DFM, DFSA, DFSM, DSA, DSM
        // going backwards from the string ending.
        std::vector<std::unordered_map<char_t, int_t>> transitions(size);
        transitions[zero][str.at(size - one)] = one;

        for (int_t now = one, prev_time{}; now < size; ++now)
        {
            const auto &prevs = transitions[prev_time];
            auto &currs = transitions[now];
            currs = prevs;

            const auto &chopin = str[size - one - now];
            currs[chopin] = static_cast<int_t>(now + one);

            const auto iter = prevs.find(chopin);
            prev_time = iter == prevs.end() ? zero : iter->second;
        }

        return transitions;
    }

    template<std::integral int_t, class map_t, class string_t>
    [[nodiscard]] constexpr auto match_string(const map_t &transitions, const string_t &str) -> int_t
    {
        constexpr int_t zero{};

        auto position = zero;

        for (const auto &cha : str)
        {
            const auto &transit = transitions.at(position);
            const auto iter = transit.find(cha);
            position = iter == transit.end() ? zero : iter->second;
        }

        return position;
    }
} // namespace Standard::Algorithms::Numbers::Inner

namespace Standard::Algorithms::Numbers
{
    // Given a string, find the shortest string to append to get a palindrome.
    // Must find the longest suffix which is a palindrome.
    // E.g. "abb", append "a" to get a palindrome "abba".
    // Time, space O(n).
    template<std::integral int_t, class string_t, class char_t = typename string_t::value_type>
    [[nodiscard]] constexpr auto string_min_appends_to_palindrome(const string_t &str) -> int_t
    {
        constexpr int_t zero{};
        constexpr int_t one = 1;

        const auto size = Standard::Algorithms::Utilities::check_size<int_t>("string size", str.size());

        if (size <= one)
        {
            return zero;
        }

        const auto automaton = Inner::build_string_match_reverted_automaton<int_t, string_t, char_t>(str, size);

        // Now, move forward from the string beginning.
        // The matched substring will be the longest palindrome of the length at least 1.
        const auto position = Inner::match_string<int_t>(automaton, str);

        assert(zero < position && position <= size);

        return static_cast<int_t>(size - position);
    }

    // Slow time O(n*n), space O(1).
    template<std::integral int_t, class string_t>
    [[nodiscard]] constexpr auto string_min_appends_to_palindrome_slow(const string_t &str) -> int_t
    {
        constexpr int_t zero{};
        constexpr int_t one = 1;

        const auto size = Standard::Algorithms::Utilities::check_size<int_t>("string size", str.size());

        if (size <= one)
        {
            return zero;
        }

        for (int_t start{};; ++start)
        {
            assert(start < size);

            if (Standard::Algorithms::Strings::is_palindrome(&str[start], size - start))
            {
                return start;
            }
        }
    }
} // namespace Standard::Algorithms::Numbers
