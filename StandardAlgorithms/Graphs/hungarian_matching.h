#pragma once
#include"../Utilities/iota_vector.h"
#include"../Utilities/is_debug.h"
#include"../Utilities/require_utilities.h"
#include"graph_helper.h"

namespace Standard::Algorithms::Graphs::Inner
{
    constexpr std::int32_t fake_id{};
    constexpr std::int32_t oda_id = 1;

    // Time O(n).
    template<class int_t>
    [[nodiscard]] constexpr auto matching_weight(const auto &graph, const auto &matchings) -> int_t
    {
        const auto size = matchings.size();
        assert(size == graph.size());

        int_t sum{};

        for (std::size_t row{}; row < size; ++row)
        {
            const auto &col = matchings[row];
            const auto &weight = graph.at(row).at(col);

            {
                using w_t = decltype(weight);

                static_assert(sizeof(w_t) <= sizeof(int_t));
            }

            sum += static_cast<int_t>(weight);
        }

        return sum;
    }

    template<class int_t, class graph_t>
    struct hm_context final
    {
        constexpr hm_context(const graph_t &graph, const int_t &inf)
            : Graph(graph)
            , Inf(inf)
            , Row_pots(graph.size() + oda_id) // 0 initial potentials.
            , Col_pots(graph.size() + oda_id)
            , Col_row_map(graph.size() + oda_id)
            , Prev_cols(graph.size() + oda_id)
        // min_diffs, visited are re-assigned.
        {
            assert(int_t{} < inf);
        }

        [[nodiscard]] constexpr auto graph() const &noexcept -> const graph_t &
        {
            return Graph;
        }

        [[nodiscard]] constexpr auto inf() const &noexcept -> const int_t &
        {
            return Inf;
        }

        [[nodiscard]] constexpr auto row_pots() &noexcept -> std::vector<int_t> &
        {
            return Row_pots;
        }

        [[nodiscard]] constexpr auto col_pots() &noexcept -> std::vector<int_t> &
        {
            return Col_pots;
        }

        [[nodiscard]] constexpr auto col_row_map() &noexcept -> std::vector<std::int32_t> &
        {
            return Col_row_map;
        }

        [[nodiscard]] constexpr auto prev_cols() &noexcept -> std::vector<std::int32_t> &
        {
            return Prev_cols;
        }

        [[nodiscard]] constexpr auto min_diffs() &noexcept -> std::vector<int_t> &
        {
            return Min_diffs;
        }

        [[nodiscard]] constexpr auto visited() const &noexcept -> const std::vector<bool> &
        {
            return Visited;
        }

        [[nodiscard]] constexpr auto visited() &noexcept -> std::vector<bool> &
        {
            return Visited;
        }

private:
        const graph_t &Graph;
        const int_t Inf;

        std::vector<int_t> Row_pots;
        std::vector<int_t> Col_pots;
        std::vector<std::int32_t> Col_row_map;
        std::vector<std::int32_t> Prev_cols;
        std::vector<int_t> Min_diffs{};
        std::vector<bool> Visited{};
    };

    // Time O(n).
    template<class int_t, class graph_t>
    [[nodiscard]] constexpr auto choose_min_diff_column(
        hm_context<int_t, graph_t> &context, const std::int32_t prev_col) -> std::pair<int_t, std::int32_t>
    {
        const auto size = static_cast<std::int32_t>(context.graph().size());

        assert(fake_id <= prev_col && prev_col <= size);

        if constexpr (::Standard::Algorithms::is_debug)
        {
            [[maybe_unused]] const auto &raw_context = context;

            assert(!raw_context.visited().at(prev_col));
        }

        context.visited().at(prev_col) = true;

        const auto &row = context.col_row_map().at(prev_col);
        const auto &row_potential = context.row_pots().at(row);
        const auto &edges = context.graph().at(row - oda_id);

        static_assert(-oda_id < fake_id);

        auto min_diff = context.inf();
        auto chosen_column = -oda_id;

        for (auto col = oda_id; col <= size; ++col)
        {
            if (context.visited().at(col))
            {
                continue;
            }

            auto &diff = context.min_diffs().at(col);
            {
                const auto &weight = edges.at(col - oda_id);
                const auto &col_potential = context.col_pots().at(col);
                const auto candidate = static_cast<int_t>(static_cast<int_t>(weight) + col_potential - row_potential);

                assert(weight < context.inf() && candidate < context.inf());

                if (candidate < diff)
                {
                    diff = candidate, context.prev_cols().at(col) = prev_col;
                }
            }

            if (diff < min_diff)
            {
                min_diff = diff, chosen_column = col;
            }
        }

        if (chosen_column < fake_id) [[unlikely]]
        {
            throw std::runtime_error("Cannot choose a min difference column. Maybe inf is too small.");
        }

        assert(min_diff < context.inf());

        return { min_diff, chosen_column };
    }

    // Time O(n*n).
    template<class int_t, class graph_t>
    constexpr void increase_matching(hm_context<int_t, graph_t> &context)
    {
        const auto size = static_cast<std::int32_t>(context.graph().size());
        context.min_diffs().assign(size + oda_id, context.inf());
        context.visited().assign(size + oda_id, false);

        auto cur_col = fake_id;

        do
        {
            const auto [min_diff, chosen_column] = choose_min_diff_column<int_t, graph_t>(context, cur_col);
            cur_col = chosen_column;

            for (auto col = fake_id; col <= size; ++col)
            {// Include the fake column to compute the min total cost.
                if (context.visited().at(col))
                {
                    const auto &row = context.col_row_map().at(col);
                    context.row_pots().at(row) += min_diff;
                    context.col_pots().at(col) += min_diff;
                }
                else
                {
                    context.min_diffs().at(col) -= min_diff;
                }
            }
        } while (fake_id < context.col_row_map().at(cur_col));

        do
        {
            const auto &prev_col = context.prev_cols().at(cur_col);
            const auto &old_row = context.col_row_map().at(prev_col);
            context.col_row_map().at(cur_col) = old_row;
            cur_col = prev_col;
        } while (fake_id < cur_col);
    }
} // namespace Standard::Algorithms::Graphs::Inner

namespace Standard::Algorithms::Graphs
{
    // Assignment problem: given a full weighted bipartite graph Knn, n>0,
    // compute min cost perfect matching.
    // Weights can be negative. Nodes start from 0.
    // Time O(n**3).
    template<class int_t, class graph_t>
    [[nodiscard]] constexpr auto hungarian_matching(const graph_t &graph, std::vector<std::int32_t> &matchings,
        const int_t &inf = std::numeric_limits<int_t>::max() / 2) -> int_t
    {
        {
            using w_t = decltype(graph[0][0]);

            static_assert(sizeof(w_t) <= sizeof(int_t));
        }

        if (constexpr auto large = 1'000'000U; large < graph.size()) [[unlikely]]
        {
            throw std::invalid_argument("Too large (" + std::to_string(graph.size()) + ") graph size.");
        }

        require_positive(inf, "infinite cost or weight");

        const auto size = require_positive(static_cast<std::int32_t>(graph.size()), "graph size");
        require_full_graph(graph);

        Inner::hm_context<int_t, graph_t> context(graph, inf);

        for (auto row = Inner::oda_id; row <= size; ++row)
        {
            context.col_row_map().at(Inner::fake_id) = row;
            Inner::increase_matching<int_t, graph_t>(context);
        }

        matchings.resize(size);

        for (std::int32_t col{}; col < size; ++col)
        {
            const auto &row = context.col_row_map().at(col + Inner::oda_id);
            matchings.at(row - Inner::oda_id) = col;
        }

        const auto &min_total_cost = context.col_pots().at(Inner::fake_id);
        assert(min_total_cost < inf);

        return min_total_cost;
    }

    // Slow time O((n+1)!).
    template<class int_t, class graph_t>
    [[nodiscard]] constexpr auto min_cost_perfect_matching_slow(const graph_t &graph,
        std::vector<std::int32_t> &matchings, const int_t &inf = std::numeric_limits<int_t>::max() / 2) -> int_t
    {
        assert(int_t{} < inf);
        {
            using w_t = decltype(graph[0][0]);

            static_assert(sizeof(w_t) <= sizeof(int_t));
        }

        if (constexpr auto large = 15U; large < graph.size()) [[unlikely]]
        {
            // 10! == 3,628,800.
            // 15! == 1,307,674,368,000.
            throw std::invalid_argument("Too large (" + std::to_string(graph.size()) + ") graph size.");
        }

        const auto size = static_cast<std::int32_t>(graph.size());
        require_positive(size, "graph size");
        require_full_graph(graph);

        matchings.resize(size);

        auto result = inf;
        auto buf = ::Standard::Algorithms::Utilities::iota_vector<std::int32_t>(size);

        do
        {
            const auto sum = Inner::matching_weight<int_t>(graph, buf);
            if (!(sum < result))
            {
                continue;
            }

            result = sum;
            std::copy(buf.cbegin(), buf.cend(), matchings.begin());
        } while (std::next_permutation(buf.begin(), buf.end()));

        assert(result < inf);
        return result;
    }
} // namespace Standard::Algorithms::Graphs
