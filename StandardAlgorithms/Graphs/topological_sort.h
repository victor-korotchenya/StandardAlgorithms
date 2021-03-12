#pragma once
#include"../Numbers/color_2bit.h"
#include"../Utilities/is_debug.h"
#include<algorithm>
#include<concepts>
#include<optional>
#include<queue>
#include<vector>

namespace Standard::Algorithms::Graphs::Inner
{
    [[nodiscard]] constexpr auto topol_sort_dfs(
        const auto &dag, std::size_t node, auto &colors, std::vector<std::size_t> &result, auto &sta) -> bool
    {
        const auto size = dag.size();
        assert(node < size);

        sta.assign(1, std::make_pair(node, 0ZU));

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

                if constexpr (::Standard::Algorithms::is_debug)
                {
                    const auto color = colors.color(node);
                    assert(color == Standard::Algorithms::Numbers::grey);
                }

                colors.specific({ node, Standard::Algorithms::Numbers::black });
                result.push_back(node);

                if (sta.empty())
                {
                    return true;
                }

                node = sta.back().first;
                position = sta.back().second;
                assert(node < size && position <= size);
                sta.pop_back();
            }

            {
                const auto &edges = dag.at(node);
                const auto tod = static_cast<std::size_t>(
                    // todo(p3): del casts
                    static_cast<std::int32_t>(edges.at(position)));
                assert(node != tod);

                const auto color_to = colors.color(tod);

                if (const auto is_back_edge = color_to < Standard::Algorithms::Numbers::white; is_back_edge)
                {// A back-edge indicates a cycle.
                    assert(color_to == Standard::Algorithms::Numbers::grey);
                    return false;
                }

                sta.emplace_back(node, position + 1ZU);

                if (color_to == Standard::Algorithms::Numbers::white)
                {
                    colors.specific({ tod, Standard::Algorithms::Numbers::grey });
                    sta.emplace_back(tod, 0ZU);
                }
                // else got a forward- or cross-edge.
            }
        }
    }
} // namespace Standard::Algorithms::Graphs::Inner

namespace Standard::Algorithms::Graphs
{
    // Return the vertices sorted in the topological order.
    // Vertices must be from 0 to (size - 1).
    // If the directed graph is not acyclic, return null opt.
    [[nodiscard]] constexpr auto topological_sort(const auto &dag) -> std::optional<std::vector<std::size_t>>
    {
        const auto size = dag.size();

        std::vector<std::size_t> result;
        result.reserve(size);

        std::vector<std::size_t> in_degrees(size);

        for (std::size_t from{}; from < size; ++from)
        {
            for (const auto &edges = dag[from]; const auto &edge : edges)
            {
                const auto tod = static_cast<const std::int32_t>(edge); // todo(p3): del cast?

                assert(0 <= tod && static_cast<std::size_t>(tod) < size);
                ++in_degrees.at(tod);
            }
        }

        std::queue<std::size_t> zero_degree_vertices;

        for (std::size_t tod{}; tod < size; ++tod)
        {
            if (0U == in_degrees[tod])
            {
                zero_degree_vertices.push(tod);
            }
        }

        while (!zero_degree_vertices.empty())
        {
            const auto from = zero_degree_vertices.front();
            zero_degree_vertices.pop();

            result.push_back(from);

            for (const auto &edges = dag[from]; const auto &edge : edges)
            {
                const auto tod = static_cast<const std::int32_t>(edge); // todo(p3): del cast?
                if (0U == --in_degrees[tod])
                {
                    zero_degree_vertices.push(tod);
                }
            }
        }

        if (result.size() != size)
        {
            return std::nullopt;
        }

        return result;
    }

    // The vertices are ordered by their descending finish times.
    [[nodiscard]] constexpr auto topological_sort_via_dfs(const auto &dag) -> std::optional<std::vector<std::size_t>>
    {
        const auto size = dag.size();

        std::vector<std::size_t> result;
        result.reserve(size);

        using from_position_t = std::pair<std::size_t, std::size_t>;

        std::vector<from_position_t> sta;
        sta.reserve(size);

        Standard::Algorithms::Numbers::color_2bit colors;
        colors.white(size);

        for (std::size_t root{}; root < size; ++root)
        {
            if (const auto color = colors.color(root); Standard::Algorithms::Numbers::white != color)
            {
                assert(Standard::Algorithms::Numbers::black == color);
                continue;
            }

            if (!Inner::topol_sort_dfs(dag, root, colors, result, sta))
            {
                return std::nullopt;
            }
        }

        assert(result.size() == size);

        std::reverse(result.begin(), result.end());

        return result;
    }
} // namespace Standard::Algorithms::Graphs
