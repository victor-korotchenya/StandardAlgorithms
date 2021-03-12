#pragma once
#include"../Numbers/array_utilities.h" // normalize_cycle
#include"../Utilities/require_utilities.h"
#include"dijkstra.h"
#include"hamiltonian_path_count.h" // is_path_slow
#include<numeric>

namespace Standard::Algorithms::Graphs::Inner
{
    template<class digraph_t>
    [[nodiscard]] constexpr auto is_cycle_slow(const digraph_t &digraph, const std::vector<std::int32_t> &cycle) -> bool
    {
        if (!::Standard::Algorithms::Graphs::Inner::is_path_slow<digraph_t>(digraph, cycle))
        {
            return false;
        }

        assert(1U < cycle.size());

        const auto &tod = cycle.at(0);
        const auto &from = cycle.back();
        assert(from != tod);

        const auto has = ::Standard::Algorithms::Graphs::has_edge_slow<digraph_t>(from, digraph, tod);

        return has;
    }

    template<std::unsigned_integral int_t, class map_t, class prev_t, class edges_t>
    [[nodiscard]] constexpr auto sicyco_proc_edges(
        const edges_t &edges, const std::vector<std::vector<bool>> &adj_matr, const prev_t &pre, map_t &cur) -> int_t
    {
        constexpr int_t zero{};
        constexpr int_t one = 1;

        const auto node_count = static_cast<std::int32_t>(adj_matr.size());
        const auto edge_mask = one << node_count;
        assert(0 < node_count && 0U < edge_mask);

        const auto &mask = pre.first.first;
        assert(0U < mask && mask < edge_mask);

        const auto low = static_cast<int_t>(mask & (zero - mask));
        assert(0U < low && low <= mask);

        const auto first_node = static_cast<std::int32_t>(Standard::Algorithms::Numbers::most_significant_bit(low));

        assert(!(first_node < 0) && first_node < node_count);

        const auto &from = pre.first.second;
        assert(!(from < 0) && from < node_count);
        assert(((mask >> from) & one) != 0U);

        const auto &parent_count = pre.second;
        assert(0U < parent_count);

        int_t count{};

        for (const auto &edge : edges)
        {// Try to add an edge.
            const auto &tod = static_cast<std::int32_t>(edge);
            assert(!(tod < 0) && tod < node_count);

            if (tod <= first_node || ((mask >> tod) & one) != 0U)
            // A node is smaller than first_node, or already taken.
            {
                continue;
            }

            const auto mask2 = static_cast<int_t>((one << tod) | mask);
            const auto key = std::make_pair(mask2, tod);
            auto &val = cur[key];
            val += parent_count;
            assert(0U < val);

            if (!adj_matr[tod][first_node])
            {
                continue; // no back edge.
            }

            count += parent_count;
            assert(0U < count);
        }

        return count;
    }

    template<class digraph_t, std::unsigned_integral int_t, class map_t>
    [[nodiscard]] constexpr auto sicyco_main(
        const digraph_t &digraph, map_t &prev, const std::vector<std::vector<bool>> &adj_matr, map_t &cur) -> int_t
    {
        const auto node_count = static_cast<std::int32_t>(digraph.size());

        int_t count{};

        std::swap(cur, prev);
        cur.clear();

        for (const auto &pre : prev)
        {
            const auto &from = pre.first.second;
            assert(!(from < 0) && from < node_count);

            const auto &edges = digraph[from];

            count += sicyco_proc_edges<int_t, map_t>(edges, adj_matr, pre, cur);
        }

        return count;
    }
} // namespace Standard::Algorithms::Graphs::Inner

namespace Standard::Algorithms::Graphs
{
    // A directed cycle has at least 2 nodes - add 'node_count' if necessary.
    // For an undirected graph, the count must be ..
    // Let the first node in a cycle be the minimum - avoid overcounting.
    //
    // count = sum(t[mask][to] for all 'to', 'mask' when (popcount(mask) >= 2) and
    //   there is an edge {'to' -> 'min masked edge'} and 'to' > 'min masked edge')
    template<class digraph_t, std::unsigned_integral int_t,
        // std::unordered_map<std::pair<int_t, std::int32_t>, int_t, Standard::Algorithms::Numbers::pair_hash>
        class map_t, std::int32_t max_nodes = ::Standard::Algorithms::Utilities::graph_max_nodes<int_t>>
    requires(0 < max_nodes && max_nodes <= ::Standard::Algorithms::Utilities::graph_max_nodes<int_t>)
    [[nodiscard]] constexpr auto simple_cycle_count(
        const digraph_t &digraph, map_t &prev, const std::vector<std::vector<bool>> &adj_matr, map_t &cur) -> int_t
    {
        assert(&prev != &cur);

        prev.clear();
        cur.clear();

        const auto node_count = static_cast<std::int32_t>(digraph.size());
        assert(node_count == static_cast<std::int32_t>(adj_matr.size()));

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

        for (std::int32_t index{}; index < node_count - 1; ++index)
        {// A single vertex - the last node cannot start a cycle.
            assert(node_count == static_cast<std::int32_t>(adj_matr[index].size()));

            const auto mask = static_cast<int_t>(one << index);
            assert(mask != 0U);

            cur[std::make_pair(mask, index)] = 1U;
        }

        int_t count{};

        for (std::int32_t cnt{}; cnt < node_count - 1; ++cnt)
        {// (n * 2**n) main cycle.
            count += Inner::sicyco_main<digraph_t, int_t, map_t>(digraph, prev, adj_matr, cur);
        }

        return count;
    }

    // Slow.
    template<class digraph_t, class vector_map_t, std::unsigned_integral int_t,
        std::int32_t max_nodes = ::Standard::Algorithms::Utilities::graph_max_nodes<int_t>>
    requires(0 < max_nodes && max_nodes <= ::Standard::Algorithms::Utilities::graph_max_nodes<int_t>)
    [[nodiscard]] constexpr auto simple_cycle_count_slow(const digraph_t &digraph, std::vector<std::int32_t> &temp,
        vector_map_t &map1, std::vector<std::int32_t> &temp2) -> int_t
    {
        const auto node_count = static_cast<std::int32_t>(digraph.size());
        require_less_equal(node_count, max_nodes, "node count");

        map1.clear();

        if (node_count <= 1)
        {
            return 1;
        }

        temp.resize(node_count);
        std::iota(temp.begin(), temp.end(), 0);

        temp2.reserve(node_count);

        do
        {
            for (std::int32_t start{}; start < node_count - 1; ++start)
            {
                constexpr auto demand_min_nodes = 2;

                for (auto stop = start + demand_min_nodes; stop <= node_count; ++stop)
                {
                    temp2.resize(stop - start);
                    std::copy(temp.cbegin() + start, temp.cbegin() + stop, temp2.begin());

                    if (!Inner::is_cycle_slow<digraph_t>(digraph, temp2))
                    {
                        continue;
                    }

                    Standard::Algorithms::Numbers::normalize_cycle(temp2);
                    map1.insert(temp2);
                }
            }
        } while (std::next_permutation(temp.begin(), temp.end()));

        const auto count = static_cast<int_t>(map1.size());
        return count;
    }
} // namespace Standard::Algorithms::Graphs
