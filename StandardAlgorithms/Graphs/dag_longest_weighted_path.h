#pragma once
#include"../Utilities/require_utilities.h"
#include"topological_sort.h"
#include<vector>

namespace Standard::Algorithms::Graphs::Inner
{
    // Set the cost backwards.
    // Only for non-negative weights!
    // Faster than "topological sort" + visits
    template<class long_int_t, class dag_t>
    struct dag_context final
    {
        constexpr dag_context(const dag_t &dag, const std::size_t source_vertex, const long_int_t &minus_infinity)
            : Dag(dag)
            , Costs(dag.size(), minus_infinity)
            , Saw(dag.size())
        {
            const auto size = Dag.size();

            for (std::size_t index{}; index < size; ++index)
            {
                if (Dag[index].empty())
                {
                    Costs[index] = {};
                }
            }

            Costs[source_vertex] = minus_infinity;
        }

        [[nodiscard]] constexpr auto costs() const &noexcept -> const std::vector<long_int_t> &
        {
            return Costs;
        }

        constexpr void visit(const std::size_t parent)
        {
            auto &best = Costs.at(parent);
            Saw[parent] = true;

            for (const auto &edges = Dag[parent]; const auto &edge : edges)
            {
                const auto &tod = edge.vertex;
                if (!Saw[tod])
                {
                    visit(tod);
                }

                const auto cand = static_cast<long_int_t>(Costs[tod] + edge.weight);
                best = std::max(best, cand);
            }
        }

private:
        const dag_t &Dag;
        std::vector<long_int_t> Costs;
        std::vector<bool> Saw;
    };

    template<class dag_t>
    constexpr auto has_graph_negative_weight(const dag_t &dag) -> bool
    {
        const auto ite = std::find_if(dag.cbegin(), dag.cend(),
            [] [[nodiscard]] (const auto &edges)
            {
                const auto ite_2 = std::find_if(edges.cbegin(), edges.cend(),
                    [] [[nodiscard]] (const auto &edge)
                    {
                        return edge.weight < 0;
                    });

                return ite_2 != edges.cend();
            });

        auto has = ite != dag.cend();
        return has;
    }
} // namespace Standard::Algorithms::Graphs::Inner

namespace Standard::Algorithms::Graphs
{
    // Cost from any to any vertex.
    // There must be at least 1 edge.
    // Vertices are [0..(vertex count - 1)].
    // The vertex count must be >= 2.
    template<class long_int_t, class dag_t>
    constexpr auto dag_longest_weighted_path(
        const dag_t &dag, const long_int_t &minus_infinity = std::numeric_limits<long_int_t>::min()) -> long_int_t
    {
        const auto size = require_greater(dag.size(), 1U, "digraph.size.");

        const auto ordered = topological_sort(dag);
        if (!ordered.has_value()) [[unlikely]]
        {
            throw std::invalid_argument("The graph must have been a DAG.");
        }

        std::vector<long_int_t> costs(size, minus_infinity);
        std::vector<bool> has_in(size);

        for (std::size_t index{}; index < size; ++index)
        {
            for (const auto &edges = dag[index]; const auto &edge : edges)
            {
                has_in[edge.vertex] = true;
            }
        }

        for (std::size_t index{}; index < size; ++index)
        {
            if (!has_in[index] && !dag[index].empty())
            {
                costs[index] = {};
            }
        }

        for (const auto &orders = ordered.value(); const auto &order : orders)
        {
            const auto current_cost = std::max(long_int_t{}, costs[order]);

            for (const auto &edges = dag[order]; const auto &edge : edges)
            {
                const auto cand = static_cast<long_int_t>(current_cost + edge.weight);
                auto &cur = costs[edge.vertex];
                cur = std::max(cur, cand);
            }
        }

        auto result = minus_infinity;
        for (std::size_t index{}; index < size; ++index)
        {
            if (has_in[index])
            {
                result = std::max(result, costs[index]);
            }
        }

        return result;
    }

    // Return the maximum cost from the source to the destination.
    // When there is no path, return minus infinity.
    // When "destination_vertex" is -1, any vertex, reachable from the source, can be used.
    template<class long_int_t, class dag_t>
    constexpr auto dag_longest_weighted_path_slow(std::size_t source_vertex, const dag_t &dag,
        std::size_t destination_vertex = std::numeric_limits<std::size_t>::max(),
        const long_int_t &minus_infinity = std::numeric_limits<long_int_t>::min()) -> long_int_t
    {
        const auto size = require_greater(dag.size(), 1U, "digraph size");
        require_greater(size, source_vertex, "source vertex");

        if (source_vertex == destination_vertex)
        {
            return {};
        }

        if (dag[source_vertex].empty())
        {
            return minus_infinity;
        }

        const auto ordered = topological_sort_via_dfs(dag);
        if (!ordered.has_value()) [[unlikely]]
        {
            throw std::invalid_argument("The graph must have been acyclic.");
        }

        std::vector<long_int_t> costs(size, minus_infinity);
        costs[source_vertex] = {};

        auto result = minus_infinity;

        for (const auto &orders = ordered.value(); const auto &order : orders)
        {
            const auto current_cost = costs[order];
            if (destination_vertex == order)
            {
                return current_cost;
            }

            if (minus_infinity == current_cost)
            {
                continue;
            }

            for (const auto &edges = dag[order]; const auto &edge : edges)
            {
                const auto cand = static_cast<long_int_t>(current_cost + edge.weight);
                result = std::max(result, cand);

                auto &cur = costs[edge.vertex];
                cur = std::max(cur, cand);
            }
        }

        return result;
    }

    // The max/min cost in DAG with non-negative weights:
    // just use the DFS as the topological sort isn't needed here.
    template<class long_int_t, class dag_t>
    constexpr auto dag_longest_weighted_path_source(std::size_t source_vertex, const dag_t &dag,
        std::size_t destination_vertex = std::numeric_limits<std::size_t>::max(),
        const long_int_t &minus_infinity = std::numeric_limits<long_int_t>::min()) -> long_int_t
    {
        const auto size = require_greater(dag.size(), 1U, "digraph size");
        require_greater(size, source_vertex, "source vertex");

        if (source_vertex == destination_vertex)
        {
            return {};
        }

        if (dag[source_vertex].empty())
        {
            return minus_infinity;
        }

        if (const auto is_slow = destination_vertex < size || Inner::has_graph_negative_weight<dag_t>(dag); is_slow)
        {
            auto result1 = dag_longest_weighted_path_slow<long_int_t, dag_t>(
                source_vertex, dag, destination_vertex, minus_infinity);

            return result1;
        }

        Inner::dag_context<long_int_t, dag_t> context{ dag, source_vertex, minus_infinity };
        context.visit(source_vertex);

        const auto &result = context.costs()[source_vertex];
        return result;
    }
} // namespace Standard::Algorithms::Graphs
