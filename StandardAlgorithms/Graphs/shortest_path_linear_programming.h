#pragma once
// "shortest_path_linear_programming.h"
#include"../Geometry/simplex_method.h"
#include"../Utilities/floating_point_type.h"
#include"../Utilities/require_utilities.h"
#include"dijkstra.h"
#include<numeric>
#include<optional>
#include<tuple>
#include<vector>

namespace Standard::Algorithms::Graphs::Inner
{
    constexpr auto shall_print_lin_prog = Standard::Algorithms::Geometry::Inner::shall_print_lin_prog;

    template<class weight_t, std::floating_point floating_t>
    requires(std::convertible_to<floating_t, weight_t> && std::convertible_to<weight_t, floating_t>)
    [[nodiscard]] constexpr auto to_weight(const floating_t &raw_value, const weight_t &inf) -> weight_t
    {
        if constexpr (::Standard::Algorithms::is_debug)
        {
            assert(weight_t{} < inf && -inf < weight_t{});
        }

        auto weight = static_cast<weight_t>(raw_value);

        if (!(-inf < raw_value && raw_value < inf) || !(-inf < weight && weight < inf)) [[unlikely]]
        {
            auto str = ::Standard::Algorithms::Utilities::w_stream();
            str << "The distance (cost, weight) is too large: " << raw_value;

            throw std::runtime_error(str.str());
        }

        return weight;
    }

    template<class weight_t, class graph_t, std::floating_point floating_t,
        class obj_coeff_rhs_t =
            std::tuple<std::vector<floating_t>, std::vector<std::vector<floating_t>>, std::vector<floating_t>>>
    requires(std::convertible_to<floating_t, weight_t> && std::convertible_to<weight_t, floating_t>)
    [[nodiscard]] constexpr auto shortest_path_prepare_matrices(const std::int32_t source, const graph_t &graph,
        const std::optional<std::int32_t> &raw_dest = {}) -> obj_coeff_rhs_t
    {
        const auto size = require_positive(static_cast<std::size_t>(graph.size()), "graph size");

        assert(0 <= source && static_cast<std::size_t>(source) < size);

        const auto is_dest = raw_dest.has_value();
        const auto dest = raw_dest.value_or(0);
        assert(0 <= dest && static_cast<std::size_t>(dest) < size);

        const auto edge_count = std::accumulate(graph.cbegin(), graph.cend(), std::size_t{},
            [] [[nodiscard]] (const std::size_t tota, const auto &edges)
            {
                auto sub_sum = tota + static_cast<std::size_t>(edges.size());
                return sub_sum;
            });

        if (0ZU == edge_count) [[unlikely]]
        {
            throw std::runtime_error("The graph must have edges.");
        }

        const auto total_variables = size + edge_count;
        assert(size < total_variables && edge_count < total_variables);

        std::vector<floating_t> objective_func_coefficients(total_variables);

        if (is_dest)
        {// The objective function is max distances[dest].
            objective_func_coefficients.at(dest) = 1;
        }
        else
        {// The objective function is max (distances[0] + .. distances[size - 1]).
            for (std::size_t col{}; col < size; ++col)
            {
                objective_func_coefficients.at(col) = 1;
            }
        }

        // An edge from -> tod adds a slack variable, and imposes a constraint:
        //   distances[tod] <= distances[from] + graph[from][tod].weight
        // Also, (distances[source] == 0) is an extra condition.
        std::vector<std::vector<floating_t>> coef_matrix(
            edge_count + 1ZU, std::vector<floating_t>(total_variables, floating_t{}));

        std::vector<floating_t> right_sides(coef_matrix.size());

        for (std::size_t from{}, ide{}; from < size; ++from)
        {
            for (const auto &edges = graph[from]; const auto &edge : edges)
            {
                const auto &tod = edge.vertex;

                assert(static_cast<std::size_t>(tod) < size && from != static_cast<std::size_t>(tod)); // No self-edges.

                {// distances[tod] - distances[from] <= graph[from][tod].weight
                    auto &coefs = coef_matrix.at(ide);
                    coefs[from] = -1;
                    coefs.at(tod) = 1;

                    const auto slack_var = size + ide;
                    coefs.at(slack_var) = 1;
                }
                {
                    const auto &weight = edge.weight;
                    right_sides.at(ide) = weight;
                }

                assert(ide < edge_count);
                ++ide;
            }
        }

        coef_matrix.back().at(source) = 1; // distances[source] == 0.

        if constexpr (shall_print_lin_prog)
        {
            auto str = ::Standard::Algorithms::Utilities::w_stream();
            str << " pair_shortest_path_linear_programming. Source " << source;
            if (is_dest)
            {
                str << ", one destination " << dest << "\n";
            }
            else
            {
                str << ", destinations are [0 .. " << (size - 1U) << "]\n";
            }

            ::Standard::Algorithms::print("graph", graph, str);

            ::Standard::Algorithms::print("objective_func_coefficients", objective_func_coefficients, str);

            ::Standard::Algorithms::print("coef_matrix", coef_matrix, str);
            ::Standard::Algorithms::print("right_sides", right_sides, str);

            std::cout << str.str();
        }

        return obj_coeff_rhs_t(std::move(objective_func_coefficients), std::move(coef_matrix), std::move(right_sides));
    }
} // namespace Standard::Algorithms::Graphs::Inner

namespace Standard::Algorithms::Graphs
{
    // Solve some shortest path problems using Linear programming.
    // todo(p4): Solve maximum flow, maximum matching using Linear programming.

    // todo(p3): If no path is found between the two points, return inf.
    // Negative weight cycles must not exist.
    template<class weight_t, class graph_t,
        std::floating_point floating_t =
            std::conditional_t<std::is_floating_point_v<weight_t>, weight_t, Standard::Algorithms::floating_point_type>>
    requires(std::convertible_to<floating_t, weight_t> && std::convertible_to<weight_t, floating_t>)
    [[nodiscard]] constexpr auto pair_shortest_path_linear_programming(const std::int32_t source, const graph_t &graph,
        const std::int32_t destination, const weight_t &inf = std::numeric_limits<weight_t>::max() / 2)
        -> std::pair<weight_t, ::Standard::Algorithms::Geometry::simplex_result>
    {
        const auto size = require_positive(static_cast<std::size_t>(graph.size()), "graph size");
        {
            constexpr auto over_size = static_cast<std::size_t>(std::numeric_limits<std::int32_t>::max() - 1);
            static_assert(0U < over_size);

            require_greater(over_size, size, "graph size");
        }

        require_less_equal(std::int32_t{}, source, "source");
        require_less_equal(std::int32_t{}, destination, "destination");

        require_greater(size, static_cast<std::size_t>(source), "graph size vs source");
        require_greater(size, static_cast<std::size_t>(destination), "graph size vs destination");

        require_positive(inf, "inf");
        require_greater(weight_t{}, -inf, "-inf");

        if (source == destination)
        {
            // todo(p4): There could be many paths / solutions.
            return std::make_pair(weight_t{}, ::Standard::Algorithms::Geometry::simplex_result::one_solution);
        }

        auto [objective_func_coefficients, coef_matrix, right_sides] =
            Inner::shortest_path_prepare_matrices<weight_t, graph_t, floating_t>(source, graph, destination);

        std::vector<floating_t> optimal_variables(size);

        const auto [max_weight, sim_res] = ::Standard::Algorithms::Geometry::simplex_method<floating_t>(
            objective_func_coefficients, coef_matrix, right_sides, &optimal_variables);

        if (floating_t{} != optimal_variables.at(source)) [[unlikely]]
        {
            auto err = "The distance to the source(" + std::to_string(source) +
                ") should not have changed error. Maybe the graph has a negative weight cycle.";

            throw std::runtime_error(err);
        }

        auto min_cost = Inner::to_weight<weight_t, floating_t>(max_weight, inf);

        return std::make_pair(min_cost, sim_res);
    }

    template<class weight_t, class graph_t,
        std::floating_point floating_t =
            std::conditional_t<std::is_floating_point_v<weight_t>, weight_t, Standard::Algorithms::floating_point_type>>
    requires(std::convertible_to<floating_t, weight_t> && std::convertible_to<weight_t, floating_t>)
    [[nodiscard]] constexpr auto single_source_shortest_path_linear_programming(const std::int32_t source,
        const graph_t &graph, std::vector<weight_t> &distances,
        const weight_t &inf = std::numeric_limits<weight_t>::max() / 2)
        -> ::Standard::Algorithms::Geometry::simplex_result
    {
        const auto size = require_positive(static_cast<std::size_t>(graph.size()), "graph size");
        {
            constexpr auto over_size = static_cast<std::size_t>(std::numeric_limits<std::int32_t>::max() - 1);
            static_assert(0U < over_size);

            require_greater(over_size, size, "graph size");
        }

        require_less_equal(std::int32_t{}, source, "source");
        require_greater(size, static_cast<std::size_t>(source), "graph size vs source");

        require_positive(inf, "inf");
        require_greater(weight_t{}, -inf, "-inf");

        auto [objective_func_coefficients, coef_matrix, right_sides] =
            Inner::shortest_path_prepare_matrices<weight_t, graph_t, floating_t>(source, graph);

        std::vector<floating_t> optimal_variables(size);

        const auto [max_weight, sim_res] = ::Standard::Algorithms::Geometry::simplex_method<floating_t>(
            objective_func_coefficients, coef_matrix, right_sides, &optimal_variables);

        if (floating_t{} != optimal_variables.at(source)) [[unlikely]]
        {
            auto err = "The distance to the source(" + std::to_string(source) +
                ") should not have changed error. Maybe the graph has a negative weight cycle.";

            throw std::runtime_error(err);
        }

        distances.resize(size);

        std::transform(optimal_variables.cbegin(), optimal_variables.cend(), distances.begin(),
            [&inf](const auto &weig)
            {
                auto cast_weight = Inner::to_weight<weight_t, floating_t>(weig, inf);
                return cast_weight;
            });

        return sim_res;
    }
} // namespace Standard::Algorithms::Graphs
