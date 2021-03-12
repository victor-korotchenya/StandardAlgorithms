#include"light_heavy_tree_decomposition_tests.h"
#include"../Numbers/shift.h"
#include"../Utilities/random.h"
#include"../Utilities/test_utilities.h"
#include"light_heavy_tree_decomposition.h"
#include"segment_tree_add_sum.h"

namespace
{
    using weight_t = std::int64_t;
    using edge_t = std::pair<std::int32_t, weight_t>;
    using tree_t = std::vector<std::vector<edge_t>>;

    constexpr auto size_log = 5U;
    constexpr auto min_weight = 1;
    constexpr auto max_weight = 100;

    struct oper_t final
    {
        std::int32_t from{};
        std::int32_t to{};
        weight_t weight{};
        bool is_update_one_edge{};
    };

    auto operator<< (std::ostream &str, const oper_t &oper) -> std::ostream &
    {
        str << (oper.is_update_one_edge ? "Update " : "Select ") << oper.from << "-" << oper.to << "= " << oper.weight;

        return str;
    }

    struct test_case final : Standard::Algorithms::Tests::base_test_case
    {
        constexpr test_case(
            std::string &&name, tree_t &&tree, std::vector<oper_t> &&operations, std::int32_t root) noexcept
            : base_test_case(std::move(name))
            , Tree(std::move(tree))
            , Operations(std::move(operations))
            , Root(root)
        {
        }

        [[nodiscard]] constexpr auto tree() const &noexcept -> const tree_t &
        {
            return Tree;
        }

        [[nodiscard]] constexpr auto operations() const &noexcept -> const std::vector<oper_t> &
        {
            return Operations;
        }

        [[nodiscard]] constexpr auto root() const noexcept -> std::int32_t
        {
            return Root;
        }

        constexpr void validate() const override
        {
            base_test_case::validate();

            Standard::Algorithms::require_positive(Tree.size(), "tree size");
            ::Standard::Algorithms::ert::are_equal(true, Standard::Algorithms::Trees::is_tree(Tree, true), "is tree");

            Standard::Algorithms::require_positive(Root, "root");
            ::Standard::Algorithms::ert::greater(static_cast<std::int32_t>(Tree.size()), Root, "root size");

            Standard::Algorithms::require_positive(Operations.size(), "operations size");
        }

        void print(std::ostream &str) const override
        {
            ::Standard::Algorithms::print_value("root", str, Root);
            ::Standard::Algorithms::print("tree", Tree, str);
            ::Standard::Algorithms::print("operations", Operations, str);
        }

private:
        tree_t Tree;
        std::vector<oper_t> Operations;
        std::int32_t Root;
    };

    constexpr void generate_operation(
        const tree_t &tree, oper_t &oper, Standard::Algorithms::Utilities::random_t<std::int32_t> &rnd)
    {
        const auto size = static_cast<std::int32_t>(tree.size()) - 1;
        oper.from = rnd(1, size);

        if (oper.is_update_one_edge)
        {
            const auto &edges = tree[oper.from];
            const auto size2 = static_cast<std::int32_t>(edges.size());
            assert(0 < size2);

            const auto ind = rnd(0, size2 - 1);
            oper.to = edges[ind].first;
            assert(oper.from != oper.to);

            oper.weight = rnd(min_weight, max_weight);
            return;
        }

        // might traverse many edges.
        oper.weight = {};
        do
        {
            oper.to = rnd(1, size);
        } while (oper.from == oper.to);
    }

    void gen_random_test(std::vector<test_case> &test_cases, Standard::Algorithms::Utilities::random_t<std::int32_t> &rnd,
        const std::int32_t att)
    {
        constexpr auto min_size = 2;
        constexpr auto max_size = ::Standard::Algorithms::Numbers::shift_left(static_cast<std::int32_t>(1), size_log);

        const auto size = rnd(min_size, max_size);
        const auto root = rnd(1, size);

        auto tree =
            Standard::Algorithms::Trees::build_random_tree_weighted<edge_t, weight_t>(size, min_weight, max_weight, true);
        assert(tree.size() == std::size_t(size) + 1LLU);

        std::vector<oper_t> ops(rnd(
            // NOLINTNEXTLINE
            5, 50));

        for (std::size_t index{}; index < ops.size(); ++index)
        {
            auto &oper = ops[index];

            oper.is_update_one_edge = index + 1U != ops.size() && static_cast<bool>(rnd);

            generate_operation(tree, oper, rnd);
        }

        test_cases.emplace_back("Random" + std::to_string(att), std::move(tree), std::move(ops), root);
    }

    void generate_test_cases(std::vector<test_case> &test_cases)
    {
        test_cases.push_back({ std::string("base1"),
            {// 1 - 2 - 3
              //  10  200
                {},
                // NOLINTNEXTLINE
                { edge_t(2, 10) },
                // NOLINTNEXTLINE
                { edge_t(1, 10), edge_t(3, 200) },
                // NOLINTNEXTLINE
                { edge_t(2, 200) } },
            {// NOLINTNEXTLINE
                oper_t{ 1, 2, 10, false },
                // NOLINTNEXTLINE
                oper_t{ 1, 3, 210, false },
                // NOLINTNEXTLINE
                oper_t{ 3, 2, 200, false },
                //
                // NOLINTNEXTLINE
                oper_t{ 2, 3, 5, true },
                // NOLINTNEXTLINE
                oper_t{ 1, 2, 10, false },
                // NOLINTNEXTLINE
                oper_t{ 1, 3, 15, false },
                // NOLINTNEXTLINE
                oper_t{ 3, 2, 5, false },
                //
                // NOLINTNEXTLINE
                oper_t{ 2, 1, 4, true },
                // NOLINTNEXTLINE
                oper_t{ 1, 2, 4, false },
                // NOLINTNEXTLINE
                oper_t{ 1, 3, 9, false },
                // NOLINTNEXTLINE
                oper_t{ 2, 3, 5, false } },
            1 });

        Standard::Algorithms::Utilities::random_t<std::int32_t> rnd{};

        constexpr auto max_attempts = 1;

        for (std::int32_t att{}; att < max_attempts; ++att)
        {
            gen_random_test(test_cases, rnd, att);
        }
    }

    template<class res_t, class tree_t>
    constexpr auto find_node_slow(const std::int32_t from, tree_t &tree,
        const std::int32_t tod) -> res_t & // todo(p3): return ref?
    {
        assert(from != tod);

        auto &edges = tree.at(from);

        // Cannot be cbegin() as a reference is returned.
        const auto ite = std::find_if(edges.begin(), edges.end(),
            [tod](const auto &node)
            {
                return node.first == tod;
            });

        ::Standard::Algorithms::ert::are_equal(
            true, ite != edges.end(), "find_node_slow(" + std::to_string(from) + ", " + std::to_string(tod) + ")");

        return *ite;
    }

    constexpr auto build_sum_tree(const tree_t &tree, const std::vector<std::int32_t> &orders,
        const std::vector<std::vector<std::int32_t>> &parents, const std::vector<std::int32_t> &depths)
        -> Standard::Algorithms::Trees::segment_tree_add_sum<>
    {
        const auto size = static_cast<std::int32_t>(tree.size());

        Standard::Algorithms::Trees::segment_tree_add_sum<> sum_tree(size_log);
        const auto sum_tree_size = sum_tree.size();
        auto &data = sum_tree.data();

        auto unprocessed_edges = size - 2;
        assert(0 < unprocessed_edges);

        for (auto from = 1; from < size; ++from)
        {
            for (const auto &edges = tree[from]; const auto &edge : edges)
            {
                const auto &tod = edge.first;

                if (depths[tod] < depths[from])
                {
                    continue;
                }

                assert(depths[from] < depths[tod] && parents[tod][0] == from);

                const auto &order = orders[tod];
                assert(0 < order && order < size);

                data[order - 1LL + sum_tree_size] = edge.second;

                --unprocessed_edges;

                assert(!(unprocessed_edges < 0));
            }
        }

        ::Standard::Algorithms::ert::are_equal(0, unprocessed_edges, "unprocessed_edges");

        sum_tree.init();

        return sum_tree;
    }

    // Select in O(log(n)**2).
    template<class sum_op_t, class dec_t>
    constexpr auto select(const sum_op_t &sum_op, const std::int32_t from, const dec_t &dec, const std::int32_t tod)
        -> weight_t
    {
        const auto parent = dec.lca(from, tod);

        weight_t ws1{};
        dec.query_up(parent, from, sum_op, ws1);

        weight_t ws2{};
        dec.query_up(parent, tod, sum_op, ws2);

        const auto sum = ws1 + ws2;
        return static_cast<weight_t>(sum);
    }

    // Update in O(log(n)).
    template<class sum_tree_t>
    constexpr void update(const std::vector<std::int32_t> &orders,
        // Can be stored in a map.
        const weight_t old_weight, const std::vector<std::int32_t> &depths, const weight_t new_weight,
        std::int32_t from, sum_tree_t &sum_tree, std::int32_t tod)
    {
        const auto delta = new_weight - old_weight;
        if (delta == weight_t{})
        {
            return;
        }

        if (depths[from] < depths[tod])
        {
            std::swap(from, tod);
        }

        assert(depths[tod] < depths[from]);

        const auto &max_order_rex = orders[from];

        {
            const auto &min_order = orders[tod];

            Standard::Algorithms::require_greater(max_order_rex, min_order, "max_order_rex");
        }

        sum_tree.add(delta, max_order_rex - 1, max_order_rex);
    }

    template<class sum_tree_t>
    constexpr void update_test(const oper_t &operation,

        // const std::string& name, // todo(p3): unused arg?

        const std::vector<std::int32_t> &orders, tree_t &tree2, const std::vector<std::int32_t> &depths,
        sum_tree_t &sum_tree)
    {
        auto [from, tod, new_weight, is_update_one_edge] = operation;

        // Can be stored in a map.
        weight_t old_weight{};

        {// Slow
            if (depths[from] < depths[tod])
            {
                std::swap(from, tod);
            }

            assert(depths[tod] < depths[from]);

            auto &node1 = find_node_slow<edge_t &>(from, tree2, tod);
            auto &node2 = find_node_slow<edge_t &>(tod, tree2, from);

            auto &weight1 = node1.second;
            auto &weight2 = node2.second;
            assert(0 < weight1 && &node1 != &node2 && weight1 == weight2);

            if (weight1 == new_weight)
            {
                ++new_weight;
            }

            old_weight = weight1;
            weight1 = weight2 = new_weight;
        }

        update(orders, old_weight, depths, new_weight, from, sum_tree, tod);
    }

    constexpr auto weight_up_slow(const tree_t &tree, const std::int32_t par,
        const std::vector<std::vector<std::int32_t>> &parents, std::int32_t chi) -> weight_t
    {
        weight_t sum{};

        while (chi != par)
        {
            const auto &par1 = parents[chi][0];
            assert(chi != par1 && 0 < par1);

            const auto &node = find_node_slow<const edge_t &>(chi, tree, par1);
            const auto &weight = node.second;
            sum += weight;
            chi = par1;
        }

        return sum;
    }

    template<class sum_op_t, class dec_t>
    constexpr void select_test(const oper_t &operation, const std::string &name,
        const std::vector<std::vector<std::int32_t>> &parents, const sum_op_t &sum_op, const dec_t &dec,
        const tree_t &tree)
    {
        const auto &[from, tod, weight, is_update_one_edge] = operation;
        const auto par = dec.lca(from, tod);
        const auto up1 = weight_up_slow(tree, par, parents, from);
        const auto slow = up1 + weight_up_slow(tree, par, parents, tod);

        if (0 < weight)
        {
            ::Standard::Algorithms::ert::are_equal(weight, slow, name + " weight");
        }

        const auto actual = select(sum_op, from, dec, tod);
        ::Standard::Algorithms::ert::are_equal(slow, actual, name);
    }

    constexpr void run_test_case(const test_case &test)
    {
        const auto size = static_cast<std::int32_t>(test.tree().size()) - 1;

        // No constant. // NOLINTNEXTLINE
        Standard::Algorithms::Trees::light_heavy_tree_decomposition<edge_t,
            decltype([](const edge_t &edge) noexcept -> const edge_t::first_type &
        {
            return edge.first;
        })> dec{};

        auto &tree = dec.reset(size);
        tree = test.tree();

        dec.dfs(test.root());

        const auto &orders = dec.node_visit_orders();
        const auto &parents = dec.parents();
        const auto &depths = dec.depths();

        auto sum_tree = build_sum_tree(tree, orders, parents, depths);

        const auto sum_op = [&sum_tree](const std::int32_t start, const std::int32_t stop, weight_t &ans)
        {
            Standard::Algorithms::require_positive(start, "start");
            Standard::Algorithms::require_greater(stop, start, "stop");
            Standard::Algorithms::require_less_equal(
                static_cast<std::size_t>(stop), sum_tree.size(), "stop vs sum tree size");

            const auto sum1 = sum_tree.sum(start, stop);
            ans += sum1;
        };

        // No changes to the original tree.
        auto tree2 = tree;

        // No constant. // NOLINTNEXTLINE
        for (std::size_t index{}; index < test.operations().size(); ++index)
        {
            const auto &operation = test.operations()[index];
            const auto name = "operation " + std::to_string(index);

            if (operation.is_update_one_edge)
            {
                update_test(operation, // name,
                    orders, tree2, depths, sum_tree);
            }
            else
            {
                select_test<decltype(sum_op)>(operation, name, parents, sum_op, dec, tree2);
            }
        }
    }
} // namespace

// Given a weighted tree, do:
// - Update edge weight.
// - Select edge weight sum along a path from-to where from!=to.
void Standard::Algorithms::Trees::Tests::light_heavy_tree_decomposition_tests()
{
    Standard::Algorithms::Tests::test_utilities<test_case>::run_tests(run_test_case, generate_test_cases);
}
