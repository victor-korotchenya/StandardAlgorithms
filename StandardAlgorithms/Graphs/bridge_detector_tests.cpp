#include"bridge_detector_tests.h"
#include"../Utilities/remove_duplicates.h"
#include"../Utilities/test_utilities.h"
#include"bridge_detector.h"
#include"create_random_graph.h"
#include"graph_helper.h"

namespace
{
    template<class val_t>
    constexpr void sort_pairs(std::vector<std::pair<val_t, val_t>> &pairs, const bool min_first = true)
    {
        if (min_first)
        {
            for (auto &par : pairs)
            {
                if (par.second < par.first)
                {
                    std::swap(par.second, par.first);
                }
            }
        }

        std::sort(pairs.begin(), pairs.end());
    }

    struct test_case final : Standard::Algorithms::Tests::base_test_case
    {
        constexpr test_case(std::string &&name, std::vector<std::vector<std::int32_t>> &&graph,
            std::vector<std::pair<std::int32_t, std::int32_t>> &&bridges, std::vector<std::int32_t> &&cut_nodes,
            bool is_computed = true) noexcept
            : base_test_case(std::move(name))
            , Graph(std::move(graph))
            , Bridges(std::move(bridges))
            , Cut_nodes(std::move(cut_nodes))
            , Ist_computed(is_computed)
        {
            sort_pairs(Bridges);
            Standard::Algorithms::remove_duplicates(Bridges);

            Standard::Algorithms::sort_remove_duplicates(Cut_nodes);
        }

        [[nodiscard]] constexpr auto graph() const &noexcept -> const std::vector<std::vector<std::int32_t>> &
        {
            return Graph;
        }

        [[nodiscard]] constexpr auto bridges() const &noexcept
            -> const std::vector<std::pair<std::int32_t, std::int32_t>> &
        {
            return Bridges;
        }

        [[nodiscard]] constexpr auto cut_nodes() const &noexcept -> const std::vector<std::int32_t> &
        {
            return Cut_nodes;
        }

        [[nodiscard]] constexpr auto is_computed() const noexcept -> bool
        {
            return Ist_computed;
        }

        constexpr void validate() const override
        {
            base_test_case::validate();

            const auto size = Standard::Algorithms::require_positive(Graph.size(), "graph size.");

            Standard::Algorithms::Graphs::require_simple_graph(Graph);
            Standard::Algorithms::Graphs::require_undirected_graph(Graph);

            // There can be at most n-1 bridges in a simple graph.
            ::Standard::Algorithms::ert::greater(size, Bridges.size(), "graph size vs. bridges size.");

            const std::string bridges_name("bridges");

            for (const auto &par : Bridges)
            {
                ::Standard::Algorithms::ert::not_equal(par.first, par.second, "Self loops not allowed.");

                Standard::Algorithms::Graphs::demand_edge_exists(par.first, Graph, par.second, bridges_name);
            }

            if (Cut_nodes.empty())
            {
                return;
            }

            ::Standard::Algorithms::ert::greater_or_equal(Cut_nodes[0], 0, "0-th cut node");

            ::Standard::Algorithms::ert::greater(static_cast<std::int32_t>(size), Cut_nodes.back(), "last cut node");
        }

        void print(std::ostream &str) const override
        {
            ::Standard::Algorithms::print("graph", Graph, str);
            ::Standard::Algorithms::print("bridges", Bridges, str);
            ::Standard::Algorithms::print("cut nodes", Cut_nodes, str);
            ::Standard::Algorithms::print_value("is computed", str, Ist_computed);
        }

private:
        std::vector<std::vector<std::int32_t>> Graph;
        std::vector<std::pair<std::int32_t, std::int32_t>> Bridges;
        std::vector<std::int32_t> Cut_nodes;
        bool Ist_computed;
    };

    constexpr void generate_test_cases(std::vector<test_case> &tests)
    {
        tests.push_back({ "K1", // todo(p4): use tests.emplace_back(
            {
                {},
            },
            {}, {} });

        tests.push_back({ "K2",
            {
                { 1 },
                { 0 },
            },
            { { 0, 1 } }, {} });

        tests.push_back({ "K1, two K2",
            {
                { 1 }, // 0
                { 0 }, {}, // 2
                { 4 }, { 3 }, // 4
            },
            { { 0, 1 }, { 3, 4 } }, {} });

        tests.push_back({ "0-1-2",
            {
                { 1 },
                { 0, 2 },
                { 1 },
            },
            {
                { 0, 1 },
                { 1, 2 },
            },
            { 1 } });

        tests.push_back({ "1-0-2",
            {
                { 1, 2 },
                { 0 },
                { 0 },
            },
            {
                { 0, 1 },
                { 0, 2 },
            },
            { 0 } });

        tests.push_back({
            "K3",
            {
                { 1, 2 },
                { 0, 2 },
                { 1, 0 },
            },
            {},
            {},
        });

        tests.push_back({ "K4 - K1", { { 2, 3, 1 }, { 2, 0 }, { 0, 1, 3 }, { 0, 2 } }, {}, {} });

        tests.push_back({
            "6-cycle",
            {
                // 0-1-3-2-4-5-0
                // NOLINTNEXTLINE
                { 1, 5 }, // 0
                { 3, 0 },
                { 4, 3 }, // 2
                { 1, 2 },
                // NOLINTNEXTLINE
                { 5, 2 }, // 4
                { 0, 4 },
            },
            {},
            {},
        });

        tests.push_back({ "edge + 5-cycle",
            {
                // 0 - 1-2-3-4-5-1
                { 1 }, // 0
                       // NOLINTNEXTLINE
                { 2, 0, 5 },
                { 3, 1 }, // 2
                { 4, 2 },
                // NOLINTNEXTLINE
                { 5, 3 }, // 4
                { 1, 4 },
            },
            { { 0, 1 } }, { 1 } });

        tests.push_back({ "7 tree",
            {
                // 0-2-1
                //   |
                // 4-5  3-2-6
                { 2 }, // 0
                { 2 },
                // NOLINTNEXTLINE
                { 0, 1, 3, 6, 5 }, // 2
                { 2 },
                // NOLINTNEXTLINE
                { 5 }, // 4
                { 2, 4 }, { 2 }, // 6
            },
            // NOLINTNEXTLINE
            { { 0, 2 }, { 1, 2 }, { 2, 5 }, // NOLINTNEXTLINE
                { 4, 5 }, { 2, 3 }, { 2, 6 } },
            // NOLINTNEXTLINE
            { 2, 5 } });

        tests.push_back({
            "Two touching K3",
            {
                // 2-0-1-2
                // |
                // 2-3-4-2
                { 1, 2 }, // 0
                { 0, 2 }, { 0, 1, 3, 4 }, // 2
                { 2, 4 }, { 2, 3 }, // 4
            },
            {},
            { 2 },
        });

        tests.push_back({ "base1 undirected",
            {
                {}, // 0
                { 2, 3 }, { 1, 3, 4 }, // 2
                // NOLINTNEXTLINE
                { 2, 4, 5, 1 }, { 2, 3 }, // 4
                { 3 },
                // 0 1-2-4-3-1
                //     |
                //     3-5
            },
            // NOLINTNEXTLINE
            { { 3, 5 } }, { 3 } });

        constexpr auto max_attempts = 1;

        for (std::int32_t att{}; att < max_attempts; ++att)
        {
            constexpr auto max_nodes = 10;
            auto graph_edges = Standard::Algorithms::Graphs::create_random_graph<std::int32_t, max_nodes>();

            tests.emplace_back("Random" + std::to_string(att), std::move(graph_edges.first),
                std::vector<std::pair<std::int32_t, std::int32_t>>(), std::vector<std::int32_t>(), false);
        }
    }

    constexpr void run_test_case(const test_case &test)
    {
        auto actual = Standard::Algorithms::Graphs::bridge_detector(test.graph());
        auto &bridges = actual.first;
        sort_pairs(bridges);

        auto cut_nodes = actual.second;
        std::sort(cut_nodes.begin(), cut_nodes.end());

        if (test.is_computed())
        {
            ::Standard::Algorithms::ert::are_equal(test.bridges(), bridges, "bridge_detector");
            ::Standard::Algorithms::ert::are_equal(test.cut_nodes(), cut_nodes, "cut_node_detector");
        }

        {
            auto bridge_slow = Standard::Algorithms::Graphs::bridge_detector_slow(test.graph());
            sort_pairs(bridge_slow);
            ::Standard::Algorithms::ert::are_equal(bridges, bridge_slow, "bridge_detector_slow");
        }

        auto cut_nodes_slow = Standard::Algorithms::Graphs::cut_node_detector_slow(test.graph());
        std::sort(cut_nodes_slow.begin(), cut_nodes_slow.end());

        ::Standard::Algorithms::ert::are_equal(cut_nodes, cut_nodes_slow, "cut_node_detector_slow");
    }
} // namespace

void Standard::Algorithms::Graphs::Tests::bridge_detector_tests()
{
    Standard::Algorithms::Tests::test_utilities<test_case>::run_tests(run_test_case, generate_test_cases);
}
