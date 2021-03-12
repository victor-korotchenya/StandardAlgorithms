#pragma once
#include <unordered_set>
#include <vector>
#include "../Utilities/ExceptionUtilities.h"
#include "../test_utilities.h"
#include "../Numbers/hash_utilities.h"

namespace Privet
{
    namespace Algorithms
    {
        namespace Graphs
        {
            template<class int_t, class edge_t>
            void demand_edge_exists(const std::vector<std::vector<edge_t>>& graph,
                const int_t from, const int_t to, const std::string& extra = {})
            {
                const auto& edges = graph[from];
                const auto it = std::find(edges.begin(), edges.end(), to);
                if (edges.end() != it)
                    return;

                std::ostringstream ss;
                ss << "The cycle edge (" << std::to_string(from) << ", " << std::to_string(to)
                    << ") must belong to the graph.";
                if (extra.size())
                    ss << ' ' << extra;

                StreamUtilities::throw_exception(ss);
            }

            template <typename t>
            void check_vertices(const int size,
                const std::vector<t>& vertices,
                const std::string& name,
                std::unordered_set<int>& uniques)
            {
                assert(name.size());
                uniques.clear();
                for (const auto& it : vertices)
                {
                    const auto& v = static_cast<int>(it);
                    Assert::Greater(size, v, "Too large vertex in " + name);
                    const auto ins = uniques.insert(v);
                    if (!ins.second)
                    {
                        std::ostringstream ss;
                        ss << "The vertex " << std::to_string(v) << " must not repeat in " << name;
                        StreamUtilities::throw_exception(ss);
                    }
                }
            }

            // vertex indices are from 0 to (n-1).
            template <class t>
            void require_simple_graph(const std::vector<std::vector<t>>& graph)
            {
                const auto size = static_cast<int>(graph.size());
                std::unordered_set<int> uniques;

                for (auto i = 0; i < size; i++)
                {
                    check_vertices(size, graph[i], "graph[" + std::to_string(i) + "]", uniques);

                    const auto it = uniques.insert(i);
                    if (!it.second)
                    {
                        std::ostringstream ss;
                        ss << "The vertex " << i << " is referring to itself (self-loop).";
                        StreamUtilities::throw_exception(ss);
                    }
                }
            }

            // The graph is assumed to be simple.
            // vertex indices are from 0 to (n-1).
            template <class t>
            void require_undirected_graph(const std::vector<std::vector<t>>& graph)
            {
                const auto size = static_cast<int>(graph.size());

                std::unordered_set<std::pair<int, int>, std::pair_hash> uniques;

                for (auto from = 0; from < size; from++)
                {
                    const auto& edges = graph[from];
                    for (const auto& edge : edges)
                    {
                        const auto& to = static_cast<int>(edge);
                        {
                            const auto reversed = std::make_pair(to, from);
                            if (uniques.erase(reversed))
                                continue;
                        }
                        const auto key = std::make_pair(from, to);
#if _DEBUG
                        const auto it =
#endif
                            uniques.insert(key);
#if _DEBUG
                        assert(it.second);
#endif
                    }
                }

                if (uniques.empty())
                    return;

                const auto& p = *uniques.begin();
                std::ostringstream ss;
                ss << "The edge " << p.first << " - " << p.second
                    << " must have a reversed matching pair. Total mismatches: "
                    << std::to_string(uniques.size());
                StreamUtilities::throw_exception(ss);
            }

            // vertex indices are from 0 to (n-1).
            template <class t>
            void require_connected_graph(const std::vector<std::vector<t>>& graph,
                std::vector<bool>& vis,
                std::vector<int>& q)
            {
                assert(0);// todo: p0. fix it.
                const auto size = static_cast<int>(graph.size());
                if (size <= 1) return;

                q.clear();
                q.push_back(0);
                vis.assign(size, false);
                vis[0] = true;
                do
                {
                    const auto from = q.back();
                    q.pop_back();
                    for (const auto& edge : graph[from])
                    {
                        const auto& to = static_cast<int>(edge);
                        if (!vis[to])
                        {
                            vis[to] = true;
                            q.push_back(to);
                        }
                    }
                } while (q.size());

                for (auto i = 0; i < size; ++i)
                {
                    if (!vis[i])
                        throw std::runtime_error("Node " + std::to_string(i) + " must be reachable from 0.");
                }
            }

            // vertex indices are from 0 to (n-1).
            template <typename t>
            void require_connected_graph(const std::vector<std::vector<t>>& graph)
            {
                std::vector<int> temp;
                std::vector<bool> vis;
                require_connected_graph<t>(graph, vis, temp);
            }

            // Build n by n adjacency matrix from a digraph.
            template <typename t>
            void graph_list_to_adjacency_matrix(const std::vector<std::vector<t>>& source,
                std::vector<std::vector<bool>>& target)
            {
                const auto node_count = static_cast<int>(source.size());
                target.assign(node_count, std::vector<bool>(node_count));

                for (auto from = 0; from < node_count; ++from)
                {
                    const auto& edges_in = source[from];
                    auto& edges_out = target[from];
                    for (const auto& e : edges_in)
                    {
                        const auto to = static_cast<int>(e);
                        edges_out[to] = true;
                    }
                }
            }

            // vertex indices are from 0 to (n-1).
            template <class v_t>
            void require_full_graph(const std::vector<std::vector<v_t>>& graph,
                const bool demand_symmetric = false)
            {
                const auto size = graph.size();

                for (size_t i = 0; i < size; ++i)
                {
                    const auto cs = graph[i].size();
                    if (cs != size)
                        throw std::invalid_argument("Adjacency matrix at index " + std::to_string(cs) + " size (" + std::to_string(cs) + ") must be " + std::to_string(size) + ".");
                }

                if (!demand_symmetric || !size)
                    return;

                for (size_t from = 0; from < size - 1; ++from)
                {
                    const auto& g_f = graph[from];
                    for (auto to = from + 1u; to < size; ++to)
                    {
                        if (g_f[to] != graph[to][from])
                            throw std::invalid_argument("Graph is not symmetric at [" + std::to_string(from) + ", " + std::to_string(to) + "].");
                    }
                }
            }
        }
    }
}