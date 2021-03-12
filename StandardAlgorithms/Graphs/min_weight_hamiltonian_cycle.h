#pragma once

#include <algorithm>
#include <cassert>
#include <numeric>
#include <vector>
#include "../Utilities/ExceptionUtilities.h"
#include "min_weight_hamiltonian_path.h" // calc_path_weight_slow

namespace
{
    template <class digraph_t, class weight_t, weight_t infinity_w>
    bool calc_cycle_weight_slow(const digraph_t& digraph,
        const std::vector<int>& temp,
        weight_t& w)
    {
        assert(temp.size());
        if (!calc_path_weight_slow<digraph_t, weight_t, infinity_w>(digraph, temp, w))
            return false;

        // Edge {back -> front}.
        const auto& from = temp.back(), & to = temp.front();
        assert(from != to);

        const auto edge_cost = ::Privet::Algorithms::Graphs::edge_weight_slow<digraph_t, weight_t, infinity_w>(digraph, from, to);
        if (edge_cost >= infinity_w)
            return false;

        w += edge_cost;
        assert(w < infinity_w);
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
            // Weights are non-negative.
            // If the cycle is not needed, use 2 maps (less space): prev and current.
            //
            // Space complexity: O(2**n * n).
            // Time complexity: O(2**n * n**2).
            //
            // t[mask][v] cheapest Hamiltonian cycle, starting from 0 and ending with v, in the masked vertices including v, where v != 0; Karp TSP traveling sales person.
            //
            // t[mask][to] is
            // 1) 0 when mask = 1, to = 0.
            //    The starting node 0.
            // 2) min(infinity_w, {t[mask ^ (1llu << to)][from] + w[from, to]}) for
            //      all 'from' in [0..n-1]
            //    when (to != from &&((mask >> to)  &1) &&((mask >> from)  &1) &&
            //      edge (from, to) exists &&w(from, to) < infinity_w &&
            //      t[mask ^ (1llu << to)][from] < infinity_w &&
            //      // Only for cycle:
            //      to &&(mask  &1))
            // 3) infinity_w otherwise.
            //
            // min_weight = min({t[(1llu << n) - 1][to] + w(to, 0)}) for
            //   all 'to' in [1..n-1]
            //   when (to &&w(to, 0) < infinity_w &&
            //     t[(1llu << n) - 1][to] < infinity_w).
            //   Note that to != 0 here in the min weight cycle.
            template<class digraph_t,
                class weight_t,
                class u_number_t,
                // std::unordered_map<std::pair<u_number_t, int>, weight_t, pair_hash>
                class map_t,
                weight_t infinity_w = std::numeric_limits<weight_t>::max(),
                int max_nodes = (sizeof(u_number_t) << 3) - 1>
                weight_t min_weight_hamiltonian_cycle(const digraph_t& digraph,
                    std::vector<int>& path,
                    std::vector<map_t>& ms, const bool shall_restore_path = true)
            {
                static_assert(std::is_unsigned_v<u_number_t> && max_nodes > 0 && max_nodes <= (sizeof(u_number_t) << 3) - 1 && infinity_w > 0);

                const auto node_count = static_cast<int>(digraph.size());
                ms.clear();
                path.clear();

                RequireNotGreater(node_count, max_nodes, "node_count");
                if (node_count <= 1)
                {
                    if (node_count) path.push_back(0);
                    return {};
                }

                ms.resize(shall_restore_path ? node_count : 2);
                constexpr auto start_node = 0;
                constexpr u_number_t one = 1;
                {
                    auto& cur = ms[0];
                    cur[std::make_pair(one, start_node)] = 0;
                }

                const auto edge_mask = one << node_count;
                assert(edge_mask > 0);

                for (auto cnt = 0; cnt < node_count - 1; ++cnt)
                {// (n * 2**n) main cycle.
                    auto& cur = ms[get_index(shall_restore_path, cnt + 1)];
                    if (!shall_restore_path)
                        cur.clear();

                    const auto& prev = ms[get_index(shall_restore_path, cnt)];
                    for (const auto& p : prev)
                    {
                        const auto& mask = p.first.first;
                        assert(mask > 0 && mask < edge_mask);

                        const auto& from = p.first.second;
                        assert(from >= 0 && from < node_count && ((mask >> from)& one));

                        const auto& parent_weight = p.second;
                        assert(parent_weight >= 0 && parent_weight < infinity_w);

                        const auto& edges = digraph[from];
                        for (const auto& e : edges)
                        {// Try to add an edge.
                            const auto& edge_cost = e.Weight;
                            if (edge_cost >= infinity_w)
                                continue;

                            const auto& to = e.Vertex;
                            if ((mask >> to) & one)
                                // A node is already taken.
                                continue;

                            assert(to > 0 && (mask & 1));// Cycle only

                            const auto mask2 = (one << to) | mask;
                            const auto k = std::make_pair(mask2, to);
                            const weight_t cand = parent_weight + edge_cost;
                            assert(cand >= 0 && cand < infinity_w);

                            auto ins = cur.insert({ k, cand });
                            if (!ins.second)
                            {//Update.
                                auto it = ins.first;
                                if (cand < it->second)
                                    it->second = cand;
                                assert(it->second >= 0 && it->second < infinity_w);
                            }
                        }
                    }
                }

                const auto full_mask = edge_mask - one;
                assert(full_mask);

                // min_weight = min({t[(1llu << n) - 1][to] + w(to, 0)}) for
                //   all 'to' in [1..n-1]
                //   when (to &&w(to, 0) < infinity_w &&
                //     t[(1llu << n) - 1][to] < infinity_w).
                auto min_weight = infinity_w, min_edge_cost = infinity_w;
                auto last_node = -1;
                {
                    const auto& cur = ms[get_index(shall_restore_path, node_count - 1)];
                    for (auto to = start_node + 1; to < node_count; ++to)
                    {
                        const auto edge_cost = ::Privet::Algorithms::Graphs::edge_weight_slow<digraph_t, weight_t, infinity_w>(digraph, to, start_node);
                        if (edge_cost >= infinity_w)
                            // {to -> 0} edge not found.
                            continue;

                        const auto p = std::make_pair(full_mask, to);
                        const auto it = cur.find(p);
                        if (it == cur.end())
                            continue;

                        const auto& parent_cost = it->second;
                        assert(parent_cost >= 0 && parent_cost < infinity_w);

                        const auto cand = parent_cost + edge_cost;
                        if (min_weight <= cand)
                            continue;

                        min_weight = cand;
                        min_edge_cost = edge_cost;
                        last_node = to;
                    }
                }

                if (shall_restore_path && min_weight < infinity_w && last_node >= 0 && last_node < node_count)
                {
                    const auto path_weight = min_weight - min_edge_cost;
                    restore_path<digraph_t, weight_t, u_number_t, map_t, infinity_w>(digraph, ms, path, path_weight, full_mask, last_node, shall_restore_path);

                    assert(path.size() && path.size() == digraph.size() && path.front() == start_node);
                }

                return min_weight;
            }

            // Slow.
            template<class digraph_t, class weight_t, weight_t infinity_w = std::numeric_limits<weight_t>::max(), int max_nodes = (sizeof(int) << 3) - 1>
            weight_t min_weight_hamiltonian_cycle_slow(const digraph_t& digraph,
                std::vector<int>& path,
                std::vector<int>& temp)
            {
                static_assert(max_nodes > 0 && max_nodes <= (sizeof(int) << 3) - 1 && infinity_w > 0);
                path.clear();

                const auto node_count = static_cast<int>(digraph.size());
                RequireNotGreater(node_count, max_nodes, "node_count");
                if (node_count <= 1)
                {
                    if (node_count) path.push_back(0);
                    return {};
                }

                path.resize(node_count);
                temp.resize(node_count);
                std::iota(temp.begin(), temp.end(), 0);

                auto min_weight = infinity_w;
                do
                {
                    weight_t w;
                    const auto has = calc_cycle_weight_slow<digraph_t, weight_t, infinity_w>(digraph, temp, w);
                    if (!has || w >= min_weight)
                        continue;

                    min_weight = w;
                    std::copy(temp.begin(), temp.end(), path.begin());
                } while (std::next_permutation(temp.begin(), temp.end()));

                assert(min_weight <= infinity_w);
                if (min_weight == infinity_w)
                    path.clear();

                return min_weight;
            }
        }
    }
}