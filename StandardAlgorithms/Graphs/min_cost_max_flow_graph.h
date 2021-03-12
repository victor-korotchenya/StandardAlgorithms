#pragma once
#include"../Numbers/color_2bit.h"
#include"flow_edge.h"
#include<algorithm>
#include<array>
#include<concepts>
#include<limits>
#include<type_traits>
#include<vector>

namespace Standard::Algorithms::Graphs
{
    template<std::signed_integral weight_t, std::signed_integral cost_t, std::size_t max_size,
        class edge_t = cost_flow_edge<weight_t, cost_t>>
    requires(sizeof(weight_t) <= sizeof(cost_t) && 2 <= max_size)
    struct min_cost_max_flow_graph final
    {
        constexpr explicit min_cost_max_flow_graph(std::int32_t size = {})
            : Size(size)
        {
            assert(!(size < 0));
        }

        [[nodiscard]] constexpr auto size() const noexcept
        {
            return Size;
        }

        [[nodiscard]] constexpr auto graph() const &noexcept -> const std::array<std::vector<edge_t>, max_size> &
        {
            return Graph;
        }

        [[nodiscard]] constexpr auto graph() &noexcept -> std::array<std::vector<edge_t>, max_size> &
        {
            return Graph;
        }

        constexpr void init(const std::int32_t size, bool clear_graph = true)
        {
            assert(1 < size && static_cast<std::size_t>(size) <= max_size);
            Size = size;

            if (!clear_graph)
            {
                return;
            }

            for (std::int32_t index{}; index < Size; ++index)
            {
                Graph[index].clear();
            }
        }

        // Call init(n) first.
        constexpr void add_edge(
            const std::int32_t from, const std::int32_t tod, const weight_t &capacity, const cost_t &unit_cost = {})
        {
            assert(1 < Size && !(from < 0) && from < Size && !(tod < 0) && tod < Size && from != tod);

            assert(cost_t{} <= unit_cost && weight_t{} < capacity && capacity < inf && unit_cost < inf &&
                -inf < unit_cost);

            auto &edges_to = Graph[tod];
            const auto rev = static_cast<std::int32_t>(edges_to.size());

            auto &edges_from = Graph[from];
            const auto rev2 = static_cast<std::int32_t>(edges_from.size());

            edges_from.push_back({ tod, rev, capacity, weight_t{}, unit_cost });
            edges_to.push_back({ from, rev2, weight_t{}, weight_t{}, -unit_cost });
        }

        [[nodiscard]] constexpr auto min_cost_max_flow(const std::int32_t source, const std::int32_t sink,
            const weight_t limit_flow = std::numeric_limits<weight_t>::max()) -> std::pair<weight_t, cost_t>
        {
            assert(1 < Size && !(source < 0) && source < Size && !(sink < 0) && sink < Size && source != sink &&
                weight_t{} < limit_flow);

            weight_t total_flow{};
            cost_t sum_cost{};

            for (;;)
            {
                auto added_flow = static_cast<weight_t>(limit_flow - total_flow);
                if (added_flow == weight_t{} || !find_flow({ source, sink }))
                {
                    return { total_flow, sum_cost };
                }

                assert(weight_t{} < added_flow);

                min_flow(source, added_flow, sink);
                apply_flow(source, added_flow, sink, sum_cost);
                total_flow += added_flow;
            }
        }

private:
        constexpr void prev_vertex(std::int32_t &vertex) const noexcept
        {
            assert(!(vertex < 0) && vertex < Size);

            vertex = (0 < vertex ? vertex : Size) - 1;
        }

        constexpr void next_vertex(std::int32_t &vertex) const noexcept
        {
            assert(!(vertex < 0) && vertex < Size);

            if (++vertex == Size)
            {
                vertex = 0;
            }
        }

        constexpr void reset_arrays(const std::int32_t source)
        {
            assert(1 < Size && !(source < 0) && source < Size);

            Costs.assign(Size, inf);
            Costs.at(source) = cost_t{};

            Que.assign(Size, 0);
            Que.at(0) = source;

            Parents.assign(Size, 0);
            Indexes.assign(Size, 0);
            Colors.white(Size);
        }

        constexpr void visit(const std::int32_t vertex, std::int32_t &head, std::int32_t &tail)
        {
            assert(!(vertex < 0) && vertex < Size);

            auto &edges = Graph.at(vertex);
            const auto size1 = static_cast<std::int32_t>(edges.size());
            const auto &v_cost = Costs[vertex];
            Colors.specific({ vertex, Standard::Algorithms::Numbers::black });

            for (std::int32_t index{}; index < size1; ++index)
            {
                auto &edge = edges[index];
                if (edge.flow == edge.capacity)
                {
                    continue;
                }

                assert(edge.flow < edge.capacity);
                const auto cand = static_cast<cost_t>(v_cost + edge.unit_cost);
                auto &cur = Costs[edge.to];

                if (!(cand < cur))
                {
                    continue;
                }

                cur = cand;

                const auto color = Colors.color(edge.to);
                if (color == Standard::Algorithms::Numbers::white)
                {
                    Que[tail] = edge.to;
                    next_vertex(tail);
                }
                else if (Standard::Algorithms::Numbers::white < color)
                {
                    assert(Standard::Algorithms::Numbers::black == color);

                    prev_vertex(head);
                    Que[head] = edge.to;
                }

                Colors.specific({ edge.to, Standard::Algorithms::Numbers::grey });

                Parents[edge.to] = vertex;
                Indexes[edge.to] = index;
            }
        }

        // todo(p3): faster.
        [[nodiscard]] constexpr auto find_flow(const std::pair<std::int32_t, std::int32_t> &source_sink) -> bool
        {
            const auto &source = source_sink.first;
            const auto &sink = source_sink.second;

            reset_arrays(source);

            for (std::int32_t head{}, tail = 1;;)
            {
                const auto vertex = Que[head];
                next_vertex(head);

                visit(vertex, head, tail);

                if (head == tail)
                {
                    const auto &cost = Costs[sink];
                    return cost != inf;
                }
            }
        }

        constexpr void min_flow(const std::int32_t source, weight_t &added_flow, const std::int32_t sink) const
        {
            assert(weight_t{} < added_flow);

            auto vertex = sink;

            do
            {
                assert(!(vertex < 0) && vertex < Size);

                const auto &par = Parents[vertex];
                const auto &ind = Indexes[vertex];
                const auto &edge = Graph[par][ind];
                const auto dif = static_cast<weight_t>(edge.capacity - edge.flow);
                assert(weight_t{} < dif);

                added_flow = std::min(added_flow, dif);
                vertex = Parents[vertex];
            } while (vertex != source);
        }

        constexpr void apply_flow(
            const std::int32_t source, const weight_t added_flow, const std::int32_t sink, cost_t &cost)
        {
            assert(cost_t{} <= cost && weight_t{} < added_flow);

            auto vertex = sink;
            do
            {
                assert(!(vertex < 0) && vertex < Size);

                const auto &par = Parents[vertex];
                const auto &ind = Indexes[vertex];
                auto &edge = Graph[par][ind];
                edge.flow += added_flow;

                const auto &rev = edge.rev_edge_index;
                auto &other = Graph[vertex][rev];
                other.flow -= added_flow;

                const auto add = static_cast<cost_t>(edge.unit_cost * static_cast<cost_t>(added_flow));
                cost += add;
                assert(cost_t{} <= add && cost_t{} <= cost);

                vertex = Parents[vertex];
            } while (vertex != source);
        }

        static constexpr cost_t inf = std::numeric_limits<cost_t>::max() / 2;
        static_assert(cost_t{} < inf && -inf < cost_t{} && -inf != inf);

        std::array<std::vector<edge_t>, max_size> Graph{};
        std::vector<cost_t> Costs{};
        std::vector<std::int32_t> Parents{};
        std::vector<std::int32_t> Indexes{};
        std::vector<std::int32_t> Que{};
        Standard::Algorithms::Numbers::color_2bit Colors{};
        std::int32_t Size;
    };
} // namespace Standard::Algorithms::Graphs
