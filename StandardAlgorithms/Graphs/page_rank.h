#pragma once
#include <algorithm>
#include <cassert>
#include <numeric>
#include <vector>
#include "../Utilities/ExceptionUtilities.h"

namespace
{
    constexpr auto damping_factor_max = 1;

    template <typename edge_t, typename float_number_t>
    void visit(const std::vector<std::vector<edge_t>>& graph,
        // todo: finish. const std::vector<std::vector<edge_t>> &g_rev,
        const std::vector<float_number_t>& previous,
        const float_number_t& damping_factor,
        std::vector<float_number_t>& current)
    {
        assert(previous.size() == current.size() && previous.size() > 0);

        const auto n = static_cast<int>(previous.size());
        const auto constant_part = (static_cast<float_number_t>(1) - damping_factor) / n;

        for (auto from = 0; from < n; ++from)
        {
            const auto& edges = graph[from];
            const auto& previous_from = previous[from];
            if (edges.empty() || previous_from <= constant_part)
                continue;

            const auto transfer = previous_from - constant_part;
            current[from] -= transfer;

            const auto transfer_share = transfer / static_cast<float_number_t>(edges.size());
            for (const auto& to : edges)
                current[to] += transfer_share;
        }
    }

    template <typename float_number_t>
    bool shall_stop(const std::vector<float_number_t>& previous,
        const std::vector<float_number_t>& current,
        const float_number_t& epsilon)
    {
        const auto n = static_cast<int>(previous.size());
#ifdef _DEBUG
        const auto sum = std::accumulate(current.begin(), current.end(), static_cast<float_number_t>(0));
        const auto delta_max = static_cast<float_number_t>(0.001);
        assert(sum > 1 - delta_max && sum < 1 + delta_max);
#endif

        for (auto i = 0; i < n; ++i)
        {
            const auto val = std::fabs(previous[i] - current[i]);
            if (val > epsilon)
                return false;
        }

        return true;
    }
}

namespace Privet
{
    namespace Algorithms
    {
        namespace Graphs
        {
            template <typename edge_t, typename float_number_t>
            void page_rank_simple(const std::vector<std::vector<edge_t>>& graph,
                std::vector<float_number_t>& buffer,
                std::vector<float_number_t>& result,
                const float_number_t& epsilon,
                const float_number_t& damping_factor = damping_factor_max)
            {
                static_assert(std::is_floating_point<float_number_t>::value);

                const auto n = static_cast<int>(graph.size());
                RequirePositive(n, "graph size");
                RequirePositive(epsilon, "epsilon");
                RequirePositive(damping_factor, "damping_factor");
                if (damping_factor > damping_factor_max)
                {
                    const std::string error = "The damping_factor " + std::to_string(damping_factor) +
                        " cannot exceed " + std::to_string(damping_factor_max);
                    throw std::runtime_error(error);
                }

                //todo: del 2nd graph?
                std::vector<std::vector<edge_t>> g_rev(n);
                for (auto from = 0; from < n; ++from)
                    for (const auto& to : graph[from])
                        g_rev[to].push_back(from);
                {
                    result.clear();
                    const auto share = 1.0 / static_cast<float_number_t>(n);
                    result.resize(n, share);

                    buffer.resize(n);
                }

                const auto max_attempts = n < 300 ? n * 2 : n;
                for (auto i = 0; i < max_attempts; ++i)
                {
                    // todo: remove copy - replace with mem_set?
                    //std::swap(buffer, result); std::fill(result.begin(), result.end(), 0);
                    std::copy(result.begin(), result.end(), buffer.begin());
                    visit(graph,
                        // todo: g_rev,
                        buffer, damping_factor, result);

                    if (shall_stop<float_number_t>(buffer, result, epsilon))
                        break;
                }
            }
        }
    }
}