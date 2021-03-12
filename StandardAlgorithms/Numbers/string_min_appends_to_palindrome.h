#pragma once
#include <unordered_map>
#include "../Strings/string_utilities.h"

namespace Privet
{
    namespace Algorithms
    {
        namespace Numbers
        {
            // Compute the min appends to a string to get a palindromes.
            // Must find the longest suffix which is a palindrome.
            // Time O(n), space O(n).
            template<class string_t, class char_t = char, class int_t = int>
            int_t string_min_appends_to_palindrome(const string_t& s)
            {
                const auto size = static_cast<int_t>(s.size());
                if (size <= 1)
                    return 0;

                // DFA, DFM, DFSA, DFSM, DSA, DSM.
                std::vector<std::unordered_map<char, int_t>> transitions(size);
                transitions[0][s.back()] = 1;

                for (int_t i = 1, x = 0; i < size; ++i)
                {
                    const auto& prev = transitions[x];
                    auto& cur = transitions[i];
                    cur = prev;

                    const auto& c = s[size - 1 - i];
                    cur[c] = i + 1;

                    const auto it = prev.find(c);
                    x = it == prev.end() ? int_t(0) : it->second;
                }

                int_t state = 0;
                for (const auto& c : s)
                {
                    const auto& prev = transitions[state];
                    const auto it = prev.find(c);
                    state = it == prev.end() ? int_t(0) : it->second;
                }

                assert(state > 0 && state <= size);
                return size - state;
            }

            // Slow time O(n*n), space O(1).
            template<class string_t, class int_t = int>
            int_t string_min_appends_to_palindrome_slow(const string_t& s)
            {
                const auto size = static_cast<int_t>(s.size());
                if (size <= 1)
                    return 0;

                for (int_t start = 0; ; ++start)
                {
                    assert(start < size);
                    if (Privet::Algorithms::Strings::is_palindrome(&s[start], size - start))
                        return start;
                }
            }
        }
    }
}