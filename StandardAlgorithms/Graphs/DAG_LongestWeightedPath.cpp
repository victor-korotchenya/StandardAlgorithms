#include <algorithm>
#include <cassert>
#include <climits>

#include "DAG_LongestWeightedPath.h"
#include "TopologicalSort.h"

using namespace std;
using namespace Privet::Algorithms::Graphs;
using namespace Privet::Algorithms;

namespace
{
    constexpr auto infinity = numeric_limits<int64_t>::min() / 4;

    // Set the cost backwards.
    // Only for non-negative weights!
    // Faster than "topological sort" + visits

    struct dag_context final
    {
        const dag_t* const dag;
        vector<int64_t> costs;
        vector<bool> saw;

        dag_context(const dag_t* const pdag, const int dag_size, const int source_vertex)
            : dag(pdag), costs(dag_size, infinity), saw(dag_size)
        {
            for (auto i = 0 * dag_size; i < dag_size; ++i)
            {
                if (0 == (*dag)[i].size())
                {
                    costs[i] = 0;
                }
            }

            costs[source_vertex] = infinity;
        }

        void visit(const int parent)
        {
            saw[parent] = true;
            auto& best = costs[parent];

            for (const auto& e : (*dag)[parent])
            {
                const auto& to = e.Vertex;
                if (!saw[to])
                {
                    visit(to);
                }

                const auto cand = costs[to] + e.Weight;
                best = max(best, cand);
            }
        }
    };
}

int64_t Graphs::dag_longest_weighted_path(const dag_t& dag, const int n)
{
    const auto ordered = topological_sort(dag, n);

    vector<int64_t> costs(n, infinity);
    vector<bool> has_in(n);
    for (auto i = 0; i < n; ++i)
    {
        for (const auto& e : dag[i])
            has_in[e.Vertex] = true;
    }

    for (auto i = 0; i < n; ++i)
    {
        if (!has_in[i] && dag[i].size())
            costs[i] = 0;
    }

    for (const auto& order : ordered)
    {
        const auto current_cost = std::max<std::int64_t>(0, costs[order]);
        for (const auto& e : dag[order])
        {
            const auto cand = current_cost + e.Weight;
            costs[e.Vertex] = max(costs[e.Vertex], cand);
        }
    }

    auto result = infinity;
    for (auto i = 0; i < n; ++i)
    {
        if (has_in[i])
            result = max(result, costs[i]);
    }

    return result;
}

int64_t Privet::Algorithms::Graphs::dag_longest_weighted_path_slow(
    const dag_t& dag, const int n,
    const int source_vertex, const int target_vertex)
{
    if (source_vertex == target_vertex)
        return 0;
    if (dag[source_vertex].empty())
        return infinity;

    const auto ordered = topological_sort(dag, n);
    vector<int64_t> costs(n, infinity);
    costs[source_vertex] = 0;

    auto result = infinity;
    for (const auto& order : ordered)
    {
        if (target_vertex == order)
            return costs[target_vertex];

        const auto current_cost = costs[order];
        if (infinity == current_cost)
            continue;

        for (const auto& e : dag[order])
        {
            const auto cand = current_cost + e.Weight;
            result = max(result, cand);
            costs[e.Vertex] = max(costs[e.Vertex], cand);
        }
    }

    return result;
}

int64_t Privet::Algorithms::Graphs::dag_longest_weighted_path(
    const dag_t& dag, const int n,
    const int source_vertex, const int target_vertex)
{
    if (source_vertex == target_vertex)
        return 0;
    if (dag[source_vertex].empty())
        return infinity;

    auto isbad = 0 <= target_vertex;
    for (auto i = 0; i < n && !isbad; ++i)
    {
        for (const auto& e : dag[i])
        {
            if (e.Weight < 0)
            {
                isbad = true;
                break;
            }
        }
    }

    if (isbad)
    {
        const auto result1 = dag_longest_weighted_path_slow(dag, n, source_vertex, target_vertex);
        return result1;
    }

    dag_context context(&dag, n, source_vertex);
    context.visit(source_vertex);
    const auto& result = context.costs[source_vertex];
    return result;
}