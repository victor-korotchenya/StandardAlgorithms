#pragma once
#include"../Numbers/one_time_queue.h"
#include"edmonds_karp_flow.h"
#include<algorithm>
#include<limits>
#include<vector>

namespace Standard::Algorithms::Graphs::Inner
{
    template<class weight_t, class edge_t>
    constexpr auto bfs_find_shortest_paths(const std::int32_t source, const std::vector<std::vector<edge_t>> &graph,
        const std::int32_t destination, std::vector<std::int32_t> &levels,
        Standard::Algorithms::Numbers::one_time_queue<std::int32_t, std::int32_t> &que) -> bool
    {
        que.reset();
        que.push(source);

        levels.assign(graph.size(), -1);
        levels[source] = {};

        do
        {
            const auto from = que.pop();
            const auto level_from = levels[from];

            for (const auto &edges = graph[from]; const auto &edge : edges)
            {
                if (edge.flow < edge.capacity && levels[edge.to] < 0)
                {
                    que.push(edge.to);
                    levels[edge.to] = level_from + 1;
                }
            }
        } while (!que.is_empty());

        return 0 <= levels[destination];
    }

    template<class weight_t, class edge_t>
    constexpr void dfs_add_flow_destination(std::vector<std::vector<edge_t>> &graph, edge_t &edge,
        const std::vector<std::pair<std::int32_t, weight_t>> &sta, const std::vector<std::int32_t> &visits,
        std::int32_t stack_size, const weight_t &flow1)
    {
        assert(0 < stack_size);

        for (auto *ptr = &edge;;)
        {
            ptr->flow += flow1;

            auto &cell = graph[ptr->to][ptr->rev_edge_index];
            cell.flow -= flow1;

            if (0 == --stack_size)
            {
                return;
            }

            const auto &top = sta[stack_size - 1LL];
            const auto &vis = visits[top.first];
            ptr = &graph[top.first][vis];
        }
    }

    template<class weight_t, class edge_t>
    constexpr auto dfs_add_flow(const std::int32_t source, std::vector<std::vector<edge_t>> &graph,
        const std::int32_t destination, const std::vector<std::int32_t> &levels,
        std::vector<std::pair<std::int32_t, weight_t>> &sta, std::vector<std::int32_t> &visits) -> weight_t
    {
        assert(!sta.empty());

        sta.at(0) = std::make_pair(source, std::numeric_limits<weight_t>::max());

        auto stack_size = 1;
        auto shall_delete = false;
        for (;;)
        {
// NOLINTNEXTLINE
Label_1:
            assert(0 < stack_size);

            const auto &top = sta.at(stack_size - 1LL);
            const auto from = top.first;
            const auto weigh = top.second;
            auto &edges = graph[from];
            const auto size = static_cast<std::int32_t>(edges.size());
            auto &visi = visits[from];
            visi += shall_delete && visi < size ? 1 : 0;

            while (visi < size)
            {
                auto &edge = edges[visi];

                if (edge.flow < edge.capacity && levels[from] + 1 == levels[edge.to])
                {
                    const auto increase = std::min(weigh, static_cast<weight_t>(edge.capacity - edge.flow));
                    assert(weight_t() < increase);

                    if (destination == edge.to)
                    {
                        dfs_add_flow_destination<weight_t, edge_t>(graph, edge, sta, visits, stack_size, increase);

                        return increase;
                    }

                    sta[stack_size++] = std::make_pair(edge.to, increase);
                    shall_delete = false;

                    // With goto life is easier.
                    // NOLINTNEXTLINE
                    goto Label_1;
                }

                ++visi;
            }

            if (0 == --stack_size)
            {
                return {};
            }

            shall_delete = true;
        }
    }
} // namespace Standard::Algorithms::Graphs::Inner

namespace Standard::Algorithms::Graphs
{
    // Finds the max flow using Dinic's algorithm.
    // The "graph" MUST have all the reverse edges!
    template<std::signed_integral weight_t, class edge_t>
    constexpr auto dinic_flow_old(
        const std::int32_t source, std::vector<std::vector<edge_t>> &graph, const std::int32_t destination) -> weight_t
    {
        check_data_and_zero_flow<weight_t, edge_t>(source, graph, destination);

        const auto size = graph.size();

        weight_t result{};
        std::vector<std::int32_t> levels(size);
        std::vector<std::int32_t> visits(size);
        std::vector<std::pair<std::int32_t, weight_t>> sta(size);
        Standard::Algorithms::Numbers::one_time_queue<std::int32_t, std::int32_t> que(size);

        while (Inner::bfs_find_shortest_paths<weight_t, edge_t>(source, graph, destination, levels, que))
        {
            visits.assign(size, 0);

            for (;;)
            {
                const auto sub_flow =
                    Inner::dfs_add_flow<weight_t, edge_t>(source, graph, destination, levels, sta, visits);

                if (!(weight_t() < sub_flow))
                {
                    break;
                }

                result += sub_flow;
            }
        }

        return result;
    }
} // namespace Standard::Algorithms::Graphs
