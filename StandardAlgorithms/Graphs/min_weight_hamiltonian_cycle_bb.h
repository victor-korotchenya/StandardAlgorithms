#pragma once
#include"../Utilities/project_constants.h"
#include"../Utilities/require_utilities.h"
#include<array>
#include<optional>
#include<vector>

namespace Standard::Algorithms::Graphs::Inner
{
    template<class digraph_t, class long_weight_t, class weight_t, long_weight_t infinity_w>
    requires(sizeof(weight_t) <= sizeof(long_weight_t))
    struct min_ham_weight_bb_context final
    {
        // #pragma clang diagnostic push
        // #pragma ide diagnostic ignored "OCInconsistentNamingInspection"

        const digraph_t &digraph;
        long_weight_t min_weight = infinity_w;
        std::vector<std::int32_t> best_path{};
        std::vector<std::int32_t> current_path{};
        std::vector<bool> visited{};

        // Whether the edge {i, 0} exists, and its weight.
        std::vector<std::optional<weight_t>> edge_to_initial{};

        // #pragma clang diagnostic pop
    };

    template<class long_weight_t, long_weight_t infinity_w, class context_t, class edge_t>
    constexpr void try_set_edge_to_initial(context_t &context, const edge_t &edge, const std::int32_t from)
    {
        const auto &tod = edge.vertex;
        const auto &weight = edge.weight;

        assert(from != tod && long_weight_t{} <= weight && weight < infinity_w);

        if (constexpr std::int32_t initial_vertex{}; tod != initial_vertex)
        {
            return;
        }

        auto &opt = context.edge_to_initial[from];
        assert(!opt.has_value()); // A simple graph is expected.

        opt = std::make_optional(weight);
    }

    template<class long_weight_t, class weight_t, long_weight_t infinity_w, class context_t>
    constexpr auto validate_bb_edges(context_t &context, const std::int32_t from) -> bool
    {
        assert(from < static_cast<std::int32_t>(context.digraph.size()));

        const auto &edges = context.digraph[from];
        if (edges.empty())
        {
            // throw std::invalid_argument("The graph[" + std::to_string(from) + "] must be connected.");
            return false;
        }

        for (std::size_t index{}; index < edges.size(); ++index)
        {
            const auto &edge = edges[index];

            try_set_edge_to_initial<long_weight_t, infinity_w, context_t>(context, edge, from);

            if (0U < index && edge.weight < edges[index - 1U].weight) [[unlikely]]
            {
                throw std::invalid_argument("The digraph[" + std::to_string(from) + "][" + std::to_string(index) +
                    "] weights had better not decrease.");
            }
        }

        return true;
    }

    template<class context_t>
    constexpr auto from_min_weight(const context_t &context, const std::int32_t from) noexcept -> const auto &
    {
        assert(
            !(from < 0) && from < static_cast<std::int32_t>(context.digraph.size()) && !context.digraph[from].empty());

        return context.digraph[from][0].weight;
    }

    template<class long_weight_t, class weight_t, long_weight_t infinity_w, class context_t>
    constexpr auto validate_get_initial_bound(context_t &context) -> std::optional<long_weight_t>
    {
        assert(1U < context.digraph.size());

        long_weight_t sum{};

        for (std::int32_t from{}; from < static_cast<std::int32_t>(context.digraph.size()); ++from)
        {
            if (!validate_bb_edges<long_weight_t, weight_t, infinity_w, context_t>(context, from))
            {
                return std::nullopt;
            }

            const auto &mini = from_min_weight(context, from);
            sum += static_cast<long_weight_t>(mini);

            assert(weight_t{} <= mini && mini <= sum && sum < infinity_w);
        }

        return std::make_optional(sum);
    }

    template<class long_weight_t, class context_t>
    constexpr void try_cycle_bb(long_weight_t weight, context_t &context)
    {
        constexpr std::int32_t initial_vertex{};

        assert(!context.current_path.empty());

        const auto &last = context.current_path.back();
        assert(initial_vertex < last && last < static_cast<std::int32_t>(context.digraph.size()));

        const auto &has_weight = context.edge_to_initial[last];
        if (const auto has_edge = has_weight.has_value(); !has_edge)
        {
            return;
        }

        weight += static_cast<long_weight_t>(has_weight.value());

        if (!(weight < context.min_weight))
        {
            return;
        }

        context.min_weight = weight;

        constexpr auto already_initially_set = 1;

        std::copy(context.current_path.cbegin() + already_initially_set, context.current_path.cend(),
            context.best_path.begin() + already_initially_set);

        assert(context.best_path.front() == initial_vertex && context.best_path.back() != initial_vertex);
    }

    template<class long_weight_t, long_weight_t infinity_w, class context_t>
    requires(long_weight_t{} < infinity_w)
    constexpr void min_weight_hamiltonian_cycle_bb_rec(
        const long_weight_t current_weight, context_t &context, const long_weight_t bound, const std::int32_t level)
    {
        assert(0 < level && level < static_cast<std::int32_t>(context.digraph.size()));
        assert(long_weight_t{} <= current_weight && current_weight < infinity_w);
        assert(long_weight_t{} <= bound && bound < infinity_w);

        const auto &from = context.current_path.at(level - 1);
        assert(!(from < 0) && from < static_cast<std::int32_t>(context.digraph.size()));

        const auto &edges = context.digraph[from];

        for (const auto &edge : edges)
        {
            const auto &tod = edge.vertex;
            assert(!(tod < 0) && tod < static_cast<std::int32_t>(context.digraph.size()));

            if (context.visited[tod])
            {
                continue;
            }

            const auto current_2 = static_cast<long_weight_t>(current_weight + static_cast<long_weight_t>(edge.weight));

            const auto bound_2 =
                static_cast<long_weight_t>(bound - static_cast<long_weight_t>(from_min_weight(context, from)));

            assert(long_weight_t{} <= std::min(current_2, bound_2) && std::max(current_2, bound_2) < infinity_w);

            if (!(bound_2 + current_2 < context.min_weight))
            {
                continue;
            }

            context.current_path[level] = tod;

            if (level + 1 == static_cast<std::int32_t>(context.digraph.size()))
            {
                try_cycle_bb<long_weight_t, context_t>(current_2, context);
                continue;
            }

            context.visited[tod] = true;

            min_weight_hamiltonian_cycle_bb_rec<long_weight_t, infinity_w, context_t>(
                current_2, context, bound_2, level + 1);

            context.visited[tod] = false;
        }
    }
} // namespace Standard::Algorithms::Graphs::Inner

namespace Standard::Algorithms::Graphs
{
    // Slow branch and bound.
    // 0-based vertices.
    // For good heuristic, lower weights must come first.
    // todo(p3): If the graph is not strongly connected, return infinity_w.
    // todo(p3): better bound: instead of min(ix.weight), take min of half sum of 2 edges {x, i}, {i, y}
    //  where x != y; when no such x,y then the graph is disconnected - broken preconditions. Special case n=2.
    // todo(p3): try only the cheapest remaining edges - will have to temporarily erase the chosen.
    template<class digraph_t, class long_weight_t, class weight_t,
        long_weight_t infinity_w = std::numeric_limits<long_weight_t>::max() / 3>
    requires(sizeof(weight_t) <= sizeof(long_weight_t) && long_weight_t{} < infinity_w)
    [[nodiscard]] constexpr auto min_weight_hamiltonian_cycle_bb(
        const digraph_t &digraph, std::vector<std::int32_t> &path) -> long_weight_t
    {
        require_less_equal(
            digraph.size(), static_cast<std::size_t>(Standard::Algorithms::Utilities::stack_max_size), "node count");

        const auto node_count = static_cast<std::int32_t>(digraph.size());
        if (node_count <= 1)
        {
            path.assign(node_count, 0);
            return {};
        }

        using context_t = Inner::min_ham_weight_bb_context<digraph_t, long_weight_t, weight_t, infinity_w>;

        context_t context{ digraph };
        {
            constexpr std::int32_t initial_vertex{};

            context.best_path.assign(node_count, initial_vertex);
            context.current_path.assign(node_count, initial_vertex);
            context.visited.assign(node_count, false);
            context.visited[initial_vertex] = true;
            context.edge_to_initial.assign(node_count, std::nullopt);
        }

        const auto has_bound =
            Inner::validate_get_initial_bound<long_weight_t, weight_t, infinity_w, context_t>(context);
        if (!has_bound.has_value())
        {
            path.clear();
            return infinity_w;
        }

        constexpr auto level = 1;

        Inner::min_weight_hamiltonian_cycle_bb_rec<long_weight_t, infinity_w, context_t>(
            long_weight_t{}, context, has_bound.value(), level);

        const auto &min_weight = context.min_weight;
        if (min_weight == infinity_w)
        {
            // #pragma clang diagnostic push
            // #pragma ide diagnostic ignored "UnreachableCode"

            path.clear();

            // #pragma clang diagnostic pop
        }
        else
        {
            assert(long_weight_t{} <= min_weight && min_weight < infinity_w);
            path = std::move(context.best_path);
        }

        return min_weight;
    }
} // namespace Standard::Algorithms::Graphs
