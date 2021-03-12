#pragma once
#include<algorithm>
#include<cassert>
#include<concepts>
#include<functional> // bind
#include<utility>

namespace Standard::Algorithms::Graphs
{
    template<class edge_t>
    [[nodiscard]] constexpr auto is_edge_to(const edge_t &edge, const std::int32_t tod) -> bool
    {
        const auto &to_2 = static_cast<std::int32_t>(edge);
        return to_2 == tod;
    }

    // Weight is ignored.
    template<class digraph_t>
    [[nodiscard]] constexpr auto has_edge_slow(
        const std::int32_t from, const digraph_t &digraph, const std::int32_t tod) -> bool
    {
        const auto node_count = static_cast<std::int32_t>(digraph.size());

        assert(from != tod && !(from < 0) && from < node_count && !(tod < 0) && tod < node_count);

        const auto &edges = digraph.at(from);

        // todo(p3): faster search.
        const auto pred = std::bind(is_edge_to<decltype(digraph[0][0])>, std::placeholders::_1, tod);

        const auto iter = std::find_if(edges.cbegin(), edges.cend(), pred);
        auto has = iter != edges.cend();
        return has;
    }

    // Return infinity_w if no edge found.
    template<class digraph_t, class weight_t, weight_t infinity_w>
    requires(weight_t{} < infinity_w)
    [[nodiscard]] constexpr auto edge_weight_slow(
        const std::int32_t from, const digraph_t &digraph, const std::int32_t tod) -> weight_t
    {
        const auto node_count = static_cast<std::int32_t>(digraph.size());

        assert(from != tod && !(from < 0) && from < node_count && !(tod < 0) && tod < node_count);

        const auto &edges = digraph[from];

        const auto pred = std::bind(is_edge_to<decltype(digraph[0][0])>, std::placeholders::_1, tod);

        const auto iter = std::find_if(edges.cbegin(), edges.cend(), pred);
        if (iter == edges.cend())
        {
            return infinity_w;
        }

        const auto &edge = *iter;
        assert(edge.vertex == tod);

        const auto &edge_cost = edge.weight;
        assert(weight_t{} <= edge_cost && edge_cost <= infinity_w);

        return edge_cost;
    }
} // namespace Standard::Algorithms::Graphs
