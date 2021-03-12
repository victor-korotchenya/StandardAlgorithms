#include <ostream>
#include <string>
#include <vector>
#include "LowestCommonAncestor.h"
#include "lowest_common_ancestor.h"
#include "lowest_common_ancestor_tests.h"
#include "../Assert.h"

using namespace std;
using namespace Privet::Algorithms::Trees;
using namespace Privet::Algorithms;

namespace
{
    template <typename T>
    struct TreeNode final
    {
        TreeNode* Left = nullptr, * Right = nullptr;
        T Datum{};

        bool operator ==(const TreeNode& b) const
        {
            const bool result = Datum == b.Datum && Left == b.Left && Right == b.Right;
            return result;
        }

        friend ostream& operator <<
            (ostream& str, const TreeNode& b)
        {
            str << "Datum=" << b.Datum;
            return str;
        }
    };

    using data_t = int;
    using node_t = TreeNode<data_t>;

    struct LCATestCase final
    {
        string Name;
        node_t* Root{}, * A{}, * B{}, * Expected{};

        string to_string() const
        {
            const auto result = to_string(A)
                + "_" + to_string(B)
                + "_" + to_string(Expected);
            return result;
        }

    private:
        static string to_string(node_t* node)
        {
            const auto result = nullptr == node ? "nullptr" : std::to_string(node->Datum);
            return result;
        }
    };

    vector<node_t> BuildTree(vector<vector<int>>& tree, const int size)
    {
        assert(size > 0);

        vector<node_t> nodes(size);
        tree.resize(size + 1ll);

        for (auto i = 0; i < size; i++)
        {
            nodes[i].Datum = i;

            const auto left_index = i << 1 | 1;
            if (left_index >= size)
                continue;

            nodes[i].Left = &nodes[left_index];

            tree[i + 1ll].push_back(left_index + 1);
            tree[left_index + 1ll].push_back(i + 1);

            const auto right_index = (i << 1) + 2;
            if (right_index >= size)
                continue;

            nodes[i].Right = &nodes[right_index];

            tree[i + 1ll].push_back(right_index + 1);
            tree[right_index + 1ll].push_back(i + 1);
        }

        return nodes;
    }

    void add_test_case(vector<LCATestCase>& tests, node_t* root,
        data_t u, data_t v, data_t expected)
    {
        LCATestCase test;
        test.Root = root;
        test.A = root + u;
        test.B = root + v;
        test.Expected = root + expected;

        tests.push_back(test);
    }

    vector<LCATestCase> BuildTestCases(const vector<node_t>& nodes)
    {
        vector<LCATestCase> tests;
        auto root = const_cast<node_t*>(nodes.data());

        add_test_case(tests, root, 0, 0, 0);
        add_test_case(tests, root, 1, 2, 0);
        add_test_case(tests, root, 3, 4, 1);
        add_test_case(tests, root, 3, 9, 1);
        add_test_case(tests, root, 3, 10, 1);
        add_test_case(tests, root, 3, 8, 3);
        add_test_case(tests, root, 8, 3, 3);
        add_test_case(tests, root, 15, 16, 7);

        return tests;
    }
}

namespace Privet::Algorithms::Trees::Tests
{
    void lowest_common_ancestor_tests()
    {
        constexpr auto size = 32;
        lowest_common_ancestor<int, int(*)(const int&)> lca2(&refl<int>);
        auto& tree = lca2.reset(size);

        const auto nodes = BuildTree(tree, size);
        Assert::NotEmpty(nodes, "nodes");
        lca2.dfs();

        auto tests = BuildTestCases(nodes);
        Assert::NotEmpty(tests, "tests");

        vector<vector<pair<int, int>>> lca_requests(nodes.size() + 1);

        for (auto& test : tests)
        {
            if (test.Name.empty())
                test.Name = test.to_string();

            const auto from = test.A->Datum + 1,
                to = test.B->Datum + 1,
                expected = test.Expected->Datum + 1;
            assert(expected > 0);
            {
                if (from != to)
                {// Either should work.
                    constexpr auto garbage = -1;
                    lca_requests[from].emplace_back(to, garbage);
                    lca_requests[to].emplace_back(from, garbage);
                }
                else
                    lca_requests[to].emplace_back(to, to);
            }

            const auto actual = simple_binary_lca(test.Root, test.A, test.B);
            Assert::AreEqualByPtr(test.Expected, actual, test.Name);

            const auto actual2 = lca2.lca(from, to);
            Assert::AreEqual(expected, actual2, test.Name + " lca");
        }

        const lowest_common_ancestor_offline_fast fast(tree, lca_requests, refl<int>);

        const auto fi = [&lca_requests](const int from, const int to) {
            const auto& reqs = lca_requests[from];

            // A few values - O(n) time.
            const auto it = find_if(reqs.begin(), reqs.end(), [to](const pair<int, int>& p) {
                return to == p.first;
                });
            const auto val = it == reqs.end() ? -2 : it->second;
            return val;
        };

        for (const auto& test : tests)
        {
            const auto from = test.A->Datum + 1,
                to = test.B->Datum + 1,
                expected = test.Expected->Datum + 1;

            auto actual = fi(from, to);
            if (actual < 0)
                actual = fi(to, from);

            Assert::AreEqual(expected, actual, test.Name + " fast_lca");
        }
    }

    void euler_tour_tests()
    {
        const vector<vector<int>> tree{
            {
                { 1,2,3 },//0
                { 0,5,6,7,8, },
                { 0,4 },//2
                { 0,9,12 },
                { 2 },//4
                { 1,10,11 },
                { 1 },//6
                { 1 },
                { 1 },//8
                { 3 },
                { 5 },//10
                { 5 },
                { 3 },//12
            } };
        //        0
        //     1     2  3
        //   5___678   4 9_12
        //10_11
        const vector<int> expected_tour{
            0,1,5,10,5,11,5,1,6,1,7,1,8,1,
            0,2,4,2,
            0,3,9,3,12,3,0
        },
            expected_times{ 0,1,15,19,16,2,8,10,12,20,3,5,22, };
        Assert::AreEqual(tree.size(), expected_times.size(), "expected_times.size");

        vector<int> actual_times(tree.size()), actual_tour(tree.size() * 2 - 1);
        auto first_time = actual_times.data(), tour = actual_tour.data();
        euler_tour(tree, first_time, tour);

        Assert::AreEqual(expected_tour, actual_tour, "euler_tour");
        Assert::AreEqual(expected_times, actual_times, "first_times");

        lowest_common_ancestor<int, int(*)(const int&)> lca2(&refl<int>);
        auto& tree2 = lca2.reset(static_cast<int>(tree.size()));
        for (auto u = 0; u < static_cast<int>(tree.size()); ++u)
        {
            const auto& vs = tree[u];
            auto& vs2 = tree2[u + 1ll];
            for (const auto& v : vs)
                vs2.push_back(v + 1);
        }

        lca2.dfs();
        auto& tour2 = lca2.get_tour();
        for (auto& v : tour2)
            --v;

        Assert::AreEqual(expected_tour, tour2, "euler_tour2");
    }
}