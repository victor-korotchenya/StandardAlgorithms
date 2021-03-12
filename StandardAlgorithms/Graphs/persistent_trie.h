#pragma once
#include"../Utilities/require_utilities.h"
#include<unordered_map>
#include<unordered_set>
#include<vector>

namespace Standard::Algorithms::Trees
{
    // Given a set of strings, answer m queries:
    // - Whether a non-empty string s exists in an existing version v.
    // - Add a non-empty string s, creating a new version if s is new indeed.
    // Versions are 0 based.
    //
    // todo(p3): change a past version.
    template<class string_t = std::string, class char_t = typename string_t::value_type,
        class map_t = std::unordered_map<char_t, std::size_t>>
    struct persistent_trie final
    {
        //   Version 2, the suffix 'w' means word ending.
        //    Root_v2
        //   /  \;
        //  A    C
        //  |    |;
        //  Bw   Dw
        //
        //
        // Add "CE", create a new root.
        //   Version 2          Version 3
        //    Root_v2            Root_v3
        //   /  \               /       \;
        //  A    C     pointer_to_A      C
        //  |    |                     /   \;
        //  Bw   Dw        pointer_to_D    Ew
        constexpr persistent_trie()
            : Nodes(2)
            , Roots(1, 1)
        {
        }

        [[nodiscard]] constexpr auto max_version() const noexcept -> std::size_t
        {
            return Roots.size() - 1U;
        }

        constexpr auto add(const string_t &str) -> bool
        {
            throw_if_empty("added string", str);

            const auto nodes_size0 = Nodes.size();
            auto node_id = nodes_size0;
            {
                const auto old_node_id = Roots.back();
                Roots.push_back(nodes_size0);

                try
                {
                    Nodes.push_back(Nodes.at(old_node_id));
                }
                catch (...)
                {
                    Roots.pop_back();
                    throw;
                }
            }

            const auto str_size = str.size();
            std::size_t index{};

            auto has_add = false;
            for (;;)
            {
                assert(index < str_size && 0U < node_id && node_id <= Nodes.size());

                const auto new_child_id = Nodes.size();
                Nodes.push_back({});

                auto &node = Nodes.at(node_id);

                const auto &cha = str[index];
                const auto iter = node.children.find(cha);

                if (iter == node.children.end())
                {
                    has_add = true;
                }
                else
                {
                    const auto &old_child_id = iter->second;
                    const auto &old_child = Nodes.at(old_child_id);
                    Nodes.back() = old_child;
                }

                node.children[cha] = new_child_id;

                if (++index < str_size)
                {
                    node_id = new_child_id;
                    continue;
                }

                auto &child = Nodes[new_child_id];
                if (has_add || !child.is_word_end)
                {
                    return child.is_word_end = true;
                }

                // Cancel add.
                Nodes.resize(nodes_size0);
                Roots.pop_back();
                return false;
            }
        }

        [[nodiscard]] constexpr auto contains(const string_t &str, const std::size_t version) const -> bool
        {
            throw_if_empty("searched string", str);
            require_less_equal(version, max_version(), "version");

            const auto str_size = str.size();
            std::size_t index{};

            auto node_id = Roots.at(version);
            for (;;)
            {
                assert(index < str_size && 0U < node_id);

                const auto &node = Nodes.at(node_id);
                const auto &cha = str[index];
                const auto iter = node.children.find(cha);

                if (iter == node.children.end())
                {
                    return false;
                }

                const auto &child_id = iter->second;
                if (++index < str_size)
                {
                    node_id = child_id;
                    continue;
                }

                const auto &child = Nodes.at(child_id);
                return child.is_word_end;
            }
        }

private:
        struct node_t final
        {
            map_t children{};
            bool is_word_end{};
        };

        std::vector<node_t> Nodes;
        std::vector<std::size_t> Roots;
    };

    // Slow.
    template<class string_t = std::string, class char_t = typename string_t::value_type>
    struct persistent_trie_slow final
    {
        constexpr persistent_trie_slow()
            : Versions(1)
        {
        }

        [[nodiscard]] constexpr auto max_version() const noexcept -> std::size_t
        {
            return Versions.size() - 1U;
        }

        constexpr auto add(const string_t &str) -> bool
        {
            throw_if_empty("added string", str);

            if (Versions.back().contains(str))
            {
                return false;
            }

            Versions.push_back(Versions.back());

            try
            {
                Versions.back().insert(str);

                return true;
            }
            catch (...)
            {
                Versions.pop_back();
                throw;
            }
        }

        [[nodiscard]] constexpr auto contains(const string_t &str, const std::size_t version) const -> bool
        {
            throw_if_empty("searched string", str);
            require_less_equal(version, max_version(), "version");

            const auto &data = Versions.at(version);
            auto has = data.contains(str);
            return has;
        }

private:
        std::vector<std::unordered_set<string_t>> Versions;
    };
} // namespace Standard::Algorithms::Trees
