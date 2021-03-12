#pragma once
#include"../Geometry/simplex_method.h"
#include"../Numbers/bit_utilities.h"
#include"../Utilities/check_size.h"
#include"../Utilities/project_constants.h"
#include"../Utilities/require_utilities.h"
#include<iostream>
#include<tuple>
#include<vector>

namespace Standard::Algorithms::Graphs::Inner
{
    constexpr auto shall_print_lin_prog_mvc = false;

    template<class graph_t, std::unsigned_integral int_t>
    constexpr auto count_covered_edges(const graph_t &graph, std::vector<std::vector<bool>> &vis, const int_t mask)
        -> std::int32_t
    {
        const auto graph_size = graph.size();

        for (int_t from{}; from < graph_size; ++from)
        {
            vis[from].assign(graph_size, false);
        }

        std::int32_t covered_edges{};

        for (int_t from{}; from < graph_size; ++from)
        {
            if (((mask >> from) & 1U) == 0U)
            {
                continue;
            }

            const auto &edges = graph[from];

            for (const auto &tod : edges)
            {
                const auto mini = std::min<int_t>(from, tod);
                const auto maxi = std::max<int_t>(from, tod);

                if (vis[mini][maxi])
                {
                    continue;
                }

                vis[mini][maxi] = true;
                ++covered_edges;
            }
        }

        return covered_edges;
    }

    // If a vertex cover of candidate_vertex_count nodes exists, return the mask of the chosen nodes.
    // Else return 0.
    template<class graph_t, std::unsigned_integral int_t>
    constexpr auto vertex_cover_mask(const graph_t &graph, const std::int32_t edge_count,
        std::vector<std::vector<bool>> &vis, const std::int32_t candidate_vertex_count) -> int_t
    {
        const auto graph_size = graph.size();

        assert(0 < edge_count && 0 < candidate_vertex_count && 0U < graph_size &&
            static_cast<std::size_t>(candidate_vertex_count) <= graph_size);

        constexpr int_t one = 1;

        const auto edge_mask = static_cast<int_t>(one << graph_size);

        auto mask = static_cast<int_t>((one << candidate_vertex_count) - one);

        do
        {
            assert(0U < mask && mask < edge_mask);

            const auto covered_edges = count_covered_edges<graph_t, int_t>(graph, vis, mask);

            if (covered_edges == edge_count)
            {
                return mask;
            }

            const auto next_mask = Standard::Algorithms::Numbers::next_greater_same_pop_count(mask);

            assert(!(covered_edges < 0) && covered_edges < edge_count && mask < next_mask);

            mask = next_mask;
        } while (mask < edge_mask);

        return {};
    }

    template<std::floating_point floating_t, class graph_t,
        class obj_coeff_rhs_t =
            std::tuple<std::vector<floating_t>, std::vector<std::vector<floating_t>>, std::vector<floating_t>>>
    [[nodiscard]] constexpr auto mvc_prepare_matrices(const graph_t &graph, const std::size_t edge_count)
        -> obj_coeff_rhs_t
    {
        const auto size = static_cast<std::size_t>(graph.size());
        if (0ZU == size || 0ZU == edge_count || 0ZU < (edge_count & 1ZU)) [[unlikely]]
        {
            throw std::runtime_error(
                "The graph must be simple and undirected; the edge count must be positive and even.");
        }

        const auto total_variables = size + size + edge_count;
        assert(size < size + size && size + size < total_variables && edge_count < total_variables);

        constexpr floating_t one{ 1 };
        constexpr floating_t zero{};
        constexpr floating_t minus_one = -one;
        static_assert(minus_one < zero && zero < one);

        // The objective function is min (variables[0] + .. + variables[size - 1]) which
        // is the same as max (-variables[0] - .. - variables[size - 1]).
        std::vector<floating_t> objective_func_coefficients(total_variables);
        std::fill(objective_func_coefficients.begin(), objective_func_coefficients.begin() + size, minus_one);

        // For each i in [0..size), 0 <= variables[i] <= 1. Or variables[i] + variables[i + size] == 1.
        // Here a variable at size or greater is a slack variable.
        std::vector<std::vector<floating_t>> coef_matrix(
            size + edge_count, std::vector<floating_t>(total_variables, zero));
        std::vector<floating_t> right_sides(coef_matrix.size());

        for (std::size_t index{}; index < size; ++index)
        {
            auto &coefs = coef_matrix[index];
            coefs[index] = coefs[index + size] = right_sides[index] = one;
        }

        // For each ide-th edge from->tod, where (0 <= from < size) and (0 <= tod < size),
        // 1 <= (variables[from] + variables[tod]).
        // Or -variables[from] - variables[tod] + variables[size*2 + ide] == -1.
        // Note. Taking the minus sign before the "variables[size*2 + ide]" produces all 0 results which is wrong.
        // So, a slack variable is added, not subtracted.
        for (std::size_t from{}, ide{}; from < size; ++from)
        {
            for (const auto &edges = graph[from]; const auto &tod : edges)
            {
                assert(static_cast<std::size_t>(tod) < size && from != static_cast<std::size_t>(tod)); // No self-edges.

                {// -variables[from] - variables[tod] + variables[size*2 + ide] == -1.
                    auto &coefs = coef_matrix.at(ide + size);
                    coefs[from] = coefs.at(tod) = right_sides.at(ide + size) = minus_one;

                    const auto slack_var = size * 2ZU + ide;
                    coefs.at(slack_var) = one;
                }

                assert(ide < edge_count);
                ++ide;
            }
        }

        return obj_coeff_rhs_t(std::move(objective_func_coefficients), std::move(coef_matrix), std::move(right_sides));
    }

    constexpr void print_simplex_internals_mvc(
        const auto &objective_func_coefficients, const auto &coef_matrix, const auto &right_sides, auto &str)
    {
        if constexpr (!shall_print_lin_prog_mvc)
        {
            return;
        }

        ::Standard::Algorithms::print("objective_func_coefficients", objective_func_coefficients, str);
        ::Standard::Algorithms::print("coef_matrix", coef_matrix, str);
        ::Standard::Algorithms::print("right_sides", right_sides, str);
    }
} // namespace Standard::Algorithms::Graphs::Inner

namespace Standard::Algorithms::Graphs
{
    inline constexpr auto mvc_approx_ratio = 2;

    // Simple, undirected, unweighted graph, might be disconnected. Nodes start from 0.
    // If a graph is directed, add the missing back edges if any to make it undirected.
    // Vertex cover VC: for every edge either node is selected, possibly both.
    // Minimum vertex cover: select the minimum possible number of vertices to cover every edge.
    //
    // todo(p3): Small opt. While there is a node of degree 1, include its neighbor node to VC.
    // Time complexity O(2**n * n).
    template<class graph_t, std::unsigned_integral int_t>
    constexpr void minimum_vertex_cover_still(const graph_t &graph, const std::int32_t edge_count,
        std::vector<std::int32_t> &result,
        const std::pair<std::int32_t, std::int32_t> &left_right_estimate = {
            1, std::numeric_limits<std::int32_t>::max() })
    {
        assert(left_right_estimate.first <= left_right_estimate.second);

        const auto graph_size = Standard::Algorithms::Utilities::check_size<std::int32_t>("graph size", graph.size());
        {
            constexpr auto bits = static_cast<std::int32_t>(sizeof(int_t) << 3U);
            static_assert(0 < bits);

            require_greater(bits, graph_size, "node count");
        }

        require_non_negative(edge_count, "edge count");

        {
            const auto max_edge_count = graph_size * (graph_size - 1LL) / 2;
            require_less_equal(edge_count, max_edge_count, "edge count");
        }

        result.clear();

        if (edge_count <= 0 || graph_size <= 1)
        {
            return;
        }

        // Binary search for the vertex count minimum.
        std::vector<std::vector<bool>> vis(graph_size);

        auto left = std::max<std::int32_t>(1, left_right_estimate.first);
        auto right = std::min<std::int32_t>(graph_size, left_right_estimate.second);
        assert(0 < left && left <= right && right <= graph_size);

        int_t best_mask{};

        do
        {
            const auto mid = std::midpoint(left, right);
            const auto mask = Inner::vertex_cover_mask<graph_t, int_t>(graph, edge_count, vis, mid);

            if (mask != 0U)
            {
                if constexpr (::Standard::Algorithms::is_debug)
                {
                    const auto bit_count = std::popcount(mask);
                    assert(bit_count == mid);
                }

                right = mid - 1, best_mask = mask;
            }
            else
            {
                left = mid + 1;
            }
        } while (left <= right);

        if (best_mask == 0U) [[unlikely]]
        {
            throw std::runtime_error("Inner error in VC: zero best mask.");
        }

        for (std::int32_t index{}; index < graph_size; ++index)
        {
            if (((best_mask >> index) & 1U) != 0U)
            {
                result.push_back(index);
            }
        }
    }

    // Two-approximation for a directed or undirected graph.
    // It can be used to get a precise bound.
    // Time complexity O(n + m).
    template<class graph_t, std::integral int_t>
    constexpr void minimum_vertex_cover_2_approximation(const graph_t &graph, std::vector<int_t> &result)
    {
        result.clear();

        if (graph.empty())
        {
            return;
        }

        const auto graph_size = Standard::Algorithms::Utilities::check_size<int_t>("graph size", graph.size());

        std::vector<bool> visited(graph_size);

        for (int_t from{}; from < graph_size; ++from)
        {
            if (visited[from])
            {
                continue;
            }

            const auto &edges = graph[from];

            for (const auto &edge : edges)
            {
                const auto tod = static_cast<int_t>(edge);
                if (visited[tod])
                {
                    continue;
                }

                visited[from] = true;
                result.push_back(from);

                if (from != tod)
                {
                    visited[tod] = true;
                    result.push_back(tod);
                }

                break;
            }
        }
    }

    // It is also an approximation using 0-1 integer linear programing.
    // A relaxation is to use a floating point number between 0.0 and 1.0 instead of integers 0 or 1.
    // It can be adapted for a weighted graph, where an edge has a non-negative cost or weight.
    template<std::floating_point floating_t, class graph_t, std::integral int_t>
    constexpr void minimum_vertex_cover_2_relax(const graph_t &graph, std::vector<int_t> &result)
    {
        result.clear();

        const auto size = static_cast<std::size_t>(graph.size());
        const auto edge_count = std::accumulate(graph.begin(), graph.end(), std::size_t{},
            [] [[nodiscard]] (const std::size_t total, const auto &edges)
            {
                auto sub_sum = total + static_cast<std::size_t>(edges.size());
                return sub_sum;
            });
        if (size <= 1U || edge_count == 0U)
        {
            return;
        }

        auto [objective_func_coefficients, coef_matrix, right_sides] =
            Inner::mvc_prepare_matrices<floating_t, graph_t>(graph, edge_count);

        if constexpr (Inner::shall_print_lin_prog_mvc)
        {
            auto str = ::Standard::Algorithms::Utilities::w_stream();
            ::Standard::Algorithms::print("minimum_vertex_cover_2_relax Graph", graph, str);

            Inner::print_simplex_internals_mvc(objective_func_coefficients, coef_matrix, right_sides, str);
            std::cout << str.str();
        }

        std::vector<floating_t> optimal_variables(size);

        const auto [max_weight, sim_res] = ::Standard::Algorithms::Geometry::simplex_method<floating_t>(
            objective_func_coefficients, coef_matrix, right_sides, &optimal_variables);

        if constexpr (Inner::shall_print_lin_prog_mvc)
        {
            auto str = ::Standard::Algorithms::Utilities::w_stream();
            str << "minimum_vertex_cover_2_relax Result " << sim_res << ".\n";
            ::Standard::Algorithms::print("optimal_variables", optimal_variables, str);

            Inner::print_simplex_internals_mvc(objective_func_coefficients, coef_matrix, right_sides, str);
            std::cout << str.str();
        }

        if (sim_res != ::Standard::Algorithms::Geometry::simplex_result::one_solution &&
            sim_res != ::Standard::Algorithms::Geometry::simplex_result::many_solutions) [[unlikely]]
        {
            auto str = ::Standard::Algorithms::Utilities::w_stream();
            str << "Bad simplex result " << sim_res;

            throw std::runtime_error(str.str());
        }

        assert(size == optimal_variables.size());
        result.reserve(size);

        constexpr floating_t zero{};
        constexpr floating_t one{ 1 };
        constexpr floating_t half = one / floating_t{ 2 };
        static_assert(zero < half && half < one && half + half == one);

        for (std::size_t vertex{}; vertex < size; ++vertex)
        {
            const auto &opt = optimal_variables.at(vertex);
            if (opt < half)
            {
                continue;
            }

            result.push_back(static_cast<int_t>(vertex));
        }
    }

    // It may run faster on large graphs due to preliminary approximation.
    template<class graph_t, std::unsigned_integral int_t>
    constexpr void minimum_vertex_cover(
        const graph_t &graph, const std::int32_t edge_count, std::vector<std::int32_t> &result)
    {
        result.clear();

        if (graph.empty() || edge_count <= 0)
        {
            return;
        }

        const auto graph_size = Standard::Algorithms::Utilities::check_size<std::int32_t>("graph size", graph.size());

        minimum_vertex_cover_2_approximation<graph_t, std::int32_t>(graph, result);

        const auto *const appr_name = "approximation size";
        const auto approximation_size =
            Standard::Algorithms::Utilities::check_size<std::int32_t>(appr_name, result.size());
        require_positive(approximation_size, appr_name);
        require_less_equal(approximation_size, graph_size, "approximation size vs graph size");

        const std::pair<std::int32_t, std::int32_t> left_right_estimate{
            std::max<std::int32_t>(1, approximation_size / mvc_approx_ratio), approximation_size
        };

        minimum_vertex_cover_still<graph_t, int_t>(graph, edge_count, result, left_right_estimate);
    }
} // namespace Standard::Algorithms::Graphs
