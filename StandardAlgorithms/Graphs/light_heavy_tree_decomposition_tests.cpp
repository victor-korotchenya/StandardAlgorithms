#include "light_heavy_tree_decomposition.h"
#include "light_heavy_tree_decomposition_tests.h"
#include "segment_tree_add_sum.h"
#include "../Utilities/Random.h"
#include "../test_utilities.h"

using namespace std;
using namespace Privet::Algorithms::Trees;
using namespace Privet::Algorithms::Tests;
using namespace Privet::Algorithms;

namespace
{
    using weight_t = int64_t;
    using edge_t = pair<int, int64_t>;
    using tree_t = vector<vector<edge_t>>;

    constexpr auto size_log = 5, min_weight = 1, max_weight = 100;

    const edge_t::first_type& get_first(const edge_t& p)
    {
        return p.first;
    }

    struct oper_t final
    {
        int from{}, to{};
        weight_t weight{};
        bool is_update_one_edge{};
    };

    ostream& operator <<(ostream& str, oper_t const& op)
    {
        str << (op.is_update_one_edge ? "Update " : "Select ")
            << op.from << "-" << op.to << "= " << op.weight;

        return str;
    }

    struct test_case final : base_test_case
    {
        tree_t tree;
        vector<oper_t> operations;
        int root;

        test_case(string&& name,
            tree_t&& tree,
            vector<oper_t>&& operations,
            int root)
            : base_test_case(forward<string>(name)),
            tree(forward<tree_t>(tree)),
            operations(forward<vector<oper_t>>(operations)),
            root(root)
        {
        }

        void Validate() const override
        {
            base_test_case::Validate();

            RequirePositive(tree.size(), "tree size");
            Assert::AreEqual(true, is_tree(tree, true), "is tree");

            RequirePositive(root, "root");
            Assert::Greater(static_cast<int>(tree.size()), root, "root size");

            RequirePositive(operations.size(), "operations size");
        }

        void Print(ostream& str) const override
        {
            base_test_case::Print(str);
            ::PrintValue(str, "root", root);
            ::Print("tree", tree, str);
            ::Print("operations", operations, str);
        }
    };

    void generate_operation(const tree_t& tree, oper_t& op, IntRandom& r)
    {
        const auto n = static_cast<int>(tree.size()) - 1;
        op.from = r(1, n);

        if (op.is_update_one_edge)
        {
            const auto& edges = tree[op.from];
            const auto size2 = static_cast<int>(edges.size());
            assert(size2 > 0);

            const auto ind = r(0, size2 - 1);
            op.to = edges[ind].first;
            assert(op.from != op.to);

            op.weight = r(min_weight, max_weight);
            return;
        }

        //might traverse many edges.
        op.weight = {};
        do
        {
            op.to = r(1, n);
        } while (op.from == op.to);
    }

    void gen_random_test(vector<test_case>& test_cases, IntRandom& r, const int att)
    {
        const auto n = r(2, 1 << size_log);
        const auto root = r(1, n);

        auto tree = build_random_tree_weighted<edge_t, weight_t>(n, min_weight, max_weight, true);
        assert(tree.size() == n + 1ll);

        vector<oper_t> ops(r(5, 50));

        for (size_t i = 0; i < ops.size(); ++i)
        {
            auto& op = ops[i];
            op.is_update_one_edge = i + 1 != ops.size() && (r(0, 1) & 1);
            generate_operation(tree, op, r);
        }

        test_cases.emplace_back("random" + to_string(att), move(tree), move(ops), root);
    }

    void generate_test_cases(vector<test_case>& test_cases)
    {
        test_cases.push_back({ "base1"s, {
            // 1 - 2 - 3
            //  10  200
            {},
            {edge_t(2,10)},
            {edge_t(1,10), edge_t(3,200)},
            {edge_t(2,200)},
        }, {
            oper_t(1, 2, 10, false),
            oper_t(1, 3, 210, false),
            oper_t(3, 2, 200, false),
            //
            oper_t(2, 3, 5, true),
            oper_t(1, 2, 10, false),
            oper_t(1, 3, 15, false),
            oper_t(3, 2, 5, false),
            //
            oper_t(2, 1, 4, true),
            oper_t(1, 2, 4, false),
            oper_t(1, 3, 9, false),
            oper_t(2, 3, 5, false),
        },
        1 });

        IntRandom r;

        for (auto att = 0, att_max = 1; att < att_max; ++att)
            gen_random_test(test_cases, r, att);
    }

    template<class t, class tr>
    t find_node_slow(tr& tree, const int from, const int to)
    {
        assert(from != to);

        auto& edges = tree[from];
        auto it = find_if(edges.begin(), edges.end(), [to](const edge_t& node) {
            return node.first == to;
            });

        assert(it != edges.end());
        return *it;
    }

    auto build_sum_tree(const tree_t& tree, const vector<int>& orders, const vector<vector<int>>& parents, const vector<int>& depths)
    {
        const auto size = static_cast<int>(tree.size());

        segment_tree_add_sum<> sum_tree(size_log);
        const auto sum_tree_size = sum_tree.size();
        auto& data = sum_tree.data();

        auto unprocessed_edges = size - 2;
        assert(unprocessed_edges > 0);

        for (auto from = 1; from < size; ++from)
        {
            for (const auto& u : tree[from])
            {
                const auto& to = u.first;
                if (depths[from] > depths[to])
                    continue;

                assert(depths[from] < depths[to] && parents[to][0] == from);

                const auto& order = orders[to];
                assert(order > 0 && order < size);

                data[order - 1ll + sum_tree_size] = u.second;
                --unprocessed_edges;
                assert(unprocessed_edges >= 0);
            }
        }

        Assert::AreEqual(0, unprocessed_edges, "unprocessed_edges");

        sum_tree.init();
        return sum_tree;
    }

    // Select in O(log(n)**2).
    template<class sum_op_t, class dec_t>
    weight_t select(const sum_op_t& sum_op, const dec_t& dec,
        const int from, const int to)
    {
        const auto parent = dec.lca(from, to);
        weight_t s1{}, s2{};
        dec.query_up(parent, from, sum_op, s1);
        dec.query_up(parent, to, sum_op, s2);
        const auto res = s1 + s2;
        return res;
    }

    // Update in O(log(n)).
    template<class sum_tree_t>
    void update(const vector<int>& orders, const vector<int>& depths,
        // Can be stored in a map.
        const weight_t old_weight, const weight_t new_weight,
        sum_tree_t& sum_tree, int from, int to)
    {
        const auto delta = new_weight - old_weight;
        if (!delta)
            return;

        if (depths[from] < depths[to])
            swap(from, to);
        assert(depths[from] > depths[to]);

        const auto& max_order_rex = orders[from];
#if _DEBUG
        {
            const auto& min_order = orders[to];
            assert(min_order < max_order_rex);
        }
#endif
        sum_tree.add(delta, max_order_rex - 1, max_order_rex);
    }

    template<class sum_tree_t>
    void update_test(const oper_t& operation, const string& name, const vector<int>& orders, const vector<int>& depths, tree_t& tree2, sum_tree_t& sum_tree)
    {
        auto [from, to, new_weight, is_update_one_edge] = operation;

        // Can be stored in a map.
        weight_t old_weight;
        {//Slow
            if (depths[from] < depths[to])
                swap(from, to);
            assert(depths[from] > depths[to]);

            auto& node1 = find_node_slow<edge_t&>(tree2, from, to);
            auto& node2 = find_node_slow<edge_t&>(tree2, to, from);

            auto& weight1 = node1.second, & weight2 = node2.second;
            assert(weight1 > 0 && &node1 != &node2 && weight1 == weight2);

            if (weight1 == new_weight)
                ++new_weight;

            old_weight = weight1;
            weight1 = weight2 = new_weight;
        }

        update(orders, depths,
            old_weight, new_weight,
            sum_tree, from, to);
    }

    weight_t weight_up_slow(const tree_t& tree, const vector<vector<int>>& parents, const int par, int chi)
    {
        weight_t r{};
        while (chi != par)
        {
            const auto& par1 = parents[chi][0];
            assert(chi != par1 && par1 > 0);

            const auto& node = find_node_slow<const edge_t&>(tree, chi, par1);
            const auto& weight = node.second;
            r += weight;
            chi = par1;
        }

        return r;
    }

    template<class sum_op_t, class dec_t>
    void select_test(const oper_t& operation, const string& name, const vector<vector<int>>& parents, const sum_op_t& sum_op, const dec_t& dec, const tree_t& tree)
    {
        auto [from, to, weight, is_update_one_edge] = operation;
        const auto par = dec.lca(from, to);
        const auto up1 = weight_up_slow(tree, parents, par, from);
        const auto slow = up1 + weight_up_slow(tree, parents, par, to);
        if (weight > 0)
            Assert::AreEqual(weight, slow, name + " weight");

        const auto actual = select(sum_op, dec, from, to);
        Assert::AreEqual(slow, actual, name);
    }

    void run_test_case(const test_case& test)
    {
        const auto n = static_cast<int>(test.tree.size()) - 1;

        light_heavy_tree_decomposition<edge_t, const int& (*)(const edge_t&)> dec(&get_first);
        auto& tree = dec.reset(n);
        tree = test.tree;

        dec.dfs(test.root);

        const auto& orders = dec.get_node_visit_orders();
        const auto& parents = dec.get_parents();
        const auto& depths = dec.get_depths();

        auto sum_tree = build_sum_tree(tree, orders, parents, depths);

        const auto sum_op = [&sum_tree](const int start, const int stop, weight_t& ans) {
            assert(start > 0 && start < stop&& stop <= sum_tree.size());
            const auto sum1 = sum_tree.get_sum(start, stop);
            ans += sum1;
        };

        // No changes to the original tree.
        auto tree2 = tree;
        for (size_t i = 0; i < test.operations.size(); ++i)
        {
            const  auto& operation = test.operations[i];
            const auto name = "operation " + to_string(i);

            if (operation.is_update_one_edge)
                update_test(operation, name, orders, depths, tree2, sum_tree);
            else
                select_test<decltype(sum_op)>(operation, name, parents, sum_op, dec, tree2);
        }
    }
}

// Given a weighted tree, do:
// - Update edge weight.
// - Select edge weight sum along a path from-to where from!=to.
void Privet::Algorithms::Trees::Tests::light_heavy_tree_decomposition_tests()
{
    test_utilities<test_case>::run_tests(run_test_case, generate_test_cases);
}