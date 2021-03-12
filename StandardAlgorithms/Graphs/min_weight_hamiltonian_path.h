#pragma once

#include <algorithm>
#include <cassert>
#include <numeric>
#include <vector>
#include "../Utilities/ExceptionUtilities.h"
#include "../Numbers/bit_utilities.h"
#include "vertex_utilities.h" // edge_weight_slow

namespace
{
    constexpr int inline get_index(const bool shall_restore_path, const int cnt) noexcept
    {
        const auto r = shall_restore_path ? cnt : cnt & 1;
        return r;
    }

    template <class digraph_t, class weight_t, class u_number_t, class map_t, weight_t infinity_w>
    void restore_path(const digraph_t& digraph,
        const std::vector<map_t>& ms,
        std::vector<int>& path,
        const weight_t min_weight, const u_number_t full_mask, const int last_node,
        const bool shall_restore_path)
    {
        constexpr u_number_t one = 1;
        const auto node_count = static_cast<int>(digraph.size());

        auto weight = min_weight;
        auto to = last_node;
        auto mask = full_mask ^ (one << to);
        path.clear();
        path.reserve(node_count);
        path.push_back(to);
        auto index = node_count - 1;
        do
        {
            --index;
            assert(weight >= 0 && to >= 0 && to < node_count);
            auto from = 0;
            for (; from < node_count; ++from)
            {
                if (from == to)
                    continue;

                const auto edge_cost = ::Privet::Algorithms::Graphs::edge_weight_slow<digraph_t, weight_t, infinity_w>(digraph, from, to);
                if (edge_cost >= infinity_w)
                    continue;

                const auto p = std::make_pair(mask, from);
                const auto& cur = ms[get_index(shall_restore_path, index)];
                const auto it2 = cur.find(p);
                if (it2 == cur.end())
                    continue;

                const auto& parent_cost = it2->second,
                    cand = parent_cost + edge_cost;
                assert(cand >= 0 && cand < infinity_w&& parent_cost >= 0 && parent_cost < infinity_w);
                if (weight != cand)
                {
                    // assert(weight < cand);
                    continue;
                }

                weight -= edge_cost;
                assert(weight >= 0 && weight < infinity_w);
                break;
            }

            assert(from >= 0 && from < node_count);
            assert((mask >> from) & one);
            path.push_back(from);
            mask ^= one << from;
            to = from;
        } while (mask);

        assert(!mask && !weight);
        std::reverse(path.begin(), path.end());
    }

    template <class digraph_t, class weight_t, class u_number_t, class map_t, weight_t infinity_w>
    void process_mask_draft(const digraph_t& digraph,
        map_t& t,
        const u_number_t mask)
    {
        constexpr u_number_t one = 1;
        const auto node_count = static_cast<int>(digraph.size());

        for (auto from = 0; from < node_count; ++from)
        {
            if (!((mask >> from) & one))
                continue;

            for (auto to = 0; to < node_count; ++to)
            {
                if (from == to || !((mask >> to) & one))
                    continue;

                weight_t parent_cost;
                {
                    const u_number_t mask_prev = mask ^ (one << to);
                    const auto p1 = std::make_pair(mask_prev, from);
                    const auto it1 = t.find(p1);
                    if (it1 == t.end())
                        continue;

                    // t[mask ^ (1llu << to)][from] < infinity_w)
                    parent_cost = it1->second;
                    assert(parent_cost >= 0 && parent_cost < infinity_w);
                }

                const auto edge_cost = ::Privet::Algorithms::Graphs::edge_weight_slow<digraph_t, weight_t, infinity_w>(digraph, from, to);
                if (edge_cost >= infinity_w)
                    continue;

                const auto p = std::make_pair(mask, to);
                const weight_t cand = parent_cost + edge_cost;
                assert(cand >= 0 && cand < infinity_w);
                auto ins = t.insert({ p, cand });
                if (!ins.second)
                {//Update.
                    auto it2 = ins.first;
                    if (cand < it2->second)
                        it2->second = cand;
                    assert(it2->second >= 0 && it2->second < infinity_w);
                }
            }
        }
    }

    template <class digraph_t, class weight_t, class u_number_t, class map_t, weight_t infinity_w>
    void restore_path_slow(const digraph_t& digraph, const map_t& t, std::vector<int>& path,
        const weight_t min_weight, const u_number_t full_mask, const int last_node)
    {
        constexpr u_number_t one = 1;
        const auto node_count = static_cast<int>(digraph.size());

        auto weight = min_weight;
        auto to = last_node;
        auto mask = full_mask ^ (one << to);
        path.clear();
        path.reserve(node_count);
        path.push_back(to);
        do
        {
            assert(weight >= 0 && to >= 0 && to < node_count);
            auto from = 0;
            for (; from < node_count; ++from)
            {
                if (from == to)
                    continue;

                const auto edge_cost = ::Privet::Algorithms::Graphs::edge_weight_slow<digraph_t, weight_t, infinity_w>(digraph, from, to);
                if (edge_cost >= infinity_w)
                    continue;

                const auto p = std::make_pair(mask, from);
                const auto it2 = t.find(p);
                if (it2 == t.end())
                    continue;

                const auto& parent_cost = it2->second,
                    cand = parent_cost + edge_cost;
                assert(cand >= 0 && cand < infinity_w&& parent_cost >= 0 && parent_cost < infinity_w);
                if (weight != cand)
                {
                    //assert(weight < cand);
                    continue;
                }

                weight -= edge_cost;
                assert(weight >= 0 && weight < infinity_w);
                break;
            }

            assert(from >= 0 && from < node_count);
            assert((mask >> from) & one);
            path.push_back(from);
            mask ^= one << from;
            to = from;
        } while (mask);

        assert(!mask && !weight);
        std::reverse(path.begin(), path.end());
    }

    template <class digraph_t, class weight_t, weight_t infinity_w>
    bool calc_path_weight_slow(const digraph_t& digraph,
        const std::vector<int>& temp,
        weight_t& w)
    {
        const auto node_count = static_cast<int>(digraph.size());
        w = {};
        for (auto i = 0; i < node_count - 1; ++i)
        {
            const auto& from = temp[i];
            const auto& to = temp[i + 1];
            assert(from != to);

            const auto edge_cost = ::Privet::Algorithms::Graphs::edge_weight_slow<digraph_t, weight_t, infinity_w>(digraph, from, to);
            if (edge_cost >= infinity_w)
                return false;

            assert(edge_cost >= 0);
            w += edge_cost;
            assert(w >= 0);
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
            // Weights are non-negative.
            // If the path is not needed, use 2 maps (less space): prev and current.
            //
            // Space complexity: O(2**n * n).
            // Time complexity: O(2**n * n**2).
            //
            // t[mask][v] cheapest Hamiltonian path, ending with v, in the masked vertices including v; Karp TSP traveling sales person.
            //
            // t[mask][to] is
            // 1) 0 when (mask == (1llu << to)) for all 'to' in [0..n-1].
            //    A single vertex.
            // 2) min(infinity_w, {t[mask ^ (1llu << to)][from] + w[from, to]}) for
            //      all 'from' in [0..n-1]
            //    when (to != from &&((mask >> to)  &1) &&((mask >> from)  &1) &&
            //      edge (from, to) exists &&w(from, to) < infinity_w &&
            //      t[mask ^ (1llu << to)][from] < infinity_w)
            // 3) infinity_w otherwise.
            //
            // min_weight = min(t[(1llu << n) - 1][to] for all 'to')
            template<class digraph_t,
                class weight_t,
                class u_number_t,
                // std::unordered_map<std::pair<u_number_t, int>, weight_t, pair_hash>
                class map_t,
                weight_t infinity_w = std::numeric_limits<weight_t>::max(),
                int max_nodes = (sizeof(u_number_t) << 3) - 1>
                weight_t min_weight_hamiltonian_path(const digraph_t& digraph,
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
                constexpr u_number_t one = 1;
                {
                    auto& cur = ms[0];
                    for (auto i = 0; i < node_count; ++i)
                    {// A single vertex.
                        const u_number_t mask = one << i;
                        assert(mask);
                        cur[std::make_pair(mask, i)] = 0;
                    }
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

                // min_weight = min(t[(1llu << n) - 1][to] for all 'to')
                auto min_weight = infinity_w;
                auto last_node = -1;
                {
                    const auto& cur = ms[get_index(shall_restore_path, node_count - 1)];
                    for (auto to = 0; to < node_count; ++to)
                    {
                        const auto p = std::make_pair(full_mask, to);
                        const auto it = cur.find(p);
                        if (it == cur.end())
                            continue;

                        const auto& parent_cost = it->second;
                        assert(parent_cost >= 0 && parent_cost < infinity_w);
                        if (min_weight <= parent_cost)
                            continue;

                        min_weight = parent_cost;
                        last_node = to;
                    }
                }

                assert(min_weight >= 0 && min_weight < infinity_w&& last_node >= 0 && last_node < node_count);
                if (shall_restore_path && min_weight < infinity_w && last_node >= 0 && last_node < node_count)
                {
                    restore_path<digraph_t, weight_t, u_number_t, map_t, infinity_w>(digraph, ms, path, min_weight, full_mask, last_node, shall_restore_path);
                }

                if (min_weight == infinity_w)
                    path.clear();

                return min_weight;
            }

            // Draft slow.
            template<class digraph_t,
                class weight_t,
                class u_number_t,
                // std::unordered_map<std::pair<u_number_t, int>, weight_t, pair_hash>
                class map_t,
                weight_t infinity_w = std::numeric_limits<weight_t>::max(),
                int max_nodes = (sizeof(u_number_t) << 3) - 1>
                weight_t min_weight_hamiltonian_path_draft(const digraph_t& digraph,
                    std::vector<int>& path,
                    map_t& t)
            {
                static_assert(std::is_unsigned_v<u_number_t> && max_nodes > 0 && max_nodes <= (sizeof(u_number_t) << 3) - 1 && infinity_w > 0);

                t.clear();
                path.clear();

                const auto node_count = static_cast<int>(digraph.size());
                RequireNotGreater(node_count, max_nodes, "node_count");
                if (node_count <= 1)
                {
                    if (node_count) path.push_back(0);
                    return {};
                }

                constexpr u_number_t one = 1;
                for (auto i = 0; i < node_count; ++i)
                {// A single vertex.
                    const u_number_t mask = one << i;
                    assert(mask);
                    t[std::make_pair(mask, i)] = 0;
                }

                const auto edge_mask = one << node_count;
                assert(edge_mask > 0);

                for (auto cnt = 2; cnt <= node_count; ++cnt)
                {
                    u_number_t mask = (one << cnt) - one;
                    assert(mask);
                    do
                    {
                        process_mask_draft<digraph_t, weight_t, u_number_t, map_t, infinity_w>(digraph, t, mask);

                        const auto next_mask = Privet::Algorithms::Numbers::next_greater_same_pop_count(mask);
                        mask = next_mask;
                    } while (mask < edge_mask);
                }

                const auto full_mask = edge_mask - one;
                assert(full_mask);

                auto min_weight = infinity_w;
                auto last_node = -1;
                for (auto to = 0; to < node_count; ++to)
                {
                    const auto p = std::make_pair(full_mask, to);
                    const auto it = t.find(p);
                    if (it == t.end())
                        continue;

                    const auto& parent_cost = it->second;
                    assert(parent_cost >= 0 && parent_cost < infinity_w);
                    if (min_weight <= parent_cost)
                        continue;

                    min_weight = parent_cost;
                    last_node = to;
                }

                assert(min_weight >= 0 && min_weight < infinity_w&& last_node >= 0 && last_node < node_count);
                if (min_weight < infinity_w && last_node >= 0)
                {
                    restore_path_slow<digraph_t, weight_t, u_number_t, map_t, infinity_w>(digraph, t, path, min_weight, full_mask, last_node);
                }

                if (min_weight == infinity_w)
                    path.clear();

                return min_weight;
            }

            // Slow.
            template<class digraph_t, class weight_t, weight_t infinity_w = std::numeric_limits<weight_t>::max(), int max_nodes = (sizeof(int) << 3) - 1>
            weight_t min_weight_hamiltonian_path_slow(const digraph_t& digraph,
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
                    const auto has = calc_path_weight_slow<digraph_t, weight_t, infinity_w>(digraph, temp, w);
                    if (!has || w >= min_weight)
                        continue;

                    min_weight = w;
                    std::copy(temp.begin(), temp.end(), path.begin());
                } while (std::next_permutation(temp.begin(), temp.end()));

                assert(min_weight < infinity_w);
                if (min_weight == infinity_w)
                    path.clear();

                return min_weight;
            }
        }
    }
}