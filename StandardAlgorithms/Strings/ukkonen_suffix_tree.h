#pragma once
#include <vector>

// The sorted order is needed e.g. for a suffix array build.
#define use_suffix_tree_map 1

#if use_suffix_tree_map
#include <map>
#else
#include <unordered_map>
#endif

//#define shall_dfs_print 1

#if shall_dfs_print
#include <iostream>
#endif

namespace Privet
{
    namespace Algorithms
    {
        namespace Strings
        {
            constexpr auto suffix_tree_root_id = 1;

            template<class string_t>
            void demand_last_char_unique(const string_t& s)
            {
                const auto size = s.size();
                if (size <= 1)
                    return;

                for (size_t i = 0; i < size - 1; ++i)
                    if (s[i] == s.back())
                    {
                        assert(0);
                        throw std::invalid_argument("The last char '" + std::to_string(s.back()) + "' is duplicated at " + std::to_string(i));
                    }
            }

            template<class int_t, class map_t, class char_t>
            auto find_child(const map_t& m, const char_t& c)
            {
                const auto it = m.find(c);
                return it == m.end() ? int_t() : it->second;
            }

            template<class int_t, class nodes_t>
            void set_end_values_instead_pointers(const std::vector<int_t>& ends, nodes_t& nodes)
            {
                assert(ends.size() && nodes.size());

                for (auto& node : nodes)
                {
                    auto& e = node.e;
                    assert(e >= -1);
                    if (e < 0)
                        continue;

                    const auto& v = ends[e];
                    assert(node.b <= v);
                    e = v;
                }
            }

            template<class char_t, class int_t>
            struct full_suffix_node_t final
            {
                static_assert(std::is_signed_v<int_t>);

                // Index in the nodes array - serves for debugging, can be deleted.
                int_t d;

                // todo: rename to children
#if use_suffix_tree_map
                std::map
#else
                std::unordered_map
#endif
                    <char_t, int_t> ch;

                //(start, end) interval specifies the edge, by which the node is connected to its parent.
                // Each edge will connect two nodes, one parent and one child, and (start, end) interval of a given edge will be stored in the child node.
                // Lets say there are two nods A and B connected by an edge with indices (5, 8) then this indices (5, 8) will be stored in node B.
                // -1 for root.
                int_t b,
                    // After creation, the inclusive end position unless the root that has -1.
                    // During creation only, points to vs[end].
                    e;

                // For root node, suffixLink will be set to 0
                // For internal nodes, suffixLink will be set to root by default in current extension and may change in next extension.
                //pointer to other node via suffix link
                int_t suffix_link;

                //For leaf nodes, it is the suffix starting position in the source text, and also the index of suffix for the path from root to leaf.
                // Else -1.
                // suffix_index will be set to -1 by default and actual suffix index will be set later for leaves at the end of all phases
                int_t suffix_index;

                full_suffix_node_t(int_t id = {}, int_t begin = {}, int_t end = {}, int_t suffix_link = {}, int_t suffix_index = int_t() - int_t(1),
                    const
#if use_suffix_tree_map
                    std::map
#else
                    std::unordered_map
#endif
                    <char_t, int_t>& children = {})
                    : d(id),
                    ch(children),
                    b(begin),
                    e(end),
                    suffix_link(suffix_link),
                    suffix_index(suffix_index)
                {
                }

                int_t text_length() const
                {
                    const auto len = static_cast<int_t>(e - b + int_t(1));
                    assert(len > 0 && d > suffix_tree_root_id);
                    return len;
                }
            };

            // Ukkonen's suffix tree built in O(n) for a small alphabet.
            // A suffix tree for n chars has exactly n leaves.
            // The string last character is unique.
            template<class string_t, class int_t = int, class char_t = typename string_t::value_type,
                // Can be used for a generalized tree of 2 strings.
                char_t separator = '~', class node_t = full_suffix_node_t<char_t, int_t>>
                struct ukkonen_suffix_tree final
            {
                static_assert(std::is_signed_v<int_t>);

                using node_alias_t = node_t;

                explicit ukkonen_suffix_tree(const string_t& s,
                    const bool use_separator = false, const bool ia_demand_last_char_unique = true, const bool shall_fix_ends = true) : s(s), use_separator(use_separator)
                {
                    {
                        const auto size = static_cast<int_t>(s.size());
                        if (size <= 0)
                            throw std::invalid_argument("size " + std::to_string(size));

                        if (ia_demand_last_char_unique)
                            demand_last_char_unique(s);

                        nodes.reserve(size << 1);
                        ends.reserve(size + 1);

                        nodes.resize(1);
                        ends.push_back(-int_t(1));

                        // Build the suffix tree and print the edge labels along with suffix_index.
                        // suffix_index for leaf edges will be >= 0 and for non-leaf edges will be -1

                        // Root is a special node with start and end indices as -1, as it has no parent from where an edge comes to root
                        const auto end = add_end(-int_t(1));
                        active_node = add_node(-int_t(1), end, int_t(0));
                        assert(end > 0 && active_node > 0 && active_node == suffix_tree_root_id);

                        for (int_t i = {}; i < size; ++i)
                            extend(i);
                    }

                    auto& node = nodes[suffix_tree_root_id];
                    dfs(node);

                    //nodes.shrink_to_fit();
                    //ends.shrink_to_fit();
                    if (shall_fix_ends)
                        set_end_values_instead_pointers(ends, nodes);
                }

                size_t size() const noexcept
                {
                    return nodes.size() - 1u;
                }

                const string_t& s;

                std::vector<node_t> nodes;
                std::vector<int_t> ends;

            private:
                //lastNewNode will point to newly created internal node, waiting for it's suffix link to be set, which might get a neww suffix link (other than root) in next extension of same phase. lastNewNode will be set to nullptr when last newly created internal node (if there is any) got it's suffix link reset to neww internal node created in next extension of same phase.
                int_t last_new_node = {},
                    active_node = {};

                // Input string character index
                int_t active_edge = {};
                int_t active_length = {};

                // remainingSuffixCount tells how many suffixes yet to be added in tree
                int_t remaining_suffix_count = {};

                bool use_separator;

                int_t add_end(const int_t end)
                {
                    ends.push_back(end);
                    return static_cast<int_t>(ends.size() - 1u);
                }

                int_t add_node(const int_t start, const int_t end, const int_t suffix_link = suffix_tree_root_id)
                {
                    const auto id = static_cast<int_t>(nodes.size());
                    assert(id > 0);
                    node_t node;

                    nodes.emplace_back(id, start, end, suffix_link);
                    return id;
                }

                // To use only during construction.
                int_t text_length(const node_t& node) const
                {
                    assert(node.d > suffix_tree_root_id);

                    const auto& e = ends[node.e],
                        len = static_cast<int_t>(e - node.b + int_t(1));
                    assert(len > 0);
                    return len;
                }

                bool walk_down(node_t& cur)
                {
                    const auto length = text_length(cur);
                    // activePoint change for walk down (APCFWD) using Skip/Count Trick (Trick 1).
                    // If activeLength is greater than current edge length, set next internal node as activeNode and adjust activeEdge and activeLength accordingly to represent same activePoint
                    if (active_length < length)
                        return false;

                    active_edge += length;
                    active_length -= length;
                    active_node = cur.d;
                    return true;
                }

                void extend(const int_t pos)
                {
                    // Extension Rule 1, this takes care of extending all leaves created so far in tree
                    ends[0] = pos;

                    // Increment remainingSuffixCount indicating that a neww suffix added to the list of suffixes yet to be added in tree
                    ++remaining_suffix_count;

                    //set lastNewNode to nullptr while starting a neww phase, indicating there is no internal node waiting for it's suffix link reset in current phase
                    last_new_node = {};

                    const auto& c = s[pos];
                    //Add all suffixes (yet to be added) one by one in tree
                    while (remaining_suffix_count > 0)
                    {
                        if (!active_length)
                            //APCFALZ
                            active_edge = pos;

                        const auto& ac = s[active_edge];
                        auto child_id = find_child<int_t>(nodes[active_node].ch, ac);
                        // There is no outgoing edge starting with activeEdge from activeNode
                        if (!child_id)
                        {//Extension Rule 2 (A neww leaf edge gets created)
                            child_id = add_node(pos, {});
                            nodes[active_node].ch[ac] = child_id;

                            //A neww leaf edge is created in above line starting from an existng node (the current activeNode), and if there is any internal node waiting for it's suffix link get reset, point the suffix link from that last internal node to current activeNode. Then set

                            //lastNewNode
                            //to nullptr indicating no more node waiting for suffix link
                            //reset.
                            if (last_new_node)
                            {
                                nodes[last_new_node].suffix_link = active_node;
                                last_new_node = {};
                            }
                        }
                        else
                        {// There is an outgoing edge starting with activeEdge from activeNode
                         // Get the next node at the end of edge starting with activeEdge
                            auto& child0 = nodes[child_id];
                            if (walk_down(child0))
                                continue; //Start from next node (the neww activeNode)

                            const auto& c2 = s[child0.b + active_length];
                            // Extension Rule 3 (current character being processed is already on the edge)
                            if (c2 == c)
                            {
                                //If a newly created node waiting for it's suffix link to be set, then set suffix link of that waiting node to current active node
                                if (last_new_node && active_node != suffix_tree_root_id)
                                {
                                    nodes[last_new_node].suffix_link = active_node;
                                    last_new_node = {};
                                }

                                //APCFER3
                                ++active_length;
                                // STOP all further processing in this phase and move on to next phase
                                break;
                            }

                            //We will be here when activePoint is in middle of the edge being traversed and current character being processed is not on the edge (we fall off the tree). In this case, we add a neww internal node and a neww leaf edge going out of that neww node. This is Extension Rule 2, where a neww leaf edge and a neww internal node get created

                            int_t child2_id;
                            {//New internal node
                                const auto end = add_end(static_cast<int_t>(child0.b + active_length - static_cast<int_t>(1)));

                                //New leaf coming out of neww internal node
                                child2_id = add_node(child0.b, end);
                                const auto child3_id = add_node(pos, {});

                                // Note: nodez ref like 'child0' maybe invalid!
                                auto& child = nodes[child_id],
                                    & child2 = nodes[child2_id];

                                child.b += active_length;

                                const auto& ac2 = s[child.b];
                                assert(c != ac2);

                                child2.ch[c] = child3_id;
                                //split->children[text[next->start]] = next;
                                //child2.ch[ac] = child_id;
                                child2.ch[ac2] = child_id;

                                nodes[active_node].ch[ac] = child2_id;
                            }

                            // We got a neww internal node here. If there is any internal node created in last extensions of same phase which is still waiting for it's suffix link reset, do it now.
                            if (last_new_node)
                                //suffixLink of lastNewNode points to current newly created internal node
                                nodes[last_new_node].suffix_link = child2_id;

                            //Make the current newly created internal node waiting for it's suffix link reset (which is pointing to root at present). If we come across any other internal node (existing or newly created) in next extension of same phase, when a neww leaf edge gets added (i.e. when Extension Rule 2 applies is any of the next extension of same phase) at that point, suffixLink of this node will point to that internal node.
                            last_new_node = child2_id;
                        }

                        // One suffix got added in tree, decrement the countzzz of suffixes yet to be added.
                        --remaining_suffix_count;

                        if (active_node != suffix_tree_root_id)
                            //APCFER2C2
                            active_node = nodes[active_node].suffix_link;
                        else if (active_length > 0)
                        {//APCFER2C1
                            --active_length;
                            active_edge = static_cast<int_t>(pos - remaining_suffix_count + static_cast<int_t>(1));
                        }
                    }
                }

                //Print the suffix tree as well along with setting suffix index
                //So tree will be printed in DFS manner
                //Each edge along with it's suffix index will be printed
                void dfs(node_t& n, const int_t height = {})
                {
#if shall_dfs_print
                    if (n.start >= 0)
                    {//A non-root node.
                        auto b = n.b;
                        const auto& e = ends[n.e];
                        for (; b <= e && (!use_separator || s[b] != separator); ++b)
                            std::cout << s[b];

                        if (b == e)
                            printf("%c", separator);
                    }
#endif
                    auto leaf = true;
                    for (const auto& p : n.ch)
                    {
                        const auto& child_id = p.second;
                        assert(child_id > 0);

#if shall_dfs_print
                        if (leaf && n.b >= 0)
                            std::cout << "  child [" << n.suffix_index << "]\n";
#endif
                        leaf = false;
                        auto& child = nodes[child_id];
                        const auto h1 = text_length(child),
                            h2 = static_cast<int_t>(height + h1);

                        dfs(child, h2);
                    }

                    if (leaf)
                    {
                        if (use_separator)
                        {
                            const auto& e = ends[n.e];
                            for (auto i = n.b; i <= e; ++i)
                            {
                                if (s[i] == separator)
                                    //Remove other word characters
                                    n.e = add_end(i);
                            }
                        }

                        n.suffix_index = static_cast<int_t>(s.size() - height);
#if shall_dfs_print
                        std::cout << " [" << n.suffix_index << "]\n";
#endif
                    }
                }
            };
        }
    }
}