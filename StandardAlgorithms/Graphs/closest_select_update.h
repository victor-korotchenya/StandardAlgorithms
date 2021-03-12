#pragma once
#include <numeric>
#include <vector>
#include "../Utilities/ExceptionUtilities.h"
#include "centroid_decomposition.h"

namespace Privet::Algorithms::Trees
{
    // Given a tree:
    // 1st) Mark a node.
    // Last) For a node, find the closest/minimum marked node distance.
    // Time: build O(n*log(n)), query O(log(n)).
    template<class edge_t>
    void closest_select_update(const std::vector<std::vector<edge_t>>& tree,
        // node, is update
        const std::vector<std::pair<int, bool>>& operations,
        // min distance, closest node
        std::vector<std::pair<int, int>>& select_answers,
        const bool ignore_0th = false)
    {
        assert(tree.size() && operations.size() >= 2 && operations[0].second && !operations.back().second);

        select_answers.clear();
        select_answers.reserve(operations.size());

        centroid_decomposition<edge_t, true> dec(tree, ignore_0th);
        const auto& parents = dec.get_parents();
        const auto& parent_distances = dec.get_parent_distances();

        auto& distances = dec.get_temp();
        distances.assign(tree.size(), std::numeric_limits<int>::max() >> 1);

        std::vector<int> closest_nodes(tree.size(), -1);

        for (const auto [source_node, is_update] : operations)
        {
            assert(source_node >= static_cast<int>(ignore_0th) && source_node < tree.size());
            auto sum_dist = 0, node = source_node;

            if (is_update)
            {
                for (;;)
                {
                    auto& cur = distances[node];
                    if (cur <= sum_dist)
                        break;

                    cur = sum_dist;
                    closest_nodes[node] = source_node;

                    const auto& par = parents[node];
                    if (par < 0)
                        break;

                    sum_dist += parent_distances[node];
                    assert(sum_dist > 0 && parent_distances[node] >= 0 && par >= static_cast<int>(ignore_0th) && par < tree.size());
                    node = par;
                }

                continue;
            }

            auto dist = distances[source_node], closest = closest_nodes[node];
            if (dist)
                for (;;)
                {
                    sum_dist += parent_distances[node];
                    assert(sum_dist >= 0 && parent_distances[node] >= 0);
                    node = parents[node];
                    if (node < 0)
                        break;

                    assert(node >= static_cast<int>(ignore_0th) && node < tree.size());

                    const auto& cur = distances[node],
                        cand = cur + sum_dist;
                    if (dist > cand)
                    {
                        dist = cand;
                        closest = closest_nodes[node];
                        assert(dist > 0 && closest >= 0 && closest < tree.size());
                    }

                    if (!cur)
                        break;
                }

            assert(dist >= 0 && closest >= 0 && closest < tree.size());
            select_answers.emplace_back(dist, closest);
        }
    }
}