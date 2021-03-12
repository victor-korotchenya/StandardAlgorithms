#include "min_cost_perfect_matching_tests.h"
#include "min_cost_perfect_matching.h"
#include "../Utilities/Random.h"
#include "../test_utilities.h"
#include "../Utilities/PrintUtilities.h"
#include "../elapsed_time_ns.h"

//#define PrintMcMm 1

#if PrintMcMm
#include <iostream>
#endif

using namespace std;
using namespace Privet::Algorithms::Graphs::Tests;
using namespace Privet::Algorithms::Graphs;
using namespace Privet::Algorithms::Tests;
using namespace Privet::Algorithms;

namespace
{
    using weight_t = short;
    using weight_t2 = int64_t;
    const weight_t2 not_computed = -1;

    using graph_t = vector<vector<weight_t>>;
    using matching_t = vector<size_t>;

    struct test_case final : base_test_case
    {
        graph_t graph;
        matching_t matching;
        weight_t2 expected;

        test_case(string&& name,
            graph_t&& graph,
            matching_t&& matching,
            weight_t2 expected)
            : base_test_case(forward<string>(name)),
            graph(forward<graph_t>(graph)),
            matching(forward<matching_t>(matching)),
            expected(expected)
        {
        }

        void Validate() const override
        {
            base_test_case::Validate();

            const auto size = graph.size();
            RequirePositive(size, "size");
            Assert::AreEqual(0u, size & 1u, "even size");

            require_full_graph(graph, true);
            for (size_t from = 0; from < size; ++from)
            {
                const auto prefix = to_string(from) + ", ";
                Assert::AreEqual(weight_t(), graph[from][from], prefix + prefix);

                for (size_t to = 0; to < size; ++to)
                    RequireNonNegative(graph[from][to], prefix + to_string(from));
            }

            Assert::GreaterOrEqual(expected, not_computed, "expected");
            if (expected < 0)
                return;

            Assert::AreEqual(size, matching.size(), "matching.size");
            require_unique(matching, "matching");

            const auto mm = minmax_element(matching.begin(), matching.end());
            Assert::AreEqual(0u, *mm.first, "matching.min");
            Assert::AreEqual(size - 1u, *mm.second, "matching.max");

            for (size_t from = 0; from < size; ++from)
                Assert::NotEqual(from, matching[from], "matching at " + to_string(from));
        }

        void Print(ostream& str) const override
        {
            base_test_case::Print(str);
            ::Print("graph", graph, str);
            ::Print("matching", matching, str);
            PrintValue(str, "expected", expected);
        }
    };

    template<class random_t>
    void random_full_symmetric_graph(graph_t& graph, random_t& r, const size_t size, const weight_t min_value = 0, const weight_t max_value = 20)
    {
        graph.clear();
        if (!size)
            return;

        graph.resize(size);

        for (size_t from = 0; from < size; ++from)
        {
            auto& es = graph[from];
            es.resize(size);
        }

        for (size_t from = 0; from < size - 1; ++from)
        {
            auto& g1 = graph[from];
            for (auto to = from + 1u; to < size; ++to)
            {
                g1[to] = graph[to][from] = r(min_value, max_value);
            }
        }
    }

    void generate_test_cases(vector<test_case>& test_cases)
    {
        test_cases.emplace_back("base2", graph_t{ {0, 20}, {20, 0}, }, matching_t{ 1, 0 }, 20);
        test_cases.emplace_back("base4",
            graph_t{
                {0, 20, 30, 40,},
                {20, 0, 70, 80,},
                {30, 70, 0, 10,},
                {40, 80, 10, 0,},
            },
            matching_t{ 1, 0, 3, 2 }, 30);

        ShortRandom r;
        graph_t graph;
        matching_t matching;

        for (auto att = 0, att_max = 1; att < att_max; ++att)
        {
            const auto n = r(1,
#if _DEBUG
                5
#else
                10
#endif
            ) << 1;

            matching.resize(n);
            for (auto from = 0; from < n - 1; ++from)
                matching[from] = static_cast<weight_t>(from + 1);
            matching.back() = 0;

            random_full_symmetric_graph(graph, r, n);

            test_cases.emplace_back("random" + to_string(att), move(graph), move(matching), not_computed);
        }
    }

    void run_test_case(const test_case& test)
    {
        elapsed_time_ns t0;
        matching_t matching;
        const auto actual = min_cost_perfect_matching<weight_t2, weight_t>(test.graph, matching);
        const auto elapsed0 = t0.elapsed();

        if (test.expected >= 0)
            Assert::AreEqual(test.expected, actual, "min_cost_perfect_matching");

        elapsed_time_ns t1;
        const auto they = min_cost_perfect_matching_they<weight_t2, weight_t>(test.graph);
        const auto elapsed1 = t1.elapsed();

        Assert::AreEqual(actual, they, "min_cost_perfect_matching_they");

#if PrintMcMm
        const auto ratio = elapsed0 > 0 ? static_cast<long double>(elapsed1) / elapsed0 : 0.0;
        auto ps = to_string(test.graph.size());
        while (ps.size() < 2)
            ps = "0" + ps;

        cout << " n " << ps
            //<< ", me " << elapsed0
            //<< ", th " << elapsed1
            << ", th/me " << setprecision(20) << ratio
            << '\n';
#endif
        if (test.graph.size() >= 6)
            return;

        matching_t matching_slow;
        const auto slow = min_cost_perfect_matching_slow<weight_t2, weight_t>(test.graph, matching_slow);
        Assert::AreEqual(actual, slow, "min_cost_perfect_matching_slow");

        // todo: p2. compare the matchings.
    }
}

void Privet::Algorithms::Graphs::Tests::min_cost_perfect_matching_tests()
{
    test_utilities<test_case>::run_tests(run_test_case, generate_test_cases);
}