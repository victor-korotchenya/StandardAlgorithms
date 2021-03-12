#pragma once

#include <algorithm>
#include <cassert>
#include <functional> // _1
#include <numeric>
#include <vector>
#include "../Utilities/ExceptionUtilities.h"
#include "vertex_utilities.h" // has_edge_slow

namespace
{
    template <class digraph_t>
    bool is_path_slow(const digraph_t& digraph, const std::vector<int>& path)
    {
        const auto size = static_cast<int>(path.size());
        for (auto i = 0; i < size - 1; ++i)
        {
            const auto& from = path[i];
            const auto& to = path[i + 1];
            assert(from != to);

            if (!::Privet::Algorithms::Graphs::has_edge_slow<digraph_t>(digraph, from, to))
                return false;
        }

        return true;
    }
}

namespace Privet
{
    namespace Algorithms
    {
        namespace Graphs
        {
            // Simple connected directed graph.
            // Nodes start from 0.
            //
            // Space complexity: O(2**n * n).
            // Time complexity: O(2**n * n**2).
            //
            // t[mask][v] count of Hamiltonian paths, ending with v, in the masked vertices including v.
            //
            // t[mask][to] is
            // 1) 1 when (mask == (1llu << to)) for all 'to' in [0..n-1].
            //    A single vertex.
            // 2) sum({t[mask ^ (1llu << to)][from]) for
            //      all 'from', 'to' in [0..n-1]}
            //    when (to != from &&((mask >> to)  &1) &&((mask >> from)  &1) &&
            //      edge (from, to) exists)
            // 3) 0 otherwise.
            //
            // count = sum(t[(1llu << n) - 1][to] for all 'to')
            //
            // Last note. It can be amended to count Hamilt cycles: start from 0-th node, ...
            template<class digraph_t,
                class u_number_t,
                // std::unordered_map<std::pair<u_number_t, int>, u_number_t, pair_hash>
                class map_t,
                int max_nodes = (sizeof(u_number_t) << 3) - 1>
                u_number_t hamiltonian_path_count(const digraph_t& digraph,
                    map_t& prev, map_t& cur)
            {
                static_assert(std::is_unsigned_v<u_number_t> && max_nodes > 0 && max_nodes <= (sizeof(u_number_t) << 3) - 1);

                prev.clear();
                cur.clear();

                const auto node_count = static_cast<int>(digraph.size());
                RequireNotGreater(node_count, max_nodes, "node_count");
                if (node_count <= 1)
                    return 1;

                prev.reserve(node_count);
                cur.reserve(node_count);

                constexpr u_number_t one = 1;
                const auto edge_mask = one << node_count;
                assert(edge_mask > 0);

                for (auto i = 0; i < node_count; ++i)
                {// A single vertex.
                    const u_number_t mask = one << i;
                    assert(mask);
                    cur[std::make_pair(mask, i)] = 1;
                }

                for (auto cnt = 0; cnt < node_count - 1; ++cnt)
                {// (n * 2**n) main cycle.
                    std::swap(cur, prev);
                    cur.clear();

                    for (const auto& p : prev)
                    {
                        const auto& mask = p.first.first;
                        assert(mask > 0 && mask < edge_mask);

                        const auto& from = p.first.second;
                        assert(from >= 0 && from < node_count && ((mask >> from)& one));

                        const auto& parent_count = p.second;
                        assert(parent_count > 0);

                        const auto& edges = digraph[from];
                        for (const auto& e : edges)
                        {// Try to add an edge.
                            const auto& to = static_cast<int>(e);
                            if ((mask >> to) & one)
                                // A node is already taken.
                                continue;

                            const auto mask2 = (one << to) | mask;
                            const auto k = std::make_pair(mask2, to);
                            auto& val = cur[k];
                            val += parent_count;
                            assert(val > 0);
                        }
                    }
                }

                const auto full_mask = edge_mask - one;
                assert(full_mask);

                // count = sum(t[(1llu << n) - 1][to] for all 'to')
                u_number_t count = 0;
                for (auto to = 0; to < node_count; ++to)
                {
                    const auto p = std::make_pair(full_mask, to);
                    const auto it = cur.find(p);
                    if (it == cur.end())
                        continue;

                    const auto& parent_count = it->second;
                    assert(parent_count > 0 && count + parent_count > 0);

                    count += parent_count;
                    assert(count > 0);
                }

                assert(count >= 0);
                return count;
            }

            // Slow.
            template<class digraph_t, class number_t, int max_nodes = (sizeof(number_t) << 3) - 1>
            number_t hamiltonian_path_count_slow(const digraph_t& digraph,
                std::vector<int>& temp)
            {
                static_assert(max_nodes > 0 && max_nodes <= (sizeof(number_t) << 3) - 1);

                const auto node_count = static_cast<int>(digraph.size());
                RequireNotGreater(node_count, max_nodes, "node_count");
                if (node_count <= 1)
                    return 1;

                temp.resize(node_count);
                std::iota(temp.begin(), temp.end(), 0);

                number_t count = 0;
                do
                {
                    const auto is = is_path_slow<digraph_t>(digraph, temp);
                    count += is;
                } while (std::next_permutation(temp.begin(), temp.end()));

                assert(count >= 0);
                return count;
            }
        }
    }
}