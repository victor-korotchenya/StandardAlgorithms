#pragma once

#include <algorithm>
#include <cassert>
#include <numeric>
#include <vector>
#include "../Utilities/ExceptionUtilities.h"
#include "hamiltonian_path_count.h" // is_path_slow

namespace Privet
{
    namespace Algorithms
    {
        namespace Graphs
        {
            // A path has at least 2 nodes - add 'node_count' if necessary.
            // For an undirected graph, the count must be halved, and 2-node paths deleted.
            // See hamiltonian_path_count.h.
            //
            // count = sum(t[mask][to] for all 'to', 'mask' if popcount(mask) >= 2)
            template<class digraph_t,
                class u_number_t,
                // std::unordered_map<std::pair<u_number_t, int>, u_number_t, pair_hash>
                class map_t,
                int max_nodes = (sizeof(u_number_t) << 3) - 1>
                u_number_t simple_path_count(const digraph_t& digraph,
                    map_t& prev, map_t& cur)
            {
                static_assert(std::is_unsigned_v<u_number_t> && max_nodes > 0 && max_nodes <= (sizeof(u_number_t) << 3) - 1);

                prev.clear();
                cur.clear();

                const auto node_count = static_cast<int>(digraph.size());
                RequireNotGreater(node_count, max_nodes, "node_count");
                if (node_count <= 1)
                    return 0;

                prev.reserve(node_count);
                cur.reserve(node_count);

                constexpr u_number_t one = 1;
                const auto edge_mask = one << node_count;
                assert(edge_mask > 0);

                u_number_t count = 0;

                for (auto i = 0; i < node_count; ++i)
                {// A single vertex.
                    const u_number_t mask = one << i;
                    assert(mask);
                    cur[std::make_pair(mask, i)] = 1;
                }

                for (auto cnt = 0; cnt < node_count - 1; ++cnt)
                {// (n * 2**n) main cycle.
                    std::swap(cur, prev);
                    cur.clear();

                    for (const auto& p : prev)
                    {
                        const auto& mask = p.first.first;
                        assert(mask > 0 && mask < edge_mask);

                        const auto& from = p.first.second;
                        assert(from >= 0 && from < node_count && ((mask >> from)& one));

                        const auto& parent_count = p.second;
                        assert(parent_count > 0);

                        const auto& edges = digraph[from];
                        for (const auto& e : edges)
                        {// Try to add an edge.
                            const auto& to = static_cast<int>(e);
                            if ((mask >> to) & one)
                                // A node is already taken.
                                continue;

                            const auto mask2 = (one << to) | mask;
                            const auto k = std::make_pair(mask2, to);
                            auto& val = cur[k];
                            val += parent_count;
                            assert(val > 0);

                            count += parent_count;
                            assert(count > 0);
                        }
                    }
                }

                assert(count >= 0);
                return count;
            }

            // Slow.
            template<class digraph_t, class vector_map_t, class number_t, int max_nodes = (sizeof(number_t) << 3) - 1>
            number_t simple_path_count_slow(const digraph_t& digraph,
                vector_map_t& m,
                std::vector<int>& temp,
                std::vector<int>& temp2)
            {
                static_assert(max_nodes > 0 && max_nodes <= (sizeof(number_t) << 3) - 1);

                const auto node_count = static_cast<int>(digraph.size());
                RequireNotGreater(node_count, max_nodes, "node_count");
                m.clear();
                if (node_count <= 1)
                    return 0;

                temp.resize(node_count);
                std::iota(temp.begin(), temp.end(), 0);
                temp2.reserve(node_count);

                constexpr number_t one = 1;
                do
                {
                    for (auto start = 0; start < node_count - 1; ++start)
                    {
                        constexpr auto demand_min_nodes = 2;
                        for (auto stop = start + demand_min_nodes; stop <= node_count; ++stop)
                        {
                            temp2.resize(stop - start);
                            std::copy(temp.begin() + start, temp.begin() + stop, temp2.begin());
                            const auto is = is_path_slow<digraph_t>(digraph, temp2);
                            if (is)
                                m.insert(temp2);
                        }
                    }
                } while (std::next_permutation(temp.begin(), temp.end()));

                const auto count = static_cast<number_t>(m.size());
                assert(count >= 0);
                return count;
            }
        }
    }
}