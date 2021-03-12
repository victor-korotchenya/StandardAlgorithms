#pragma once
#include <algorithm>
#include <numeric> // iota
#include "string_utilities.h"
#include "ukkonen_suffix_tree.h"

namespace
{
    template<class int_t, class string_t, class string_t2>
    int match_edge(const string_t& pattern, const string_t2& text,
        int_t start, const int_t end, int_t i)
    {
        static_assert(std::is_signed_v<int_t>);

        const auto size = static_cast<int_t>(pattern.size());
        assert(start >= 0 && start <= end && end < static_cast<int_t>(text.size()) && i >= 0 && i < size);

        for (; start <= end && i < size; ++start, ++i)
        {
            if (text[start] != pattern[i])
                return -1;
        }

        const auto r = size == i;
        return r;
    }

    template<class int_t, class node_t>
    void find_all_leaves(const std::vector<node_t>& nodes, std::vector<int_t>& result, int_t node_id)
    {
        std::vector<int_t> q;
        q.push_back(node_id);
#if _DEBUG
        size_t cnt{};
#endif
        do
        {
#if _DEBUG
            assert(++cnt < nodes.size());
#endif
            node_id = q.back();
            assert(node_id > 0);
            q.pop_back();

            const auto& node = nodes[node_id];
            if (node.suffix_index >= 0)
            {// A leaf
                result.push_back(node.suffix_index);
                continue;
            }

            for (const auto& p : node.ch)
                q.push_back(p.second);
        } while (q.size());

        std::reverse(result.begin(), result.end());
    }
}

namespace Privet
{
    namespace Algorithms
    {
        namespace Strings
        {
            // Return the first starting position of the pattern in the text,
            // -1 if not found.
            template<class int_t, class node_t, class string_t, class string_t2>
            int_t find_string(const std::vector<node_t>& nodes, const string_t& pattern, const string_t2& text)
            {
                static_assert(std::is_signed_v<int_t>);
                assert(nodes.size() && text.size());

                const auto size = static_cast<int_t>(pattern.size());
                assert(size >= 0);
                if (!size)
                    return {};

                if (size > static_cast<int_t>(text.size()))
                    return -int_t(1);

                int_t i = {}, node_id = suffix_tree_root_id;
                do
                {
                    assert(node_id > 0 && node_id < static_cast<int_t>(nodes.size()));
                    const auto& node = nodes[node_id];

                    if (node_id != suffix_tree_root_id)
                    {
                        const auto m = match_edge<int_t>(pattern, text, node.b, node.e, i);
                        if (m < 0)
                            break;

                        if (m > 0)
                        {
                            const auto index_in_text = static_cast<int_t>(node.b - i);

                            assert(index_in_text >= 0 && index_in_text < static_cast<int_t>(text.size()));

                            return index_in_text;
                        }

                        // A partial match.
                        const auto length = node.text_length();
                        i += length;
                        assert(i < size);
                    }

                    const auto& c = pattern[i];
                    node_id = find_child<int_t>(node.ch, c);
                } while (node_id);

                return -int_t(1);
            }

            template<class int_t, class node_t, class string_t, class string_t2>
            void find_all_strings(const std::vector<node_t>& nodes, const string_t& pattern, const string_t2& text,
                std::vector<int_t>& result,
                const bool sort_result = true)
            {
                static_assert(std::is_signed_v<int_t>);
                assert(nodes.size() && text.size());

                const auto size = static_cast<int_t>(pattern.size());
                assert(size >= 0);
                if (!size)
                {
                    result.resize(size);
                    std::iota(result.begin(), result.end(), int_t());
                    return;
                }

                result.clear();
                if (size > static_cast<int_t>(text.size()))
                    return;

                int_t i = {}, node_id = suffix_tree_root_id;
                do
                {
                    assert(node_id > 0 && node_id < static_cast<int_t>(nodes.size()));
                    const auto& node = nodes[node_id];

                    if (node_id != suffix_tree_root_id)
                    {
                        const auto m = match_edge<int_t>(pattern, text, node.b, node.e, i);
                        if (m < 0)
                            return;

                        if (m > 0)
                        {
                            find_all_leaves<int_t, node_t>(nodes, result, node_id);
                            assert(result.size());
                            if (sort_result)
                                std::sort(result.begin(), result.end());
#if _DEBUG
                            const auto index_in_text = static_cast<int_t>(node.b - i);
                            assert(index_in_text >= 0 && index_in_text < static_cast<int_t>(text.size()) && result.end() != std::find(result.begin(), result.end(), index_in_text));
#endif
                            return;
                        }

                        // A partial match.
                        const auto length = node.text_length();
                        i += length;
                        assert(i < size);
                    }

                    const auto& c = pattern[i];
                    node_id = find_child<int_t>(node.ch, c);
                } while (node_id);
            }

            // Repeated substrings will start at some inner node that has one child for a character in a suffix tree.
            template<class string_t, class int_t = int, class tree_t = ukkonen_suffix_tree<string_t, int_t>>
            std::pair<int_t, int_t> longest_repeated_substring(const string_t& text)
            {
                static_assert(std::is_signed_v<int_t>);

                const auto size = static_cast<int_t>(text.size());
                if (size <= 1)
                    return {};

                const tree_t full_tree(text);
                const auto& nodes = full_tree.nodes;

                std::vector<std::pair<int_t, int_t>> q;
                q.emplace_back(int_t(suffix_tree_root_id), int_t());

                int_t left = {}, right = {};
                do
                {
                    const auto node_id = q.back().first,
                        height = q.back().second;
                    assert(node_id > 0 && node_id < static_cast<int_t>(nodes.size()));
                    q.pop_back();

                    const auto& node = nodes[node_id];
                    if (node.suffix_index >= 0)
                    {// Leaf
                        const int_t len = node.text_length(),
                            cand = height - len;
                        assert(len > 0 && cand >= 0);

                        if (right - left < cand)
                        {
                            left = node.suffix_index;
                            right = left + cand;
                        }

                        continue;
                    }

                    for (const auto& p : node.ch)
                    {//Inner nodes
                        const auto& node_id2 = p.second;
                        assert(node_id2 > 0 && node_id2 < static_cast<int_t>(nodes.size()));

                        const auto& node2 = nodes[node_id2];
                        const auto len = node2.text_length();

                        q.emplace_back(node_id2, height + len);
                    }
                } while (q.size());

                return { left, right };
            }

            template<class int_t, class string_t, class node_t>
            std::vector<int_t> build_suffix_array(const string_t& text, const std::vector<node_t>& nodes)
            {
                static_assert(std::is_signed_v<int_t>);
                assert(nodes.size() >= 2);

                const auto size = static_cast<int_t>(text.size());
                RequirePositive(size, "size");

                std::vector<int_t> result(size, -1);
                std::vector<int_t> q{ suffix_tree_root_id };
                int_t idx{};

                do
                {
                    const auto node_id = q.back();
                    assert(node_id > 0);
                    q.pop_back();

                    const auto& node = nodes[node_id];

                    if (node.suffix_index >= 0)
                    {// Leaf
                        result[idx++] = node.suffix_index;
                        continue;
                    }

                    for (const auto& p : node.ch)
                        q.push_back(p.second);
                } while (q.size());

                std::reverse(result.begin(), result.end());
                return result;
            }

            template<class int_t,
                class string_t, class pair_t, class node_t>
                signed char lcs_dfs(const std::vector<node_t>& nodes, const int_t size1, const int_t node_id, int_t& start, int_t& height_max, const int_t height = {})
            {
                const auto& node = nodes[node_id];

                constexpr signed char inner_node_id = -1,
                    first_id = 0,
                    second_id = 1,
                    shared_inner_id = 2;
                assert(node.suffix_index == static_cast<int_t>(inner_node_id));

                auto suffix_index = inner_node_id;

                for (const auto& p : node.ch)
                {
                    const auto& node_id2 = p.second;
                    const auto& node2 = nodes[node_id2];
                    const auto height2 = height + node2.text_length();

                    const auto cal = node2.suffix_index >= 0 ? (// A leaf
                        node2.suffix_index <= size1 // Is the first string
                        ? first_id : second_id)
                        : lcs_dfs<int_t, string_t, pair_t, node_t>(nodes, size1, node_id2, start, height_max, height2);

                    if (suffix_index == inner_node_id)
                        suffix_index = cal;
                    else if (suffix_index == first_id && cal == second_id ||
                        suffix_index == second_id && cal == first_id ||
                        suffix_index == shared_inner_id)
                    {
                        suffix_index = shared_inner_id;
                        if (height_max < height)
                        {
                            height_max = height;
                            start = node.e - height + 1;
                        }
                    }
                }

                return suffix_index;
            }

            // Find the longest common substring of 2 strings: start and stop in each string.
            // If nothing is found, return 0s.
            // Time, space O(m+n).
            template<class int_t,
                class string_t, class pair_t, class char_t = typename string_t::value_type,
                char_t separator = '~', char_t separator2 = '#',
                class tree_t = ukkonen_suffix_tree<string_t, int_t, char_t, separator>>
                void longest_common_substring(const string_t& x, const string_t& y,
                    pair_t& result)
            {
                static_assert(std::is_signed_v<int_t> && separator != separator2);

                result = {};
                const auto size1 = static_cast<int_t>(x.size());
                const auto size2 = static_cast<int_t>(y.size());
                if (!size2 || !size1)
                    return;

#if _DEBUG
                for (const auto& str : { x, y })
                    for (const auto& c : { separator, separator2 })
                    {// Separators must not appear in either string.
                        const auto f = std::find(str.begin(), str.end(), c);
                        assert(f == str.end());
                    }
#endif
                constexpr auto use_separator = true;
                const string_t text = x + separator + y + separator2;
                const tree_t full_tree(text, use_separator);
                const auto& nodes = full_tree.nodes;

                using node_t = typename tree_t::node_alias_t;
                int_t start = {}, height = {};

                lcs_dfs<int_t, string_t, pair_t, node_t>(nodes, size1, suffix_tree_root_id, start, height);

                auto& p = start < size1 ? result.first : result.second;
                p.first = start;
                p.second = start + height;
            }

            // Find the longest palindromic substring in O(n) time, space via
            // the longest common substring of s and its reverse r when
            // both have the same position.
            // For sample, s = "abacdfgdcaba", reversed s = "abacdgfdcaba",
            // their LCS is "abacd" which is not a palindrome.
            //
            // See also longest_palindromic_substring_manacher
            template<class int_t,
                class string_t, class char_t = typename string_t::value_type,
                class tree_t = ukkonen_suffix_tree<string_t, int_t, char_t>>
                std::pair<int, int> longest_palindromic_substring(const string_t& s)
            {
                const auto size = static_cast<int_t>(s.size());
                if (size <= 1)
                    return { 0, size };

                // const tree_t full_tree(s);
                // const auto &nodes = full_tree.nodes;

                // using node_t = typename tree_t::node_alias_t;
                int_t start = {}, height = {};

                // todo: p2. end.

                return { start, start + height };
            }
        }
    }
}