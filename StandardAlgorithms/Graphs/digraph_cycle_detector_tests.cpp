#include"digraph_cycle_detector_tests.h"
#include"../Utilities/test_utilities.h"
#include"digraph_cycle_detector.h"
#include"graph_helper.h"
#include"topological_sort.h"
#include<unordered_set>

namespace
{
    using vertex_t = std::int32_t;

    template<class vertex_t>
    constexpr void demand_cycle_exists(
        const std::vector<std::vector<vertex_t>> &graph, const std::vector<vertex_t> &cycle)
    {
        const auto size = static_cast<vertex_t>(cycle.size());
        if (size == 0)
        {
            return;
        }

        Standard::Algorithms::require_greater(cycle.size(), 1U, "The cycle size cannot be 1.");

        for (auto index = 1; index < size; ++index)
        {
            Standard::Algorithms::Graphs::demand_edge_exists(cycle[index - 1], graph, cycle[index]);
        }

        Standard::Algorithms::Graphs::demand_edge_exists(cycle.back(), graph, cycle.front());
    }

    struct test_case final : Standard::Algorithms::Tests::base_test_case
    {
        constexpr test_case(
            std::string &&name, std::vector<std::vector<vertex_t>> &&graph, std::vector<vertex_t> &&cycle) noexcept
            : base_test_case(std::move(name))
            , Graph(std::move(graph))
            , Cycle(std::move(cycle))
        {
        }

        [[nodiscard]] constexpr auto graph() const &noexcept -> const std::vector<std::vector<vertex_t>> &
        {
            return Graph;
        }

        [[nodiscard]] constexpr auto cycle() const &noexcept -> const std::vector<vertex_t> &
        {
            return Cycle;
        }

        void validate() const override
        {
            base_test_case::validate();

            const auto size = Standard::Algorithms::require_positive(Graph.size(), "graph size.");
            ::Standard::Algorithms::ert::greater_or_equal(size, Cycle.size(), "graph size vs. cycle size.");

            Standard::Algorithms::Graphs::require_simple_graph(Graph);
            demand_cycle_exists(Graph, Cycle);

            std::unordered_set<vertex_t> uniques;

            Standard::Algorithms::Graphs::check_vertices(static_cast<vertex_t>(size), Cycle, "cycle", uniques);
        }

        void print(std::ostream &str) const override
        {
            ::Standard::Algorithms::print("graph", Graph, str);
            ::Standard::Algorithms::print("cycle", Cycle, str);
        }

private:
        std::vector<std::vector<vertex_t>> Graph;
        std::vector<vertex_t> Cycle;
    };

    constexpr void generate_test_cases(std::vector<test_case> &tests)
    {
        tests.push_back({ "5-cycle",
            // NOLINTNEXTLINE
            { { 1 }, { 2 }, { 3 }, { 4 }, { 5 }, { 1 } },
            // NOLINTNEXTLINE
            { 1, 2, 3, 4, 5 } });

        tests.push_back({ "no-cycle", { { 1 }, { 2, 3 }, { 4 }, { 2, 4 }, {}, {} }, {} });

        tests.push_back({ "2-cycle on 4 nodes", { { 1 }, {}, { 0, 3 }, { 2 }, { 1 } }, { 2, 3 } });
    }

    constexpr void run_test_case(const test_case &test)
    {
        {
            const auto actual = Standard::Algorithms::Graphs::digraph_cycle_detector(test.graph());
            ::Standard::Algorithms::ert::are_equal(test.cycle(), actual, "digraph_cycle_detector");
        }

        const auto expected_has_cycle = !test.cycle().empty();
        {
            const auto topol = Standard::Algorithms::Graphs::topological_sort(test.graph());
            const auto actual = !topol.has_value();

            ::Standard::Algorithms::ert::are_equal(expected_has_cycle, actual, "topological_sort has cycle");
        }
        {
            const auto topol = Standard::Algorithms::Graphs::topological_sort_via_dfs(test.graph());
            const auto actual = !topol.has_value();

            ::Standard::Algorithms::ert::are_equal(expected_has_cycle, actual, "topological_sort_via_dfs has cycle");
        }
    }
} // namespace

void Standard::Algorithms::Graphs::Tests::digraph_cycle_detector_tests()
{
    Standard::Algorithms::Tests::test_utilities<test_case>::run_tests(run_test_case, generate_test_cases);
}
