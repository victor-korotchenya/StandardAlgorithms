#pragma once
#include"../Numbers/arithmetic.h"
#include"../Utilities/iota_vector.h"
#include"../Utilities/require_utilities.h"
#include"graph_helper.h"

namespace Standard::Algorithms::Graphs::Inner
{
    constexpr auto perf_match_graph_max_size = 30U;

    template<std::signed_integral weight_t2, class graph_t>
    struct min_w_perf_match_context final
    {
        constexpr explicit min_w_perf_match_context(const graph_t &graph, const bool init_start = true)
            : Graph(graph)
            , Buf(1LLU << require_less_equal(graph.size(), perf_match_graph_max_size, "graph size"),
                  -static_cast<weight_t2>(1))
        {
            const auto size = graph.size();

            assert(2 <= size && size <= perf_match_graph_max_size && Buf.at(0) < weight_t2());

            if (init_start)
            {
                Buf.at(0) = weight_t2();
                assert(Buf.back() < weight_t2());
                return;
            }

            const auto mask_all = (1LLU << size) - 1LLU;
            Buf.at(mask_all) = weight_t2();
            assert(Buf[0] < weight_t2() && Buf.back() == weight_t2());
        }

        [[nodiscard]] constexpr auto graph() const &noexcept -> const graph_t &
        {
            return Graph;
        }

        [[nodiscard]] constexpr auto buf() const &noexcept -> const std::vector<weight_t2> &
        {
            return Buf;
        }

        [[nodiscard]] constexpr auto buf() &noexcept -> std::vector<weight_t2> &
        {
            return Buf;
        }

private:
        const graph_t &Graph;
        std::vector<weight_t2> Buf;
    };

    template<std::signed_integral weight_t, class graph_t>
    constexpr void min_cost_perfect_matching_rec(
        min_w_perf_match_context<weight_t, graph_t> &context, const std::uint32_t mask)
    {
        constexpr auto one = 1U;
        constexpr auto inf = std::numeric_limits<weight_t>::max();
        static_assert(weight_t{} < inf);

        [[maybe_unused]] const auto size = static_cast<std::uint32_t>(context.graph().size());

        assert(1U < size && 0U < mask && mask < (1LLU << size) && 2 <= Standard::Algorithms::Numbers::pop_count(mask));

        auto &boo = context.buf()[mask];
        assert(boo < weight_t{});
        boo = inf;

        const auto from = Standard::Algorithms::Numbers::most_significant_bit(mask);

        auto lefts = mask ^ (one << from);
        assert(from < size && lefts < mask && 0U < lefts);

        const auto &edges = context.graph()[from];

        do
        {
            const auto tod = Standard::Algorithms::Numbers::most_significant_bit(lefts);
            assert(tod < size && tod != from && (lefts ^ (one << tod)) < lefts);

            lefts ^= one << tod;

            const auto &edge_cost = edges[tod];
            assert(weight_t{} <= edge_cost && edge_cost < inf / 2);

            if (!(edge_cost < boo))
            {
                continue;
            }

            const auto mask2 = mask ^ (one << from) ^ (one << tod);
            assert(mask2 < mask);

            const auto &boo2 = context.buf()[mask2];
            if (boo2 < weight_t{})
            {
                min_cost_perfect_matching_rec<weight_t, graph_t>(context, mask2);
            }

            const auto cand = static_cast<weight_t>(boo2 + static_cast<weight_t>(edge_cost));

            assert(weight_t{} <= std::min(boo2, cand) && std::max(boo2, cand) < inf / 2);

            if (cand < boo)
            {
                boo = cand;
            }
        } while (lefts != 0U);

        assert(weight_t{} <= boo && boo < inf / 2);
    }

    template<std::signed_integral weight_t, class graph_t>
    [[nodiscard]] constexpr auto min_cost_perfect_matching_other_rec(
        min_w_perf_match_context<weight_t, graph_t> &context, const std::uint32_t mask = {}) -> weight_t
    {
        auto &boo = context.buf()[mask];
        if (!(boo < 0))
        {
            return boo;
        }

        constexpr weight_t inf = std::numeric_limits<weight_t>::max() / 2;
        static_assert(weight_t{} < inf);

        boo = inf;

        constexpr auto one = 1U;
        auto from = 0U;

        while (from < context.graph().size() && (mask & (one << from)) != 0U)
        {
            ++from;
        }

        for (auto tod = from + one; tod < context.graph().size(); ++tod)
        {
            if ((mask & (one << tod)) != 0U)
            {
                continue;
            }

            const auto mask2 = mask | (1U << from) | (1U << tod);
            const auto cand =
                min_cost_perfect_matching_other_rec<weight_t>(context, mask2) + context.graph()[from][tod];

            if (cand < boo)
            {
                boo = cand;
            }
        }

        assert(0 <= boo && boo < inf);
        return boo;
    }
} // namespace Standard::Algorithms::Graphs::Inner

namespace Standard::Algorithms::Graphs
{
    // Given a full graph K(n, n) on 2*n nodes with non-negative edge costs,
    // find a minimum weight perfect matching.
    // It's used e.g. in Christofides Serdyukov 1.5 approximation to compute metric TSP.
    // todo(p3): faster LP.

    template<std::signed_integral weight_t2, std::signed_integral weight_t>
    requires(sizeof(weight_t) <= sizeof(weight_t2))
    [[nodiscard]] constexpr auto min_cost_perfect_matching(
        const std::vector<std::vector<weight_t>> &graph, std::vector<std::size_t> &matching) -> weight_t2
    {
        const auto size = graph.size();
        matching.resize(size);

        if (size == 0U)
        {
            return {};
        }

        if ((size & 1U) != 0U) [[unlikely]]
        {
            throw std::invalid_argument("Size (" + std::to_string(size) + ") must be even");
        }

        require_full_graph(graph, true);

        std::iota(matching.begin(), matching.end(), 1U);
        matching.back() = 0U;

        if (size == 2U)
        {
            return graph[0][1];
        }

        // NOLINTNEXTLINE
        assert(4U <= size && size <= 20U);

        using graph_typ = std::remove_cvref_t<decltype(graph)>;

        Inner::min_w_perf_match_context<weight_t2, graph_typ> context(graph);

        const auto &result = context.buf().back();
        assert(result < 0);

        constexpr auto one = 1U;
        const auto mask_all = (one << size) - one;
        min_cost_perfect_matching_rec<weight_t2, graph_typ>(context, mask_all);

        assert(!(result < 0));

        return result;
    }

    template<std::signed_integral weight_t2, std::signed_integral weight_t>
    requires(sizeof(weight_t) <= sizeof(weight_t2))
    [[nodiscard]] constexpr auto min_cost_perfect_matching_other(const std::vector<std::vector<weight_t>> &graph)
        -> weight_t2
    {
        const auto size = graph.size();
        if (size == 0U)
        {
            return {};
        }

        if (size & 1U) [[unlikely]]
        {
            throw std::invalid_argument("Size (" + std::to_string(size) + ") must be even");
        }

        require_full_graph(graph, true);

        // NOLINTNEXTLINE
        assert(2 <= size && size <= 20);

        Inner::min_w_perf_match_context<weight_t2, decltype(graph)> context(graph, false);

        auto result = min_cost_perfect_matching_other_rec<weight_t2>(context);

        assert(!(result < 0));

        return result;
    }

    template<class weight_t2, class weight_t>
    requires(sizeof(weight_t) <= sizeof(weight_t2))
    [[nodiscard]] constexpr auto min_cost_perfect_matching_slow(
        const std::vector<std::vector<weight_t>> &graph, std::vector<std::size_t> &matching) -> weight_t2
    {
        const auto size = graph.size();
        matching.resize(size);

        if (size == 0U)
        {
            return {};
        }

        if (size & 1U) [[unlikely]]
        {
            throw std::invalid_argument("Size (" + std::to_string(size) + ") must be even");
        }

        assert(1U < size);
        require_full_graph(graph, true);

        constexpr auto inf = std::numeric_limits<weight_t2>::max();
        auto result = inf;
        auto buf = ::Standard::Algorithms::Utilities::iota_vector<std::size_t>(size);

        do
        {
            weight_t2 sum{};

            for (std::size_t index{}; index < size; index += 2)
            {
                const auto &from = buf[index];
                const auto &tod = buf[index | 1U];
                const auto &cost = graph[from][tod];
                sum += cost;

                assert(cost < inf / 4 && sum < inf / 4 && !(sum < 0));
            }

            if (!(sum < result))
            {
                continue;
            }

            result = sum;

            for (std::size_t index{}; index < size; index += 2)
            {
                const auto &from = buf[index];
                const auto &tod = buf[index | 1U];
                matching[from] = tod;
                matching[tod] = from;
            }
        } while (std::next_permutation(buf.begin(), buf.end()));

        assert(!(result < 0));

        return result;
    }
} // namespace Standard::Algorithms::Graphs
