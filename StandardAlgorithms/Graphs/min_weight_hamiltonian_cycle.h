#pragma once
#include"../Utilities/iota_vector.h"
#include"min_weight_hamiltonian_path.h" // calc_path_weight_slow
#include<tuple>

namespace Standard::Algorithms::Graphs::Inner
{
    template<class digraph_t, class weight_t, weight_t infinity_w>
    requires(weight_t{} < infinity_w)
    [[nodiscard]] constexpr auto calc_cycle_weight_slow(
        const digraph_t &digraph, const std::vector<std::int32_t> &temp, weight_t &weight) -> bool
    {
        assert(!temp.empty());

        if (!calc_path_weight_slow<digraph_t, weight_t, infinity_w>(digraph, temp, weight))
        {
            return false;
        }

        // Edge {back -> front}.
        const auto &tod = temp.at(0);
        const auto &from = temp.back();
        assert(from != tod);

        const auto edge_cost =
            ::Standard::Algorithms::Graphs::edge_weight_slow<digraph_t, weight_t, infinity_w>(from, digraph, tod);
        if (!(edge_cost < infinity_w))
        {
            return false;
        }

        weight += edge_cost;
        assert(weight < infinity_w);

        return true;
    }

    template<class digraph_t, class weight_t, std::unsigned_integral int_t, class map_t, weight_t infinity_w>
    requires(weight_t{} < infinity_w)
    [[nodiscard]] constexpr auto find_ham_cycle_min_weight(const digraph_t &digraph, const std::vector<map_t> &maps,
        const bool shall_restore_path) -> std::tuple<weight_t, weight_t, std::int32_t>
    {
        constexpr std::int32_t start_node{};
        constexpr int_t one = 1;

        const auto node_count = static_cast<std::int32_t>(digraph.size());
        const auto &cur = maps[restore_index(shall_restore_path, node_count - 1)];

        const auto full_mask = static_cast<int_t>((one << node_count) - one);

        assert(2 <= node_count && 0U < full_mask);

        // min_weight = min({t[(1LLU << n) - 1][to] + w(to, 0)}) for
        //   all 'to' in [1..n-1]
        //   when (to &&w(to, 0) < infinity_w &&
        //     t[(1LLU << n) - 1][to] < infinity_w).
        auto min_weight = infinity_w;
        auto min_edge_cost = infinity_w;
        auto last_node = -1;

        for (auto tod = start_node + 1; tod < node_count; ++tod)
        {
            const auto edge_cost = ::Standard::Algorithms::Graphs::edge_weight_slow<digraph_t, weight_t, infinity_w>(
                tod, digraph, start_node);

            if (!(edge_cost < infinity_w))
            {// {tod -> 0} edge not found.
                continue;
            }

            const auto par = std::make_pair(full_mask, tod);
            const auto iter = cur.find(par);
            if (iter == cur.end())
            {
                continue;
            }

            const auto &parent_cost = iter->second;
            assert(weight_t{} <= parent_cost && parent_cost < infinity_w);

            const auto cand = static_cast<weight_t>(parent_cost + edge_cost);
            if (!(cand < min_weight))
            {
                continue;
            }

            min_weight = cand;
            min_edge_cost = edge_cost;
            last_node = tod;
        }

        return { min_weight, min_edge_cost, last_node };
    }
} // namespace Standard::Algorithms::Graphs::Inner

namespace Standard::Algorithms::Graphs
{
    // Karp TSP (Traveling salesman problem).
    // Simple connected directed graph.
    // Nodes start from 0.
    // Weights are non-negative.
    // If the cycle is not needed, use 2 maps (less space): prev and current.
    //
    // Space complexity O(2**n * n).
    // Time complexity O(2**n * n**2).
    //
    // t[mask][v] cheapest Hamiltonian cycle, starting from 0 and ending with v, in the masked vertices including v,
    // where v != 0.
    //
    // t[mask][to] is
    // 1) 0 when mask = 1, to = 0.
    //    The starting node 0.
    // 2) min(infinity_w, {t[mask ^ (1LLU << to)][from] + w[from, to]}) for
    //      all 'from' in [0..n-1]
    //    when (to != from &&((mask >> to)  &1) &&((mask >> from)  &1) &&
    //      edge (from, to) exists &&w(from, to) < infinity_w &&
    //      t[mask ^ (1LLU << to)][from] < infinity_w &&
    //      // Only for cycle:
    //      to &&(mask  &1))
    // 3) infinity_w otherwise.
    //
    // min_weight = min({t[(1LLU << n) - 1][to] + w(to, 0)}) for
    //   all 'to' in [1..n-1]
    //   when (to &&w(to, 0) < infinity_w &&
    //     t[(1LLU << n) - 1][to] < infinity_w).
    // (to != 0) in the min weight cycle.
    template<class digraph_t, class weight_t, std::unsigned_integral int_t,
        // std::unordered_map<std::pair<int_t, std::int32_t>, weight_t, Standard::Algorithms::Numbers::pair_hash>
        class map_t, weight_t infinity_w = std::numeric_limits<weight_t>::max(),
        std::int32_t max_nodes = ::Standard::Algorithms::Utilities::graph_max_nodes<int_t>>
    requires(0 < max_nodes && max_nodes <= ::Standard::Algorithms::Utilities::graph_max_nodes<int_t> &&
        weight_t{} < infinity_w)
    [[nodiscard]] constexpr auto min_weight_hamiltonian_cycle(const digraph_t &digraph, std::vector<std::int32_t> &path,
        std::vector<map_t> &maps, const bool shall_restore_path = true) -> weight_t
    {
        const auto node_count = static_cast<std::int32_t>(digraph.size());
        require_less_equal(node_count, max_nodes, "node count");
        maps.clear();
        path.clear();

        if (node_count <= 1)
        {
            if (0 < node_count)
            {
                path.push_back(0);
            }

            return {};
        }

        maps.resize(shall_restore_path ? node_count : 2);

        constexpr std::int32_t start_node{};
        constexpr int_t one = 1;
        {
            auto &cur = maps[0];
            cur[std::make_pair(one, start_node)] = 0;
        }

        constexpr auto is_cycle = true;

        Inner::min_weight_ham_path_main<digraph_t, weight_t, int_t, map_t, infinity_w, is_cycle>(
            digraph, maps, shall_restore_path);

        const auto [min_weight, min_edge_cost, last_node] =
            Inner::find_ham_cycle_min_weight<digraph_t, weight_t, int_t, map_t, infinity_w>(
                digraph, maps, shall_restore_path);

        if (shall_restore_path && min_weight < infinity_w && !(last_node < 0) && last_node < node_count)
        {
            const auto full_mask = (one << node_count) - one;
            assert(0U < full_mask);

            const auto path_weight = static_cast<weight_t>(min_weight - min_edge_cost);

            Inner::restore_ham_path<digraph_t, weight_t, int_t, map_t, infinity_w>(
                digraph, maps, path, path_weight, full_mask, last_node, shall_restore_path);

            assert(!path.empty() && path.size() == digraph.size() && path.front() == start_node);
        }

        return min_weight;
    }

    // See also min_weight_hamiltonian_cycle_bb.
    // Slow.
    template<class digraph_t, class weight_t, weight_t infinity_w = std::numeric_limits<weight_t>::max(),
        std::int32_t max_nodes = ::Standard::Algorithms::Utilities::graph_max_nodes<std::int32_t>>
    requires(0 < max_nodes && max_nodes <= ::Standard::Algorithms::Utilities::graph_max_nodes<std::int32_t> &&
        weight_t{} < infinity_w)
    [[nodiscard]] constexpr auto min_weight_hamiltonian_cycle_slow(
        const digraph_t &digraph, std::vector<std::int32_t> &path) -> weight_t
    {
        const auto node_count = static_cast<std::int32_t>(digraph.size());
        require_less_equal(node_count, max_nodes, "node count");

        path.assign(node_count, 0);

        if (node_count <= 1)
        {
            return {};
        }

        auto temp = Standard::Algorithms::Utilities::iota_vector<std::int32_t>(node_count);
        auto min_weight = infinity_w;

        do
        {
            weight_t weight{};
            const auto has = Inner::calc_cycle_weight_slow<digraph_t, weight_t, infinity_w>(digraph, temp, weight);

            if (!has || !(weight < min_weight))
            {
                continue;
            }

            min_weight = weight;
            std::copy(temp.cbegin(), temp.cend(), path.begin());
        } while (std::next_permutation(temp.begin(), temp.end()));

        assert(min_weight <= infinity_w);

        if (min_weight == infinity_w)
        {
            path.clear();
        }

        return min_weight;
    }
} // namespace Standard::Algorithms::Graphs
