#pragma once
// "ukkonen_suffix_tree_utilities.h"
#include"string_utilities.h"
#include"ukkonen_suffix_tree.h"
#include<algorithm>

namespace Standard::Algorithms::Strings::Inner
{
    template<std::signed_integral int_t, class string_t, class string_t2>
    [[nodiscard]] constexpr auto match_edge(
        const string_t &pattern, const string_t2 &text, int_t begin1, const int_t end2, int_t index) -> std::int32_t
    {
        const auto size = static_cast<int_t>(pattern.size());

        assert(!(begin1 < int_t{}) && begin1 <= end2 && end2 < static_cast<int_t>(text.size()));
        assert(!(index < int_t{}) && index < size);

        for (; begin1 <= end2 && index < size; ++begin1, ++index)
        {
            if (text[begin1] != pattern[index])
            {
                return -1;
            }
        }

        auto has = size == index;
        return has;
    }

    inline constexpr void enque_second(const auto &source, auto &dest)
    {
        std::transform(source.cbegin(), source.cend(), std::back_inserter(dest),
            [] [[nodiscard]] (const auto &par)
            {
                return par.second;
            });
    }

    template<class int_t, class node_t>
    constexpr void find_all_leaves(const std::vector<node_t> &nodes, std::vector<int_t> &result, int_t node_id)
    {
        constexpr int_t zero{};

        std::vector<int_t> que{ node_id };
        [[maybe_unused]] std::size_t cnt{};

        do
        {
            if constexpr (::Standard::Algorithms::is_debug)
            {
                ++cnt;
                assert(cnt < nodes.size());
            }

            node_id = que.back();
            assert(zero < node_id);

            que.pop_back();

            const auto &node = nodes[node_id];

            if (!(node.suffix_index < zero))
            {// A leaf
                result.push_back(node.suffix_index);
                continue;
            }

            enque_second(node.ch, que);
        } while (que.size());

        std::reverse(result.begin(), result.end());
    }
} // namespace Standard::Algorithms::Strings::Inner

namespace Standard::Algorithms::Strings
{
    // Return the first starting position of the pattern in the text,
    // -1 if not found.
    template<std::signed_integral int_t, class node_t, class string_t, class string_t2>
    [[nodiscard]] constexpr auto find_string(
        const std::vector<node_t> &nodes, const string_t &pattern, const string_t2 &text) -> int_t
    {
        constexpr int_t zero{};

        const auto size = static_cast<int_t>(pattern.size());
        assert(!nodes.empty() && !text.empty() && !(size < zero));

        if (size == zero)
        {
            return {};
        }

        if (static_cast<int_t>(text.size()) < size)
        {
            return -static_cast<int_t>(1);
        }

        int_t index{};
        auto node_id = suffix_tree_root_id;

        do
        {
            assert(zero < node_id && node_id < static_cast<int_t>(nodes.size()));

            const auto &node = nodes[node_id];

            if (node_id != suffix_tree_root_id)
            {
                const auto mat = Inner::match_edge<int_t>(pattern, text, node.b, node.e, index);

                if (mat < zero)
                {
                    break;
                }

                if (zero < mat)
                {
                    const auto index_in_text = static_cast<int_t>(node.b - index);

                    assert(!(index_in_text < zero) && index_in_text < static_cast<int_t>(text.size()));

                    return index_in_text;
                }

                // A partial match.
                const auto length = text_length(node);
                index += length;
                assert(index < size);
            }

            const auto &cha = pattern[index];
            node_id = find_child<int_t>(node.ch, cha);
        } while (node_id != zero);

        return -static_cast<int_t>(1);
    }

    template<std::signed_integral int_t, class node_t, class string_t, class string_t2>
    constexpr void find_all_strings(
        const std::vector<node_t> &nodes, const string_t &pattern, const string_t2 &text, std::vector<int_t> &result)
    {
        constexpr int_t zero{};

        const auto size = static_cast<int_t>(pattern.size());
        assert(!nodes.empty() && !text.empty() && !(size < zero));

        result.clear();

        if (size == zero || static_cast<int_t>(text.size()) < size)
        {
            return;
        }

        int_t index{};
        int_t node_id = suffix_tree_root_id;

        do
        {
            assert(zero < node_id && node_id < static_cast<int_t>(nodes.size()));

            const auto &node = nodes[node_id];

            if (node_id != suffix_tree_root_id)
            {
                const auto mat = Inner::match_edge<int_t>(pattern, text, node.b, node.e, index);

                if (mat < zero)
                {
                    return;
                }

                if (zero < mat)
                {
                    Inner::find_all_leaves<int_t, node_t>(nodes, result, node_id);
                    assert(!result.empty());

                    if constexpr (::Standard::Algorithms::is_debug)
                    {
                        const auto index_in_text = static_cast<int_t>(node.b - index);

                        assert(!(index_in_text < zero) && index_in_text < static_cast<int_t>(text.size()) &&
                            result.cend() != std::find(result.cbegin(), result.cend(), index_in_text));
                    }

                    return;
                }

                // A partial match.
                const auto length = text_length(node);
                index += length;
            }

            assert(index < size);

            const auto &cha = pattern[index];
            node_id = find_child<int_t>(node.ch, cha);
        } while (node_id != zero);
    }

    template<std::signed_integral int_t, class string_t, class node_t>
    [[nodiscard]] constexpr auto build_suffix_array(const string_t &text, const std::vector<node_t> &nodes)
        -> std::vector<int_t>
    {
        constexpr int_t zero{};

        assert(1U < nodes.size());

        const auto size = static_cast<int_t>(text.size());
        require_positive(size, "size");

        std::vector<int_t> result(size, -1);
        std::vector<int_t> que{ suffix_tree_root_id };
        int_t idx{};

        do
        {
            const auto node_id = que.back();
            assert(zero < node_id);

            que.pop_back();

            const auto &node = nodes[node_id];

            if (const auto is_leaf = !(node.suffix_index < zero); is_leaf)
            {
                result[idx++] = node.suffix_index;
                continue;
            }

            Inner::enque_second(node.ch, que);
        } while (!que.empty());

        std::reverse(result.begin(), result.end());
        return result;
    }

    template<class int_t, class string_t, class pair_t, class node_t>
    constexpr auto lcf_dfs(const int_t size1, const std::vector<node_t> &nodes, const int_t node_id, int_t &start,
        int_t &height_max, const int_t height = {}) -> std::int8_t
    {
        constexpr int_t zero{};
        constexpr std::int8_t inner_node_id = -1;
        constexpr std::int8_t first_id = 0;
        constexpr std::int8_t second_id = 1;
        constexpr std::int8_t shared_inner_id = 2;

        const auto &node = nodes[node_id];
        assert(node.suffix_index == static_cast<int_t>(inner_node_id));

        auto suffix_index = inner_node_id;

        for (const auto &par : node.ch)
        {
            const auto &node_id2 = par.second;
            const auto &node2 = nodes[node_id2];
            const auto height2 = height + text_length(node2);

            const auto cal = !(node2.suffix_index < zero)
                ? ( // A leaf
                      node2.suffix_index <= size1 // Is the first string
                          ? first_id
                          : second_id)
                : lcf_dfs<int_t, string_t, pair_t, node_t>(size1, nodes, node_id2, start, height_max, height2);

            if (suffix_index == inner_node_id)
            {
                suffix_index = cal;
            }
            else if ((suffix_index == first_id && cal == second_id) || (suffix_index == second_id && cal == first_id) ||
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

    // Find the LCF (longest common factor or longest common substring) of 2 strings: start and stop in each string.
    // If nothing is found, return 0s.
    // Time, space O(m+n).
    template<std::signed_integral int_t, class string_t, class pair_t, class char_t = typename string_t::value_type,
        char_t separator = '~', char_t separator2 = '#',
        class tree_t = ukkonen_suffix_tree<string_t, int_t, char_t, separator>>
    requires(separator != separator2)
    constexpr void longest_common_factor(const string_t &one, const string_t &two, pair_t &result)
    {
        result = {};

        constexpr int_t zero{};

        const auto size1 = static_cast<int_t>(one.size());
        const auto size2 = static_cast<int_t>(two.size());
        if (size2 == zero || size1 == zero)
        {
            return;
        }

        if constexpr (::Standard::Algorithms::is_debug)
        {
            for (const auto &str : { one, two })
            {
                for (const auto &cha : { separator, separator2 })
                {// Separators must not appear in either string.
                    const auto fin = std::find(str.cbegin(), str.cend(), cha);

                    assert(fin == str.cend());
                }
            }
        }

        constexpr auto use_separator = true;

        const string_t text = one + separator + two + separator2;

        const tree_t full_tree(text, use_separator);
        const auto &nodes = full_tree.nodes();

        using node_t = typename tree_t::node_t;

        int_t start{};
        int_t height{};

        lcf_dfs<int_t, string_t, pair_t, node_t>(size1, nodes, suffix_tree_root_id, start, height);

        auto &ppp = start < size1 ? result.first : result.second;
        ppp.first = start;
        ppp.second = start + height;
    }

    // Find the longest palindromic factor or substring in O(n) time, space via
    // the longest common factor of s and its reverse r when
    // both have the same position.
    // For sample, s = "abacdfgdcaba", reversed s = "abacdgfdcaba",
    // their LCF is "abacd" which is not a palindrome.
    //
    // See also longest_palindromic_factor_manacher
    template<class int_t, class string_t, class char_t = typename string_t::value_type,
        class tree_t = ukkonen_suffix_tree<string_t, int_t, char_t>>
    [[nodiscard]] constexpr auto longest_palindromic_factor(const string_t &str) -> std::pair<int_t, int_t>
    {
        const auto size = static_cast<int_t>(str.size());
        if (size <= static_cast<int_t>(1))
        {
            return { int_t{}, size };
        }

        // const tree_t full_tree(str);
        // const auto &nodes = full_tree.nodes;

        // using node_t = typename tree_t::node_t;
        int_t start{};
        int_t height{};

        // todo(p2): finish.

        return { start, start + height };
    }
} // namespace Standard::Algorithms::Strings
