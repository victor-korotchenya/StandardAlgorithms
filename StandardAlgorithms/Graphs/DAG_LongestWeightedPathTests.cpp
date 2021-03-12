#include <algorithm>
#include <cassert>
#include <random>
#include "../test_utilities.h"

#include "DAG_LongestWeightedPath.h"
#include "DAG_LongestWeightedPathTests.h"

using namespace std;
using namespace Privet::Algorithms::Graphs::Tests;
using namespace Privet::Algorithms::Graphs;
using namespace Privet::Algorithms::Tests;
using namespace Privet::Algorithms;

namespace
{
    void AddWeightedEdge(
        dag_t& dag,
        const int sourceVertex,
        const int targetVertex,
        const int weight)
    {
#ifdef _DEBUG
        if (sourceVertex == targetVertex)
        {
            ostringstream ss;
            ss << "The source vertex (" << (sourceVertex) << ") == target (" << targetVertex << ").";
            StreamUtilities::throw_exception(ss);
        }
#endif
        dag[sourceVertex].push_back(WeightedVertex<int, int>(targetVertex, weight));
    }
}

int rnd(random_device& rd)
{
    int result = rd();
#ifdef _DEBUG
    result = result % 100;
#endif

    return result;
}
void Privet::Algorithms::Graphs::Tests::DAG_LongestWeightedPathTests()
{
    constexpr auto vertex_count = 5;
    dag_t dag(vertex_count);

    random_device rd;
    const int path02 = rnd(rd), path21 = rnd(rd),
        path31 = rnd(rd), path32 = rnd(rd),
        path43 = rnd(rd);

    AddWeightedEdge(dag, 0, 2, path02);
    AddWeightedEdge(dag, 2, 1, path21);
    AddWeightedEdge(dag, 3, 1, path31);
    AddWeightedEdge(dag, 3, 2, path32);
    AddWeightedEdge(dag, 4, 3, path43);
    //0 -> 2 -> 1
    //     ^  /
    //4 -> 3

    constexpr auto sourceVertex = 3, targetVertex = 1;
    const auto expected = max<int64_t>(path31, static_cast<int64_t>(path32) + path21);
    const auto actual_slow = dag_longest_weighted_path_slow(dag, vertex_count, sourceVertex, targetVertex);
    Assert::AreEqual(expected, actual_slow, "Error in dag_longest_weighted_path_slow(s, t).");

    const auto actual = dag_longest_weighted_path(dag, vertex_count, sourceVertex, targetVertex);
    Assert::AreEqual(expected, actual, "Error in dag_longest_weighted_path(s, t).");

    const auto expected_from = max<int64_t>(expected, path32);
    const auto actual_from = dag_longest_weighted_path(dag, vertex_count, sourceVertex);
    Assert::AreEqual(expected_from, actual_from, "Error in dag_longest_weighted_path(s).");

    const auto actual_from_slow = dag_longest_weighted_path_slow(dag, vertex_count, sourceVertex);
    Assert::AreEqual(expected_from, actual_from_slow, "Error in dag_longest_weighted_path_slow(s).");

    const auto expected_whole = max<int64_t>({ path02, path21, path31, path32, path43,
        //2 segments
        static_cast<int64_t>(path02) + path21,
        static_cast<int64_t>(path32) + path21,
        static_cast<int64_t>(path43) + path31,
        static_cast<int64_t>(path43) + path32,
        //3 segments
        static_cast<int64_t>(path43) + path32 + path21,
        });

    const auto actual_whole = dag_longest_weighted_path(dag, vertex_count);
    Assert::AreEqual(expected_whole, actual_whole, "Error in dag_longest_weighted_path.");
}