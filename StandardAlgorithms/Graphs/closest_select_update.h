#pragma once
#include"../Utilities/require_utilities.h"
#include"centroid_decomposition.h"

namespace Standard::Algorithms::Trees::Inner
{
    template<class edge_t>
    constexpr void clo_sel_update([[maybe_unused]] const std::int32_t tree_size, [[maybe_unused]] const bool ignore_0th,
        const std::int32_t source_node, centroid_decomposition<edge_t, true> &dec,
        std::vector<std::int32_t> &closest_nodes)
    {
        const auto &parents = dec.parents();
        const auto &parent_distances = dec.parent_distances();

        auto &distances = dec.temp();
        std::int32_t dist_sum{};

        for (auto node = source_node;;)
        {
            assert(static_cast<std::int32_t>(ignore_0th) <= node && node < tree_size);

            auto &cur = distances[node];
            if (cur <= dist_sum)
            {
                return;
            }

            cur = dist_sum;
            closest_nodes[node] = source_node;

            const auto &par = parents[node];
            if (par < 0)
            {
                return;
            }

            dist_sum += parent_distances[node];

            assert(0 < dist_sum && !(parent_distances[node] < 0));

            node = par;
        }
    }

    template<class edge_t>
    constexpr auto clo_select([[maybe_unused]] const std::int32_t tree_size, [[maybe_unused]] const bool ignore_0th,
        const std::int32_t source_node, const centroid_decomposition<edge_t, true> &dec,
        const std::vector<std::int32_t> &closest_nodes)
        -> std::pair<std::int32_t, std::int32_t> // min distance, closest node
    {
        const auto &parents = dec.parents();
        const auto &parent_distances = dec.parent_distances();
        const auto &distances = dec.temp();

        assert(static_cast<std::int32_t>(ignore_0th) <= source_node && source_node < tree_size);

        auto min_dist = distances[source_node];
        auto closest = closest_nodes[source_node];

        if (min_dist == 0)
        {
            return { min_dist, closest };
        }

        std::int32_t dist_sum{};

        for (auto node = source_node;;)
        {
            dist_sum += parent_distances[node];
            assert(!(dist_sum < 0) && !(parent_distances[node] < 0));

            node = parents[node];
            if (node < 0)
            {
                break;
            }

            assert(static_cast<std::int32_t>(ignore_0th) <= node && node < tree_size);

            const auto &cur = distances[node];
            const auto cand = cur + dist_sum;
            if (cand < min_dist)
            {
                min_dist = cand;
                closest = closest_nodes[node];

                assert(0 < min_dist && !(closest < 0) && closest < tree_size);
            }

            if (cur == 0)
            {
                break;
            }
        }

        return { min_dist, closest };
    }
} // namespace Standard::Algorithms::Trees::Inner

namespace Standard::Algorithms::Trees
{
    // Given a tree:
    // 1st) Mark a node.
    // Last) For a node, find the closest/minimum marked node distance.
    // Time: build O(n*log(n)), query O(log(n)).
    template<class edge_t>
    constexpr void closest_select_update(const std::vector<std::vector<edge_t>> &tree,
        // node, is update
        const std::vector<std::pair<std::int32_t, bool>> &operations,
        // min distance, closest node
        std::vector<std::pair<std::int32_t, std::int32_t>> &select_answers, const bool ignore_0th = false)
    {
        const auto tree_size = static_cast<std::int32_t>(tree.size());

        assert(!tree.empty() && 0 < tree_size && 1U < operations.size() && operations[0].second &&
            !operations.back().second);

        select_answers.clear();
        select_answers.reserve(operations.size());

        centroid_decomposition<edge_t, true> dec(tree, ignore_0th);

        auto &distances = dec.temp();
        distances.assign(tree_size, std::numeric_limits<std::int32_t>::max() / 2);

        std::vector<std::int32_t> closest_nodes(tree_size, -1);

        for (const auto &oper : operations)
        {
            const auto &source_node = oper.first;
            assert(static_cast<std::int32_t>(ignore_0th) <= source_node && source_node < tree_size);

            if (const auto &is_update = oper.second; is_update)
            {
                Inner::clo_sel_update<edge_t>(tree_size, ignore_0th, source_node, dec, closest_nodes);
                continue;
            }

            const auto min_distance_closest_node =
                Inner::clo_select<edge_t>(tree_size, ignore_0th, source_node, dec, closest_nodes);

            const auto &min_dist = min_distance_closest_node.first;
            const auto &closest = min_distance_closest_node.second;

            assert(!(std::min(min_dist, closest) < 0) && std::max(min_dist, closest) < tree_size);

            select_answers.emplace_back(min_dist, closest);
        }
    }
} // namespace Standard::Algorithms::Trees
