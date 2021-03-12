#include "min_value_in_path.h"
#include "min_value_in_path_tests.h"
#include "light_heavy_tree_decomposition.h"
#include "SegmentTree.h"
#include "../test_utilities.h"

using namespace std;
using namespace Privet::Algorithms::Trees;
using namespace Privet::Algorithms;
using namespace Privet::Algorithms::Tests;

namespace
{
    using weight_t = int64_t;
    using edge_t = pair<int, weight_t>;
    using tree_t = vector<vector<edge_t>>;
    using min_tree_t = SegmentTree<weight_t, const weight_t& (*)(const weight_t&, const weight_t&)>;

    constexpr weight_t min_weight = 1, max_weight = 1000;

    const edge_t::first_type& get_first(const edge_t& p)
    {
        return p.first;
    }

    struct oper_t final
    {
        int from{}, to{};
        weight_t weight{};
        bool is_update{};
    };

    ostream& operator <<(ostream& str, oper_t const& op)
    {
        str << (op.is_update ? "Update " : "Select ")
            << op.from << "-" << op.to << "= " << op.weight;

        return str;
    }

    struct test_case final : base_test_case
    {
        tree_t tree;
        vector<oper_t> operations;

        test_case(string&& name,
            tree_t&& tree,
            vector<oper_t>&& operations)
            : base_test_case(forward<string>(name)),
            tree(forward<tree_t>(tree)),
            operations(forward<vector<oper_t>>(operations))
        {
        }

        void Validate() const override
        {
            base_test_case::Validate();

            Assert::GreaterOrEqual(tree.size(), min_size_tree_1based, "tree size.");
            Assert::AreEqual(true, is_tree(tree, true), "is tree.");

            RequirePositive(operations.size(), "operations size.");
        }

        void Print(ostream& str) const override
        {
            base_test_case::Print(str);
            ::Print("tree", tree, str);
            ::Print("operations", operations, str);
        }
    };

    void gen_random_test(vector<test_case>& test_cases, IntRandom& r, const int att)
    {
        const auto size = r(min_size_tree_1based, 30);

        auto tree = build_random_tree_weighted<edge_t, weight_t>(size, min_weight, max_weight, true);

        vector<oper_t> ops(r(5, 50));

        for (size_t i = 0; i < ops.size(); ++i)
        {
            auto& op = ops[i];
            op.is_update = i + 1 != ops.size() && (r(0, 1) & 1);
            op.weight = op.is_update ? r(1, 25000) : weight_t();
            op.from = r(1, size - 1);

            if (op.is_update)
            {// Update 1 edge.
                const auto& edges = tree[op.from];
                const auto size2 = static_cast<int>(edges.size());
                assert(size2 > 0);

                const auto ind = r(0, size2 - 1);
                op.to = edges[ind].first;
            }
            else
            {//might traverse many edges.
                do
                {
                    op.to = r(1, size - 1);
                } while (op.from == op.to);
            }
        }

        test_cases.emplace_back("random" + to_string(att), move(tree), move(ops));
    }

    void generate_test_cases(vector<test_case>& test_cases)
    {
        test_cases.push_back({ "base1"s, {
            // 1-2-3
            {},
            {edge_t(2,10)},
            {edge_t(1,10), edge_t(3,200)},
            {edge_t(2,200)},
        }, {
            oper_t(1, 2, 10, false),
            oper_t(1, 3, 10, false),
            oper_t(3, 2, 200, false),
            //
            oper_t(2, 3, 5, true),
            oper_t(1, 2, 10, false),
            oper_t(1, 3, 5, false),
            oper_t(3, 2, 5, false),
            //
            oper_t(2, 1, 4, true),
            oper_t(1, 3, 4, false),
            oper_t(1, 2, 4, false),
            oper_t(3, 2, 5, false),
        } });

        IntRandom r;

        for (auto att = 0, att_max = 1; att < att_max; ++att)
            gen_random_test(test_cases, r, att);
    }

    auto build_min_tree(const tree_t& tree, const vector<int>& orders, const vector<vector<int>>& parents, const vector<int>& depths)
    {
        const auto size = static_cast<int>(tree.size());

        min_tree_t min_tree(size, std::min<weight_t>, std::numeric_limits<weight_t>::max());

        assert(size == min_tree.max_index_exclusive());
        auto& data = min_tree.data();

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

                data[order - 1ll + size] = u.second;
                --unprocessed_edges;
                assert(unprocessed_edges >= 0);
            }
        }

        Assert::AreEqual(0, unprocessed_edges, "unprocessed_edges");
        min_tree.init();
        return min_tree;
    }

    // Select in O(log(n)**2).
    template<class min_op_t, class dec_t>
    weight_t select(const min_op_t& min_op, const dec_t& dec,
        const int from, const int to)
    {
        assert(from != to);

        const auto parent = dec.lca(from, to);
        auto mi = std::numeric_limits<weight_t>::max();

        for (const auto& node : { from, to })
        {
            if (parent != node)
                dec.query_up(parent, node, min_op, mi);
        }

        assert(mi < std::numeric_limits<weight_t>::max());
        return mi;
    }

    // Update in O(log(n)).
    void update(const vector<int>& orders, const vector<int>& depths,
        const weight_t new_weight,
        min_tree_t& min_tree, int from, int to)
    {
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
        min_tree.set(max_order_rex - 1ll, new_weight);
    }

    void run_test_case(const test_case& test)
    {
        const auto n = static_cast<int>(test.tree.size()) - 1;

        light_heavy_tree_decomposition<edge_t, const int& (*)(const edge_t&)> dec(&get_first);
        auto& tree = dec.reset(n);
        tree = test.tree;

        constexpr auto root = 1;
        dec.dfs(root);

        const auto& orders = dec.get_node_visit_orders();
        const auto& parents = dec.get_parents();
        const auto& depths = dec.get_depths();

        auto min_tree = build_min_tree(tree, orders, parents, depths);

        const auto min_op = [&min_tree](const int start, const int stop, weight_t& ans) {
            assert(start > 0 && start < stop&& stop <= min_tree.max_index_exclusive());
            const auto cand = min_tree.get(start, stop);
            ans = min(ans, cand);
        };

        auto tree_slow = test.tree;

        for (auto i = 0; i < static_cast<int>(test.operations.size()); ++i)
        {
            const auto& op = test.operations[i];
            try
            {
                if (op.is_update)
                {
                    update(orders, depths, op.weight, min_tree, op.from, op.to);

                    min_value_in_path_update_slow<edge_t, weight_t>(tree_slow, op.from, op.to, op.weight);
                }
                else
                {
                    const auto actual = select(min_op, dec, op.from, op.to);
                    if (op.weight)
                        Assert::AreEqual(op.weight, actual, "min_value_in_path_select");

                    const auto slow = min_value_in_path_select_slow<edge_t, weight_t>(tree_slow, op.from, op.to);
                    Assert::AreEqual(actual, slow, "min_value_in_path_select_slow");
                }
            }
            catch (const exception& ex)
            {
                throw runtime_error(ex.what() + " Error at op "s + to_string(i));
            }
        }
    }
}

void Privet::Algorithms::Trees::Tests::min_value_in_path_tests()
{
    test_utilities<test_case>::run_tests(run_test_case, generate_test_cases);
}