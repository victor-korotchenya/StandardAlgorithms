#pragma once
#include"pattern_text_view.h"
#include<string_view>

namespace Standard::Algorithms::Strings
{
    // Matches any 1 character.
    inline constexpr auto question = '?';

    // An asterisk, also a star or a gap, matches 0 or more characters.
    inline constexpr auto star = '*';

    // Whether a text is fully matched by a pattern using wildcards '*' and/or '?'.
    // A plain char/character/symbol is neither '?' nor '*'.
    // When a partial match is needed, wrap the pattern with '*' on both sides.
    // The search is case sensitive.
    // The patterns "b", "*b", "b*", "Abc" do not match the text "abc",
    // whereas "*b*", "*b?" do.

    // Time O(|pattern| + |text|), space O(|pattern|).
    // todo(p3): [[nodiscard]] auto is_wildcard_match(
    // const pattern_text_view<const std::string_view, const std::string_view> &pat_text) -> bool;

    // No star - match each symbol, total time O(n).
    // Else split the pattern into sections;
    // match both the prefix and the suffix if either is not empty;
    // match each section in the middle between the first and last stars.
    // - Of course, a plain pattern means KMP may be utilized.
    // - Also if a pattern has only '?', sizes are comparable in O(1).
    // Naive slow time O(|pattern| * |text|), space O(|pattern|).
    // Note. KMP won't help here because "1?" and "?2" match in general,
    // but they don't e.g. in "1?2" as ? must equal both "1" and "2" simultaneously which is impossible.
    [[nodiscard]] auto is_wildcard_match_naive(
        const pattern_text_view<const std::string_view, const std::string_view> &pat_text) -> bool;

    // Build an automaton.
    // Main problem: there could be O(|pattern|) statuses for each text character.
    // Still slow time O(|pattern| * |text| * std::unordered_set time), space O(|pattern|).
    [[nodiscard]] auto is_wildcard_match_slow_still(
        const pattern_text_view<const std::string_view, const std::string_view> &pat_text) -> bool;

    // Slow time O(|pattern| * |text|), large space O(|pattern| * |text|).
    // The |text| cannot be too large because recursion is used (could be fixed).
    // Also, the used space could be decreased down to min(|pattern|, |text|).
    // Nevertheless, it could run faster than filling the whole 2D buffer
    // since not all cells are either reachable or needed in case of success.
    [[nodiscard]] auto is_wildcard_match_slow(
        const pattern_text_view<const std::string_view, const std::string_view> &pat_text) -> bool;
} // namespace Standard::Algorithms::Strings
