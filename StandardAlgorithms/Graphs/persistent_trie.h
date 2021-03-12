#pragma once
#include <unordered_map>
#include <unordered_set>
#include <vector>
#include "../Utilities/ExceptionUtilities.h"

namespace Privet::Algorithms::Trees
{
    // Given a set of strings, answer m queries:
    // - Whether a non-empty string s exists in an existing version v.
    // - Add a non-empty string s, creating a new version if s is new indeed.
    // Versions are 0 based.
    //
    // todo: p2. Ability to change a past version.
    template<class string_t = std::string,
        class char_t = typename string_t::value_type>
        struct persistent_trie final
    {
        //   Version 2, the suffix 'w' means word ending.
        //    Root_v2
        //   /  \ _
        //  A    C
        //  |    |
        //  Bw   Dw
        //
        //
        // Add "CE", create a new root.
        //   Version 2          Version 3
        //    Root_v2            Root_v3
        //   /  \               /       \ _
        //  A    C     pointer_to_A      C
        //  |    |                     /   \_
        //  Bw   Dw        pointer_to_D    Ew

        explicit persistent_trie()
            : nodes(2), roots(1, 1)
        {
        }

        auto version() const
        {
            return roots.size() - 1u;
        }

        bool add(const string_t& s)
        {
            ThrowIfEmpty(s, "added string");

            const auto nodes_size0 = nodes.size();
            auto node_id = nodes_size0;
            {
                const auto old_node_id = roots.back();
                roots.push_back(nodes_size0);
                nodes.push_back(nodes[old_node_id]);
            }

            const auto size = s.size();
            size_t i = 0;

            auto has_add = false;
            for (;;)
            {
                assert(i < size&& node_id > 0);
                const auto& c = s[i];

                const auto new_child_id = nodes.size();
                nodes.push_back({});

                auto& node = nodes[node_id];
                const auto it = node.children.find(c);
                if (it == node.children.end())
                    has_add = true;
                else
                {
                    const auto& old_child_id = it->second;
                    const auto& old_child = nodes[old_child_id];
                    nodes.back() = old_child;
                }

                node.children[c] = new_child_id;
                if (++i < size)
                {
                    node_id = new_child_id;
                    continue;
                }

                auto& child = nodes[new_child_id];
                if (has_add || !child.is_word_end)
                    return (child.is_word_end = true);

                // Cancel add.
                nodes.resize(nodes_size0);
                roots.pop_back();
                return false;
            }
        }

        bool find(const string_t& s, const size_t version) const
        {
            ThrowIfEmpty(s, "searched string");
            RequireNotGreater(version, this->version(), "version");

            const auto size = s.size();
            size_t i = 0;

            auto node_id = roots[version];
            for (;;)
            {
                assert(i < size&& node_id > 0);
                const auto& c = s[i];

                const auto& node = nodes[node_id];
                const auto it = node.children.find(c);
                if (it == node.children.end())
                    return false;

                const auto& child_id = it->second;
                if (++i < size)
                {
                    node_id = child_id;
                    continue;
                }

                const auto& child = nodes[child_id];
                return child.is_word_end;
            }
        }

    private:
        struct node_t final
        {
            std::unordered_map<char_t, size_t> children;
            bool is_word_end = {};
        };

        std::vector<node_t> nodes;
        std::vector<size_t> roots;
    };

    // Slow.
    template<class string_t = std::string,
        class char_t = typename string_t::value_type>
        struct persistent_trie_slow final
    {
        explicit persistent_trie_slow()
        {
            versions.push_back({});
        }

        auto version() const
        {
            return versions.size() - 1u;
        }

        bool add(const string_t& s)
        {
            ThrowIfEmpty(s, "added string");
            if (versions.back().count(s))
                return false;

            versions.push_back(versions.back());
            versions.back().insert(s);
            return true;
        }

        bool find(const string_t& s, const size_t version) const
        {
            ThrowIfEmpty(s, "searched string");
            RequireNotGreater(version, this->version(), "version");

            const auto& ar = versions[version];
            const auto cnt = ar.count(s);
            return cnt;
        }

    private:
        std::vector<std::unordered_set<string_t>> versions;
    };
}