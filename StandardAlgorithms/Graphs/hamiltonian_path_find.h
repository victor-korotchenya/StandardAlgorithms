#pragma once
#include"../Numbers/arithmetic.h"
#include"../Utilities/iota_vector.h"
#include"../Utilities/require_utilities.h"
#include"hamiltonian_path_count.h"
#include<numeric>
#include<unordered_map>
#include<unordered_set>

namespace Standard::Algorithms::Graphs::Inner
{
    template<class digraph_t, std::unsigned_integral int_t>
    constexpr auto build_out_nodes(const digraph_t &digraph) -> std::vector<int_t>
    {
        constexpr int_t one = 1;
        const auto node_count = static_cast<std::int32_t>(digraph.size());
        std::vector<int_t> masks(node_count);

        for (std::int32_t from{}; from < node_count; ++from)
        {
            const auto &edges = digraph[from];
            auto &mask = masks[from];

            for (const auto &edge : edges)
            {
                const auto &tod = static_cast<std::int32_t>(edge);
                assert(from != tod);

                mask |= one << tod;
            }
        }

        return masks;
    }

    template<class digraph_t, std::unsigned_integral int_t, class maps_t, class get_t>
    constexpr auto restore_found_path(const digraph_t &digraph, const maps_t &mps, get_t get)
        -> std::vector<std::int32_t>
    {
        const auto node_count = static_cast<std::int32_t>(digraph.size());

        constexpr int_t one = 1;

        const auto full_mask = static_cast<int_t>((one << node_count) - one);
        assert(full_mask != 0U && 1 < node_count);

        auto cnt1 = node_count - 1;

        if (const auto path_found = get(mps, cnt1).contains(full_mask); !path_found)
        {
            return {};
        }

        std::vector<std::int32_t> path;
        path.reserve(node_count);

        auto mask = full_mask;

        [[maybe_unused]] std::unordered_set<std::int32_t> ensure_unique_nodes;

        do
        {
            const auto &mmm = get(mps, cnt1);
            const auto ite = mmm.find(mask);
            assert(ite != mmm.end());

            std::int32_t tod{};
            auto mask2 = ite->second;

            do
            {
                assert(mask2);

                tod = static_cast<std::int32_t>(Standard::Algorithms::Numbers::most_significant_bit(mask2));

                assert((mask & (one << tod)) && !(tod < 0) && tod < node_count);

                if (path.empty())
                {
                    break;
                }

                mask2 ^= one << tod;

                // Ensure the edge exists.
                const auto &from = path.back();

                if (::Standard::Algorithms::Graphs::has_edge_slow<digraph_t>(from, digraph, tod))
                {
                    break;
                }
            } while (mask2);

            assert(mask & (one << tod));

            if constexpr (::Standard::Algorithms::is_debug)
            {
                const auto ins = ensure_unique_nodes.insert(tod);
                assert(ins.second);
            }

            path.push_back(tod);
            mask ^= one << tod;
            --cnt1;
        } while (mask);

        assert(path.size() == digraph.size() && cnt1 == -1);
        return path;
    }

    template<class digraph_t, std::unsigned_integral int_t, class map_t, class map_vector_t>
    constexpr void ham_pafi_main(const int_t &edge_mask, const std::vector<int_t> &out_nodes, const int_t &full_mask,
        const std::int32_t cnt, map_vector_t &mps)
    {
        constexpr int_t zero{};
        constexpr int_t one = 1;

        const auto &prev = mps[cnt];
        auto &cur = mps[cnt + 1];

        for (const auto &pre : prev)
        {
            const auto &mask = pre.first;
            assert(0 < mask && mask < full_mask);

            const auto &to_mask = pre.second;
            assert(0 < to_mask && to_mask < full_mask && (mask & to_mask));

            const int_t mask_cands = full_mask & (~mask);
            assert(0 < mask_cands && mask_cands < full_mask && !(mask & mask_cands));

            auto cands = mask_cands;
            do
            {
                const int_t low = cands & (zero - cands);
                assert(0 < cands && 0 < low);
                cands ^= low;

                const auto from = Standard::Algorithms::Numbers::most_significant_bit(low);

                assert(low == (one << from));

                const auto &from_mask = out_nodes[from];
                const int_t xxx = to_mask & from_mask;
                if (zero == xxx)
                {
                    continue;
                }

                // There is at least 1 edge 'from' -> 'to_mask'.
                const int_t mask_new = mask | low;
                assert(0 < mask_new && mask_new < edge_mask);

                auto &val = cur[mask_new];
                assert(!(val < 0) && val < edge_mask);

                val += low;
                assert(0 < val && val < edge_mask);
            } while (cands);
        }
    }
} // namespace Standard::Algorithms::Graphs::Inner

namespace Standard::Algorithms::Graphs
{
    // Simple connected directed graph.
    // Nodes start from 0.
    // If the path is not needed, use 2 maps (less space): prev and current.
    //
    // Idea to divide by O(n): a mask stores a set of vertices when n is small.
    // Space complexity O(2**n).
    // Time complexity O(2**n * n).
    //
    // out_nodes[i] for i=[0..n-1] is a number, marking edges from i.
    // For a graph [{0-1}, {0-3}, {2-0}],
    //   out_nodes[0] = 2**1 + 2**3 = 10,
    //   out_nodes[1] = out_nodes[3] = 0 (no out edges),
    //   out_nodes[2] = 2**0 = 1.
    //
    // The mask is the set of bits indicating which nodes form a path.
    // t[mask] is the node set that can start(!) the mask.
    // In the digraph '0->1', t[3] = 2**0 = 1;
    // digraph '1->0', t[3] = 2**1 = 2.
    // '1-0', t[3] = 2**0 + 2**1 = 3 - path can start from any node (clique).
    //
    // t[mask] is
    // 1) mask when (mask == (1LLU << from)) for all 'from' in [0..n-1].
    //    A single vertex.
    // 2) sum({t[mask ^ (1LLU << from)] * 2**from}) for all 'from'
    //    when (popcount(mask) >= 2 &&((mask >> from)  &1) &&
    //      // There is at least 1 edge 'from' -> 't[mask ^ (1LLU << from)]'
    //      (t[mask ^ (1LLU << from)]  &out_nodes[from]))
    // 3) 0 otherwise.
    //
    // A solution exists and can be restored when (t[(1LLU << n) - 1] != 0).
    template<class digraph_t, std::unsigned_integral int_t, class map_t = std::unordered_map<int_t, int_t>,
        class map_vector_t = std::vector<map_t>,
        std::int32_t max_nodes = ::Standard::Algorithms::Utilities::graph_max_nodes<int_t>>
    requires(0 < max_nodes && max_nodes <= ::Standard::Algorithms::Utilities::graph_max_nodes<int_t>)
    [[nodiscard]] constexpr auto hamiltonian_path_find(const digraph_t &digraph, map_vector_t &mps)
        -> std::vector<std::int32_t>
    {
        const auto node_count = require_less_equal(static_cast<std::int32_t>(digraph.size()), max_nodes, "node count");

        mps.clear();

        if (node_count <= 1)
        {
            // It is not an initializer list.
            // NOLINTNEXTLINE
            return std::vector<std::int32_t>(node_count, 0);
        }

        // Which outgoing nodes exist.
        const auto out_nodes = Inner::build_out_nodes<digraph_t, int_t>(digraph);

        constexpr int_t one = 1;
        {
            mps.resize(node_count);
            auto &cur = mps[0];

            for (std::int32_t index{}; index < node_count; ++index)
            {// A single vertex.
                const int_t mask = one << index;
                assert(mask);

                cur[mask] = mask;
            }
        }

        const int_t edge_mask = one << node_count;
        const int_t full_mask = edge_mask - one;
        assert(edge_mask && full_mask && full_mask < edge_mask);

        for (std::int32_t cnt{}; cnt < node_count - 1; ++cnt)
        {// (n * 2**n) main cycle.
            Inner::ham_pafi_main<digraph_t, int_t, map_t, map_vector_t>(edge_mask, out_nodes, full_mask, cnt, mps);
        }

        const auto get = [] [[nodiscard]] (const map_vector_t &ms_val, const std::int32_t ind) -> const map_t &
        {
            return ms_val[ind];
        };

        auto result = Inner::restore_found_path<digraph_t, int_t>(digraph, mps, get);
        return result;
    }

    // Draft, slow.
    template<class digraph_t, std::unsigned_integral int_t, class map_t = std::unordered_map<int_t, int_t>,
        std::int32_t max_nodes = ::Standard::Algorithms::Utilities::graph_max_nodes<int_t>>
    requires(0 < max_nodes && max_nodes <= ::Standard::Algorithms::Utilities::graph_max_nodes<int_t>)
    [[nodiscard]] constexpr auto hamiltonian_path_find_draft(const digraph_t &digraph, map_t &mps)
        -> std::vector<std::int32_t>
    {
        const auto node_count = static_cast<std::int32_t>(digraph.size());
        require_less_equal(node_count, max_nodes, "node count");

        mps.clear();
        if (node_count <= 1)
        {
            std::vector<std::int32_t> path(node_count);
            return path;
        }

        const auto out_nodes = Inner::build_out_nodes<digraph_t, int_t>(digraph);

        constexpr int_t zero{};
        constexpr int_t one = 1;
        const int_t edge_mask = one << node_count;
        assert(0U < edge_mask);

        for (auto mask = one; mask < edge_mask; ++mask)
        {// (2**n) main cycle.
            {
                const int_t low = mask & (zero - mask);
                const int_t prev_mask = mask ^ low;
                assert(low && prev_mask < mask);

                if (!prev_mask)
                {// A single vertex.
                    mps[mask] = mask;
                    continue;
                }
            }

            // At least 2 vertices.
            auto mask_cop = mask;
            int_t *val{};
            do
            {// Go thru all 'from' nodes - bits in mask.
                const int_t low = mask_cop & (zero - mask_cop);
                const int_t prev_mask = mask ^ low;
                assert(low && prev_mask && prev_mask < mask);

                mask_cop ^= low;

                const auto ite = mps.find(prev_mask);
                if (ite == mps.end())
                {
                    continue;
                }

                const auto from = Standard::Algorithms::Numbers::most_significant_bit(low);

                assert(low == (one << from));

                const auto &to_mask = ite->second;
                const auto &from_mask = out_nodes[from];
                const auto xxx = static_cast<int_t>(to_mask & from_mask);

                if (0U == xxx)
                {
                    continue;
                }

                // There is at least 1 edge 'from' -> 'to_mask'.
                if (val == nullptr)
                {
                    val = &(mps[mask]);
                }

                assert(val != nullptr);

                *val += low;

                assert(0 < *val);
            } while (mask_cop);
        }

        const auto get = [] [[nodiscard]] (const map_t &ms_val, std::int32_t) -> const map_t &
        {
            return ms_val;
        };

        auto result = Inner::restore_found_path<digraph_t, int_t>(digraph, mps, get);
        return result;
    }

    // Slow.
    template<class digraph_t, std::int32_t max_nodes = ::Standard::Algorithms::Utilities::graph_max_nodes<std::int32_t>>
    requires(0 < max_nodes && max_nodes <= ::Standard::Algorithms::Utilities::graph_max_nodes<std::int32_t>)
    [[nodiscard]] constexpr auto hamiltonian_path_find_slow(const digraph_t &digraph) -> std::vector<std::int32_t>
    {
        const auto node_count = require_less_equal(static_cast<std::int32_t>(digraph.size()), max_nodes, "node count");

        auto path = ::Standard::Algorithms::Utilities::iota_vector<std::int32_t>(node_count);

        do
        {
            if (Inner::is_path_slow<digraph_t>(digraph, path))
            {
                return path;
            }
        } while (std::next_permutation(path.begin(), path.end()));

        return {};
    }
} // namespace Standard::Algorithms::Graphs
