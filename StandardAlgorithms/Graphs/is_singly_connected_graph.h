#pragma once
#include"../Numbers/color_2bit.h"
#include"../Utilities/require_utilities.h"

namespace Standard::Algorithms::Graphs::Inner
{
    // Time O(n*log(n) + (# of self-loops)).
    [[maybe_unused]] constexpr auto is_sin_conn_graph_clear(auto &dag) -> bool
    {
        const auto size = dag.size();

        // A star graph on n>0 nodes will have (2*n-2) edges:
        // 0 <-> 1, 0 <-> 2, .., 0 <-> n-1.
        const auto max_edge_count = static_cast<std::uint64_t>(size) * 2U - 2U;
        assert(1U < size && size <= max_edge_count);

        std::size_t edge_count{};

        for (std::size_t from{}; from < size; ++from)
        {
            auto &edges = dag.at(from);

            for (std::size_t pos{}; pos < edges.size();)
            {
                auto &tod = edges.at(pos);

                if (const auto is_self_loop = tod == from; is_self_loop)
                {
                    tod = edges.back();
                    edges.pop_back();
                    continue;
                }

                require_greater(size, tod, "bad vertex to");
                ++pos;
            }

            edge_count += edges.size();

            if (max_edge_count < edge_count || !(edges.size() < size))
            {
                return false;
            }

            std::sort(edges.begin(), edges.end());

            if (const auto iter = std::adjacent_find(edges.cbegin(), edges.cend()); iter != edges.cend())
            {// A multi-edge means there are at least 2 paths between from and (*iter).
                return false;
            }
        }

        return true;
    }

    // Time O(n*n*log(n)).
    [[nodiscard]] constexpr auto is_sin_conn_graph_clear_slow(auto &dag) -> bool
    {
        const auto size = dag.size();

        for (std::size_t from{}; from < size; ++from)
        {
            auto &edges = dag.at(from);

            for (std::size_t pos{}; pos < edges.size();)
            {
                auto &tod = edges.at(pos);

                if (const auto is_self_loop = tod == from; is_self_loop)
                {
                    tod = edges.back();
                    edges.pop_back();
                    continue;
                }

                require_greater(size, tod, "bad vertex to");
                ++pos;
            }

            std::sort(edges.begin(), edges.end());

            if (const auto iter = std::adjacent_find(edges.cbegin(), edges.cend()); iter != edges.cend())
            {
                return false;
            }
        }

        return true;
    }

    // Time O(|V| + |E|).
    [[nodiscard]] constexpr auto is_sin_conn_graph_slow(const auto &dag, std::size_t node, auto &colors, auto &sta)
        -> bool
    {
        const auto size = dag.size();
        assert(node < size);

        sta.assign(1, std::make_pair(node, 0ZU));

        colors.white(size);
        colors.specific({ node, Standard::Algorithms::Numbers::grey });

        for (std::size_t position{};;)
        {
            assert(!sta.empty());

            node = sta.back().first;
            position = sta.back().second;
            assert(node < size && position <= size);

            sta.pop_back();

            for (;;)
            {
                const auto &edges = dag.at(node);
                if (position < edges.size())
                {
                    break;
                }

                if (sta.empty())
                {
                    return true;
                }

                if constexpr (::Standard::Algorithms::is_debug)
                {
                    [[maybe_unused]] const auto color = colors.color(node);
                    assert(color == Standard::Algorithms::Numbers::grey);
                }

                colors.specific({ node, Standard::Algorithms::Numbers::black });

                node = sta.back().first;
                position = sta.back().second;
                assert(node < size && position <= size);

                sta.pop_back();
            }

            {
                const auto &edges = dag.at(node);
                const auto &tod = edges.at(position);
                assert(node != tod);

                const auto color_to = colors.color(tod);

                if (const auto is_forward_or_cross_edge = Standard::Algorithms::Numbers::white < color_to;
                    is_forward_or_cross_edge)
                {
                    assert(color_to == Standard::Algorithms::Numbers::black);
                    return false;
                }

                sta.emplace_back(node, position + 1ZU);

                if (color_to == Standard::Algorithms::Numbers::white)
                {
                    colors.specific({ tod, Standard::Algorithms::Numbers::grey });
                    sta.emplace_back(tod, 0ZU);
                    continue;
                }

                // A back-edge.
                if constexpr (::Standard::Algorithms::is_debug)
                {
                    assert(color_to == Standard::Algorithms::Numbers::grey);
                }
            }
        }
    }
} // namespace Standard::Algorithms::Graphs::Inner

namespace Standard::Algorithms::Graphs
{
    // A digraph is singly-connected if any two distinct vertices are connected by maximum 1 simple path.
    // A simple path consists of distinct vertices. Thus, self-loops are ignored.
    // Still slow time O(n**2).
    [[nodiscard]] constexpr auto is_singly_connected_graph( // The graph might be edited.
        auto &dag) -> bool
    {
        const auto size = dag.size();
        if (size <= 1U)
        {
            return true;
        }

        if (!Inner::is_sin_conn_graph_clear(dag))
        {
            return false;
        }

        // Here (|E| <= 2*size - 2), which means that DFS takes a linear time O(|V|).

        using from_position_t = std::pair<std::size_t, std::size_t>;

        std::vector<from_position_t> sta;
        sta.reserve(size);

        Standard::Algorithms::Numbers::color_2bit colors;

        for (std::size_t root{}; root < size; ++root)
        {
            if (!dag.at(root).empty() && !Inner::is_sin_conn_graph_slow(dag, root, colors, sta))
            {
                return false;
            }
        }

        return true;
    }

    // Slow time O(n**3).
    [[nodiscard]] constexpr auto is_singly_connected_graph_slow( // The graph might be edited.
        auto &dag) -> bool
    {
        const auto size = dag.size();
        if (size <= 1U)
        {
            return true;
        }

        if (!Inner::is_sin_conn_graph_clear_slow(dag))
        {
            return false;
        }

        using from_position_t = std::pair<std::size_t, std::size_t>;

        std::vector<from_position_t> sta;
        sta.reserve(size);

        Standard::Algorithms::Numbers::color_2bit colors;

        for (std::size_t root{}; root < size; ++root)
        {
            if (!dag.at(root).empty() && !Inner::is_sin_conn_graph_slow(dag, root, colors, sta))
            {
                return false;
            }
        }

        return true;
    }
} // namespace Standard::Algorithms::Graphs
