#pragma once
#include"../Utilities/require_utilities.h"
#include"weighted_vertex.h"
#include<vector>

namespace Standard::Algorithms::Graphs
{
    template<class vertex_t, class weight_t, class edge_t1 = weighted_vertex<vertex_t, weight_t>>
    struct graph final
    {
        using edge_t = edge_t1;

        constexpr explicit graph(const std::size_t vertex_count)
            : Adjacency_lists(require_positive(vertex_count, "vertex count"))
        {
        }

        [[nodiscard]] constexpr auto size() const noexcept -> std::size_t
        {
            return Adjacency_lists.size();
        }

        [[nodiscard]] constexpr auto adjacency_lists() const &noexcept -> const std::vector<std::vector<edge_t>> &
        {
            return Adjacency_lists;
        }

        [[nodiscard]] constexpr auto to_plain_graph() const
            -> std::vector<std::vector<std::pair<std::int32_t, std::int64_t>>>
        {
            std::vector<std::vector<std::pair<std::int32_t, std::int64_t>>> plain(Adjacency_lists.size());

            std::transform(Adjacency_lists.cbegin(), Adjacency_lists.cend(), plain.begin(),
                [] [[nodiscard]] (const auto &edges)
                {
                    std::vector<std::pair<std::int32_t, std::int64_t>> news(edges.size());

                    std::transform(edges.cbegin(), edges.cend(), news.begin(),
                        [] [[nodiscard]] (const auto &edge)
                        {
                            return std::make_pair(static_cast<std::int32_t>(edge.vertex), edge.weight);
                        });

                    return news;
                });

            return plain;
        }

        // todo(p2): Add a check for duplicates.
        constexpr void add_edge(const vertex_t vertex0, const vertex_t vertex1, const weight_t weight = 1,
            const bool add_reverse_edge = true)
        {
            if (vertex0 == vertex1)
            {
                auto str = ::Standard::Algorithms::Utilities::w_stream();
                str << "Cannot add self loop for the vertex (" << vertex0 << ").";
                throw_exception(str);
            }

            const auto vertex_count = Adjacency_lists.size();
            check_vertex(vertex0, vertex_count);
            check_vertex(vertex1, vertex_count);

            Adjacency_lists[vertex0].push_back(edge_t{ vertex1, weight });

            if (add_reverse_edge)
            {
                Adjacency_lists[vertex1].push_back(edge_t{ vertex0, weight });
            }
        }

private:
        inline static constexpr void check_vertex(const vertex_t vertex, const std::size_t vertex_count)
        {
            if (vertex < vertex_t{} || vertex_count <= static_cast<std::size_t>(vertex)) [[unlikely]]
            {
                auto str = ::Standard::Algorithms::Utilities::w_stream();
                str << "The vertex (" << vertex << ") must be inclusively between 0 and " << (vertex_count - 1U) << ".";
                throw_exception(str);
            }
        }

        std::vector<std::vector<edge_t>> Adjacency_lists;
    };
} // namespace Standard::Algorithms::Graphs
