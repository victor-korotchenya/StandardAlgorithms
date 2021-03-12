#pragma once
#include"../Numbers/disjoint_set.h"
#include<algorithm>
#include<cassert>
#include<cstddef>
#include<tuple>
#include<utility>
#include<vector>

namespace Standard::Algorithms::Graphs::Inner
{
    template<class to_weight_t, class weight_t>
    constexpr auto boruvka_relax_edge(to_weight_t &cand, const std::int32_t tod, const weight_t &new_weight) -> bool
    {
        if (cand.first < 0 || new_weight < cand.second) // todo(p3): introduce inf to remove "cand.first < 0 || ".
        {
            cand = std::make_pair(tod, new_weight);
            return true;
        }

        return false;
    }

    template<class weight_t, class from_to_weight_t>
    constexpr auto try_boruvka_relax(std::vector<from_to_weight_t> &editable_graph, const std::int32_t vertex_count,
        std::vector<std::pair<std::int32_t, weight_t>> &cands,
        Standard::Algorithms::Numbers::disjoint_set<std::int32_t> &dsu) -> bool
    {
        auto changed = false;
        cands.assign(vertex_count, std::pair<std::int32_t, weight_t>(-1, weight_t{}));

        for (std::size_t index{}; index < editable_graph.size();)
        {
            auto &edge = editable_graph[index];
            const auto &from = std::get<0>(edge);
            const auto &tod = std::get<1>(edge);

            assert(from != tod && !(std::min(from, tod) < 0) && std::max(from, tod) < vertex_count);

            if (dsu.are_connected(from, tod))
            {
                edge = editable_graph.back();
                editable_graph.pop_back();
                continue;
            }

            const auto &weight = std::get<2>(edge);
            if (boruvka_relax_edge(cands[from], tod, weight))
            {
                changed = true;
            }

            // Both edges might be relaxed.
            if (boruvka_relax_edge(cands[tod], from, weight))
            {
                changed = true;
            }

            ++index;
        }

        return changed;
    }

    template<class weight_t2, class weight_t, class from_to_weight_t>
    requires(sizeof(weight_t) <= sizeof(weight_t2))
    [[nodiscard]] constexpr auto boruvka_apply_cands(const std::int32_t vertex_count,
        std::vector<from_to_weight_t> &result_tree, const std::vector<std::pair<std::int32_t, weight_t>> &cands,
        Standard::Algorithms::Numbers::disjoint_set<std::int32_t> &dsu) -> weight_t2
    {
        weight_t2 weights{};

        [[maybe_unused]] auto changed = false;

        for (std::int32_t index{}; index < vertex_count; ++index)
        {
            const auto &cand = cands[index];
            const auto &to_ind = cand.first;
            assert(index != to_ind);

            if (to_ind < 0 || !dsu.unite(to_ind, index))
            {
                continue;
            }

            if constexpr (::Standard::Algorithms::is_debug)
            {
                changed = true;
            }

            const auto &from = std::min(index, to_ind);
            const auto &tod = std::max(index, to_ind);
            const auto &weight = cand.second;
            result_tree.emplace_back(from, tod, weight);
            weights += weight;
            assert(result_tree.size() < static_cast<std::size_t>(vertex_count));
        }

        if constexpr (::Standard::Algorithms::is_debug)
        {
            assert(changed);
        }

        return weights;
    }
} // namespace Standard::Algorithms::Graphs::Inner

namespace Standard::Algorithms::Graphs
{
    // The graph is undirected, weighted.
    // The graph will be edited.
    // 0-based vertices.
    // Time O((v + e)*log(v)).
    template<class weight_t2, class weight_t, class from_to_weight_t>
    requires(sizeof(weight_t) <= sizeof(weight_t2))
    [[nodiscard]] constexpr auto boruvka_mst(std::vector<from_to_weight_t> &editable_graph,
        const std::int32_t vertex_count, std::vector<from_to_weight_t> &result_tree) -> weight_t2
    {
        result_tree.clear();

        if (vertex_count <= 1 || editable_graph.empty())
        {
            return {};
        }

        std::vector<std::pair<std::int32_t, weight_t>> cands;
        Standard::Algorithms::Numbers::disjoint_set<std::int32_t> dsu(vertex_count);

        for (weight_t2 weights{};;)
        {
            if (!Inner::try_boruvka_relax<weight_t, from_to_weight_t>(editable_graph, vertex_count, cands, dsu))
            {
                return weights;
            }

            const auto weight = Inner::boruvka_apply_cands<weight_t2, weight_t, from_to_weight_t>(
                vertex_count, result_tree, cands, dsu);
            weights += weight;
        }
    }
} // namespace Standard::Algorithms::Graphs
