#pragma once

#include <array>
#include <cassert>
#include <limits>
#include <type_traits>
#include <vector>
#include "flow_edge.h"

namespace Privet::Algorithms::Graphs
{
    template <class weight_t, class cost_t, int n_max, class edge_t = cost_flow_edge<weight_t, cost_t>>
    class min_cost_max_flow_graph
    {
        static_assert(sizeof(weight_t) <= sizeof(cost_t) && std::is_signed_v<weight_t> && std::is_signed_v<cost_t> && n_max >= 2);

        std::array<std::vector<edge_t>, n_max> graph = {};

        std::vector<cost_t> costs = {};
        std::vector<int> parents = {}, inds = {}, q = {};
        std::vector<char> ty = {};

        static constexpr cost_t inf = std::numeric_limits<cost_t>::max() / 2;
        static_assert(inf > 0 && -inf < 0 && -inf != inf);

        int n = {};

    public:
        explicit min_cost_max_flow_graph(int n = {}) : n(n)
        {
            assert(n >= 0);
        }

        auto size() const noexcept
        {
            return n;
        }

        auto& get_graph() noexcept
        {
            return graph;
        }

        void init(const int n, bool clear_graph = true)
        {
            assert(n >= 2 && n <= n_max);

            this->n = n;

            if (!clear_graph)
                return;

            for (auto i = 0; i < n; ++i)
                graph[i].clear();
        }

        // Call init(n) first.
        void add_edge(const int from, const int to, const weight_t capacity, const cost_t unit_cost = {})
        {
            assert(n >= 2 && from >= 0 && from < n&& to >= 0 && to < n&& from != to && unit_cost >= 0 && capacity > 0 && capacity < inf&& unit_cost < inf&& unit_cost > -inf);

            const auto t = static_cast<int>(graph[to].size()),
                t2 = static_cast<int>(graph[from].size());

            graph[from].emplace_back(to, t, capacity, weight_t(), unit_cost);
            graph[to].emplace_back(from, t2, weight_t(), weight_t(), -unit_cost);
        }

        std::pair<weight_t, cost_t> min_cost_max_flow(const int source, const int sink, const weight_t limit_flow = std::numeric_limits<weight_t>::max())
        {
            assert(n >= 2 && source >= 0 && source < n&& sink >= 0 && sink < n&& source != sink && limit_flow > 0);

            weight_t flow{};
            cost_t cost{};

            for (;;)
            {
                auto added_flow = limit_flow - flow;
                if (!added_flow || !find_flow(source, sink))
                    return { flow, cost };

                assert(added_flow > 0);
                min_flow(source, sink, added_flow);
                apply_flow(source, sink, added_flow, cost);
                flow += added_flow;
            }
        }

    private:
        bool find_flow(const int source, const int sink)
        {
            costs.assign(n, inf);
            parents.assign(n, 0);
            inds.assign(n, 0);
            q.assign(n, 0);
            ty.assign(n, 0);

            auto head = 0, tail = 1;
            costs[source] = 0;
            q[0] = source;

            for (;;)
            {
                const auto v = q[head];
                ty[v] = 2;
                if (++head == n)
                    head = 0;

                auto& edges = graph[v];
                const auto size = static_cast<int>(edges.size());
                for (auto i = 0; i < size; ++i)
                {
                    auto& edge = edges[i];
                    if (edge.flow == edge.capacity)
                        continue;

                    assert(edge.flow < edge.capacity);
                    const auto c = costs[v] + edge.unit_cost;
                    if (costs[edge.to] <= c)
                        continue;

                    costs[edge.to] = c;
                    if (!ty[edge.to])
                    {
                        q[tail] = edge.to;
                        if (++tail == n)
                            tail = 0;
                    }
                    else if (ty[edge.to] == 2)
                    {
                        if (!head--)
                            head = n - 1;
                        q[head] = edge.to;
                    }

                    ty[edge.to] = 1;
                    parents[edge.to] = v;
                    inds[edge.to] = i;
                }

                if (head == tail)
                    return costs[sink] != inf;
            }
        }

        void min_flow(const int source, const int sink, weight_t& added_flow) const
        {
            assert(added_flow > 0);
            auto v = sink;

            do
            {
                assert(v >= 0 && v < n);

                const auto& par = parents[v];
                const auto& ind = inds[v];
                const auto& edge = graph[par][ind];
                const auto dif = edge.capacity - edge.flow;
                assert(dif > 0);

                if (added_flow > dif)
                    added_flow = dif;

                v = parents[v];
            } while (v != source);
        }

        void apply_flow(const int source, const int sink, const weight_t added_flow,
            cost_t& cost)
        {
            assert(cost >= 0 && added_flow > 0);
            auto v = sink;
            do
            {
                assert(v >= 0 && v < n);

                const auto& par = parents[v];
                const auto& ind = inds[v];
                auto& edge = graph[par][ind];
                const auto& rev = edge.rev_edge_index;

                edge.flow += added_flow;
                graph[v][rev].flow -= added_flow;

                const auto c = edge.unit_cost * added_flow;
                cost += c;

                v = parents[v];
            } while (v != source);

            assert(cost >= 0);
        }
    };
}