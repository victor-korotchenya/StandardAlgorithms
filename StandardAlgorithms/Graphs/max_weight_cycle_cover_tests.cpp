#include"max_weight_cycle_cover_tests.h"
#include"../Utilities/simple_test_case.h"
#include"../Utilities/test_utilities.h"
#include"graph_helper.h"
#include"hungarian_matching.h"
#include"max_weight_cycle_cover.h"
#include"weighted_vertex.h"

namespace
{
    using int_t = std::int32_t;
    using long_int_t = std::int64_t;
    using edge_t = Standard::Algorithms::weighted_vertex<int_t, int_t>;
    using digraph_t = std::vector<std::vector<edge_t>>;

    using cycles_t = std::vector<std::vector<std::int32_t>>;
    using output_t = std::pair<cycles_t, long_int_t>;

    struct unique_tag final
    {
    };

    using test_case = Standard::Algorithms::Tests::simple_test_case<unique_tag, digraph_t, output_t>;

    constexpr void mark_used(std::vector<bool> &useds, const std::integral auto &vertex)
    {
        if (useds.at(vertex)) [[unlikely]]
        {
            auto err = "The vertex (" + std::to_string(vertex) + ") is duplicated error.";

            throw std::runtime_error(err);
        }

        useds.at(vertex) = true;
    }

    constexpr void add_edge_weight(const auto &from_tod, const digraph_t &digraph, auto &actual_weight_sum)
    {
        const auto &from = from_tod.first;
        const auto &tod = from_tod.second;

        ::Standard::Algorithms::ert::not_equal(from, tod, "self-edge not allowed");

        const auto &edges = digraph.at(from);
        const auto edge_iter = std::find_if(edges.cbegin(), edges.cend(),
            [&tod](const auto &edge)
            {
                return edge.vertex == tod;
            });

        if (edge_iter == edges.cend()) [[unlikely]]
        {
            auto err =
                "The edge (" + std::to_string(from) + " -> " + std::to_string(tod) + ") must exist in the graph.";

            throw std::runtime_error(err);
        }

        actual_weight_sum += edge_iter->weight;

        Standard::Algorithms::require_non_negative(actual_weight_sum, "partial actual total weight sum");
    }

    constexpr void simple_validate(const test_case &test)
    {
        const auto &digraph = test.input();
        {
            constexpr auto require_non_negative_weights = true;

            Standard::Algorithms::Graphs::require_simple_graph<edge_t, require_non_negative_weights>(digraph);
        }

        const auto size =
            Standard::Algorithms::require_positive(static_cast<std::int32_t>(digraph.size()), "digraph size");

        const auto &[cycles, max_weight] = test.output();
        Standard::Algorithms::require_between(1ZU, cycles.size(), digraph.size(), "cycles size");

        Standard::Algorithms::require_non_negative(max_weight, "max weight");

        std::vector<bool> useds(size);
        long_int_t actual_weight_sum{};

        for (std::size_t index{}; const auto &cycle : cycles)
        {
            const auto name = "expected cycle " + std::to_string(index);
            Standard::Algorithms::require_positive(cycle.size(), name + " size");
            if (0U < index)
            {
                const auto &prev_cycle = cycles.at(index - 1U);
                Standard::Algorithms::require_greater(cycle, prev_cycle, "cycles must be sorted. " + name);
            }

            ++index;

            const auto &front = cycle.front();
            mark_used(useds, front);

            if (1ZU == cycle.size())
            {
                continue;
            }

            for (std::size_t ind_2{}; ++ind_2 < cycle.size();)
            {
                const auto &pre = cycle.at(ind_2 - 1);
                const auto &curr = cycle.at(ind_2);
                mark_used(useds, curr);

                add_edge_weight(std::make_pair(pre, curr), digraph, actual_weight_sum);
            }

            const auto &back = cycle.back();
            add_edge_weight(std::make_pair(back, front), digraph, actual_weight_sum);
        }

        ::Standard::Algorithms::ert::are_equal(max_weight, actual_weight_sum, "actual total weight sum");

        if (const auto iter = std::find(useds.cbegin(), useds.cend(), false); iter != useds.cend()) [[unlikely]]
        {
            const auto bad_pos = iter - useds.cbegin();
            auto err = "The vertex (" + std::to_string(bad_pos) + ") must have been matched.";

            throw std::runtime_error(err);
        }
    }

    constexpr void generate_test_cases(std::vector<test_case> &tests)
    {
        // Weights. The ned means 'no edge'.
        // 000 | 100 | 003
        // 020 | 000 | ned
        // 006 | ned | 000
        //
        // 0-1-0 weight (100 + 20), 2 weight (0), 120 in total
        // 0-2-0 weight (3 + 6), 1 weight (0), 9 in total
        tests.emplace_back("graph 3",
            digraph_t{// NOLINTNEXTLINE
                { { 1, 100 }, { 2, 3 } }, // 0 // NOLINTNEXTLINE
                { { 0, 20 } }, // 1 // NOLINTNEXTLINE
                { { 0, 6 } } }, // 2 // NOLINTNEXTLINE
            output_t{ cycles_t{ { 1, 0 }, { 2 } }, // NOLINTNEXTLINE
                120 });

        tests.emplace_back("K2",
            digraph_t{// NOLINTNEXTLINE
                { { 1, 10 } }, // 0 // NOLINTNEXTLINE
                { { 0, 368 } } }, // 1 // NOLINTNEXTLINE
            output_t{ cycles_t{ { 1, 0 } }, 378 });

        tests.emplace_back("graph 2 - no cycles",
            digraph_t{// NOLINTNEXTLINE
                { { 1, 10 } }, // 0 // NOLINTNEXTLINE
                {} }, // 1 // NOLINTNEXTLINE
            output_t{ cycles_t{ { 0 }, { 1 } }, 0 });

        tests.emplace_back("K1", digraph_t{ {} }, output_t{ cycles_t{ { 0 } }, 0 });
    }

    constexpr void run_test_case(const test_case &test)
    {
        auto perfect_matcher = [] [[nodiscard]] (const auto &graph_2, auto &matchings_2)
        {
            auto weight = Standard::Algorithms::Graphs::hungarian_matching<long_int_t>(graph_2, matchings_2);
            return weight;
        };

        const auto &digraph = test.input();

        std::vector<std::vector<std::int32_t>> cycles;

        const auto fast_weight = Standard::Algorithms::Graphs::max_weight_cycle_cover<int_t, long_int_t, digraph_t>(
            digraph, cycles, perfect_matcher);

        std::sort(cycles.begin(), cycles.end());

        const auto &[expected_cycles, expected_max_weight] = test.output();

        ::Standard::Algorithms::ert::are_equal(expected_max_weight, fast_weight, "max_weight_cycle_cover weight");

        ::Standard::Algorithms::ert::are_equal(expected_cycles, cycles, "max_weight_cycle_cover cycles");
    }
} // namespace

void Standard::Algorithms::Graphs::Tests::max_weight_cycle_cover_tests()
{
    ::Standard::Algorithms::Tests::test_utilities<test_case>::run_tests(run_test_case, generate_test_cases);
}
