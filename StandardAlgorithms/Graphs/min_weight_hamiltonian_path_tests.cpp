#include"min_weight_hamiltonian_path_tests.h"
#include"../Numbers/hash_utilities.h"
#include"../Numbers/xor_1.h"
#include"../Utilities/test_utilities.h"
#include"graph_helper.h"
#include"hamiltonian_path_count.h"
#include"hamiltonian_path_find.h"
#include"min_weight_hamiltonian_cycle.h"
#include"min_weight_hamiltonian_cycle_bb.h"
#include"min_weight_hamiltonian_path.h"
#include"simple_cycle_count.h"
#include"simple_path_count.h"
#include"weighted_vertex.h"
#include<unordered_map>
#include<unordered_set>

// todo(p3): split into several tests.

namespace
{
    using int_t = std::uint32_t;
    using weight_t = std::uint64_t;
    using edge_t = Standard::Algorithms::weighted_vertex<std::int32_t, weight_t>;
    using neighbors_t = std::vector<edge_t>;
    using digraph_t = std::vector<neighbors_t>;

    // is map often slower?
    // using map_t = map<pair<int_t, std::int32_t>, weight_t>;
    // {mask, last node} = weight

    using map_64 = std::unordered_map<std::pair<int_t, std::int32_t>, weight_t, Standard::Algorithms::Numbers::pair_hash>;
    using map_32 = std::unordered_map<std::pair<int_t, std::int32_t>, int_t, Standard::Algorithms::Numbers::pair_hash>;

    // A path consists of unique nodes.
    using vector_map_t = std::unordered_set<std::vector<std::int32_t>, Standard::Algorithms::Numbers::vector_hash>;

    constexpr weight_t infinity_w = std::numeric_limits<weight_t>::max() / 7;

    constexpr auto max_shift = std::numeric_limits<std::uint32_t>::max();

    constexpr void add_edge(
        const std::int32_t source, digraph_t &digraph, const std::int32_t dest, const weight_t weight)
    {
        const auto node_count = static_cast<std::int32_t>(digraph.size());

        assert(!(source < 0) && source != dest && source < node_count && !(dest < 0) && dest < node_count);

        digraph[source].emplace_back(dest, weight);
    }

    struct test_case final : Standard::Algorithms::Tests::base_test_case
    {
        constexpr test_case(std::string &&name, const weight_t weight, digraph_t &&digraph,
            std::pair<std::vector<std::int32_t> &&, int_t> paths, const int_t hamilt_count,
            std::pair<std::vector<std::int32_t> &&, int_t> cycles, const weight_t cycle_weight)
            : base_test_case(std::move(name))
            , Digraph(std::move(digraph))
            , Path_expected(std::move(paths.first))
            , Weight_expected(weight)
            , Hamilt_count_expected(hamilt_count)
            , Paths_expected(paths.second)
            , Cycles_expected(cycles.second)
            , Cycle_expected(std::move(cycles.first))
            , Cycle_weight_expected(cycle_weight)
        {
            for (auto &edges : Digraph)
            {// For good heuristic, lower weights must come first.
                std::sort(edges.begin(), edges.end());
            }

            Standard::Algorithms::Graphs::require_simple_graph<edge_t>(Digraph);
            ::Standard::Algorithms::ert::are_equal(Path_expected.size(), Digraph.size(), "Must be a path.");
            ::Standard::Algorithms::ert::greater(Hamilt_count_expected, 0U, "Hamilt_count_expected");
            ::Standard::Algorithms::ert::greater_or_equal(Paths_expected, Hamilt_count_expected, "Paths expected");

            const auto cys = Cycle_expected.size();
            ::Standard::Algorithms::ert::greater_or_equal(Cycles_expected, 0U < cys ? 1U : 0U, "Cycles expected");
        }

        [[nodiscard]] constexpr auto digraph() const &noexcept -> const digraph_t &
        {
            return Digraph;
        }

        [[nodiscard]] constexpr auto path_expected() const &noexcept -> const std::vector<std::int32_t> &
        {
            return Path_expected;
        }

        [[nodiscard]] constexpr auto weight_expected() const &noexcept -> const weight_t &
        {
            return Weight_expected;
        }

        [[nodiscard]] constexpr auto hamilt_count_expected() const &noexcept -> const int_t &
        {
            return Hamilt_count_expected;
        }

        [[nodiscard]] constexpr auto paths_expected() const &noexcept -> const int_t &
        {
            return Paths_expected;
        }

        [[nodiscard]] constexpr auto cycles_expected() const &noexcept -> const int_t &
        {
            return Cycles_expected;
        }

        [[nodiscard]] constexpr auto cycle_expected() const &noexcept -> const std::vector<std::int32_t> &
        {
            return Cycle_expected;
        }

        [[nodiscard]] constexpr auto cycle_weight_expected() const &noexcept -> const weight_t &
        {
            return Cycle_weight_expected;
        }

        void print(std::ostream &str) const override
        {
            ::Standard::Algorithms::print(", digraph ", Digraph, str);
            ::Standard::Algorithms::print(", path expected ", Path_expected, str);
            str << "weight expected " << Weight_expected << ", ham count expected " << Hamilt_count_expected
                << ", paths expected " << Paths_expected << ", cycles expected " << Cycles_expected;

            ::Standard::Algorithms::print(", cycle expected ", Cycle_expected, str);
            str << "cycle weight expected " << Cycle_weight_expected;
        }

private:
        digraph_t Digraph;

        std::vector<std::int32_t> Path_expected;
        weight_t Weight_expected;

        int_t Hamilt_count_expected;
        int_t Paths_expected;
        int_t Cycles_expected;

        std::vector<std::int32_t> Cycle_expected;
        weight_t Cycle_weight_expected;
    };

    constexpr void edge1(
        const std::int32_t first_node, std::vector<test_case> &tests, std::string &&name, const weight_t &weight)
    {
        constexpr auto node_count = 2;
        digraph_t digraph(node_count);

        const auto from = first_node;
        const auto tod = ::Standard::Algorithms::Numbers::xor_1(first_node);
        add_edge(from, digraph, tod, weight << 2U);
        add_edge(tod, digraph, from, weight);

        const auto is_weight = weight != weight_t{};

        std::vector<std::int32_t> path = { is_weight ? tod : from, is_weight ? from : tod };
        auto path_cop = path;
        assert(path[0] != path[1]);

        constexpr auto hamilt = 2;
        constexpr auto paths = 2;
        constexpr auto cycles = 1;
        constexpr auto cycle_weight_ratio = 5;

        tests.emplace_back(std::move(name), weight, std::move(digraph), std::make_pair(std::move(path), paths), hamilt,
            std::make_pair(std::move(path_cop), cycles), weight * cycle_weight_ratio);
    }

    constexpr void generate_test_cases(std::vector<test_case> &tests)
    {
        {
            constexpr auto node_count = 3;
            digraph_t digraph(node_count);
            // a - 1 - b - 1 - c
            // c - 98 > a ; that is b->c is the only incoming to c edge.
            // abca is the only Hamiltonian cycle of cost 100.
            // However, abcba cost is 4, but it is not a ham cycle as 2 nodes repeat: a, b.
            constexpr weight_t low = 1;
            constexpr weight_t high = 98;

            add_edge(0, digraph, 1, low);
            add_edge(1, digraph, 0, low);
            add_edge(1, digraph, 2, low);
            add_edge(2, digraph, 1, low);
            add_edge(2, digraph, 0, high);

            std::vector<std::int32_t> path = { 0, 1, 2 };
            auto path2 = path;

            constexpr auto hamilt = 4;
            constexpr auto pp2 = 5;
            constexpr auto paths = pp2 + hamilt;
            constexpr auto cycles = 3;

            tests.emplace_back("TSP 3", 2, std::move(digraph), std::make_pair(std::move(path), paths), hamilt,
                std::make_pair(std::move(path2), cycles), low * 2 + high);
        }
        {
            constexpr auto node_count = 3;
            digraph_t digraph(node_count);

            // 2--1->0
            constexpr weight_t weight = 10;

            add_edge(2, digraph, 1, weight);
            add_edge(1, digraph, 2, weight);
            add_edge(1, digraph, 0, weight);

            std::vector<std::int32_t> path = { 2, 1, 0 };
            std::vector<std::int32_t> empty;

            constexpr auto hamilt = 1;
            constexpr auto pp2 = 3;
            constexpr auto paths = pp2 + hamilt;
            constexpr auto cycles = 1;

            tests.emplace_back("2-1->0", weight * 2, std::move(digraph), std::make_pair(std::move(path), paths), hamilt,
                std::make_pair(std::move(empty), cycles), infinity_w);
        }
        {
            edge1(0, tests, "0-1_0w", 0);

            constexpr weight_t weight = 19;
            edge1(0, tests, "0-1", weight);
            edge1(1, tests, "1-0", weight);
        }
        {
            constexpr auto node_count = 4;
            digraph_t digraph(node_count);

            // NOLINTNEXTLINE
            add_edge(0, digraph, 1, 5);
            // NOLINTNEXTLINE
            add_edge(1, digraph, 0, 10);

            // NOLINTNEXTLINE
            add_edge(1, digraph, 2, 15);
            // NOLINTNEXTLINE
            add_edge(2, digraph, 1, 20);

            // NOLINTNEXTLINE
            add_edge(2, digraph, 3, 25);
            // NOLINTNEXTLINE
            add_edge(3, digraph, 2, 30);
            {
                constexpr auto large_w = infinity_w / 5;
                add_edge(2, digraph, 0, large_w);
            }
            //     ---
            //   /    ^
            //  v      \;
            // 0 -- 1 -- 2 -- 3
            // 0123
            // 3201
            // 3210
            constexpr auto hamilt = 3;
            constexpr auto pp2 = 7;
            constexpr auto pp3 = 7;
            constexpr auto paths = pp2 + pp3 + hamilt;
            constexpr auto cycles = 4;

            tests.emplace_back("4 nodes",
                // NOLINTNEXTLINE
                45, std::move(digraph), std::make_pair(std::vector<std::int32_t>{ 0, 1, 2, 3 }, paths), hamilt,
                std::make_pair(std::vector<std::int32_t>(), cycles), infinity_w);
        }
        {
            constexpr auto node_count = 4;
            digraph_t digraph(node_count);
            //       3
            //     / | \;
            //   ^   |   ^
            //  /    |    \;
            // 1-----0-----2
            // 1023
            // 1302
            // 2013
            // 2301
            constexpr weight_t weight = 10;

            {
                auto shift = max_shift;

                for (auto index = 1; index < node_count; ++index)
                {
                    add_edge(0, digraph, index, weight << ++shift);
                    add_edge(index, digraph, 0, weight << ++shift);
                }
            }

            add_edge(1, digraph, 3, weight);
            add_edge(2, digraph, 3, weight);

            constexpr auto hamilt = 4;
            constexpr auto pp2 = 8;
            constexpr auto pp3 = 2 + 3 + 3 + 2;
            constexpr auto paths = pp2 + pp3 + hamilt;
            constexpr auto cycles = 5;

            tests.emplace_back("4-star",
                weight *
                    // NOLINTNEXTLINE
                    7,
                std::move(digraph), std::make_pair(std::vector<std::int32_t>{ 1, 0, 2, 3 }, paths), hamilt,
                std::make_pair(std::vector<std::int32_t>(), cycles), infinity_w);
        }
        {
            constexpr auto node_count = 4;
            digraph_t digraph(node_count);
            // Add the edge 3->1
            //
            //       3
            //     / | \;
            //   /   |   ^
            //  /    |    \;
            // 1-----0-----2
            // 1023
            // 1302
            // 2013
            // 2301
            // extra:
            // 0231
            // 2031
            // 2310
            // 3102
            const weight_t weight = 10;
            auto shift = max_shift;

            for (auto index = 1; index < node_count; ++index)
            {
                add_edge(0, digraph, index, weight << ++shift);
                add_edge(index, digraph, 0, weight << ++shift);
            }

            add_edge(1, digraph, 3, weight);
            add_edge(2, digraph, 3, weight);
            add_edge(3, digraph, 1, weight << ++shift);

            constexpr auto hamilt = 8;
            constexpr auto pp2 = 9;
            constexpr auto pp3 = 3 + 3 + 4 + 3;
            constexpr auto paths = pp2 + pp3 + hamilt;
            constexpr auto cycles = 8;

            tests.emplace_back("4-star31",
                weight *
                    // NOLINTNEXTLINE
                    7,
                std::move(digraph), std::make_pair(std::vector<std::int32_t>{ 1, 0, 2, 3 }, paths), hamilt,
                std::make_pair(std::vector<std::int32_t>{ 0, 2, 3, 1 }, cycles),
                // NOLINTNEXTLINE
                weight * 71);
        }
    }

    constexpr void hack_zero_weight_path(const std::vector<std::int32_t> &expected, std::vector<std::int32_t> &path)
    {
        if (1U < expected.size() && expected.size() == path.size() && expected[0] != path[0])
        {
            std::reverse(path.begin(), path.end());
        }
    }

    void run_test_case_hamilt_find(const test_case &test)
    {
        {
            const auto path = Standard::Algorithms::Graphs::hamiltonian_path_find_slow<digraph_t>(test.digraph());

            const auto ish = test.digraph().size() == path.size() &&
                Standard::Algorithms::Graphs::Inner::is_path_slow<digraph_t>(test.digraph(), path);

            ::Standard::Algorithms::ert::are_equal(true, ish, "hamiltonian_path_find_slow");
        }
        {
            std::unordered_map<int_t, int_t> map1;

            const auto path =
                Standard::Algorithms::Graphs::hamiltonian_path_find_draft<digraph_t, int_t>(test.digraph(), map1);

            const auto ish = test.digraph().size() == path.size() &&
                Standard::Algorithms::Graphs::Inner::is_path_slow<digraph_t>(test.digraph(), path);

            ::Standard::Algorithms::ert::are_equal(true, ish, "hamiltonian_path_find_draft");
        }
        {
            std::vector<std::unordered_map<int_t, int_t>> maps;

            const auto path = Standard::Algorithms::Graphs::hamiltonian_path_find<digraph_t, int_t>(test.digraph(), maps);

            const auto ish = test.digraph().size() == path.size() &&
                Standard::Algorithms::Graphs::Inner::is_path_slow<digraph_t>(test.digraph(), path);

            ::Standard::Algorithms::ert::are_equal(true, ish, "hamiltonian_path_find");
        }
    }

    void run_test_case_min_weight_path(const test_case &test)
    {
        std::vector<std::int32_t> path;
        {
            std::vector<std::int32_t> temp;

            const auto weight =
                Standard::Algorithms::Graphs::min_weight_hamiltonian_path_slow<digraph_t, weight_t, infinity_w>(
                    path, test.digraph(), temp);

            ::Standard::Algorithms::ert::are_equal(test.weight_expected(), weight, "weight_slow");
            ::Standard::Algorithms::ert::are_equal(test.path_expected(), path, "path_slow");
        }
        {
            path.clear();

            map_64 tam;

            const auto weight = Standard::Algorithms::Graphs::min_weight_hamiltonian_path_draft<digraph_t, weight_t,
                int_t, map_64, infinity_w>(test.digraph(), path, tam);

            ::Standard::Algorithms::ert::are_equal(test.weight_expected(), weight, "weight_draft");

            hack_zero_weight_path(test.path_expected(), path);
            ::Standard::Algorithms::ert::are_equal(test.path_expected(), path, "path_draft");
        }
        {
            std::vector<map_64> maps;
            for (const auto &shall_restore_path : { false, true })
            {
                path.clear();
                maps.clear();

                const auto weight = Standard::Algorithms::Graphs::min_weight_hamiltonian_path<digraph_t, weight_t, int_t,
                    map_64, infinity_w>(test.digraph(), path, maps, shall_restore_path);

                const auto *const suf = shall_restore_path ? "1" : "0";
                ::Standard::Algorithms::ert::are_equal(test.weight_expected(), weight, std::string("weight_") + suf);

                if (shall_restore_path)
                {
                    hack_zero_weight_path(test.path_expected(), path);
                    ::Standard::Algorithms::ert::are_equal(test.path_expected(), path, std::string("path_") + suf);
                }
            }
        }
    }

    constexpr void run_test_case_min_weight_cycle(const test_case &test)
    {
        std::vector<std::int32_t> path;
        {
            const auto weight =
                Standard::Algorithms::Graphs::min_weight_hamiltonian_cycle_slow<digraph_t, weight_t, infinity_w>(
                    test.digraph(), path);

            ::Standard::Algorithms::ert::are_equal(test.cycle_weight_expected(), weight, "weight_cycle_slow");

            hack_zero_weight_path(test.cycle_expected(), path);
            ::Standard::Algorithms::ert::are_equal(test.cycle_expected(), path, "cycle_slow");
        }
        {
            path.clear();

            const auto weight =
                Standard::Algorithms::Graphs::min_weight_hamiltonian_cycle_bb<digraph_t, weight_t, weight_t, infinity_w>(
                    test.digraph(), path);

            ::Standard::Algorithms::ert::are_equal(
                test.cycle_weight_expected(), weight, "min_weight_hamiltonian_cycle_bb");

            hack_zero_weight_path(test.cycle_expected(), path);
            ::Standard::Algorithms::ert::are_equal(test.cycle_expected(), path, "min_weight_hamiltonian_cycle_bb");
        }
        {
            std::vector<map_64> maps;

            for (const auto &shall_restore_path : { false, true })
            {
                path.clear();
                maps.clear();

                const auto weight = Standard::Algorithms::Graphs::min_weight_hamiltonian_cycle<digraph_t, weight_t, int_t,
                    map_64, infinity_w>(test.digraph(), path, maps, shall_restore_path);

                const auto *const suf = shall_restore_path ? "1" : "0";
                ::Standard::Algorithms::ert::are_equal(
                    test.cycle_weight_expected(), weight, std::string("weight_cycle_") + suf);

                if (shall_restore_path)
                {
                    hack_zero_weight_path(test.cycle_expected(), path);
                    ::Standard::Algorithms::ert::are_equal(test.cycle_expected(), path, std::string("cycle_") + suf);
                }
            }
        }
    }

    void run_test_case_hamilt(const test_case &test)
    {
        {
            std::vector<std::int32_t> temp;

            const auto count =
                Standard::Algorithms::Graphs::hamiltonian_path_count_slow<digraph_t, int_t>(test.digraph(), temp);
            ::Standard::Algorithms::ert::are_equal(test.hamilt_count_expected(), count, "hamilt_count_slow");
        }
        {
            map_32 prev{};
            map_32 cur{};

            const auto count =
                Standard::Algorithms::Graphs::hamiltonian_path_count<digraph_t, int_t, map_32>(test.digraph(), prev, cur);
            ::Standard::Algorithms::ert::are_equal(test.hamilt_count_expected(), count, "hamilt_count");
        }
    }

    void run_test_case_paths(const test_case &test)
    {
        {
            vector_map_t map1;
            std::vector<std::int32_t> temp;
            std::vector<std::int32_t> temp2;

            const auto count = Standard::Algorithms::Graphs::simple_path_count_slow<digraph_t, vector_map_t, int_t>(
                test.digraph(), temp, map1, temp2);
            ::Standard::Algorithms::ert::are_equal(test.paths_expected(), count, "paths_expected_slow");
        }
        {
            map_32 prev{};
            map_32 cur{};

            const auto count =
                Standard::Algorithms::Graphs::simple_path_count<digraph_t, int_t, map_32>(prev, test.digraph(), cur);

            ::Standard::Algorithms::ert::are_equal(test.paths_expected(), count, "paths_expected");
        }
    }

    void run_test_case_cycles(const test_case &test)
    {
        {
            vector_map_t map1;
            std::vector<std::int32_t> temp;
            std::vector<std::int32_t> temp2;

            const auto count = Standard::Algorithms::Graphs::simple_cycle_count_slow<digraph_t, vector_map_t, int_t>(
                test.digraph(), temp, map1, temp2);
            ::Standard::Algorithms::ert::are_equal(test.cycles_expected(), count, "simple_cycle_count_slow");
        }
        {
            const auto size = test.digraph().size();
            map_32 prev{};
            map_32 cur{};

            std::vector<std::vector<bool>> adj_matr(size, std::vector<bool>(size, false));

            Standard::Algorithms::Graphs::graph_list_to_adjacency_matrix(test.digraph(), adj_matr);

            const auto count = Standard::Algorithms::Graphs::simple_cycle_count<digraph_t, int_t, map_32>(
                test.digraph(), prev, adj_matr, cur);

            ::Standard::Algorithms::ert::are_equal(test.cycles_expected(), count, "simple_cycle_count");
        }
    }

    void run_test_case(const test_case &test)
    {
        run_test_case_min_weight_path(test);
        run_test_case_hamilt(test);
        run_test_case_paths(test);
        run_test_case_hamilt_find(test);
        run_test_case_min_weight_cycle(test);
        run_test_case_cycles(test);
    }
} // namespace

void Standard::Algorithms::Graphs::Tests::min_weight_hamiltonian_path_tests()
{
    // todo(p2): Is it hanging?
    Standard::Algorithms::Tests::test_utilities<test_case>::run_tests<all_cores>(run_test_case, generate_test_cases);
}
