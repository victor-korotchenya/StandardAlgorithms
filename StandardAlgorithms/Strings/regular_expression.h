#pragma once
#include <string_view>

namespace Privet::Algorithms::Strings
{
    // "*" matches 0 or more characters,
    // "?" matches 1 character.
    //The search is case sensitive.
    // todo: p2. bool is_wildcard_match(const std::string_view& pattern, const std::string_view& text);

    // Non-linear n*m time, O(|pattern|) space.
    bool is_wildcard_match_slow_still(const std::string_view& pattern, const std::string_view& text);

    // Slow O(|pattern|*|text|) time, space.
    bool is_wildcard_match_slow(const std::string_view& pattern, const std::string_view& text);
}