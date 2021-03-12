#pragma once
#include <algorithm>
#include <cassert>
#include <forward_list>
#include <map>
#include <unordered_map>
#include <unordered_set>
#include <utility>
#include <vector>
#include "../Utilities/ExceptionUtilities.h"
#include "../Numbers/Arithmetic.h"

namespace
{
    template <typename string_t, typename char_t>
    void fallback_kmp(const string_t& pattern, const std::vector<int>& table, const char_t& c, int& j)
    {
        while (0 < j && c != pattern[j])
            j = table[j];
    }
}

namespace Privet::Algorithms::Strings
{
    // Given a pattern string p of length m,
    // return a table t where for i=[1..m-1],
    // t[i] is the prefix and the longest proper suffix of substring p[0..i].
    // Samples:
    //  "ABCDABD"
    //  01234567 - indexes
    //  00000120 - table_clr
    //
    //  "AAAAAAA"
    //  01234567 - indexes
    //  00123456 - table_clr
    template<typename string_t>
    std::vector<int> knuth_morris_pratt_prep(const string_t& pattern)
    {
        RequirePositive(pattern.size(), "pattern size");

        const auto m = static_cast<int>(pattern.size());

        // todo: p1. use result(m);
        std::vector<int> result(m + 1ll);

        for (auto i = 1, j = 0; i < m; ++i)
        {
            fallback_kmp<string_t, decltype(pattern[i])>(pattern, result, pattern[i], j);

            if (pattern[i] == pattern[j])
                ++j;

            result[i + 1ll] = j;
        }

        return result;
    }

    template<typename string_t>
    void knuth_morris_pratt(const string_t& pattern, const std::vector<int>& table, const string_t& text,
        std::vector<int>& result)
    {
        assert(pattern.size() + 1 == table.size());
        RequirePositive(pattern.size(), "pattern size");
        RequirePositive(text.size(), "text size");

        result.clear();

        const auto n = static_cast<int>(text.size()),
            m = static_cast<int>(pattern.size());
        if (n < m)
            return;

        for (auto i = 0, j = 0; i < n; ++i)
        {
            fallback_kmp<string_t, decltype(pattern[i])>(pattern, table, text[i], j);

            if (text[i] == pattern[j] && m == ++j)
            {
                result.push_back(i + 1 - m);
                j = table[m];
                assert(j >= 0 && j < m);
            }
        }
    }
}