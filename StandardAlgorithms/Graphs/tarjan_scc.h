#pragma once
#include<array>
#include<cstdint>
#include<vector>

namespace Standard::Algorithms::Graphs
{
    // for testing only.
    constexpr auto scc_max_size = 1001;

    // Input.
    [[nodiscard]] extern auto tarjan_graph_ptr() noexcept -> std::array<std::vector<std::int32_t>, scc_max_size> &;
    [[nodiscard]] extern auto tarjan_graph_size() noexcept -> std::int32_t &;

    // SCC Tarjan strongly connected components can be used to solve 2-SAT in 2-CNF:
    // - Build an implication graph, where (a | b) is modeled by (!a -> b) && (!b -> a).
    // - Find SCCs.
    // If a SCC contains both x, !x, then there is no solution.
    // For each SCC c in reverse topological order, for each node x in c where x is None, assign x=true.
    //
    // The input is a directed graph,
    // where the nodes indexes are from 0 to (n-1).
    // Self loops are not allowed.
    extern void tarjan_scc();

    // output.
    [[nodiscard]] extern auto tarjan_component_cnt() noexcept -> std::int32_t;
    [[nodiscard]] extern auto tarjan_vertex_to_component() noexcept -> const std::array<std::int32_t, scc_max_size> &;

    [[nodiscard]] extern auto tarjan_components() noexcept -> std::vector<std::vector<std::int32_t>> &;
} // namespace Standard::Algorithms::Graphs
