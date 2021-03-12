#pragma once
#include <numeric>
#include <type_traits>
#include <vector>
#include "../Numbers/Arithmetic.h"
#include "../Utilities/ExceptionUtilities.h"
#include "graph_helper.h"

namespace
{
    template<class weight_t2, class graph_t>
    struct min_w_perf_match_context final
    {
        const graph_t& graph;
        std::vector<weight_t2> buf;

        explicit min_w_perf_match_context(const graph_t& graph, const bool init_start = true)
            : graph(graph),
            buf(1llu << graph.size(), weight_t2(-1))
        {
            const auto size = graph.size();
            assert(size >= 2 && size <= 30 && buf[0] < 0);

            if (init_start)
            {
                buf[0] = 0;
                assert(buf.back() < 0);
                return;
            }

            const auto mask_all = (1llu << size) - 1llu;
            buf[mask_all] = 0;
            assert(buf[0] < 0 && buf.back() == 0);
        }
    };

    template<class weight_t, class graph_t>
    void min_cost_perfect_matching_rec(min_w_perf_match_context<weight_t, graph_t>& context, const unsigned mask)
    {
        static_assert(std::is_signed_v<weight_t>);
        constexpr auto one = 1u;
        constexpr auto inf = std::numeric_limits<weight_t>::max();

#if _DEBUG
        const auto size = static_cast<unsigned>(context.graph.size());
        assert(size >= 2u && mask > 0 && mask < 1llu << size && PopCount(mask) >= 2);
#endif

        auto& b = context.buf[mask];
        assert(b < 0);

        b = inf;
        assert(0 < b);

        const auto from = MostSignificantBit(mask);
        auto lefts = mask ^ one << from;
#if _DEBUG
        assert(from < size&& lefts < mask&& lefts > 0);
#endif

        const auto& g1 = context.graph[from];
        do
        {
            const auto to = MostSignificantBit(lefts);
#if _DEBUG
            assert(to < size&& to != from && (lefts ^ one << to) < lefts);
#endif
            lefts ^= one << to;

            const auto mask2 = mask ^ one << from ^ one << to;
            assert(mask2 < mask);
            const auto& b2 = context.buf[mask2];
            if (b2 < 0)
                min_cost_perfect_matching_rec<weight_t, graph_t>(context, mask2);

            const auto cand = b2 + static_cast<weight_t>(g1[to]);
            assert(b2 >= 0 && cand >= 0 && g1[to] >= 0 && cand < inf / 2);
            if (b > cand)
                b = cand;
        } while (lefts);
    }

    template<class weight_t, class graph_t>
    weight_t min_cost_perfect_matching_they_rec(min_w_perf_match_context<weight_t, graph_t>& context, const unsigned mask = {})
    {
        static_assert(std::is_signed_v<weight_t>);

        auto& b = context.buf[mask];
        if (b >= 0)
            return b;

        constexpr auto inf = std::numeric_limits<weight_t>::max() / 2;
        b = inf;
        assert(b > 0);

        constexpr auto one = 1u;
        auto from = 0u;
        while (from < context.graph.size() && mask & one << from)
            ++from;

        for (auto to = from + 1u; to < context.graph.size(); ++to)
        {
            if (mask & 1u << to)
                continue;

            const auto mask2 = mask | 1u << from | 1u << to;
            const auto cand = min_cost_perfect_matching_they_rec<weight_t>(context, mask2) + context.graph[from][to];

            if (b > cand)
                b = cand;
        }

        assert(b >= 0 && b < inf);
        return b;
    }
}

namespace Privet
{
    namespace Algorithms
    {
        namespace Graphs
        {
            // Given a full graph on even number of nodes
            // with possibly negative edge costs,
            // find a minimum weight perfect matching.
            template<class weight_t2, class weight_t>
            weight_t2 min_cost_perfect_matching(const std::vector<std::vector<weight_t>>& g, std::vector<size_t>& matching)
            {
                static_assert(sizeof(weight_t) <= sizeof(weight_t2) && std::is_signed_v<weight_t2>);

                const auto size = g.size();
                matching.resize(size);

                if (!size)
                    return {};

                if (size & 1u)
                    throw std::invalid_argument("Size (" + std::to_string(size) + ") must be even");

                require_full_graph(g, true);

                std::iota(matching.begin(), matching.end(), 1u);
                matching.back() = 0u;
                if (size == 2)
                    return g[0][1];

                assert(size >= 4 && size <= 20);

                using graph_typ = std::remove_cv_t<std::remove_reference_t<decltype(g)>>;
                min_w_perf_match_context<weight_t2, graph_typ> context(g);
                const auto& result = context.buf.back();
                assert(result < 0);

                constexpr auto one = 1u;
                const auto mask_all = (one << size) - one;
                min_cost_perfect_matching_rec<weight_t2, graph_typ>(context, mask_all);

                assert(result >= 0);
                return result;
            }

            template<class weight_t2, class weight_t>
            weight_t2 min_cost_perfect_matching_they(const std::vector<std::vector<weight_t>>& g)
            {
                static_assert(sizeof(weight_t) <= sizeof(weight_t2) && std::is_signed_v<weight_t2>);

                const auto size = g.size();
                if (!size)
                    return {};

                if (size & 1u)
                    throw std::invalid_argument("Size (" + std::to_string(size) + ") must be even");

                require_full_graph(g, true);
                assert(size >= 2 && size <= 20);

                min_w_perf_match_context<weight_t2, decltype(g)> context(g, false);

                const auto result = min_cost_perfect_matching_they_rec<weight_t2>(context);

                assert(result >= 0);
                return result;
            }

            template<class weight_t2, class weight_t>
            weight_t2 min_cost_perfect_matching_slow(const std::vector<std::vector<weight_t>>& g, std::vector<size_t>& matching)
            {
                static_assert(sizeof(weight_t) <= sizeof(weight_t2));

                const auto size = g.size();
                matching.resize(size);

                if (!size)
                    return {};

                if (size & 1u)
                    throw std::invalid_argument("Size (" + std::to_string(size) + ") must be even");

                assert(size >= 2);
                require_full_graph(g, true);

                std::vector<size_t> buf(size);
                std::iota(buf.begin(), buf.end(), 0u);

                constexpr auto inf = std::numeric_limits<weight_t2>::max();
                auto result = inf;
                do
                {
                    weight_t2 sum{};
                    for (size_t i = 0; i < size; i += 2)
                    {
                        const auto& from = buf[i], & to = buf[i | 1u];
                        const auto& cost = g[from][to];
                        sum += cost;
                        assert(cost < inf / 4 && sum < inf / 4 && sum >= 0);
                    }

                    if (result <= sum)
                        continue;

                    result = sum;
                    for (size_t i = 0; i < size; i += 2)
                    {
                        const auto& from = buf[i], & to = buf[i | 1u];
                        matching[from] = to, matching[to] = from;
                    }
                } while (std::next_permutation(buf.begin(), buf.end()));

                assert(result >= 0);
                return result;
            }
        }
    }
}