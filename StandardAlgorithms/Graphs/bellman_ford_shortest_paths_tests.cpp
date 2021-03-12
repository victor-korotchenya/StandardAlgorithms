#include "DijkstraAlgorithmTests.h"
#include "bellman_ford_shortest_paths.h"
#include "random_graph_utilities.h"
#include "WeightedVertex.h"
#include "../elapsed_time_ns.h"
#include "../Utilities/PrintUtilities.h"
#include "../test_utilities.h"

//#define Print_bellman_ford_test 1

#ifdef Print_bellman_ford_test
#include <iostream>
constexpr auto million = 1000000.0;
#endif

using namespace std;
using namespace Privet::Algorithms::Tests;
using namespace Privet::Algorithms::Graphs;
using namespace Privet::Algorithms;

namespace
{
    using weight_t = int64_t;
    using int_t = int;
    using edge_t = WeightedVertex<int_t, weight_t >;
    using graph_t = vector<vector<edge_t>>;

    constexpr int_t max_nodes = //1000 *
        10, limit_edges = //100 * 1000 *
        10;
    constexpr weight_t min_weight = -5, max_weight = max_nodes * 2, inf = (max_weight + 10) * max_nodes;

    bool run_bellman_ford_test(const graph_t& graph,
        vector<weight_t>& distances, vector<int_t>& parents,
        vector<weight_t>& distances2, vector<int_t>& parents2,
        deque<int>& temp, deque<int>& temp2, std::vector<bool>& used)
    {
#ifdef Print_bellman_ford_test
        elapsed_time_ns t0;
#endif
        const auto actual = bellman_ford_shortest_paths_fast(graph, 0, distances, parents, temp, temp2, used, inf);

#ifdef Print_bellman_ford_test
        const auto elapsed0 = t0.elapsed();
        elapsed_time_ns t1;
#endif
        const auto actual1 = bellman_ford_shortest_paths_slow(graph, 0, distances2, parents2, inf);

#ifdef Print_bellman_ford_test
        const auto elapsed1 = t1.elapsed();
#endif

        Assert::AreEqual(actual, actual1, "bellman_ford_shortest_paths.");
        if (actual)
        {
            Assert::AreEqual(distances, distances2, "distances in bellman_ford_shortest_paths.");
            // parents might differ - skip comparing.
        }

#ifdef Print_bellman_ford_test
        const auto ratio = elapsed0 > 0 ? elapsed1 * 1.0 / elapsed0 : 0.0;

        const auto rep = " n " + to_string(graph.size()) +
            ", has " + to_string(actual) +
            ", t0 " + to_string(elapsed0 / million) +
            ",  t1 " + to_string(elapsed1 / million) +
            ",  t1/t0 " + to_string(ratio) +
            "\n";
        cout << rep;
#endif
        return actual;
    }

    template<class graph_t>
    bool run_bellman_ford_test_wrapper(const graph_t& graph,
        vector<weight_t>& distances, vector<int_t>& parents,
        vector<weight_t>& distances2, vector<int_t>& parents2,
        deque<int>& temp, deque<int>& temp2, std::vector<bool>& used)
    {
        try
        {
            const auto actual = run_bellman_ford_test(graph, distances, parents, distances2, parents2, temp, temp2, used);
            return actual;
        }
        catch (const exception& e)
        {
            stringstream ss;
            ss << e.what();
            ::Print(" Graph ", graph, ss);

            throw runtime_error(ss.str());
        }
    }

    void bellman_ford_functional(vector<weight_t>& distances, vector<int_t>& parents,
        vector<weight_t>& distances2, vector<int_t>& parents2,
        deque<int>& temp, deque<int>& temp2, std::vector<bool>& used)
    {
        // todo: p2. more tests.
        graph_t graph{
        {{1,-1},{2,-1}},
        {{0,-1},{2,-1}},
        {{1,-1},{0,-1}},
        };

        const auto actual = run_bellman_ford_test_wrapper(graph, distances, parents, distances2, parents2, temp, temp2, used);
        Assert::AreEqual(false, actual, "all -1");
    }

    void bellman_ford_speed(vector<weight_t>& distances, vector<int_t>& parents,
        vector<weight_t>& distances2, vector<int_t>& parents2,
        deque<int>& temp, deque<int>& temp2, std::vector<bool>& used)
    {
        for (auto att = 0; att < 1; ++att)
        {
#ifdef Print_bellman_ford_test
            auto rep = "  gen graph attempt " + to_string(att) + "\n";
            cout << rep;
            elapsed_time_ns tim;
#endif
            const auto graph_edges = create_random_graph_weighted<edge_t, weight_t, int_t, max_nodes, false, 1, limit_edges>(min_weight, max_weight);

#ifdef Print_bellman_ford_test
            const auto elapsed = tim.elapsed();
            rep.pop_back();
            rep += " v " + to_string(graph_edges.first.size()) +
                " e " + to_string(graph_edges.second) +
                " done in " + to_string(elapsed / million) + " ms.\n";
            cout << rep;
#endif
            run_bellman_ford_test_wrapper(graph_edges.first, distances, parents, distances2, parents2, temp, temp2, used);
        }
    }
}

namespace Privet::Algorithms::Graphs::Tests
{
    void bellman_ford_shortest_paths_tests()
    {
        vector<weight_t> distances, distances2;
        vector<int_t> parents, parents2;

        deque<int> temp, temp2;
        std::vector<bool> used;

        bellman_ford_functional(distances, parents, distances2, parents2, temp, temp2, used);

        bellman_ford_speed(distances, parents, distances2, parents2, temp, temp2, used);
    }
}