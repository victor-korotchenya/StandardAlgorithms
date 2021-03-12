#pragma once
#include"topological_sort.h"
#include<algorithm>
#include<concepts>
#include<queue>
#include<ranges>
#include<string>
#include<type_traits>
#include<utility>

namespace Standard::Algorithms::Graphs::Internal
{
    void restore_critical_path(std::size_t source, const std::vector<std::size_t> &parents, std::size_t destination,
        std::vector<std::size_t> &critical_path);
} // namespace Standard::Algorithms::Graphs::Internal

namespace Standard::Algorithms::Graphs
{
    // For a DAG, compute for each vertex: the earliest and latest completion times;
    // for each edge: the slack - the activity maximum delay.
    // Also, find a critical path of 0 slack edges (also, the longest path).
    // Vertices must be from 0 to (size - 1).
    // Edge weights must be non-negative.

    // Return the minimum total time.
    template<class graph_t,
        // Must be large enough to handle an overflow.
        std::integral weight_t>
    constexpr auto compute_earliest_completion_times(const graph_t &dag, std::vector<std::size_t> &topol_order,
        std::vector<std::pair<weight_t, weight_t>> &vertex_earliest_latest_times) -> weight_t
    {
        const auto size = static_cast<std::size_t>(dag.size());

        if (auto tops = topological_sort(dag); tops.has_value()) [[likely]]
        {
            topol_order = std::move(tops.value());

            assert(size == topol_order.size());
        }
        else
        {
            throw std::invalid_argument("The DAG is not acyclic.");
        }

        vertex_earliest_latest_times.assign(size, std::make_pair(weight_t{}, weight_t{}));

        weight_t total_time{};

        for (const auto &from : topol_order)
        {
            assert(from < size);

            const auto &from_weight = vertex_earliest_latest_times[from].first;
            assert(weight_t{} <= from_weight);

            for (const auto &edge : dag[from])
            {
                const auto tod = static_cast<const std::int32_t>(edge); // todo(p4): del cast?
                assert(0 <= tod && static_cast<std::size_t>(tod) < size);

                const auto weight = static_cast<weight_t>(edge.weight);

                if constexpr (std::is_signed_v<weight_t>)
                {
                    if (weight < weight_t{}) [[unlikely]]
                    {
                        throw std::runtime_error("The edge " + std::to_string(from) + " - " + std::to_string(tod) +
                            " weight (" + std::to_string(weight) + ") must be non-negative.");
                    }
                }

                const auto sum = static_cast<weight_t>(weight + from_weight);
                assert(weight_t{} <= weight && weight_t{} <= sum);

                auto &earliest = vertex_earliest_latest_times[tod].first;
                earliest = std::max(earliest, sum);
                total_time = std::max(total_time, sum);
            }
        }

        assert(weight_t{} <= total_time);

        return total_time;
    }

    template<class graph_t,
        // Must be large enough to handle an overflow.
        std::integral weight_t>
    constexpr void compute_latest_and_slacks(const graph_t &dag, const std::vector<std::size_t> &topol_order,
        const weight_t total_time, std::vector<std::pair<weight_t, weight_t>> &vertex_earliest_latest_times,
        std::vector<std::vector<weight_t>> &edge_slacks)
    {
        const auto size = static_cast<std::size_t>(dag.size());

        assert(size == topol_order.size() && size == vertex_earliest_latest_times.size() && !(total_time < weight_t{}));

        edge_slacks.clear();
        edge_slacks.resize(size);

#if defined(__clang__)
        // NOLINTNEXTLINE
        for (auto iter = topol_order.crbegin(); iter != topol_order.crend(); ++iter)
        {
            const auto &from = *iter; // todo(p3): switch to else after Clang 14 is released.   error: no member named
                                      // 'reverse' in namespace 'std::views'
#else
        for (const auto &from : topol_order | std::views::reverse)
        {
#endif

            assert(from < size);

            auto &from_times = vertex_earliest_latest_times[from];
            const auto &from_earliest = from_times.first;
            assert(weight_t{} <= from_earliest);

            auto &from_latest = from_times.second;
            from_latest = total_time;

            const auto &edges = dag[from];
            auto &slacks = edge_slacks[from];
            slacks.reserve(edges.size());

            for (const auto &edge : edges)
            {
                const auto tod = static_cast<const std::int32_t>(edge); // todo(p4): del cast?
                assert(0 <= tod && static_cast<std::size_t>(tod) < size);

                const auto &weight = edge.weight;
                const auto &tod_times = vertex_earliest_latest_times[tod];
                const auto &tod_earliest = tod_times.first;
                const auto &tod_latest = tod_times.second;

                assert(weight_t{} <= weight && weight <= tod_earliest);
                assert(from_earliest <= tod_earliest && tod_earliest <= tod_latest);

                const auto cand = static_cast<weight_t>(tod_latest - weight);
                assert(from_earliest <= cand);

                from_latest = std::min(from_latest, cand);

                assert(
                    weight_t{} <= from_latest && from_earliest <= from_latest && from_earliest + weight <= tod_latest);

                const auto activity_slack = static_cast<weight_t>(tod_latest - from_earliest - weight);
                assert(weight_t{} <= activity_slack);

                slacks.push_back(activity_slack);
            }
        }
    }

    template<class graph_t, std::integral weight_t>
    void work_critical_path(const graph_t &dag, const std::size_t source,
        const std::vector<std::vector<weight_t>> &edge_slacks, const std::size_t destination,
        std::vector<std::size_t> &critical_path)
    {
        const auto size = static_cast<std::size_t>(dag.size());
        assert(size == edge_slacks.size());

        critical_path.clear();

        if (source == destination)
        {
            return;
        }

        if (!(source < size)) [[unlikely]]
        {
            auto error = "The source " + std::to_string(source) + " cannot exceed the DAG size " + std::to_string(size);
            throw std::runtime_error(error);
        }

        if (!(destination < size)) [[unlikely]]
        {
            auto error = "The destination " + std::to_string(destination) + " cannot exceed the DAG size " +
                std::to_string(size);
            throw std::runtime_error(error);
        }

        constexpr auto npos = std::size_t{} - static_cast<std::size_t>(1);

        std::vector<std::size_t> parents(size, npos);
        std::queue<std::size_t> que;
        que.push(source);

        do
        {
            const auto from = que.front();
            assert(from != destination && from < size);
            que.pop();

            const auto &edges = dag[from];
            const auto &slacks = edge_slacks[from];
            assert(edges.size() == slacks.size());

            for (std::size_t index{}; index < edges.size(); ++index)
            {
                if (weight_t{} != slacks[index])
                {
                    continue;
                }

                const auto &edge = edges[index];
                const auto tod = static_cast<std::size_t>(static_cast<const std::int32_t>(edge)); // todo(p4): del cast?
                assert(tod < size);

                parents[tod] = from;
                if (tod != destination)
                {
                    que.push(tod);
                    continue;
                }

                Internal::restore_critical_path(source, parents, destination, critical_path);
                return;
            }
        } while (!que.empty());

        throw std::runtime_error("There is no critical path found from the source " + std::to_string(source) +
            " to the destination " + std::to_string(destination));
    }
} // namespace Standard::Algorithms::Graphs
