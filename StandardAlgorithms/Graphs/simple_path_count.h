#pragma once
#include"../Utilities/require_utilities.h"
#include"hamiltonian_path_count.h"
#include<numeric>
#include<vector>

namespace Standard::Algorithms::Graphs
{
    // A path has at least 2 nodes - add 'node_count' if necessary.
    // For an undirected graph, the count must be halved, and 2-node paths deleted.
    // See hamiltonian_path_count.h.
    //
    // count = sum(t[mask][to] for all 'to', 'mask' if popcount(mask) >= 2)
    template<class digraph_t, std::unsigned_integral int_t,
        // std::unordered_map<std::pair<int_t, std::int32_t>, int_t, Standard::Algorithms::Numbers::pair_hash>
        class map_t, std::int32_t max_nodes = ::Standard::Algorithms::Utilities::graph_max_nodes<int_t>>
    requires(0 < max_nodes && max_nodes <= ::Standard::Algorithms::Utilities::graph_max_nodes<int_t>)
    [[nodiscard]] constexpr auto simple_path_count(map_t &prev, const digraph_t &digraph, map_t &cur) -> int_t
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
        const auto edge_mask = static_cast<int_t>(one << node_count);
        assert(0U < edge_mask);

        int_t count{};

        for (std::int32_t index{}; index < node_count; ++index)
        {// A single vertex.
            const auto mask = static_cast<int_t>(one << index);
            assert(0U < mask);

            cur[std::make_pair(mask, index)] = 1U;
        }

        for (std::int32_t cnt{}; cnt < node_count - 1; ++cnt)
        {// (n * 2**n) main cycle.
            std::swap(cur, prev);
            cur.clear();

            for (const auto &pre : prev)
            {
                const auto &mask = pre.first.first;
                assert(0U < mask && mask < edge_mask);

                const auto &from = pre.first.second;
                assert(!(from < 0) && from < node_count && ((mask >> from) & one) != 0U);

                const auto &parent_count = pre.second;
                assert(0U < parent_count);

                const auto &edges = digraph[from];

                for (const auto &edge : edges)
                {// Try to add an edge.
                    const auto &tod = static_cast<std::int32_t>(edge);

                    if (const auto is_node_already_taken = ((mask >> tod) & one) != 0U; is_node_already_taken)
                    {
                        continue;
                    }

                    const auto mask2 = static_cast<int_t>((one << tod) | mask);
                    const auto key = std::make_pair(mask2, tod);
                    auto &val = cur[key];
                    val += parent_count;
                    assert(0U < val);

                    count += parent_count;
                    assert(0U < count);
                }
            }
        }

        return count;
    }

    // Slow.
    template<class digraph_t, class vector_map_t, std::unsigned_integral int_t,
        std::int32_t max_nodes = ::Standard::Algorithms::Utilities::graph_max_nodes<int_t>>
    requires(0 < max_nodes && max_nodes <= ::Standard::Algorithms::Utilities::graph_max_nodes<int_t>)
    [[nodiscard]] constexpr auto simple_path_count_slow(const digraph_t &digraph, std::vector<std::int32_t> &temp,
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

                    if (Inner::is_path_slow<digraph_t>(digraph, temp2))
                    {
                        map1.insert(temp2);
                    }
                }
            }
        } while (std::next_permutation(temp.begin(), temp.end()));

        const auto count = static_cast<int_t>(map1.size());
        return count;
    }
} // namespace Standard::Algorithms::Graphs
