#pragma once
#include"../Utilities/check_size.h"
#include"suffix_array.h"
#include"ukkonen_suffix_tree.h"

namespace Standard::Algorithms::Strings
{
    // Return a pair {start index, stop index}.

    // Repeated substrings will start at some inner node that has a child in a suffix tree.
    // Time O(n).
    template<class string_t, std::signed_integral int_t = std::int32_t,
        class tree_t = ukkonen_suffix_tree<string_t, int_t>>
    [[nodiscard]] constexpr auto suffix_tree_longest_repeated_substring(const string_t &text) -> std::pair<int_t, int_t>
    {
        const auto size = Standard::Algorithms::Utilities::check_size<int_t>("text size", text.size());

        if (size <= 1)
        {
            return {};
        }

        const tree_t full_tree(text);
        const auto &nodes = full_tree.nodes();

        std::vector<std::pair<int_t, int_t>> que{ { static_cast<int_t>(suffix_tree_root_id), int_t{} } };

        int_t left{};
        int_t right{};

        do
        {
            const auto node_id = que.back().first;
            const auto height = que.back().second;

            assert(int_t{} < node_id && node_id < static_cast<int_t>(nodes.size()));

            que.pop_back();

            const auto &node = nodes[node_id];

            if (const auto is_leaf = !(node.suffix_index < 0); is_leaf)
            {
                const auto len = text_length(node);
                const int_t cand = height - len;

                assert(int_t{} < len && int_t{} <= cand);

                if (right - left < cand)
                {
                    left = node.suffix_index;
                    right = left + cand;
                }

                continue;
            }

            // Inner nodes
            for (const auto &par : node.ch)
            {
                const auto &node_id2 = par.second;

                assert(int_t{} < node_id2 && node_id2 < static_cast<int_t>(nodes.size()));

                const auto &node2 = nodes[node_id2];
                const auto len = text_length(node2);

                que.emplace_back(node_id2, static_cast<int_t>(height + len));
            }
        } while (!que.empty());

        return { left, right };
    }

    // Find maximum in LCP.
    // Time O(n).
    [[nodiscard]] inline auto lcp_longest_repeated_substring(const std::string_view text)
        -> std::pair<std::int32_t, std::int32_t>
    {
        const auto size = Standard::Algorithms::Utilities::check_size<std::int32_t>("text size", text.size());

        std::vector<std::int32_t> sar{};
        std::vector<std::int32_t> lcps{};
        suffix_array_other(text, sar, &lcps);

        std::int32_t start{};
        std::int32_t len{};

        for (std::int32_t index{}; index < size; ++index)
        {
            const auto &longest_substring_length = lcps.at(index);
            if (!(len < longest_substring_length))
            {
                continue;
            }

            start = sar.at(index);
            len = longest_substring_length;

            assert(start < size && len < size);
            assert(start + static_cast<std::int64_t>(len) < size);
        }

        return { start, start + len };
    }

    // Slow time O(n**3).
    [[nodiscard]] inline constexpr auto longest_repeated_substring_slow(const std::string_view text)
        -> std::pair<std::int32_t, std::int32_t>
    {
        const auto size = Standard::Algorithms::Utilities::check_size<std::int32_t>("text size", text.size());

        for (auto len = size; 0 < --len;)
        {
            for (std::int32_t start{}; start < size - len; ++start)
            {
                const auto subs = text.substr(start, len);
                const auto ite = text.find(subs, start + 1);

                if (ite != std::string::npos)
                {
                    return { start, start + len };
                }
            }
        }

        return {};
    }
} // namespace Standard::Algorithms::Strings
