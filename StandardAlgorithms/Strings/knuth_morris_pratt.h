#pragma once
#include"../Utilities/require_utilities.h"
#include<vector>

namespace Standard::Algorithms::Strings::Inner
{
    template<class string_t, class char_t>
    constexpr void knuth_morris_pratt_fallback(
        const string_t &pattern, const std::vector<std::int32_t> &table, const char_t &cha, std::int32_t &matches)
    {
        assert(0 <= matches && static_cast<std::size_t>(matches) < pattern.size());
        assert(pattern.size() == table.size());

        while (0 < matches && cha != pattern[matches])
        {
            const auto &pre = table[matches - 1];

            assert(0 <= pre && pre < matches && static_cast<std::size_t>(pre) < pattern.size());

            matches = pre;
        }
    }
} // namespace Standard::Algorithms::Strings::Inner

namespace Standard::Algorithms::Strings
{
    // Knuth, Morris, Pratt. Given a pattern string of length pat_size,
    // return a table where for index=[1..pat_size-1],
    // table[index] is the max length of the prefix such that
    // the prefix is equal to the proper suffix, both of the pattern[:index] substring.
    // Samples:
    //  "AAAAAAA", size = 7
    //  0123456 - indexes
    //  0123456 - table
    // Note. The first 1 at table index 1 means (pat[0] == pat[1]).
    //
    //  "ABCD", size = 4
    //  0123 - indexes
    //  0000 - table, no matches are here.
    //
    // Appending "A" to "ABCD",
    //  "ABCDA", size = 5
    //  01234 - indexes
    //  00001 - table, the last entry is (0+1) = 1
    // because (table[3] == 0) and (pat[0] == pat[4] == 'A'),
    // so the prefix gets longer by one symbol.
    //
    //  "ABCDABD", size = 7
    //  0123456 - indexes
    //  0000120 - table
    //
    //  "Ab_Ab1Ab_Ab Ab_Ab1Ab_Ab_", size = 24
    //  0123456789AbCdEf01234567 - indexes
    //  000120123450123456789Ab3 - table
    // Note.Before the last '_', "Ab_Ab1Ab_Ab" of length 0xb is the longest prefix/suffix.
    // Processing the last '_', as ('_' != ' '), backtrack to the previous longest proper prefix "Ab_Ab" of size 5.
    // The next char at 5 is '1' and ('_' != '1') must backtrack again to "Ab" of size 2.
    // Now, the next char at 2 is '_', so we are lucky to have a match of size 2+1,
    // where the previous 2 characters have already been computed:
    // there is no need to waste precious resources to compute it again.
    //
    // See also "z_array.h".
    template<class string_t>
    [[nodiscard]] constexpr auto knuth_morris_pratt_prep(const string_t &pattern) -> std::vector<std::int32_t>
    {
        require_positive(pattern.size(), "pattern size");

        const auto size = static_cast<std::int32_t>(pattern.size());

        std::vector<std::int32_t> table(size);

        for (std::int32_t index = 1, matches{}; index < size; ++index)
        {
            const auto &cha = pattern[index];

            Inner::knuth_morris_pratt_fallback<string_t>(pattern, table, cha, matches);

            if (const auto &prefix_next = pattern[matches]; cha == prefix_next)
            {
                ++matches;

                assert(0 < matches && matches < size);
            }

            table[index] = matches;
        }

        return table;
    }

    // When trying to match the text "ababaC.." with the pattern
    // "ababab" of size 6,
    // 012345 - indexes,
    // 001234 - fallback table,
    // eventually the text index reaches 5, and the matches variable becomes also 5.
    // Here the text symbol 'C' does not match 'b', that is text[index] != pattern[matches].
    // Since 0 < matches, it is advisable to step back by 1:
    // matches is assigned matches = table[matches-1] = 3.
    // Then pattern[3], which is 'b' again, is compared against 'C'.
    // After 1 more iteration, matches becomes 1; and finally 0.
    // In these step back cases, it is known in advance that such comparisons will fail
    // as the previous chars formed a border, and the next character is same 'b' for both the prefix and the suffix.
    // Thus, 000004 is the better fallback table, producing the same results.
    // As for the pattern "ababaa" with the original fallback table 001231, and the improved one 000031,
    // the border "aba" of length 3 has 2 different continuations:
    // the prefix "abab" and the substring "abaa".
    // So, the 3 at index 4 must not change. Consider a sample text "abababaa".
    template<class string_t>
    constexpr void improve_kmp_table(const string_t &pattern, std::vector<std::int32_t> &table)
    {
        assert(pattern.size() == table.size());

        auto size_minus_1 = table.size();

        if (size_minus_1-- // There is no next character at the last position.
            <= 2U)
        {
            return;
        }

        for (std::size_t index = 1; index <= size_minus_1; ++index)
        {
            auto &matches = table[index];
            if (0 == matches)
            {
                continue;
            }

            assert(0 < matches && static_cast<std::size_t>(matches) <= index);

            const auto &prefix_next = pattern[matches];
            const auto &suffix_next = pattern[index + 1U];

            if (prefix_next == suffix_next)
            {
                const auto &pre = table[matches - 1];
                assert(0 <= pre && pre <= matches);

                matches = pre;
            }
        }
    }

    template<class string_t>
    constexpr void better_knuth_morris_pratt_prep(const string_t &pattern, std::vector<std::int32_t> &table)
    {
        const auto size_minus_1 = static_cast<std::int32_t>(require_positive(pattern.size(), "pattern size") - 1);

        table.resize(size_minus_1 + 1LL);
        table.at(0) = 0;

        for (std::int32_t index{}, matches{}; ++index <= size_minus_1;)
        {
            const auto &cha = pattern[index];

            Inner::knuth_morris_pratt_fallback<string_t>(pattern, table, cha, matches);

            if (const auto &prefix_curr = pattern[matches]; cha == prefix_curr)
            {
                ++matches;

                assert(0 < matches && matches <= size_minus_1);
            }

            auto &tab = table[index];
            if (0 == matches || index == size_minus_1) // Last character has no next.
            {
                tab = matches;
                continue;
            }

            const auto &prefix_next = pattern[matches];
            const auto &suffix_next = pattern[index + 1];

            if (prefix_next == suffix_next)
            {
                const auto &pre = table[matches - 1];
                assert(0 <= pre && pre <= matches);

                tab = pre;
                continue;
            }

            tab = matches;
        }
    }

    template<class string_t>
    constexpr void knuth_morris_pratt(const string_t &pattern, const std::vector<std::int32_t> &table,
        const string_t &text, std::vector<std::int32_t> &result)
    {
        assert(pattern.size() == table.size());

        require_positive(pattern.size(), "pattern size");
        require_positive(text.size(), "text size");

        result.clear();

        const auto text_size = static_cast<std::int32_t>(text.size());
        const auto pat_size = static_cast<std::int32_t>(pattern.size());
        if (text_size < pat_size)
        {
            return;
        }

        for (std::int32_t index{}, matches{}; index < text_size; ++index)
        {
            const auto &tex = text[index];

            Inner::knuth_morris_pratt_fallback<string_t>(pattern, table, tex, matches);

            const auto &prefix_next = pattern[matches];

            if (tex == prefix_next && pat_size == ++matches)
            {
                result.push_back(index + 1 - pat_size);
                matches = table[pat_size - 1];

                assert(!(matches < 0) && matches < pat_size);
            }
        }
    }
} // namespace Standard::Algorithms::Strings
