#pragma once

#include <algorithm>
#include <cassert>
#include <numeric>
#include <unordered_map>
#if _DEBUG
#include <unordered_set>
#endif

#include <vector>
#include "../Utilities/ExceptionUtilities.h"
#include "../Numbers/Arithmetic.h"
#include "hamiltonian_path_count.h" // is_path_slow

namespace
{
    template <class digraph_t, class u_number_t>
    std::vector<u_number_t> build_out_nodes(const digraph_t& digraph)
    {
        constexpr u_number_t one = 1;
        const auto node_count = static_cast<int>(digraph.size());
        std::vector<u_number_t> r(node_count);

        for (auto from = 0; from < node_count; ++from)
        {
            const auto& edges = digraph[from];
            auto& mask = r[from];
            for (const auto& e : edges)
            {
                const auto& to = static_cast<int>(e);
                assert(from != to);
                mask |= one << to;
            }
        }

        return r;
    }

    template <class digraph_t, class u_number_t, class maps_t, class get_t>
    std::vector<int> restore_path(const digraph_t& digraph,
        const maps_t& ms, get_t& get)
    {
        const auto node_count = static_cast<int>(digraph.size());
        const u_number_t one = 1, full_mask = (one << node_count) - one;
        assert(full_mask && node_count > 1);

        auto j = node_count - 1;
        if (!get(ms, j).count(full_mask))
            // No path found.
            return {};

        std::vector<int> path;
        path.reserve(node_count);

        auto mask = full_mask;
#if _DEBUG
        std::unordered_set<int> ensure_unique_nodes;
#endif
        do
        {
            const auto& m = get(ms, j);
            const auto it = m.find(mask);
            assert(it != m.end());

            int to;
            auto mask2 = it->second;
            do
            {
                assert(mask2);
                to = static_cast<int>(MostSignificantBit(mask2));

                assert(mask & (one << to));
                assert(to >= 0 && to < node_count);
                if (path.empty())
                    break;

                mask2 ^= one << to;

                // Ensure the edge exists.
                const auto& from = path.back();
                if (::Privet::Algorithms::Graphs::has_edge_slow<digraph_t>(digraph, from, to))
                    break;
            } while (mask2);

            assert(mask & (one << to));
#if _DEBUG
            const auto ins = ensure_unique_nodes.insert(to);
            assert(ins.second);
#endif
            path.push_back(to);
            mask ^= one << to;
            --j;
        } while (mask);

        assert(path.size() == digraph.size() && j == -1);
        return path;
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
            // If the path is not needed, use 2 maps (less space): prev and current.
            //
            // Idea to divide by O(n): a mask stores a set of vertices when n is small.
            // Space complexity: O(2**n).
            // Time complexity: O(2**n * n).
            //
            // out_nodes[i] for i=[0..n-1] is a number, marking edges from i.
            // For a graph [{0-1}, {0-3}, {2-0}],
            //   out_nodes[0] = 2**1 + 2**3 = 10,
            //   out_nodes[1] = out_nodes[3] = 0 (no out edges),
            //   out_nodes[2] = 2**0 = 1.
            //
            // The mask is the set of bits indicating which nodes form a path.
            // t[mask] is the node set that can start(!) the mask.
            // In the digraph '0->1', t[3] = 2**0 = 1;
            // digraph '1->0', t[3] = 2**1 = 2.
            // '1-0', t[3] = 2**0 + 2**1 = 3 - path can start from any node (clique).
            //
            // t[mask] is
            // 1) mask when (mask == (1llu << from)) for all 'from' in [0..n-1].
            //    A single vertex.
            // 2) sum({t[mask ^ (1llu << from)] * 2**from}) for all 'from'
            //    when (popcount(mask) >= 2 &&((mask >> from)  &1) &&
            //      // There is at least 1 edge 'from' -> 't[mask ^ (1llu << from)]'
            //      (t[mask ^ (1llu << from)]  &out_nodes[from]))
            // 3) 0 otherwise.
            //
            // A solution exists and can be restored when (t[(1llu << n) - 1] != 0).
            template<class digraph_t,
                class u_number_t,
                class map_t = std::unordered_map<u_number_t, u_number_t>,
                class map_vector_t = std::vector<map_t>,
                int max_nodes = (sizeof(u_number_t) << 3) - 1>
                std::vector<int> hamiltonian_path_find(const digraph_t& digraph, map_vector_t& ms)
            {
                static_assert(std::is_unsigned_v<u_number_t> && max_nodes > 0 && max_nodes <= (sizeof(u_number_t) << 3) - 1);

                const auto node_count = static_cast<int>(digraph.size());
                RequireNotGreater(node_count, max_nodes, "node_count");

                ms.clear();
                if (node_count <= 1)
                {
                    std::vector<int> path;
                    if (node_count)
                        path.push_back(0);
                    return path;
                }

                // Which outgoing nodes exist.
                const auto out_nodes = build_out_nodes<digraph_t, u_number_t>(digraph);

                constexpr u_number_t zero = 0, one = 1;
                {
                    ms.resize(node_count);
                    auto& cur = ms[0];
                    for (auto i = 0; i < node_count; ++i)
                    {// A single vertex.
                        const u_number_t mask = one << i;
                        assert(mask);
                        cur[mask] = mask;
                    }
                }

                const u_number_t edge_mask = one << node_count,
                    full_mask = edge_mask - one;
                assert(edge_mask && full_mask && full_mask < edge_mask);

                for (auto cnt = 0; cnt < node_count - 1; ++cnt)
                {// (n * 2**n) main cycle.
                    const auto& prev = ms[cnt];
                    auto& cur = ms[cnt + 1];
                    for (const auto& p : prev)
                    {
                        const auto& mask = p.first;
                        assert(mask > 0 && mask < full_mask);

                        const auto& to_mask = p.second;
                        assert(to_mask > 0 && to_mask < full_mask && (mask& to_mask));

                        const u_number_t mask_cands = full_mask & (~mask);
                        assert(mask_cands > 0 && mask_cands < full_mask && !(mask& mask_cands));

                        auto cands = mask_cands;
                        do
                        {
                            const u_number_t low = cands & (zero - cands);
                            assert(cands > 0 && low > 0);
                            cands ^= low;

                            const auto from = MostSignificantBit(low);
                            assert(low == (one << from));
                            const u_number_t& from_mask = out_nodes[from],
                                x = to_mask & from_mask;
                            if (!x)
                                continue;

                            // There is at least 1 edge 'from' -> 'to_mask'.
                            const u_number_t mask_new = mask | low;
                            assert(mask_new > 0 && mask_new < edge_mask);

                            auto& val = cur[mask_new];
                            assert(val >= 0 && val < edge_mask);
                            val += low;
                            assert(val > 0 && val < edge_mask);
                        } while (cands);
                    }
                }

                const auto get = [](const map_vector_t& ms_val, const int ind)
                    -> const map_t&
                {
                    return ms_val[ind];
                };
                const auto result = restore_path<digraph_t, u_number_t>(digraph, ms, get);
                return result;
            }

            // Draft, slow.
            template<class digraph_t,
                class u_number_t,
                class map_t = std::unordered_map<u_number_t, u_number_t>,
                int max_nodes = (sizeof(u_number_t) << 3) - 1>
                std::vector<int> hamiltonian_path_find_draft(const digraph_t& digraph, map_t& m)
            {
                static_assert(std::is_unsigned_v<u_number_t> && max_nodes > 0 && max_nodes <= (sizeof(u_number_t) << 3) - 1);

                const auto node_count = static_cast<int>(digraph.size());
                RequireNotGreater(node_count, max_nodes, "node_count");

                m.clear();
                if (node_count <= 1)
                {
                    std::vector<int> path;
                    if (node_count)
                        path.push_back(0);
                    return path;
                }

                const auto out_nodes = build_out_nodes<digraph_t, u_number_t>(digraph);

                constexpr u_number_t zero = 0, one = 1;
                const u_number_t edge_mask = one << node_count;
                assert(edge_mask > 0);

                for (u_number_t mask = 1; mask < edge_mask; ++mask)
                {// (2**n) main cycle.
                    {
                        const u_number_t low = mask & (zero - mask), prev_mask = mask ^ low;
                        assert(low && prev_mask < mask);
                        if (!prev_mask)
                        {// A single vertex.
                            m[mask] = mask;
                            continue;
                        }
                    }

                    // At least 2 vertices.
                    auto mask_cop = mask;
                    u_number_t* val = nullptr;
                    do
                    {// Go thru all 'from' nodes - bits in mask.
                        const u_number_t low = mask_cop & (zero - mask_cop), prev_mask = mask ^ low;
                        assert(low && prev_mask && prev_mask < mask);
                        mask_cop ^= low;

                        const auto it = m.find(prev_mask);
                        if (it == m.end())
                            continue;

                        const auto from = MostSignificantBit(low);
                        assert(low == (one << from));
                        const u_number_t& to_mask = it->second,
                            & from_mask = out_nodes[from],
                            x = to_mask & from_mask;
                        if (!x)
                            continue;

                        // There is at least 1 edge 'from' -> 'to_mask'.
                        if (!val)
                            val = &(m[mask]);
                        assert(val);
                        *val += low;
                        assert(*val > 0);
                    } while (mask_cop);
                }

                const auto get = [](const map_t& ms_val, int) -> const map_t&
                {
                    return ms_val;
                };
                const auto result = restore_path<digraph_t, u_number_t>(digraph, m, get);
                return result;
            }

            // Slow.
            template<class digraph_t, int max_nodes = (sizeof(int) << 3) - 1>
            std::vector<int> hamiltonian_path_find_slow(const digraph_t& digraph)
            {
                static_assert(max_nodes > 0 && max_nodes <= (sizeof(int) << 3) - 1);

                const auto node_count = static_cast<int>(digraph.size());
                RequireNotGreater(node_count, max_nodes, "node_count");

                std::vector<int> path(node_count);
                std::iota(path.begin(), path.end(), 0);
                do
                {
                    if (is_path_slow<digraph_t>(digraph, path))
                        return path;
                } while (std::next_permutation(path.begin(), path.end()));

                return {};
            }
        }
    }
}