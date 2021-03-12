#include <queue>
#include <unordered_set>
#include "closest_select_update.h"
#include "closest_select_update_tests.h"
#include "../Utilities/PrintUtilities.h"
#include "../test_utilities.h"

using namespace std;
using namespace Privet::Algorithms::Trees;
using namespace Privet::Algorithms::Tests;
using namespace Privet::Algorithms;

namespace
{
    using Number = int;

    struct test_case : base_test_case
    {
        vector<vector<Number>> tree;

        // node, is update
        vector<pair<int, bool>> operations;

        // distance, node
        vector<pair<int, int>> select_answers;

        test_case(string&& name,
            vector<vector<Number>>&& tree,
            vector<pair<int, bool>>&& operations,
            vector<pair<int, int>>&& select_answers)
            : base_test_case(forward<string>(name)),
            tree(forward<vector<vector<Number>>>(tree)),
            operations(forward<vector<pair<int, bool>>>(operations)),
            select_answers(forward<vector<pair<int, int>>>(select_answers))
        {
        }

        void Validate() const override
        {
            base_test_case::Validate();

            const auto size = static_cast<int>(tree.size());
            RequirePositive(size, "tree size");
            Assert::AreEqual(true, is_tree(tree), "is tree");

            RequirePositive(operations.size(), "operations size");
            RequirePositive(select_answers.size(), "select_answers size");

            auto min_node = size, max_node = -1;
            auto selects = 0u;
            for (const auto& op : operations)
            {
                min_node = min(min_node, op.first);
                max_node = max(max_node, op.first);
                selects += !op.second;
            }

            Assert::GreaterOrEqual(min_node, 0, "min node");
            Assert::GreaterOrEqual(max_node, min_node, "max node");
            Assert::Greater(size, max_node, "max node");
            Assert::AreEqual(selects, select_answers.size(), "select_answers size2");
        }

        void Print(ostream& str) const override
        {
            base_test_case::Print(str);

            ::Print(", tree=", tree, str);
            ::Print(", operations=", operations, str);
            ::Print(", select_answers=", select_answers, str);
        }
    };

    void generate_test_case(vector<test_case>& tests, IntRandom& r, const int att)
    {
        const auto size = r(2, 20);
        auto tree = build_random_tree<Number>(size);

        const auto m = r(2, 25);
        vector<pair<int, bool>> operations(m);
        auto selects = 0;

        for (auto i = 0; i < m; ++i)
        {
            const auto is_update = !i || i + 1 != m && 0 == (r() & 1);
            auto& operation = operations[i];
            operation = { r(0, size - 1), is_update };
            selects += !operation.second;
        }

        tests.emplace_back("random" + to_string(att), move(tree), move(operations), vector<pair<int, int>>(selects, { -1,-1 }));
    }

    void generate_test_cases(vector<test_case>& tests)
    {
        IntRandom r;
        for (auto att = 0, max_att = 1; att < max_att; ++att)
            generate_test_case(tests, r, att);

        tests.push_back(
            {
              "One",
              {{}
              },
              {{0,true},{0,false},
              },
              {{0,0},}
            });
        tests.push_back(
            {
              "Edge",
              {
                {1},
                {0},
              },
               {{0,true},{0,false},{1,false},
                {1,true},{1,false},{0,false},
              },
              {{0,0},{1,0},{0,1},{0,0},}
            });
        tests.push_back(
            {
              "three_1",
              {
                {1},
                {0,2},
                {1},
              },// 0-1-2
               {{2,true},{0,false},{1,false},{2,false},
                {0,true},{0,false},{1,false},{2,false},
                {1,true},{0,false},{1,false},{2,false},
              },
              {{2,2},{1,2},{0,2},
              {0,0},{1,2},{0,2},
              {0,0},{0,1},{0,2},
              } });
        tests.push_back(
            {
              "three_0",
              {
                {1,2},
                {0},
                {0},
              },// 1-0-2
              {{2,true},{0,false},{1,false},{2,false},
                {0,true},{0,false},{1,false},{2,false},
                {1,true},{0,false},{1,false},{2,false},
              },
              {{1,2},{2,2},{0,2},
              {0,0},{1,0},{0,2},
              {0,0},{0,1},{0,2},
            } });
        tests.push_back(
            {
              "three_2",
              {
                {2},
                {2},
                {0,1},
              },// 0-2-1
               {{2,true},{0,false},{1,false},{2,false},
                {0,true},{0,false},{1,false},{2,false},
                {1,true},{0,false},{1,false},{2,false},
              },
              {{1,2},{1,2},{0,2},
              {0,0},{1,2},{0,2},
              {0,0},{0,1},{0,2},
            } });
    }

    void bfs_update(const vector<vector<Number>>& tree, queue<int>& q, vector<int>& dists, vector<unordered_set<int>>& closests, int node)
    {
        {
            auto& cl = closests[node];
            cl.clear();
            cl.insert(node);
            dists[node] = 0;
        }

        q.push(node);
        do
        {
            const auto par = q.front();
            q.pop();

            const auto cand = dists[par] + 1;
            const auto& edges = tree[par];
            for (const auto& to : edges)
            {
                auto& di = dists[to];
                if (di < cand)
                    continue;

                auto& cl = closests[to];
                if (di > cand)
                    cl.clear();

                cl.insert(node);
                di = cand;
                q.push(to);
            }
        } while (q.size());
    }

    void run_test_case(const test_case& test)
    {
        vector<pair<int, int>> select_answers;
        closest_select_update<Number>(test.tree, test.operations, select_answers);

        if (test.select_answers[0].first >= 0)
            Assert::AreEqual(test.select_answers, select_answers, "select_answers");

        queue<int> q;
        vector<int> dists(test.tree.size(), numeric_limits<int>::max() >> 1);
        vector<unordered_set<int>> closests(test.tree.size());

        auto selects = 0;

        for (const auto [node, is_update] : test.operations)
        {
            if (is_update)
            {
                bfs_update(test.tree, q, dists, closests, node);
                continue;
            }

            const auto& ans = select_answers[selects];
            const auto& actual = dists[node];
            const auto name = "select " + to_string(selects);
            Assert::AreEqual(ans.first, actual, name + " distance");

            const auto& cl = closests[node];
            const auto count = cl.count(ans.second);
            Assert::AreEqual(1, count, name + " node");

            ++selects;
        }
    }
}

namespace Privet::Algorithms::Trees::Tests
{
    void closest_select_update_tests()
    {
        test_utilities<test_case>::run_tests(run_test_case, generate_test_cases);
    }
}