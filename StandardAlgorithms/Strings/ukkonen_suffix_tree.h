#pragma once
#include"demand_char_unique.h"
#include<cassert>
#include<compare>
#include<concepts>
#include<iostream>
#include<map>
#include<unordered_map>
#include<vector>

namespace Standard::Algorithms::Strings
{
    constexpr std::int32_t suffix_tree_root_id{ 1 };

    // The sorted order is needed e.g. for a suffix array build.
    constexpr auto use_suffix_tree_map = true;

    constexpr auto shall_dfs_print = false;

    template<std::signed_integral int_t, class map_t, class char_t>
    [[nodiscard]] constexpr auto find_child(const map_t &map1, const char_t &cha) noexcept -> int_t
    {
        const auto iter = map1.find(cha);
        return iter == map1.end() ? int_t{} : iter->second;
    }

    template<std::signed_integral int_t, class nodes_t>
    constexpr void set_end_values_instead_pointers(const std::vector<int_t> &ends, nodes_t &nodes)
    {
        assert(!ends.empty() && !nodes.empty());

        for (auto &node : nodes)
        {
            auto &ind = node.e;
            assert(-int_t{ 1 } <= ind);

            if (ind < int_t{})
            {
                continue;
            }

            const auto &end1 = ends[ind];
            assert(node.b <= end1);

            ind = end1;
        }
    }

    template<class char_t, std::signed_integral int_t,
        class map_t = std::conditional_t<use_suffix_tree_map, std::map<char_t, int_t>, // else:
            std::unordered_map<char_t, int_t>>>
    struct full_suffix_node_t final
    {
        // Index in the nodes array - serves for debugging, can be deleted.
        int_t d{};

        // (start, end) interval specifies the edge, by which the node is connected to its parent.
        // Each edge will connect two nodes, one parent and one child, and (start, end) interval of a given edge will be
        // stored in the child node. Lets say there are two nods A and B connected by an edge with indices (5, 8) then
        // these indices (5, 8) will be stored in node B. -1 for root.
        int_t b{};

        // After creation, the inclusive end position unless the root that has -1.
        // During creation only, points to vs[end].
        int_t e{};

        // For root node, suffixLink will be set to 0
        // For internal nodes, suffixLink will be set to root by default in current extension and may change in next
        // extension.
        // pointer to other node via suffix link
        int_t suffix_link{};

        // For leaf nodes, it is the suffix starting position in the source text,
        // and also the index of suffix for the path from root to leaf.
        //  Else -1.
        //  suffix_index will be set to -1 by default,
        // and the actual suffix index will be set later for leaves at the end of all phases.
        int_t suffix_index = int_t{} - static_cast<int_t>(1);

        // todo(p3): rename to children
        map_t ch{};

        // todo(p3): [[nodiscard]] constexpr auto operator<=> (const full_suffix_node_t &) const noexcept = default;
    };

    template<class char_t, std::signed_integral int_t>
    [[nodiscard]] constexpr auto text_length(const full_suffix_node_t<char_t, int_t> &node) noexcept -> int_t
    {
        const auto &end1 = node.e;
        auto len = static_cast<int_t>(end1 - node.b + static_cast<int_t>(1));

        assert(int_t{} < len && suffix_tree_root_id < node.d);

        return len;
    }

    // Ukkonen's suffix tree, or suffix trie, built in O(n) time and space for a small alphabet.
    // A suffix tree for n chars has exactly n leaves.
    // The string last character must be unique.
    template<class string_t, std::signed_integral int_t = std::int32_t, class char_t = typename string_t::value_type,
        // Can be used for a generalized tree of 2 strings.
        char_t separator = '~', class node_t1 = full_suffix_node_t<char_t, int_t>>
    struct ukkonen_suffix_tree final
    {
        using node_t = node_t1;

        constexpr explicit ukkonen_suffix_tree(const string_t &str, const bool use_separator = false,
            const bool is_demand_last_char_unique = true, const bool shall_fix_ends = true)
            : Str(str)
            , Use_separator(use_separator)
        {
            constexpr int_t zero{};
            constexpr int_t minus_one = zero - int_t{ 1 };
            static_assert(minus_one < zero);

            const auto size1 = static_cast<int_t>(Str.size());
            if (!(zero < size1)) [[unlikely]]
            {
                throw std::invalid_argument("The string size " + std::to_string(size1) + " must be positive.");
            }

            if (is_demand_last_char_unique)
            {
                demand_last_char_unique(Str);
            }

            Nodes.reserve(size1 * 2LL);
            Nodes.resize(1);
            Ends.reserve(size1 + 1LL);

            // Build the suffix tree and print the edge labels along with suffix_index.
            // suffix_index for leaf edges will be >= 0 and for non-leaf edges will be -1

            // Root is a special node with start and end indices as -1,
            // as it has no parent from where an edge comes to root
            Ends.push_back(minus_one);

            const auto end = add_end(minus_one);
            Active_node = add_node(minus_one, end, zero);

            assert(zero < end && zero < Active_node && Active_node == suffix_tree_root_id);

            for (int_t index{}; index < size1; ++index)
            {
                extend(index);
            }

            assert(1ZU < Nodes.size());

            auto &node = Nodes.at(suffix_tree_root_id);
            ukkst_dfs(node);

            // Nodes.shrink_to_fit();
            // Ends.shrink_to_fit();
            if (shall_fix_ends)
            {
                set_end_values_instead_pointers(Ends, Nodes);
            }
        }

        [[nodiscard]] constexpr auto nodes() const &noexcept -> const std::vector<node_t> &
        {
            return Nodes;
        }

        [[nodiscard]] constexpr auto size() const noexcept -> std::size_t
        {
            return Nodes.size() - 1ZU;
        }

private:
        [[nodiscard]] constexpr auto add_end(const int_t &end1) -> int_t
        {
            Ends.push_back(end1);

            return static_cast<int_t>(Ends.size() - 1ZU);
        }

        [[nodiscard]] constexpr auto add_node(
            const int_t begin1, const int_t end2, const int_t suffix_link = suffix_tree_root_id) -> int_t
        {
            auto ide = static_cast<int_t>(Nodes.size());
            assert(int_t{} < ide);

#ifndef __clang__
            Nodes.emplace_back(ide, begin1, end2, suffix_link);
#else
            Nodes.push_back(node_t{ ide, begin1, end2, suffix_link });
#endif

            return ide;
        }

        // todo(p3): To be used only during construction?
        [[nodiscard]] constexpr auto construct_length(const node_t &node) const -> int_t
        {
            assert(suffix_tree_root_id < node.d);

            const auto &end1 = Ends[node.e];
            auto len = static_cast<int_t>(end1 - node.b + static_cast<int_t>(1));

            assert(int_t{} < len);

            return len;
        }

        [[nodiscard]] constexpr auto walk_down(const node_t &cur) -> bool
        {
            const auto length = construct_length(cur);
            if (Active_length < length)
            {
                return false;
            }

            Active_edge += length;
            Active_length -= length;
            Active_node = cur.d;

            return true;
        }

        constexpr void add_inner_node(const int_t pos, const char_t &act, const std::pair<int_t, int_t> &child_id_beg)
        {
            const auto &child_id = child_id_beg.first;
            const auto &beg = child_id_beg.second;
            const auto &cha = Str[pos];

            const auto end = add_end(static_cast<int_t>(beg + Active_length - static_cast<int_t>(1)));

            // New leaf
            const auto child2_id = add_node(beg, end);
            const auto child3_id = add_node(pos, {});

            auto &child = Nodes[child_id];
            auto &child2 = Nodes[child2_id];
            child.b += Active_length;

            const auto &ac2 = Str[child.b];
            assert(cha != ac2);

            child2.ch[cha] = child3_id;
            child2.ch[ac2] = child_id;
            Nodes[Active_node].ch[act] = child2_id;

            if (Last_new_node != int_t{})
            {
                Nodes[Last_new_node].suffix_link = child2_id;
            }

            Last_new_node = child2_id;
        }

        constexpr void extend(const int_t pos)
        {
            constexpr int_t zero{};

            Ends[0] = pos;
            ++Remaining_suffix_count;
            Last_new_node = zero;

            const auto &cha = Str[pos];

            while (zero < Remaining_suffix_count)
            {
                if (Active_length == zero)
                {
                    Active_edge = pos;
                }

                const auto &act = Str[Active_edge];
                auto child_id = find_child<int_t>(Nodes[Active_node].ch, act);

                if (child_id == zero)
                {
                    child_id = add_node(pos, {});
                    Nodes[Active_node].ch[act] = child_id;

                    if (Last_new_node != zero)
                    {
                        Nodes[Last_new_node].suffix_link = Active_node;
                        Last_new_node = {};
                    }
                }
                else
                {
                    auto &child0 = Nodes[child_id];

                    if (walk_down(child0))
                    {
                        continue;
                    }

                    if (const auto &char2 = Str[child0.b + Active_length]; char2 == cha)
                    {
                        if (Last_new_node && Active_node != suffix_tree_root_id)
                        {
                            Nodes[Last_new_node].suffix_link = Active_node;
                            Last_new_node = {};
                        }

                        ++Active_length;
                        break;
                    }

                    add_inner_node(pos, act, { child_id, child0.b });
                    // nodeS ref like 'child0' may now be invalid!
                }

                --Remaining_suffix_count;

                if (Active_node != suffix_tree_root_id)
                {
                    Active_node = Nodes[Active_node].suffix_link;
                }
                else if (zero < Active_length)
                {
                    --Active_length;

                    Active_edge = static_cast<int_t>(pos - Remaining_suffix_count + static_cast<int_t>(1));
                }
            }
        }

        [[maybe_unused]] constexpr void print_node([[maybe_unused]] const node_t &nod) const
        {
            if constexpr (!shall_dfs_print)
            {
                return;
            }

            if (const auto is_root = nod.start < int_t{}; is_root)
            {
                return;
            }

            auto beg = nod.b;
            const auto &end1 = Ends[nod.e];

            for (; beg <= end1 && (!Use_separator || Str[beg] != separator); ++beg)
            {
                std::cout << Str[beg];
            }

            if (beg == end1)
            {
                std::cout << separator;
            }
        }

        constexpr void ukkst_dfs(node_t &nod, const int_t height = {})
        {
            if constexpr (shall_dfs_print)
            {
                print_node(nod);
            }

            auto leaf = true;

            for (const auto &par : nod.ch)
            {
                const auto &child_id = par.second;
                assert(int_t{} < child_id);

                if constexpr (shall_dfs_print)
                {
                    if (leaf && !(nod.b < int_t{}))
                    {
                        std::cout << "  child [" << nod.suffix_index << "]\n";
                    }
                }

                leaf = false;

                auto &child = Nodes[child_id];
                const auto len1 = construct_length(child);
                const auto len2 = static_cast<int_t>(height + len1);
                ukkst_dfs(child, len2);
            }

            if (leaf)
            {
                if (Use_separator)
                {
                    assert(!(nod.e < int_t{}) && static_cast<std::size_t>(nod.e) < Ends.size());

                    for (auto index = nod.b; index <= Ends[nod.e]; ++index)
                    {
                        assert(!(index < int_t{}) && static_cast<std::size_t>(index) < Str.size());

                        if (Str[index] == separator)
                        {
                            nod.e = add_end(index);
                        }
                    }
                }

                nod.suffix_index = static_cast<int_t>(Str.size() - height);

                if constexpr (shall_dfs_print)
                {
                    std::cout << " [" << nod.suffix_index << "]\n";
                }
            }
        }

        const string_t &Str;

        std::vector<node_t> Nodes{};
        std::vector<int_t> Ends{};

        int_t Last_new_node{};
        int_t Active_node{};
        int_t Active_edge{};
        int_t Active_length{};
        int_t Remaining_suffix_count{};
        bool Use_separator{};
    };
} // namespace Standard::Algorithms::Strings
