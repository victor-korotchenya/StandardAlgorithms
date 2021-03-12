#pragma once
#include"../Utilities/check_size.h"
#include"../Utilities/clang_constexpr.h"
#include"../Utilities/require_utilities.h"
#include<cmath>
#include<numeric>
#include<vector>

namespace Standard::Algorithms::Graphs::Inner
{
    constexpr auto damping_factor_max = 1;

    template<class edge_t, std::floating_point floating_t>
    constexpr void visit(const std::vector<std::vector<edge_t>> &graph,
        // todo(p3): finish. const std::vector<std::vector<edge_t>> &gr_rev,
        const std::vector<floating_t> &previous, const floating_t &constant_part, std::vector<floating_t> &current)
    {
        const auto size = previous.size();

        assert(0U < size && size == current.size());

        for (std::size_t from{}; from < size; ++from)
        {
            const auto &edges = graph[from];
            const auto &previous_from = previous[from];

            if (edges.empty() || previous_from <= constant_part)
            {
                continue;
            }

            const auto transfer = static_cast<floating_t>(previous_from - constant_part);
            current[from] -= transfer;

            const auto transfer_share = static_cast<floating_t>(transfer / static_cast<floating_t>(edges.size()));

            for (const auto &tod : edges)
            {
                current[tod] += transfer_share;
            }
        }
    }

    template<std::floating_point floating_t>
    [[nodiscard]] constexpr auto shall_stop(const std::vector<floating_t> &previous, const floating_t &epsilon,
        const std::vector<floating_t> &current) -> bool
    {
        assert(previous.size() == current.size() && 0U < previous.size());

        if constexpr (::Standard::Algorithms::is_debug)
        {
            const auto sum = std::accumulate(current.cbegin(), current.cend(), floating_t{});

            constexpr auto delta_max = static_cast<floating_t>(0.001);

            assert(1 - delta_max < sum && sum < 1 + delta_max);
        }

        const auto size = static_cast<std::int32_t>(previous.size());

        for (std::int32_t index{}; index < size; ++index)
        {
            const auto val = ::Standard::Algorithms::fabs(previous[index] - current[index]);
            if (epsilon < val)
            {
                return false;
            }
        }

        return true;
    }
} // namespace Standard::Algorithms::Graphs::Inner

namespace Standard::Algorithms::Graphs
{
    template<class edge_t, std::floating_point floating_t>
    constexpr void page_rank_simple(const std::vector<std::vector<edge_t>> &graph, std::vector<floating_t> &buffer,
        const floating_t &epsilon, std::vector<floating_t> &result,
        // A node without inc. edge gets a share = (1.0 - d_f)/nodes.
        const floating_t &damping_factor = Inner::damping_factor_max)
    {
        const auto *const name = "graph size";

        const auto size = Standard::Algorithms::Utilities::check_size<std::int32_t>(name, graph.size());
        require_positive(size, name);

        require_positive(epsilon, "epsilon");
        require_positive(damping_factor, "damping_factor");
        require_less_equal(damping_factor, Inner::damping_factor_max, "damping_factor");

        const auto constant_part = static_cast<floating_t>((static_cast<floating_t>(1) - damping_factor) / size);

        // todo(p3): del 2nd graph?
        // std::vector<std::vector<edge_t>> gr_rev(size);
        //
        // for (std::int32_t from{}; from < size; ++from)
        //{
        //    for (const auto& tod : graph[from])
        //    {
        //        gr_rev[tod].push_back(from);
        //    }
        //}

        buffer.resize(size);
        {
            const auto share = static_cast<floating_t>(static_cast<floating_t>(1) / static_cast<floating_t>(size));

            result.assign(size, share);
        }

        constexpr auto large = 100;
        // todo(p3): 300M edges took 50 steps.
        const auto max_attempts = size < large ? size * 2 : size;

        for (std::int32_t att{}; att < max_attempts; ++att)
        {
            // todo(p3): remove copy - replace with mem_set?
            // std::swap(buffer, result); std::fill(result.begin(), result.end(), 0);
            std::copy(result.cbegin(), result.cend(), buffer.begin());

            Inner::visit(graph,
                // todo(p3): gr_rev,
                buffer, constant_part, result);

            if (Inner::shall_stop<floating_t>(buffer, epsilon, result))
            {
                break;
            }
        }
    }
} // namespace Standard::Algorithms::Graphs
