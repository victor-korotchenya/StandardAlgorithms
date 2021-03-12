#pragma once
#include <array>
#include <cassert>
#include <unordered_map>
#include <vector>

namespace Privet::Algorithms::Trees
{
    template <class key_t, class value_t>
    struct unordered_trie_map_node_t final
    {
        std::unordered_map<key_t, int> children;
        value_t value = {};
        bool has_value = false;

        int& operator[](const key_t& key)
        {
            auto& val = children[key];
            return val;
        }

        int find(const key_t& key) const
        {
            const auto it = children.find(key);
            return it == children.end() ? 0 : it->second;
        }
    };

    template <class key_t, class value_t, key_t min_key, key_t max_key>
    struct simple_trie_map_node_t final
    {
        static_assert(key_t() <= min_key && min_key <= max_key);

        std::array<int, max_key - min_key + 1> children = { 0 };
        value_t value = {};
        bool has_value = false;

        int& operator[](const key_t& key)
        {
            assert(min_key <= key && key <= max_key);

            auto& val = children[key - min_key];
            return val;
        }

        int find(const key_t& key) const
        {
            assert(min_key <= key && key <= max_key);

            const auto& val = children[key - min_key];
            return val;
        }
    };

    // Using unordered_trie_map_node_t is 10 times slower than unordered map, but
    // it is 3 times faster with imple_trie_map_node_t when a limited range is used.
    template <class key_t, class value_t,
        class node_t = unordered_trie_map_node_t<key_t, value_t>>
        class trie_map final
    {
        std::vector<node_t> nodes;

    public:
        explicit trie_map(size_t estimated_size = {})
            : nodes(1)
        {
            nodes.reserve(estimated_size + 1u);
        }

        size_t size() const noexcept
        {
            return nodes.size() - !nodes[0].has_value;
        }

        template<class string_t>
        value_t& operator[](const string_t& str)
        {
            auto start = str.begin();
            auto stop = str.end();
            auto id = 0;
            while (start != stop)
            {
                const auto& key = *start;
                auto& node = nodes[id];

                auto& d = node[key];
                if (d)
                    id = d;
                else
                {
                    id = d = static_cast<int>(nodes.size());
                    nodes.push_back({});
                    nodes.back().has_value = false;
                }

                assert(id > 0);
                ++start;
            }

            auto& node = nodes[id];
            node.has_value = true;

            return node.value;
        }

        template<class string_t>
        bool find(const string_t& str, value_t& value) const
        {
            auto start = str.begin();
            auto stop = str.end();
            auto id = 0;
            while (start != stop)
            {
                const auto& key = *start;
                const auto& node = nodes[id];
                id = node.find(key);
                if (!id)
                    return false;

                assert(id > 0);
                ++start;
            }

            const auto& node = nodes[id];
            if (!node.has_value)
                return false;

            value = node.value;
            return true;
        }
    };
}