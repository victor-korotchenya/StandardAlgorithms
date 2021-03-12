#pragma once
#include"../Utilities/project_constants.h"
#include"../Utilities/require_utilities.h"
#include"vertex_utilities.h"
#include<functional>
#include<numeric>
#include<vector>

namespace Standard::Algorithms::Graphs::Inner
{
    template<class digraph_t>
    constexpr auto is_path_slow(const digraph_t &digraph, const std::vector<std::int32_t> &path) -> bool
    {
        const auto size = static_cast<std::int32_t>(path.size());

        for (std::int32_t index{}; index < size - 1; ++index)
        {
            const auto &from = path[index];
            const auto &tod = path[index + 1];
            assert(from != tod);

            if (!::Standard::Algorithms::Graphs::has_edge_slow<digraph_t>(from, digraph, tod))
            {
                return false;
            }
        }

        return true;
    }

    template<class digraph_t, std::unsigned_integral int_t, class map_t>
    constexpr void ham_paco_main(const digraph_t &digraph, map_t &prev, map_t &cur)
    {
        const auto node_count = static_cast<std::int32_t>(digraph.size());

        constexpr int_t one = 1;
        const auto edge_mask = one << node_count;
        assert(0U < edge_mask);

        for (std::int32_t cnt{}; cnt < node_count - 1; ++cnt)
        {// (n * 2**n) main cycle.
            std::swap(cur, prev);
            cur.clear();

            for (const auto &pre : prev)
            {
                const auto &mask = pre.first.first;
                assert(0 < mask && mask < edge_mask);

                const auto &from = pre.first.second;
                assert(0 <= from && from < node_count && ((mask >> from) & one));

                const auto &parent_count = pre.second;
                assert(0 < parent_count);

                const auto &edges = digraph[from];

                for (const auto &edge : edges)
                {// Try to add an edge.
                    const auto &tod = static_cast<std::int32_t>(edge);

                    if ((mask >> tod) & one)
                    {// A node is already taken.
                        continue;
                    }

                    const auto mask2 = (one << tod) | mask;
                    const auto par = std::make_pair(mask2, tod);
                    auto &val = cur[par];
                    val += parent_count;
                    assert(0 < val);
                }
            }
        }
    }
} // namespace Standard::Algorithms::Graphs::Inner

namespace Standard::Algorithms::Graphs
{
    // Simple connected directed graph.
    // Nodes start from 0.
    //
    // Space complexity O(2**n * n).
    // Time complexity O(2**n * n**2).
    //
    // t[mask][v] count of Hamiltonian-Rudrata paths, ending with v, in the masked vertices including v.
    //
    // t[mask][tod] is
    // 1) 1 when (mask == (1LLU << tod)) for all 'tod' in [0..n-1].
    //    A single vertex.
    // 2) sum({t[mask ^ (1LLU << tod)][from]) for
    //      all 'from', 'tod' in [0..n-1]}
    //    when (tod != from && ((mask >> tod)  &1) &&((mask >> from)  &1) &&
    //      edge (from, tod) exists)
    // 3) 0 otherwise.
    //
    // count = sum(t[(1LLU << n) - 1][tod] for all 'tod')
    //
    // It can be amended to count Hamiltonian-Rudrata cycles: start from 0-th node, ..
    template<class digraph_t, std::unsigned_integral int_t,
        // std::unordered_map<std::pair<int_t, std::int32_t>, int_t, Standard::Algorithms::Numbers::pair_hash>
        class map_t, std::int32_t max_nodes = ::Standard::Algorithms::Utilities::graph_max_nodes<int_t>>
    requires(0 < max_nodes && max_nodes <= ::Standard::Algorithms::Utilities::graph_max_nodes<int_t>)
    [[nodiscard]] constexpr auto hamiltonian_path_count(const digraph_t &digraph, map_t &prev, map_t &cur) -> int_t
    {
        prev.clear();
        cur.clear();

        const auto node_count = static_cast<std::int32_t>(digraph.size());
        require_less_equal(node_count, max_nodes, "node count");

        if (node_count <= 1)
        {
            return 1;
        }

        prev.reserve(node_count);
        cur.reserve(node_count);

        constexpr int_t one = 1;
        const auto edge_mask = one << node_count;
        assert(0U < edge_mask);

        for (std::int32_t index{}; index < node_count; ++index)
        {// A single vertex.
            const int_t mask = one << index;
            assert(mask != 0U);

            cur[std::make_pair(mask, index)] = 1;
        }

        Inner::ham_paco_main<digraph_t, int_t, map_t>(digraph, prev, cur);

        const auto full_mask = edge_mask - one;
        assert(full_mask);

        // count = sum(t[(1LLU << n) - 1][tod] for all 'tod')
        int_t count{};

        for (std::int32_t tod{}; tod < node_count; ++tod)
        {
            const auto par = std::make_pair(full_mask, tod);
            const auto ite = cur.find(par);
            if (ite == cur.end())
            {
                continue;
            }

            const auto &parent_count = ite->second;
            assert(0U < parent_count && 0U < count + parent_count);

            count += parent_count;
            assert(0U < count);
        }

        return count;
    }

    // Slow.
    template<class digraph_t, class int_t,
        std::int32_t max_nodes = ::Standard::Algorithms::Utilities::graph_max_nodes<int_t>>
    requires(0 < max_nodes && max_nodes <= ::Standard::Algorithms::Utilities::graph_max_nodes<int_t>)
    [[nodiscard]] constexpr auto hamiltonian_path_count_slow(const digraph_t &digraph, std::vector<std::int32_t> &temp)
        -> int_t
    {
        const auto node_count = static_cast<std::int32_t>(digraph.size());
        require_less_equal(node_count, max_nodes, "node count");

        if (node_count <= 1)
        {
            return 1;
        }

        temp.resize(node_count);
        std::iota(temp.begin(), temp.end(), 0);

        int_t count{};
        do
        {
            const auto iss = Inner::is_path_slow<digraph_t>(digraph, temp);
            count += iss ? 1 : 0;
        } while (std::next_permutation(temp.begin(), temp.end()));

        assert(int_t{} <= count);

        return count;
    }
} // namespace Standard::Algorithms::Graphs
