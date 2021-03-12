#pragma once
// "graph_helper.h"
#include"../Numbers/hash_utilities.h"
#include"../Utilities/test_utilities.h"
#include<numeric>
#include<unordered_set>
#include<vector>

namespace Standard::Algorithms::Graphs
{
    template<std::integral int_t>
    [[nodiscard]] constexpr auto is_valid_node(const int_t &node, const std::uint64_t graph_size) noexcept -> bool
    {
        constexpr int_t zero{};

        auto valid = !(node < zero) && static_cast<std::uint64_t>(node) < graph_size;
        return valid;
    }

    [[nodiscard]] constexpr auto count_edges(const auto &graph, const bool is_directed) noexcept -> std::size_t
    {
        auto cnt = std::accumulate(graph.begin(), graph.end(), std::size_t{},
            [] [[nodiscard]] (const std::size_t tot, const auto &edges)
            {
                return tot + edges.size();
            });

        assert(is_directed || (0U == (cnt & 1U)));

        cnt >>= is_directed ? 0U : 1U;
        return cnt;
    }

    template<class int_t, class edge_t>
    [[maybe_unused]] constexpr auto demand_edge_exists(
        const int_t from, const std::vector<std::vector<edge_t>> &graph, const int_t tod, const std::string &extra = {})
    {
        const auto &edges = graph[from];

        if (const auto ite = std::find(edges.cbegin(), edges.cend(), tod); edges.cend() != ite)
        {
            return ite;
        }

        auto str = ::Standard::Algorithms::Utilities::w_stream();
        str << "The cycle edge (" << from << ", " << tod << ") must belong to the graph.";

        if (!extra.empty())
        {
            str << ' ' << extra;
        }

        throw_exception(str);
    }

    template<class edge_t, bool require_non_negative_weights = false>
    constexpr void check_vertices(const std::int32_t size, const std::vector<edge_t> &edges, const std::string &name,
        std::unordered_set<std::int32_t> &uniques)
    {
        assert(!name.empty());

        uniques.clear();

        for (const auto &edge : edges)
        {
            const auto &tod = static_cast<std::int32_t>(edge);
            ::Standard::Algorithms::ert::greater(size, tod, "Too large vertex in " + name);

            if (const auto ins = uniques.insert(tod); !ins.second)
            {
                auto str = ::Standard::Algorithms::Utilities::w_stream();
                str << "The vertex " << tod << " must not repeat in " << name;
                throw_exception(str);
            }

            if constexpr (require_non_negative_weights)
            {
                using weight_t = std::remove_cvref_t<decltype(edge.weight)>;

                if constexpr (std::is_signed_v<weight_t>)
                {
                    const auto &weight = edge.weight;
                    require_non_negative(weight, "weight");
                }
            }
        }
    }

    // vertex indices are from 0 to (n-1).
    template<class edge_t, bool require_non_negative_weights = false>
    constexpr void require_simple_graph(const std::vector<std::vector<edge_t>> &graph)
    {
        const auto size = static_cast<std::int32_t>(graph.size());
        std::unordered_set<std::int32_t> uniques;

        for (std::int32_t from{}; from < size; ++from)
        {
            check_vertices<edge_t, require_non_negative_weights>(
                size, graph[from], "graph[" + std::to_string(from) + "]", uniques);

            const auto ite = uniques.insert(from);
            if (!ite.second)
            {
                auto str = ::Standard::Algorithms::Utilities::w_stream();
                str << "The vertex " << from << " is referring to itself (self-loop).";
                throw_exception(str);
            }
        }
    }

    // The graph is assumed to be simple.
    // vertex indices are from 0 to (n-1).
    template<class edge_t>
    constexpr void require_undirected_graph(const std::vector<std::vector<edge_t>> &graph)
    {
        const auto size = static_cast<std::int32_t>(graph.size());

        std::unordered_set<std::pair<std::int32_t, std::int32_t>, Standard::Algorithms::Numbers::pair_hash> uniques;

        for (std::int32_t from{}; from < size; ++from)
        {
            const auto &edges = graph[from];

            for (const auto &edge : edges)
            {
                const auto &tod = static_cast<std::int32_t>(edge);

                if (const auto reversed = std::make_pair(tod, from); uniques.erase(reversed))
                {
                    continue;
                }

                const auto dir = std::make_pair(from, tod);

                [[maybe_unused]] const auto ite = uniques.insert(dir);

                if constexpr (::Standard::Algorithms::is_debug)
                {
                    assert(ite.second);
                }
            }
        }

        if (uniques.empty())
        {
            return;
        }

        const auto &bad = *uniques.cbegin();
        auto str = ::Standard::Algorithms::Utilities::w_stream();
        str << "The edge " << bad.first << " - " << bad.second
            << " must have a reversed matching pair. Total mismatches: " << uniques.size();

        throw_exception(str);
    }

    // vertex indices are from 0 to (n-1).
    template<class edge_t>
    constexpr void require_connected_graph(
        const std::vector<std::vector<edge_t>> &graph, std::vector<bool> &vis, std::vector<std::int32_t> &que)
    {
        assert(0); // todo(p3): fix it.

        const auto size = static_cast<std::int32_t>(graph.size());
        if (size <= 1)
        {
            return;
        }

        que.clear();
        que.push_back(0);
        vis.assign(size, false);
        vis[0] = true;

        do
        {
            const auto from = que.back();
            que.pop_back();

            for (const auto &edges = graph[from]; const auto &edge : edges)
            {
                const auto &tod = static_cast<std::int32_t>(edge);
                if (!vis[tod])
                {
                    vis[tod] = true;
                    que.push_back(tod);
                }
            }
        } while (!que.empty());

        for (std::int32_t from{}; from < size; ++from)
        {
            if (!vis[from]) [[unlikely]]
            {
                throw std::runtime_error("Node " + std::to_string(from) + " must be reachable from 0.");
            }
        }
    }

    // vertex indices are from 0 to (n-1).
    template<class edge_t>
    constexpr void require_connected_graph(const std::vector<std::vector<edge_t>> &graph)
    {
        std::vector<std::int32_t> temp;
        std::vector<bool> vis;
        require_connected_graph<edge_t>(graph, vis, temp);
    }

    // Build a boolean n-by-n adjacency matrix from a digraph.
    template<class edge_t>
    constexpr void graph_list_to_adjacency_matrix(
        const std::vector<std::vector<edge_t>> &source, std::vector<std::vector<bool>> &destination)
    {
        const auto node_count = static_cast<std::int32_t>(source.size());
        destination.assign(node_count, std::vector<bool>(node_count, false));

        for (std::int32_t from{}; from < node_count; ++from)
        {
            const auto &edges_in = source[from];
            auto &edges_out = destination[from];

            for (const auto &edge : edges_in)
            {
                const auto tod = static_cast<std::int32_t>(edge);
                edges_out.at(tod) = true;
            }
        }
    }

    // vertex indices are from 0 to (n-1).
    // The adjacency matrix must be quadratic.
    template<class edge_t>
    constexpr void require_full_graph(
        const std::vector<std::vector<edge_t>> &graph, const bool demand_symmetric = false)
    {
        const auto size = graph.size();

        for (std::size_t from{}; from < size; ++from)
        {
            if (const auto cols = graph[from].size(); cols != size) [[unlikely]]
            {
                throw std::invalid_argument("Adjacency matrix at index " + std::to_string(from) + " size (" +
                    std::to_string(cols) + ") must be " + std::to_string(size) + ".");
            }
        }

        if (!demand_symmetric || size <= 1U)
        {
            return;
        }

        for (std::size_t from{}; from < size - 1U; ++from)
        {
            const auto &edges = graph[from];

            for (auto tod = from + 1U; tod < size; ++tod)
            {
                if (edges[tod] != graph[tod][from]) [[unlikely]]
                {
                    throw std::invalid_argument(
                        "Graph is not symmetric at [" + std::to_string(from) + ", " + std::to_string(tod) + "].");
                }
            }
        }
    }
} // namespace Standard::Algorithms::Graphs
