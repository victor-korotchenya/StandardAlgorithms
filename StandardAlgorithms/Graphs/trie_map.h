#pragma once
#include<array>
#include<cassert>
#include<concepts>
#include<cstdint>
#include<unordered_map>
#include<vector>

namespace Standard::Algorithms::Trees
{
    template<class key_t, class value_t>
    struct unordered_trie_map_node_t final
    {
        std::unordered_map<key_t, std::int32_t> children{};
        value_t value{};
        bool has_value{};
    };

    template<class key_t, class value_t>
    [[nodiscard]] constexpr auto cpp23 // todo(p3): c++23 operator[]
        (unordered_trie_map_node_t<key_t, value_t> &node, const key_t &key) -> std::int32_t &
    {
        auto &pos = node.children[key];
        return pos;
    }

    template<class key_t, class value_t>
    [[nodiscard]] constexpr auto find_position(
        const unordered_trie_map_node_t<key_t, value_t> &node, const key_t &key) noexcept -> std::int32_t
    {
        const auto iter = node.children.find(key);
        return iter == node.children.end() ? 0 : iter->second;
    }

    template<std::integral key_t, class value_t, key_t min_key, key_t max_key>
    requires(key_t{} <= min_key && min_key <= max_key)
    struct simple_trie_map_node_t final
    {
        static constexpr auto arr_size = max_key + 1LL - min_key;

        static_assert(0 < arr_size);

        std::array<std::int32_t, arr_size> children{};
        value_t value{};
        bool has_value{};
    };

    template<std::integral key_t, class value_t, key_t min_key, key_t max_key>
    [[nodiscard]] constexpr auto find_position(
        const simple_trie_map_node_t<key_t, value_t, min_key, max_key> &node, const key_t &key) -> std::int32_t
    {
        assert(min_key <= key && key <= max_key);

        const auto diff = key - min_key;
        const auto &pos = node.children.at(diff);
        return pos;
    }

    template<std::integral key_t, class value_t, key_t min_key, key_t max_key>
    [[nodiscard]] constexpr auto cpp23 // todo(p3): c++23 operator[]
        (simple_trie_map_node_t<key_t, value_t, min_key, max_key> &node, const key_t &key) -> std::int32_t &
    {
        assert(min_key <= key && key <= max_key);

        auto &pos = node.children.at(key - min_key);
        return pos;
    }

    // Using unordered_trie_map_node_t is 10 times slower than unordered map, but
    // it is 3 times faster with simple_trie_map_node_t when a limited range is used.
    template<class key_t, class value_t, class node_t = unordered_trie_map_node_t<key_t, value_t>>
    struct trie_map final
    {
        constexpr explicit trie_map(std::size_t estimated_size = 0)
            : Nodes(1)
        {
            Nodes.reserve(estimated_size + 1U);
        }

        [[nodiscard]] constexpr auto size() const noexcept -> std::size_t
        {
            assert(!Nodes.empty());

            return Nodes.size() - (Nodes[0].has_value ? 0U : 1U);
        }

        template<class string_t>
        [[nodiscard]] constexpr auto operator[] (const string_t &str) -> value_t &
        {
            auto start = str.cbegin();
            auto stop = str.cend();
            std::int32_t ide{};

            while (start != stop)
            {
                const auto &key = *start;
                auto &node = Nodes.at(ide);

                // auto& pos = node[key]; // todo(p3): c++23 operator[]
                auto &pos = cpp23(node, key);

                if (pos != 0)
                {
                    ide = pos;
                }
                else
                {
                    ide = pos = static_cast<std::int32_t>(Nodes.size());
                    Nodes.push_back({});
                }

                assert(0 < ide);
                ++start;
            }

            auto &node_2 = Nodes.at(ide);
            node_2.has_value = true;

            return node_2.value;
        }

        template<class string_t>
        [[nodiscard]] constexpr auto find(const string_t &str, value_t &value) const -> bool
        {
            auto start = str.cbegin();
            auto stop = str.cend();
            std::int32_t ide{};

            while (start != stop)
            {
                const auto &key = *start;
                const auto &node = Nodes.at(ide);
                ide = find_position(node, key);

                if (ide == 0)
                {
                    return false;
                }

                assert(0 < ide);
                ++start;
            }

            const auto &node = Nodes.at(ide);
            if (!node.has_value)
            {
                return false;
            }

            value = node.value;
            return true;
        }

private:
        std::vector<node_t> Nodes;
    };
} // namespace Standard::Algorithms::Trees
