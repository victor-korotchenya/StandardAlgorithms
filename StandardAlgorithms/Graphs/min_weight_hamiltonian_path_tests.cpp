#include <unordered_map>
#include <unordered_set>
#include "WeightedVertex.h"
#include "graph_helper.h"
#include "hamiltonian_path_count.h"
#include "hamiltonian_path_find.h"
#include "min_weight_hamiltonian_cycle.h"
#include "min_weight_hamiltonian_path.h"
#include "min_weight_hamiltonian_path_tests.h"
#include "simple_cycle_count.h"
#include "simple_path_count.h"
#include "../Utilities/PrintUtilities.h"
#include "../Numbers/hash_utilities.h"
#include "../test_utilities.h"

using namespace std;
using namespace Privet::Algorithms::Graphs::Tests;
using namespace Privet::Algorithms::Graphs;
using namespace Privet::Algorithms::Tests;
using namespace Privet::Algorithms;

namespace
{
    using weight_t = uint64_t;
    using u_number_t = uint32_t;
    using edge_t = WeightedVertex<int, weight_t>;
    using neighbors_t = vector<edge_t>;
    using digraph_t = vector<neighbors_t>;

    // map must be slower?
    //using map_t = map<pair<u_number_t, int>, weight_t>;
    // {mask, last node} = weight
    using map_64 = unordered_map<pair<u_number_t, int>, weight_t, pair_hash>;
    using map_32 = unordered_map<pair<u_number_t, int>, u_number_t, pair_hash>;
    // path of nodes must be unique
    using vector_map_t = unordered_set<vector<int>, vector_hash>;

    constexpr auto infinity_w = numeric_limits<weight_t>::max() / 7;

    void add_edge(digraph_t& digraph,
        const int from, const int to, const weight_t weight)
    {
        const auto node_count = static_cast<int>(digraph.size());
        assert(from >= 0 && from < node_count&& to >= 0 && to < node_count);

        auto& vs = digraph[from];
        vs.emplace_back(to, weight);
    }

    struct test_case final : base_test_case
    {
        const digraph_t digraph;

        const vector<int> path_expected;
        const weight_t weight_expected;

        const u_number_t hamilt_count_expected;
        const u_number_t paths_expected;
        const u_number_t cycles_expected;

        const vector<int> cycle_expected;
        const weight_t cycle_weight_expected;

        test_case(string&& name,
            digraph_t&& digraph,
            vector<int>&& path,
            const weight_t weight,
            const u_number_t hamilt_count,
            const u_number_t paths,
            const u_number_t cycles,
            vector<int>&& cycle,
            const weight_t cycle_weight)
            : base_test_case(forward<string>(name)),
            digraph(forward<digraph_t>(digraph)),
            path_expected(forward<vector<int>>(path)), weight_expected(weight),
            hamilt_count_expected(hamilt_count),
            paths_expected(paths),
            cycles_expected(cycles),
            cycle_expected(forward<vector<int>>(cycle)), cycle_weight_expected(cycle_weight)
        {
            require_simple_graph<edge_t>(this->digraph);
            Assert::AreEqual(path_expected.size(), this->digraph.size(), "Must be a path.");
            Assert::GreaterOrEqual(weight, weight_t(0), "weight");
            Assert::Greater(hamilt_count, u_number_t(0), "hamilt_count");
            Assert::GreaterOrEqual(paths, hamilt_count, "paths");

            const auto cs = cycle_expected.size();
            Assert::GreaterOrEqual(cycles, u_number_t(cs > 0), "cycles");
            Assert::GreaterOrEqual(true, !cs || cs == this->digraph.size(), "cycle_expected");
        }

        void Print(ostream& str) const override
        {
            base_test_case::Print(str);
            ::Print(", digraph=", digraph, str);
            ::Print(", path_expected=", path_expected, str);
            str << "weight_expected=" << weight_expected
                << ", hamilt_count_expected=" << hamilt_count_expected
                << ", paths_expected=" << paths_expected
                << ", cycles_expected=" << cycles_expected;

            ::Print(", cycle_expected=", cycle_expected, str);
            str << "cycle_weight_expected=" << cycle_weight_expected;
        }
    };

    void edge1(vector<test_case>& tests,
        string name, const int first_node, const weight_t weight)
    {
        constexpr auto node_count = 2;
        digraph_t digraph(node_count);

        const auto from = 0 ^ first_node, to = 1 ^ first_node;
        add_edge(digraph, from, to, weight << 2);
        add_edge(digraph, to, from, weight);

        vector<int> path = { weight ? to : from, weight ? from : to, }, path_cop = path;
        assert(path[0] != path[1]);
        constexpr auto hamilt = 2, paths = 2, cycles = 1;

        tests.emplace_back(move(name), move(digraph), move(path), weight, hamilt, paths, cycles, move(path_cop), weight * 5);
    }

    void generate_test_cases(vector<test_case>& tests)
    {
        {
            constexpr auto node_count = 3;
            digraph_t digraph(node_count);
            // 2--1->0
            constexpr weight_t weight = 10;
            add_edge(digraph, 2, 1, weight);
            add_edge(digraph, 1, 2, weight);
            add_edge(digraph, 1, 0, weight);

            vector<int> path = { 2,1,0 }, empty;
            constexpr auto hamilt = 1, p2 = 3, paths = p2 + hamilt, cycles = 1;
            tests.emplace_back("2-1->0", move(digraph), move(path), weight * 2, hamilt, paths, cycles, move(empty), infinity_w);
        }
        {
            edge1(tests, "0-1_0w", 0, 0);

            constexpr weight_t weight = 19;
            edge1(tests, "0-1", 0, weight);
            edge1(tests, "1-0", 1, weight);
        }
        {
            constexpr auto node_count = 4;
            digraph_t digraph(node_count);
            add_edge(digraph, 0, 1, 5);
            add_edge(digraph, 1, 0, 10);

            add_edge(digraph, 1, 2, 15);
            add_edge(digraph, 2, 1, 20);

            add_edge(digraph, 2, 3, 25);
            add_edge(digraph, 3, 2, 30);
            add_edge(digraph, 2, 0, infinity_w);
            //     ---
            //   /    ^
            //  v      \
            // 0 -- 1 -- 2 -- 3
            // 0123
            // 3201
            // 3210
            constexpr auto hamilt = 3, p2 = 7, p3 = 7, paths = p2 + p3 + hamilt, cycles = 4;
            tests.emplace_back("4 nodes", move(digraph), vector<int> { 0, 1, 2, 3 }, 45, hamilt, paths, cycles, vector<int>(), infinity_w);
        }
        {
            constexpr auto node_count = 4;
            digraph_t digraph(node_count);
            //       3
            //     / | \
            //   ^   |   ^
            //  /    |    \
            // 1-----0-----2
            // 1023
            // 1302
            // 2013
            // 2301
            const weight_t weight = 10;
            for (auto i = 1, j = -1; i < node_count; ++i)
            {
                add_edge(digraph, 0, i, weight << ++j);
                add_edge(digraph, i, 0, weight << ++j);
            }

            add_edge(digraph, 1, 3, weight);
            add_edge(digraph, 2, 3, weight);

            constexpr auto hamilt = 4, p2 = 8, p3 = 2 + 3 + 3 + 2, paths = p2 + p3 + hamilt, cycles = 5;
            tests.emplace_back("4-star", move(digraph), vector<int> { 1, 0, 2, 3 }, weight * 7, hamilt, paths, cycles, vector<int>(), infinity_w);
        }
        {
            constexpr auto node_count = 4;
            digraph_t digraph(node_count);
            // Add the edge 3->1
            //
            //       3
            //     / | \
            //   /   |   ^
            //  /    |    \
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
            auto j = -1;
            for (auto i = 1; i < node_count; ++i)
            {
                add_edge(digraph, 0, i, weight << ++j);
                add_edge(digraph, i, 0, weight << ++j);
            }

            add_edge(digraph, 1, 3, weight);
            add_edge(digraph, 2, 3, weight);
            add_edge(digraph, 3, 1, weight << ++j);

            constexpr auto hamilt = 8, p2 = 9, p3 = 3 + 3 + 4 + 3, paths = p2 + p3 + hamilt, cycles = 8;
            tests.emplace_back("4-star31", move(digraph), vector<int>{ 1, 0, 2, 3 }, weight * 7, hamilt, paths, cycles, vector<int>{ 0, 2, 3, 1 }, weight * 71);
        }
    }

    void hack_zero_weight_path(const vector<int>& expected, vector<int>& path, const weight_t weight)
    {
        if (!weight && expected.size() == 2 && expected.size() == path.size() && expected[0] != path[0])
            swap(path[0], path[1]);
    }

    void run_test_case_hamilt_find(const test_case& test)
    {
        {
            const auto path = hamiltonian_path_find_slow<digraph_t>(test.digraph);
            const auto is = test.digraph.size() == path.size() &&
                is_path_slow<digraph_t>(test.digraph, path);
            Assert::AreEqual(true, is, "hamiltonian_path_find_slow");
        }
        {
            unordered_map<u_number_t, u_number_t> m;
            const auto path = hamiltonian_path_find_draft<digraph_t, u_number_t>(test.digraph, m);
            const auto is = test.digraph.size() == path.size() &&
                is_path_slow<digraph_t>(test.digraph, path);
            Assert::AreEqual(true, is, "hamiltonian_path_find_draft");
        }
        {
            vector<unordered_map<u_number_t, u_number_t>> ms;
            const auto path = hamiltonian_path_find<digraph_t, u_number_t>(test.digraph, ms);
            const auto is = test.digraph.size() == path.size() &&
                is_path_slow<digraph_t>(test.digraph, path);
            Assert::AreEqual(true, is, "hamiltonian_path_find");
        }
    }

    void run_test_case_min_weight_path(const test_case& test)
    {
        vector<int> path, temp;
        {
            const auto weight = min_weight_hamiltonian_path_slow<digraph_t, weight_t, infinity_w>(test.digraph, path, temp);

            Assert::AreEqual(test.weight_expected, weight, "weight_slow");
            Assert::AreEqual(test.path_expected, path, "path_slow");
        }
        {
            path.clear();

            map_64 t;
            const auto weight = min_weight_hamiltonian_path_draft<digraph_t, weight_t, u_number_t, map_64, infinity_w>(
                test.digraph, path, t);

            Assert::AreEqual(test.weight_expected, weight, "weight_draft");
            hack_zero_weight_path(test.path_expected, path, weight);
            Assert::AreEqual(test.path_expected, path, "path_draft");
        }
        {
            vector<map_64> ms;
            for (const auto& shall_restore_path : { false, true })
            {
                path.clear();
                ms.clear();
                const auto weight = min_weight_hamiltonian_path<digraph_t, weight_t, u_number_t, map_64, infinity_w>(
                    test.digraph, path, ms, shall_restore_path);

                const auto suf = to_string(shall_restore_path);
                Assert::AreEqual(test.weight_expected, weight, "weight_" + suf);
                if (shall_restore_path)
                {
                    hack_zero_weight_path(test.path_expected, path, weight);
                    Assert::AreEqual(test.path_expected, path, "path_" + suf);
                }
            }
        }
    }

    void run_test_case_min_weight_cycle(const test_case& test)
    {
        vector<int> path, temp;
        {
            const auto weight = min_weight_hamiltonian_cycle_slow<digraph_t, weight_t, infinity_w>(test.digraph, path, temp);

            Assert::AreEqual(test.cycle_weight_expected, weight, "weight_cycle_slow");
            hack_zero_weight_path(test.cycle_expected, path, 0);
            Assert::AreEqual(test.cycle_expected, path, "cycle_slow");
        }
        {
            vector<map_64> ms;
            for (const auto& shall_restore_path : { false, true })
            {
                path.clear();
                ms.clear();
                const auto weight = min_weight_hamiltonian_cycle<digraph_t, weight_t, u_number_t, map_64, infinity_w>(
                    test.digraph, path, ms, shall_restore_path);

                const auto suf = to_string(shall_restore_path);
                Assert::AreEqual(test.cycle_weight_expected, weight, "weight_cycle_" + suf);
                if (shall_restore_path)
                {
                    hack_zero_weight_path(test.cycle_expected, path, 0);
                    Assert::AreEqual(test.cycle_expected, path, "cycle_" + suf);
                }
            }
        }
    }

    void run_test_case_hamilt(const test_case& test)
    {
        vector<int> temp;
        {
            const auto count = hamiltonian_path_count_slow<digraph_t, u_number_t>(test.digraph, temp);
            Assert::AreEqual(test.hamilt_count_expected, count, "hamilt_count_slow");
        }
        {
            map_32 prev, cur;
            const auto count = hamiltonian_path_count<digraph_t, u_number_t, map_32>(test.digraph, prev, cur);
            Assert::AreEqual(test.hamilt_count_expected, count, "hamilt_count");
        }
    }

    void run_test_case_paths(const test_case& test)
    {
        {
            vector_map_t m;
            vector<int> temp, temp2;
            const auto count = simple_path_count_slow<digraph_t, vector_map_t, u_number_t>(test.digraph, m, temp, temp2);
            Assert::AreEqual(test.paths_expected, count, "paths_expected_slow");
        }
        {
            map_32 prev, cur;
            const auto count = simple_path_count<digraph_t, u_number_t, map_32>(test.digraph, prev, cur);
            Assert::AreEqual(test.paths_expected, count, "paths_expected");
        }
    }

    void run_test_case_cycles(const test_case& test)
    {
        {
            vector_map_t m;
            vector<int> temp, temp2;
            const auto count = simple_cycle_count_slow<digraph_t, vector_map_t, u_number_t>(test.digraph, m, temp, temp2);
            Assert::AreEqual(test.cycles_expected, count, "simple_cycle_count_slow");
        }
        {
            map_32 prev, cur;
            const auto size = test.digraph.size();

            vector<vector<bool>> adj_matr((size), vector<bool>(size));
            graph_list_to_adjacency_matrix(test.digraph, adj_matr);

            const auto count = simple_cycle_count<digraph_t, u_number_t, map_32>(test.digraph, adj_matr, prev, cur);
            Assert::AreEqual(test.cycles_expected, count, "simple_cycle_count");
        }
    }

    void run_test_case(const test_case& test)
    {
        run_test_case_min_weight_path(test);
        run_test_case_hamilt(test);
        run_test_case_paths(test);
        run_test_case_hamilt_find(test);
        run_test_case_min_weight_cycle(test);
        run_test_case_cycles(test);
    }
}

namespace Privet
{
    namespace Algorithms
    {
        namespace Graphs
        {
            namespace Tests
            {
                void min_weight_hamiltonian_path_tests()
                {
                    test_utilities<test_case>::run_tests<0>(run_test_case, generate_test_cases);
                }
            }
        }
    }
}