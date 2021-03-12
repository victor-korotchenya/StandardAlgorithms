#pragma once
#include"../Utilities/require_utilities.h"
#include<vector>

namespace Standard::Algorithms::Graphs::Inner
{
    template<std::signed_integral weight_t, class digraph_t>
    [[nodiscard]] constexpr auto build_complete_bipartite_graph(
        const digraph_t &digraph, const weight_t &inf, const std::int32_t size) -> std::vector<std::vector<weight_t>>
    {
        constexpr weight_t zero{};

        assert(-inf < zero && zero < inf && 0 < size);

        std::vector<std::vector<weight_t>> graph_2(size * 2LL, std::vector<weight_t>(size * 2LL, inf));

        for (std::int32_t from{}; from < size; ++from)
        {
            const auto &source_edges = digraph.at(from);
            auto &edges_2 = graph_2.at(from);

            for (const auto &source_edge : source_edges)
            {
                const auto tod = static_cast<std::int32_t>(source_edge);
                const auto &weight = source_edge.weight;
                auto negate_weight = static_cast<weight_t>(-weight);
                assert(from != tod && !(weight < zero) && !(zero < negate_weight));

                edges_2.at(tod + size) = std::move(negate_weight);
            }

            edges_2.at(from + size) = graph_2.at(from + size).at(from) = zero;
        }

        return graph_2;
    }

    inline constexpr void restore_cycles(const std::vector<std::int32_t> &matchings_2, const std::int32_t size,
        std::vector<std::vector<std::int32_t>> &cycles)
    {
        assert(0 < size);

        cycles.clear();

        std::vector<bool> useds(size * 2LL);

        for (std::int32_t row{}; row < size * 2LL; ++row)
        {
            const auto &col_2 = matchings_2.at(row);
            if (useds.at(col_2))
            {
                continue;
            }

            auto &cycle = cycles.emplace_back();
            auto count = -size;
            auto col = col_2;

            do
            {
                if (size < count++) [[unlikely]]
                {
                    throw std::runtime_error("Error restoring a cycle: too many pushes.");
                }

                assert(!useds.at(col));

                useds.at(col) = true;

                if (const auto vertex = col < size ? col : (col - size); cycle.empty() || cycle.back() != vertex)
                {
                    assert(!(vertex < 0) && vertex < size);

                    cycle.push_back(vertex);
                }

                assert(col_2 == row || col != matchings_2.at(col));
                col = matchings_2.at(col);
            } while (col_2 != col);

            assert(!cycle.empty());
        }
    }
} // namespace Standard::Algorithms::Graphs::Inner

namespace Standard::Algorithms::Graphs
{
    // Max weight cycle cover: given a non-negative weight simple digraph (or a graph),
    // split it into cycles of maximum total sum,
    // where each vertex belongs to maximum 1 possibly empty cycle.
    // Nodes start from 0.
    template<std::signed_integral weight_t, std::signed_integral long_int_t, class digraph_t, class perfect_matcher_t>
    requires(sizeof(weight_t) <= sizeof(long_int_t))
    [[nodiscard]] constexpr auto max_weight_cycle_cover(const digraph_t &digraph,
        std::vector<std::vector<std::int32_t>> &cycles, perfect_matcher_t perfect_matcher = {},
        const weight_t &inf = std::numeric_limits<weight_t>::max() / 2) -> long_int_t
    {
        {
            using wc_t = std::remove_cvref_t<decltype(digraph[0][0].weight)>;

            static_assert(std::is_same_v<weight_t, wc_t>);
        }

        if (constexpr auto large = 1'000'000U; large < digraph.size()) [[unlikely]]
        {
            throw std::invalid_argument("Too large (" + std::to_string(digraph.size()) + ") graph size.");
        }

        require_positive(inf, "infinite weight");

        constexpr weight_t zero{};
        assert(-inf < zero);

        const auto size = require_positive(static_cast<std::int32_t>(digraph.size()), "graph size");

        std::vector<std::int32_t> matchings_2;
        long_int_t max_weight{};

        {
            const auto graph_2 = Inner::build_complete_bipartite_graph<weight_t, digraph_t>(digraph, inf, size);

            {
                constexpr long_int_t one = 1;

                max_weight = static_cast<long_int_t>(perfect_matcher(graph_2, matchings_2) * -one);
            }

            if (max_weight < zero) [[unlikely]]
            {
                auto err =
                    "The computed bipartite graph max matchings weight is negative: " + std::to_string(max_weight);

                throw std::runtime_error(err);
            }

            if (const std::size_t first = matchings_2.size(), second = graph_2.size(); first != second) [[unlikely]]
            {
                auto err = "The computed bipartite graph max matchings size (" + std::to_string(first) +
                    ") must have been: " + std::to_string(second) + ".";

                throw std::runtime_error(err);
            }
        }

        Inner::restore_cycles(matchings_2, size, cycles);

        return max_weight;
    }
} // namespace Standard::Algorithms::Graphs
