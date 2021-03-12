#pragma once
#include"../Utilities/is_debug.h"
#include<cstddef>
#include<stdexcept>
#include<string>
#include<vector>

namespace Standard::Algorithms::Trees
{
    struct prefix_node final
    {
        std::string value{};
        bool has_word{};
        std::vector<prefix_node> children{};
    };

    [[nodiscard]] inline constexpr auto first_symbol(const prefix_node &node) noexcept(false) -> char
    {
        if constexpr (::Standard::Algorithms::is_debug)
        {
            if (node.value.empty()) [[unlikely]]
            {
                throw std::runtime_error("The value is empty in first_symbol.");
            }
        }

        return node.value.at(0);
    }

    [[nodiscard]] inline constexpr auto operator<(const prefix_node &one, const prefix_node &two) noexcept(false)
        -> bool
    {
        // The nodes may differ by the first symbol only.
        auto result = first_symbol(one) < first_symbol(two);
        return result;
    }

    struct find_node_result final
    {
        const prefix_node *node{};

        // The beginning of the last used position in the "search prefix".
        std::size_t word_position{};

        // How many symbols were partially matched in the "Node".
        std::size_t matched_symbol_count{};

        bool is_partial_match{};
        bool has_match{}; // todo(p4): enum class to save 1 byte?
    };

    inline constexpr void validate(const find_node_result &result)
    {
        if (nullptr == result.node) [[unlikely]]
        {
            throw std::runtime_error("The find_node_result.Node is null.");
        }

        if (result.is_partial_match && 0U == result.matched_symbol_count) [[unlikely]]
        {
            throw std::runtime_error("The find_node_result.MatchedCount is 0 for a partial match.");
        }
    }

    // Auto-complete tree (Trie) allows searching for words starting from the given prefix.
    // The search is case-sensitive.
    struct prefix_tree final
    {
        [[nodiscard]] inline constexpr auto is_empty() const noexcept -> bool
        {
            auto result = Roots.empty();
            return result;
        }

        void clear() noexcept;

        // The words are case-sensitive.
        // Duplicates are ignored.
        void add_word(std::string &&word) &;

        // Search for the words starting from a given case-sensitive prefix.
        void search(const std::string &prefix, std::vector<std::string> &result,
            std::size_t count_limit = 0U) const &noexcept(false);

private:
        std::vector<prefix_node> Roots{};
    };
} // namespace Standard::Algorithms::Trees
