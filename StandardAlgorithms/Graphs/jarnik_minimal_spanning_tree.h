#pragma once
#include"../Utilities/same_sign_leq_size.h"
#include<cassert>
#include<cstdint>
#include<limits>
#include<queue>
#include<utility>
#include<vector>

namespace Standard::Algorithms::Graphs
{
    // The graph must be connected.
    template<class weight_t, class weight_t2>
    requires(same_sign_leq_size<weight_t, weight_t2>)
    [[nodiscard]] constexpr auto jarnik_minimal_spanning_tree(
        const std::vector<std::vector<std::pair<std::int32_t, weight_t>>> &graph, const std::int32_t source_node,
        std::vector<std::int32_t> &parents,
        // temp vars:
        std::vector<bool> &visited, std::vector<weight_t2> &weights,
        // max queue - use minus.
        std::priority_queue<std::pair<weight_t2, std::int32_t>> &que) -> weight_t2
    {
        // todo(p4): static_assert(std::is_signed_v<weight_t> && std::is_signed_v<weight_t2>);

        assert(1U < graph.size() && !(source_node < 0) && source_node < static_cast<std::int32_t>(graph.size()));

        assert(que.empty());

        {
            constexpr auto no_parent = -1;
            parents.assign(graph.size(), no_parent);
        }

        visited.assign(graph.size(), false);

        weights.assign(graph.size() << 1U, std::numeric_limits<weight_t2>::max());
        weights[source_node] = {};

        que.emplace(weight_t2{}, source_node);

        weight_t2 min_w{};
        do
        {
            const auto top = que.top();
            const auto &cur = top.second;
            que.pop();

            if (visited[cur])
            {
                continue;
            }

            visited[cur] = true;
            min_w -= top.first;

            for (const auto &pars = graph[cur]; const auto &par : pars)
            {
                const auto &next = par.first;

                if (visited[next])
                {
                    continue;
                }

                auto &old1 = weights[next + graph.size()];
                const auto &new1 = par.second;

                if (!(new1 < old1))
                {
                    continue;
                }

                old1 = new1;
                weights[next] = weights[cur] + new1;
                parents[next] = cur;
                que.emplace(-new1, next);
            }
        } while (!que.empty());

        weights.resize(graph.size());

        return min_w;
    }
} // namespace Standard::Algorithms::Graphs
