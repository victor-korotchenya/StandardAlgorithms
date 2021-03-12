#pragma once
#include"../Numbers/bit_utilities.h"
#include"../Numbers/to_unsigned.h"
#include"../Utilities/project_constants.h"
#include"../Utilities/require_utilities.h"
#include"vertex_utilities.h" // edge_weight_slow
#include<numeric>
#include<vector>

namespace Standard::Algorithms::Graphs::Inner
{
    [[nodiscard]] inline constexpr auto restore_index(const bool shall_restore_path, const std::int32_t cnt) noexcept
        -> std::int32_t
    {
        auto index = shall_restore_path
            ? cnt
            : ::Standard::Algorithms::Numbers::to_signed((::Standard::Algorithms::Numbers::to_unsigned(cnt) & 1U));

        return index;
    }

    template<class digraph_t, class weight_t, std::unsigned_integral int_t, class map_t, weight_t infinity_w>
    constexpr void restore_ham_path(const digraph_t &digraph, const std::vector<map_t> &maps,
        std::vector<std::int32_t> &path, const weight_t min_weight, const int_t full_mask, const std::int32_t last_node,
        const bool shall_restore_path)
    {
        constexpr int_t one = 1;

        const auto node_count = static_cast<std::int32_t>(digraph.size());
        path.clear();
        path.reserve(node_count);
        path.push_back(last_node);

        auto weight = min_weight;
        auto tod = last_node;
        auto mask = static_cast<int_t>(full_mask ^ (one << last_node));
        auto index = node_count - 1;

        do
        {
            --index;

            assert(weight_t{} <= weight && !(tod < 0) && tod < node_count);

            const auto &cur = maps[restore_index(shall_restore_path, index)];
            std::int32_t from{};
            for (; from < node_count; ++from)
            {
                if (from == tod)
                {
                    continue;
                }

                const auto edge_cost =
                    ::Standard::Algorithms::Graphs::edge_weight_slow<digraph_t, weight_t, infinity_w>(from, digraph, tod);

                if (!(edge_cost < infinity_w))
                {
                    continue;
                }

                const auto iter = cur.find(std::make_pair(mask, from));
                if (iter == cur.end())
                {
                    continue;
                }

                const auto &parent_cost = iter->second;
                const auto cand = static_cast<weight_t>(parent_cost + edge_cost);
                assert(
                    weight_t{} <= cand && cand < infinity_w && weight_t{} <= parent_cost && parent_cost < infinity_w);

                if (weight != cand)
                {
                    // assert(weight < cand);
                    continue;
                }

                weight -= edge_cost;
                assert(weight_t{} <= weight && weight < infinity_w);
                break;
            }

            assert(!(from < 0) && from < node_count && ((mask >> from) & one) != 0U);

            path.push_back(from);
            mask ^= one << from;
            tod = from;
        } while (mask != 0U);

        assert(mask == 0U && weight == weight_t{});

        std::reverse(path.begin(), path.end());
    }

    template<class digraph_t, class weight_t, std::unsigned_integral int_t, class map_t, weight_t infinity_w,
        bool is_cycle, class from_t>
    requires(weight_t{} < infinity_w)
    constexpr void ham_path_try_edges(
        const digraph_t &digraph, map_t &cur, const int_t mask, const from_t from, const weight_t parent_weight)
    {
        constexpr int_t one = 1;

        const auto &edges = digraph[from];

        for (const auto &edge : edges)
        {
            const auto &edge_cost = edge.weight;
            if (!(edge_cost < infinity_w))
            {
                continue;
            }

            const auto &tod = edge.vertex;
            if (const auto is_node_already_taken = ((mask >> tod) & one) != 0U; is_node_already_taken)
            {
                continue;
            }

            if constexpr (is_cycle)
            {// todo(p3): remove?
                assert(0 < tod && (mask & 1U) != 0U); // Cycle only
            }

            const auto mask2 = (one << tod) | mask;
            const auto key = std::make_pair(mask2, tod);
            const auto cand = static_cast<weight_t>(parent_weight + edge_cost);
            assert(weight_t{} <= cand && cand < infinity_w);

            auto ins = cur.insert({ key, cand });
            if (ins.second)
            {
                continue;
            }

            auto ite = ins.first;
            if (cand < ite->second)
            {
                ite->second = cand;
            }

            assert(weight_t{} <= ite->second && ite->second < infinity_w);
        }
    }

    template<class digraph_t, class weight_t, std::unsigned_integral int_t, class map_t, weight_t infinity_w,
        bool is_cycle>
    requires(weight_t{} < infinity_w)
    constexpr void min_weight_ham_path_main(
        const digraph_t &digraph, std::vector<map_t> &maps, const bool shall_restore_path)
    {
        constexpr int_t one = 1;

        const auto node_count = static_cast<std::int32_t>(digraph.size());
        const auto edge_mask = static_cast<int_t>(one << node_count);
        assert(0U < edge_mask);

        for (std::int32_t cnt{}; cnt < node_count - 1; ++cnt)
        {// (n * 2**n) main cycle.
            auto &cur = maps[restore_index(shall_restore_path, cnt + 1)];
            if (!shall_restore_path)
            {
                cur.clear();
            }

            const auto &prev = maps[restore_index(shall_restore_path, cnt)];

            for (const auto &par : prev)
            {
                const auto &mask = par.first.first;
                assert(0U < mask && mask < edge_mask);

                const auto &from = par.first.second;
                assert(!(from < 0) && from < node_count && ((mask >> from) & one) != 0U);

                const auto &parent_weight = par.second;
                assert(weight_t{} <= parent_weight && parent_weight < infinity_w);

                ham_path_try_edges<digraph_t, weight_t, int_t, map_t, infinity_w, is_cycle>(
                    digraph, cur, mask, from, parent_weight);
            }
        }
    }

    template<class digraph_t, class weight_t, std::unsigned_integral int_t, class map_t, weight_t infinity_w>
    constexpr void proc_draft_edge(
        const digraph_t &digraph, map_t &map1, const int_t mask, const std::int32_t from, const int_t mask_2)
    {
        constexpr int_t one = 1;

        assert(((mask >> from) & one) != 0U && mask_2 != 0U);

        const auto node_count = static_cast<std::int32_t>(digraph.size());

        for (std::int32_t tod{}; tod < node_count; ++tod)
        {
            if (((mask_2 >> tod) & one) == 0U)
            {
                continue;
            }

            assert(from != tod);

            const auto mask_prev = static_cast<int_t>(mask ^ (one << tod));
            const auto iter = map1.find(std::make_pair(mask_prev, from));

            if (iter == map1.end())
            {
                continue;
            }

            // map1[mask ^ (1LLU << tod)][from] < infinity_w)
            const auto parent_cost = iter->second;
            assert(weight_t{} <= parent_cost && parent_cost < infinity_w);

            const auto edge_cost =
                ::Standard::Algorithms::Graphs::edge_weight_slow<digraph_t, weight_t, infinity_w>(from, digraph, tod);
            if (!(edge_cost < infinity_w))
            {
                continue;
            }

            const auto cand = static_cast<weight_t>(parent_cost + edge_cost);
            assert(weight_t{} <= cand && cand < infinity_w);

            auto ins = map1.emplace(std::make_pair(mask, tod), cand);
            if (ins.second)
            {
                continue;
            }

            auto iter_2 = ins.first;
            if (cand < iter_2->second)
            {
                iter_2->second = cand;
            }

            assert(weight_t{} <= iter_2->second && iter_2->second < infinity_w);
        }
    }

    template<class digraph_t, class weight_t, std::unsigned_integral int_t, class map_t, weight_t infinity_w>
    constexpr void process_mask_draft(const digraph_t &digraph, map_t &map1, const int_t mask)
    {
        constexpr int_t one = 1;

        const auto node_count = static_cast<std::int32_t>(digraph.size());

        for (std::int32_t from{}; from < node_count; ++from)
        {
            // todo(p4): faster by enumerating only non-zero bits.
            const auto mask_2 = static_cast<int_t>(mask ^ (one << from));

            if (((mask >> from) & one) == 0U || mask_2 == 0U)
            {
                continue;
            }

            proc_draft_edge<digraph_t, weight_t, int_t, map_t, infinity_w>(digraph, map1, mask, from, mask_2);
        }
    }

    template<class digraph_t, class weight_t, std::unsigned_integral int_t, class map_t, weight_t infinity_w>
    constexpr void restore_path_slow(const digraph_t &digraph, const map_t &map1, std::vector<std::int32_t> &path,
        const weight_t min_weight, const int_t full_mask, const std::int32_t last_node)
    {
        constexpr int_t one = 1;

        const auto node_count = static_cast<std::int32_t>(digraph.size());
        path.clear();
        path.reserve(node_count);
        path.push_back(last_node);

        auto tod = last_node;
        auto mask = static_cast<int_t>(full_mask ^ (one << last_node));
        auto weight = min_weight;

        do
        {
            assert(weight_t{} <= weight && !(tod < 0) && tod < node_count);

            std::int32_t from{};
            for (; from < node_count; ++from)
            {
                if (from == tod)
                {
                    continue;
                }

                const auto edge_cost =
                    ::Standard::Algorithms::Graphs::edge_weight_slow<digraph_t, weight_t, infinity_w>(from, digraph, tod);
                if (!(edge_cost < infinity_w))
                {
                    continue;
                }

                const auto iter = map1.find(std::make_pair(mask, from));
                if (iter == map1.end())
                {
                    continue;
                }

                const auto &parent_cost = iter->second;
                const auto cand = static_cast<weight_t>(parent_cost + edge_cost);
                assert(
                    weight_t{} <= cand && cand < infinity_w && weight_t{} <= parent_cost && parent_cost < infinity_w);

                if (weight != cand)
                {
                    // assert(weight < cand);
                    continue;
                }

                weight -= edge_cost;
                assert(weight_t{} <= weight && weight < infinity_w);
                break;
            }

            assert(!(from < 0) && from < node_count && ((mask >> from) & one) != 0U);

            path.push_back(from);
            mask ^= one << from;
            tod = from;
        } while (mask != 0U);

        assert(mask == 0U && weight_t{} == weight);

        std::reverse(path.begin(), path.end());
    }

    template<class digraph_t, class weight_t, weight_t infinity_w>
    [[nodiscard]] constexpr auto calc_path_weight_slow(
        const digraph_t &digraph, const std::vector<std::int32_t> &temp, weight_t &weight) -> bool
    {
        const auto node_count = static_cast<std::int32_t>(digraph.size());
        weight = {};

        for (std::int32_t index{}; index < node_count - 1; ++index)
        {
            const auto &tod = temp.at(index + 1);
            const auto &from = temp[index];
            assert(from != tod);

            const auto edge_cost =
                ::Standard::Algorithms::Graphs::edge_weight_slow<digraph_t, weight_t, infinity_w>(from, digraph, tod);
            if (!(edge_cost < infinity_w))
            {
                return false;
            }

            weight += edge_cost;
            assert(weight_t{} <= edge_cost && weight_t{} <= weight);
        }

        return true;
    }
} // namespace Standard::Algorithms::Graphs::Inner

namespace Standard::Algorithms::Graphs
{
    // Simple connected directed graph.
    // Nodes start from 0.
    // Weights are non-negative.
    // If the path is not needed, use 2 maps (less space): prev and current.
    //
    // Space complexity O(2**n * n).
    // Time complexity O(2**n * n**2).
    //
    // t[mask][v] cheapest Hamiltonian path, ending with v, in the masked vertices including v.
    //
    // t[mask][to] is
    // 1) 0 when (mask == (1LLU << to)) for all 'to' in [0..n-1].
    //    A single vertex.
    // 2) min(infinity_w, {t[mask ^ (1LLU << to)][from] + w[from, to]}) for
    //      all 'from' in [0..n-1]
    //    when (to != from && ((mask >> to) &1) && ((mask >> from) &1) &&
    //      edge (from, to) exists && w (from, to) < infinity_w &&
    //      t[mask ^ (1LLU << to)][from] < infinity_w)
    // 3) infinity_w otherwise.
    //
    // min_weight = min(t[(1LLU << n) - 1][to] for all 'to')
    template<class digraph_t, class weight_t, std::unsigned_integral int_t,
        // std::unordered_map<std::pair<int_t, std::int32_t>, weight_t, Standard::Algorithms::Numbers::pair_hash>
        class map_t, weight_t infinity_w = std::numeric_limits<weight_t>::max(),
        std::int32_t max_nodes = ::Standard::Algorithms::Utilities::graph_max_nodes<int_t>>
    requires(0 < max_nodes && max_nodes <= ::Standard::Algorithms::Utilities::graph_max_nodes<int_t> &&
        weight_t{} < infinity_w)
    [[nodiscard]] constexpr auto min_weight_hamiltonian_path(const digraph_t &digraph, std::vector<std::int32_t> &path,
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

        constexpr int_t one = 1;
        {
            auto &cur = maps[0];

            for (std::int32_t index{}; index < node_count; ++index)
            {// A single vertex.
                const auto mask = static_cast<int_t>(one << index);
                assert(mask != 0U);

                cur[std::make_pair(mask, index)] = 0;
            }
        }

        const auto edge_mask = one << node_count;
        assert(0U < edge_mask);

        constexpr auto is_cycle = false;

        Inner::min_weight_ham_path_main<digraph_t, weight_t, int_t, map_t, infinity_w, is_cycle>(
            digraph, maps, shall_restore_path);

        const auto full_mask = edge_mask - one;
        assert(full_mask != 0U);

        // min_weight = min(t[(1LLU << n) - 1][to] for all 'to')
        auto min_weight = infinity_w;
        auto last_node = -1;
        {
            const auto &cur = maps[Inner::restore_index(shall_restore_path, node_count - 1)];

            for (std::int32_t tod{}; tod < node_count; ++tod)
            {
                const auto par = std::make_pair(full_mask, tod);
                const auto ite = cur.find(par);
                if (ite == cur.end())
                {
                    continue;
                }

                const auto &parent_cost = ite->second;
                assert(weight_t{} <= parent_cost && parent_cost < infinity_w);

                if (!(parent_cost < min_weight))
                {
                    continue;
                }

                min_weight = parent_cost;
                last_node = tod;
            }
        }

        assert(weight_t{} <= min_weight && min_weight < infinity_w && !(last_node < 0) && last_node < node_count);

        if (shall_restore_path && min_weight < infinity_w && !(last_node < 0) && last_node < node_count)
        {
            Inner::restore_ham_path<digraph_t, weight_t, int_t, map_t, infinity_w>(
                digraph, maps, path, min_weight, full_mask, last_node, shall_restore_path);
        }

        if (min_weight == infinity_w)
        {
            path.clear();
        }

        return min_weight;
    }

    // Draft slow.
    template<class digraph_t, class weight_t, std::unsigned_integral int_t,
        // std::unordered_map<std::pair<int_t, std::int32_t>, weight_t, Standard::Algorithms::Numbers::pair_hash>
        class map_t, weight_t infinity_w = std::numeric_limits<weight_t>::max(),
        std::int32_t max_nodes = ::Standard::Algorithms::Utilities::graph_max_nodes<int_t>>
    requires(0 < max_nodes && max_nodes <= ::Standard::Algorithms::Utilities::graph_max_nodes<int_t> &&
        weight_t{} <= infinity_w)
    [[nodiscard]] constexpr auto min_weight_hamiltonian_path_draft(
        const digraph_t &digraph, std::vector<std::int32_t> &path, map_t &map1) -> weight_t
    {
        const auto node_count = static_cast<std::int32_t>(digraph.size());
        require_less_equal(node_count, max_nodes, "node count");
        map1.clear();
        path.clear();

        if (node_count <= 1)
        {
            if (node_count)
            {
                path.push_back(0);
            }

            return {};
        }

        constexpr int_t one = 1;

        for (std::int32_t index{}; index < node_count; ++index)
        {// A single vertex.
            const auto mask = static_cast<int_t>(one << index);
            assert(mask != 0U);

            map1[std::make_pair(mask, index)] = 0;
        }

        const auto edge_mask = static_cast<int_t>(one << node_count);
        assert(edge_mask != 0U);

        for (auto cnt = 2; cnt <= node_count; ++cnt)
        {
            auto mask = static_cast<int_t>((one << cnt) - one);

            do
            {
                assert(mask != 0U);

                Inner::process_mask_draft<digraph_t, weight_t, int_t, map_t, infinity_w>(digraph, map1, mask);

                const auto next_mask = Standard::Algorithms::Numbers::next_greater_same_pop_count(mask);
                assert(mask < next_mask);

                mask = next_mask;
            } while (mask < edge_mask);
        }

        const auto full_mask = static_cast<int_t>(edge_mask - one);
        assert(full_mask != 0U);

        auto min_weight = infinity_w;
        auto last_node = -1;

        for (std::int32_t tod{}; tod < node_count; ++tod)
        {
            const auto par = std::make_pair(full_mask, tod);
            const auto ite = map1.find(par);
            if (ite == map1.end())
            {
                continue;
            }

            const auto &parent_cost = ite->second;
            assert(weight_t{} <= parent_cost && parent_cost < infinity_w);

            if (!(parent_cost < min_weight))
            {
                continue;
            }

            min_weight = parent_cost;
            last_node = tod;
        }

        assert(weight_t{} <= min_weight && min_weight < infinity_w && !(last_node < 0) && last_node < node_count);

        if (min_weight < infinity_w && !(last_node < 0))
        {
            Inner::restore_path_slow<digraph_t, weight_t, int_t, map_t, infinity_w>(
                digraph, map1, path, min_weight, full_mask, last_node);
        }

        if (min_weight == infinity_w)
        {
            path.clear();
        }

        return min_weight;
    }

    // Slow.
    template<class digraph_t, class weight_t, weight_t infinity_w = std::numeric_limits<weight_t>::max(),
        std::int32_t max_nodes = ::Standard::Algorithms::Utilities::graph_max_nodes<std::int32_t>>
    requires(0 < max_nodes && max_nodes <= ::Standard::Algorithms::Utilities::graph_max_nodes<std::int32_t> &&
        weight_t{} < infinity_w)
    [[nodiscard]] constexpr auto min_weight_hamiltonian_path_slow(
        std::vector<std::int32_t> &path, const digraph_t &digraph, std::vector<std::int32_t> &temp) -> weight_t
    {
        const auto node_count = static_cast<std::int32_t>(digraph.size());
        require_less_equal(node_count, max_nodes, "node count");
        path.clear();

        if (node_count <= 1)
        {
            if (node_count)
            {
                path.push_back(0);
            }

            return {};
        }

        path.resize(node_count);
        temp.resize(node_count);
        std::iota(temp.begin(), temp.end(), 0);

        auto min_weight = infinity_w;

        do
        {
            weight_t weight{};

            const auto has = Inner::calc_path_weight_slow<digraph_t, weight_t, infinity_w>(digraph, temp, weight);

            if (!has || !(weight < min_weight))
            {
                continue;
            }

            min_weight = weight;
            std::copy(temp.cbegin(), temp.cend(), path.begin());
        } while (std::next_permutation(temp.begin(), temp.end()));

        assert(weight_t{} <= min_weight && min_weight <= infinity_w);

        if (min_weight == infinity_w)
        {
            path.clear();
        }

        return min_weight;
    }
} // namespace Standard::Algorithms::Graphs
