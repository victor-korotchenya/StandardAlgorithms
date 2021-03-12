#include"work_critical_path_tests.h"
#include"../Utilities/test_utilities.h"
#include"weighted_vertex.h"
#include"work_critical_path.h"

namespace
{
    using weight_t = std::int64_t;
    using edge_t = Standard::Algorithms::weighted_vertex<std::int32_t, weight_t>;
    using edges_t = std::vector<edge_t>;
    using graph_t = std::vector<edges_t>;

    struct wcp_test_case final : Standard::Algorithms::Tests::base_test_case
    {
        constexpr wcp_test_case(std::string &&name, graph_t &&graph, std::vector<std::size_t> &&topol_order,
            weight_t total_time, std::vector<std::pair<weight_t, weight_t>> &&vertex_earliest_latest_times,
            std::vector<std::vector<weight_t>> &&edge_slacks, std::size_t source,
            std::vector<std::size_t> &&critical_path, std::size_t destination)
            : base_test_case(std::move(name))
            , Graph(std::move(graph))
            , Topol_order(std::move(topol_order))
            , Total_time(total_time)
            , Vertex_earliest_latest_times(std::move(vertex_earliest_latest_times))
            , Edge_slacks(std::move(edge_slacks))
            , Source(source)
            , Critical_path(std::move(critical_path))
            , Destination(destination)
        {
        }

        constexpr void validate() const override
        {
            base_test_case::validate();

            ::Standard::Algorithms::ert::are_equal(Graph.size(), Topol_order.size(), "topological order size.");

            ::Standard::Algorithms::ert::greater_or_equal(Total_time, weight_t{}, "total time");

            ::Standard::Algorithms::ert::are_equal(
                Graph.size(), Vertex_earliest_latest_times.size(), "vertex_earliest_latest_times size.");

            ::Standard::Algorithms::ert::are_equal(Graph.size(), Edge_slacks.size(), "edge slacks size.");

            ::Standard::Algorithms::ert::greater_or_equal(Graph.size(), Critical_path.size(), "critical path size.");
        }

        void print(std::ostream &str) const override
        {
            ::Standard::Algorithms::print("graph", Graph, str);
            ::Standard::Algorithms::print("topological order", Topol_order, str);
            ::Standard::Algorithms::print_value("total time", str, Total_time);
            ::Standard::Algorithms::print("vertex_earliest_latest_times", Vertex_earliest_latest_times, str);
            ::Standard::Algorithms::print("edge slacks", Edge_slacks, str);
            ::Standard::Algorithms::print_value("source", str, Source);
            ::Standard::Algorithms::print("critical path", Critical_path, str);
            ::Standard::Algorithms::print_value("destination", str, Destination);
        }

        [[nodiscard]] constexpr auto graph() const &noexcept -> const graph_t &
        {
            return Graph;
        }

        [[nodiscard]] constexpr auto topol_order() const &noexcept -> const std::vector<std::size_t> &
        {
            return Topol_order;
        }

        [[nodiscard]] constexpr auto total_time() const noexcept -> weight_t
        {
            return Total_time;
        }

        [[nodiscard]] constexpr auto vertex_earliest_latest_times() const &noexcept
            -> const std::vector<std::pair<weight_t, weight_t>> &
        {
            return Vertex_earliest_latest_times;
        }

        [[nodiscard]] constexpr auto edge_slacks() const &noexcept -> const std::vector<std::vector<weight_t>> &
        {
            return Edge_slacks;
        }

        [[nodiscard]] constexpr auto source() const noexcept -> std::size_t
        {
            return Source;
        }

        [[nodiscard]] constexpr auto critical_path() const &noexcept -> const std::vector<std::size_t> &
        {
            return Critical_path;
        }

        [[nodiscard]] constexpr auto destination() const noexcept -> std::size_t
        {
            return Destination;
        }

private:
        graph_t Graph;
        std::vector<std::size_t> Topol_order;
        weight_t Total_time;
        std::vector<std::pair<weight_t, weight_t>> Vertex_earliest_latest_times;
        std::vector<std::vector<weight_t>> Edge_slacks;
        std::size_t Source;
        std::vector<std::size_t> Critical_path;
        std::size_t Destination;
    };

    constexpr void generate_test_cases(std::vector<wcp_test_case> &test_cases)
    {
        test_cases.push_back({ "an empty graph", {}, {}, 0, {}, {}, 0U, {}, 0U });

        // todo(p4): when no path, catch an exception.

        test_cases.push_back({ "a vertex", { {} }, { 0U }, 0, { { 0, 0 } }, { {} }, 0U, {}, 0U });

        {
            constexpr weight_t weigh = 50;

            test_cases.push_back({
                "an edge", { { { 1, weigh } }, {} }, // graph
                { 0U, 1U }, // topological order
                weigh, // total time
                { { 0, 0 }, { weigh, weigh } }, // vertex_earliest_latest_times
                { { 0 }, {} }, // edge slacks
                0U, // source,
                { 0U, 1U }, // critical path
                1U // destination
            });
        }
        {// A graph of 3 components.
            //            v1
            //      /20       \3
            //  v0                  v3   _10000   v4
            //      \901     /50
            //            v2
            //
            // v6  _4444  v5
            //
            // v7
            //
            // Earliest completion time:
            //            v1(20)
            //      /20       \3
            //  v0(0)               v3(951)   _10000   v4(10951)
            //      \901     /50
            //            v2(901)
            //
            // v6(0)  _4444  v5(4444)
            //
            // v7(0)
            //
            // Latest completion time:
            //            v1(948)
            //      /20       \3
            //  v0(0)               v3(951)   _10000   v4(10951)
            //      \901     /50
            //            v2(901)
            //
            // v6(6507)  _4444  v5(10951)
            //
            // v7(10951)
            constexpr weight_t w01 = 20;
            constexpr weight_t w02 = 901;
            constexpr weight_t w13 = 3;
            constexpr weight_t w23 = 50;
            constexpr weight_t w34 = 10'000;
            constexpr weight_t w65 = 4'444;
            constexpr weight_t total_time = w02 + w23 + w34; // 10951

            test_cases.push_back({
                "simple 1",
                { { { 1, w01 }, { 2, w02 } }, // vertex 0
                    { { 3, w13 } }, { { 3, w23 } }, // vertex 2
                    { { 4, w34 } }, {}, // vertex 4
                    {},
                    // NOLINTNEXTLINE
                    { { 5, w65 } }, // vertex 6
                    {} }, // graph
                // NOLINTNEXTLINE
                { 0U, 6U, 7U, 1U, 2U, 5U, 3U, 4U }, // topol_order
                total_time,
                { { 0, 0 }, // vertex 0
                    { w01, w23 + w02 - w13 }, { w02, w02 }, // vertex 2
                    { w02 + w23, w02 + w23 }, { total_time, total_time }, // vertex 4
                    { w65, total_time }, { 0, total_time - w65 }, // vertex 6
                    { 0, total_time } }, // vertex_earliest_latest_times
                { { w23 + w02 - w13 - w01, 0 }, // vertex 0
                    { w23 + w02 - w13 - w01 }, { 0 }, // vertex 2
                    { 0 }, {}, // vertex 4
                    {}, { total_time - w65 }, // vertex 6
                    {} }, // edge_slacks
                0U, // source,
                { 0U, 2U, 3U, 4U } // critical_path
                ,
                4U // destination
            });
        }
    }

    void run_test_case(const wcp_test_case &test)
    {
        const auto &dag = test.graph();

        std::vector<std::size_t> topol_order;
        std::vector<std::pair<weight_t, weight_t>> vertex_earliest_latest_times;

        const auto total_time = Standard::Algorithms::Graphs::compute_earliest_completion_times(
            dag, topol_order, vertex_earliest_latest_times);

        ::Standard::Algorithms::ert::are_equal(test.total_time(), total_time, "total time");

        ::Standard::Algorithms::ert::are_equal(test.topol_order(), topol_order, "topological sort");

        std::vector<std::vector<weight_t>> edge_slacks;
        Standard::Algorithms::Graphs::compute_latest_and_slacks(
            dag, topol_order, total_time, vertex_earliest_latest_times, edge_slacks);

        ::Standard::Algorithms::ert::are_equal(
            test.vertex_earliest_latest_times(), vertex_earliest_latest_times, "vertex_earliest_latest_times");

        ::Standard::Algorithms::ert::are_equal(test.edge_slacks(), edge_slacks, "edge slacks");

        std::vector<std::size_t> critical_path;

        Standard::Algorithms::Graphs::work_critical_path(
            dag, test.source(), edge_slacks, test.destination(), critical_path);
        ::Standard::Algorithms::ert::are_equal(test.critical_path(), critical_path, "critical path");
    }
} // namespace

void Standard::Algorithms::Graphs::Tests::work_critical_path_tests()
{
    Standard::Algorithms::Tests::test_utilities<wcp_test_case>::run_tests(run_test_case, generate_test_cases);
}
