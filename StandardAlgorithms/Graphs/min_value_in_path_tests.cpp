#include"min_value_in_path_tests.h"
#include"../Utilities/test_utilities.h"
#include"light_heavy_tree_decomposition.h"
#include"min_value_in_path.h"
#include"segment_tree.h"

namespace
{
    using weight_t = std::int64_t;
    using edge_t = std::pair<std::int32_t, weight_t>;
    using tree_t = std::vector<std::vector<edge_t>>;

    using min_tree_t =
        Standard::Algorithms::Trees::segment_tree<weight_t, const weight_t &(*)(const weight_t &, const weight_t &)>;

    constexpr weight_t min_weight = 1;
    constexpr weight_t max_weight = 1'000;

    struct oper_t final
    {
        std::int32_t from{};
        std::int32_t to{};
        weight_t weight{};
        bool is_update{};
    };

    auto operator<< (std::ostream &str, const oper_t &oper) -> std::ostream &
    {
        str << (oper.is_update ? "Update " : "Select ") << oper.from << "-" << oper.to << "= " << oper.weight;

        return str;
    }

    struct test_case final : Standard::Algorithms::Tests::base_test_case
    {
        constexpr test_case(std::string &&name, tree_t &&tree, std::vector<oper_t> &&operations) noexcept
            : base_test_case(std::move(name))
            , Tree(std::move(tree))
            , Operations(std::move(operations))
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

        constexpr void validate() const override
        {
            base_test_case::validate();

            ::Standard::Algorithms::ert::greater_or_equal(
                Tree.size(), static_cast<std::size_t>(Standard::Algorithms::Trees::min_size_tree_1based), "tree size.");

            ::Standard::Algorithms::ert::are_equal(true, Standard::Algorithms::Trees::is_tree(Tree, true), "is tree.");

            Standard::Algorithms::require_positive(Operations.size(), "operations size.");
        }

        void print(std::ostream &str) const override
        {
            ::Standard::Algorithms::print("tree", Tree, str);
            ::Standard::Algorithms::print("operations", Operations, str);
        }

private:
        tree_t Tree;
        std::vector<oper_t> Operations;
    };

    void gen_random_test(std::vector<test_case> &test_cases, Standard::Algorithms::Utilities::random_t<std::int32_t> &rnd,
        const std::int32_t att)
    {
        constexpr auto max_size = 30;

        const auto size = rnd(Standard::Algorithms::Trees::min_size_tree_1based, max_size);

        auto tree =
            Standard::Algorithms::Trees::build_random_tree_weighted<edge_t, weight_t>(size, min_weight, max_weight, true);

        std::vector<oper_t> ops(rnd(
            // NOLINTNEXTLINE
            5, 50));

        for (std::size_t index{}; index < ops.size(); ++index)
        {
            auto &oper = ops[index];

            oper.is_update = index + 1U != ops.size() && static_cast<bool>(rnd);

            constexpr auto maxw = 25'000;

            oper.weight = oper.is_update ? rnd(1, maxw) : weight_t{};
            oper.from = rnd(1, size - 1);

            if (oper.is_update)
            {// Update 1 edge.
                const auto &edges = tree[oper.from];
                const auto size2 = static_cast<std::int32_t>(edges.size());
                assert(0 < size2);

                const auto ind = rnd(0, size2 - 1);
                oper.to = edges[ind].first;
            }
            else
            {// might traverse many edges.
                do
                {
                    oper.to = rnd(1, size - 1);
                } while (oper.from == oper.to);
            }
        }

        test_cases.emplace_back("Random" + std::to_string(att), std::move(tree), std::move(ops));
    }

    void generate_test_cases(std::vector<test_case> &test_cases)
    {
        test_cases.push_back({
            "base1",
            {// 1-2-3
                {},
                // NOLINTNEXTLINE
                { edge_t(2, 10) },
                // NOLINTNEXTLINE
                { edge_t(1, 10), edge_t(3, 200) },
                // NOLINTNEXTLINE
                { edge_t(2, 200) } },
            // NOLINTNEXTLINE
            { oper_t{ 1, 2, 10, false },
                // NOLINTNEXTLINE
                oper_t{ 1, 3, 10, false },
                // NOLINTNEXTLINE
                oper_t{ 3, 2, 200, false },
                //
                // NOLINTNEXTLINE
                oper_t{ 2, 3, 5, true },
                // NOLINTNEXTLINE
                oper_t{ 1, 2, 10, false },
                // NOLINTNEXTLINE
                oper_t{ 1, 3, 5, false },
                // NOLINTNEXTLINE
                oper_t{ 3, 2, 5, false },
                //
                // NOLINTNEXTLINE
                oper_t{ 2, 1, 4, true },
                // NOLINTNEXTLINE
                oper_t{ 1, 3, 4, false },
                // NOLINTNEXTLINE
                oper_t{ 1, 2, 4, false },
                // NOLINTNEXTLINE
                oper_t{ 3, 2, 5, false } },
        });

        Standard::Algorithms::Utilities::random_t<std::int32_t> rnd{};

        constexpr auto max_attempts = 1;

        for (std::int32_t att{}; att < max_attempts; ++att)
        {
            gen_random_test(test_cases, rnd, att);
        }
    }

    constexpr auto build_min_tree(const tree_t &tree, const std::vector<std::int32_t> &orders,
        const std::vector<std::vector<std::int32_t>> &parents, const std::vector<std::int32_t> &depths) -> min_tree_t
    {
        const auto size = static_cast<std::int32_t>(tree.size());

        min_tree_t min_tree(size, std::min<weight_t>, std::numeric_limits<weight_t>::max());

        assert(static_cast<std::size_t>(size) == min_tree.max_index_exclusive());

        auto &data = min_tree.data();

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

                data[order - 1LL + size] = edge.second;
                --unprocessed_edges;

                assert(!(unprocessed_edges < 0));
            }
        }

        ::Standard::Algorithms::ert::are_equal(0, unprocessed_edges, "unprocessed_edges");

        min_tree.init();

        return min_tree;
    }

    // Select in O(log(n)**2).
    template<class min_op_t, class dec_t>
    constexpr auto select(const std::int32_t from, const min_op_t &min_op, const dec_t &dec, const std::int32_t tod)
        -> weight_t
    {
        assert(from != tod);

        const auto parent = dec.lca(from, tod);
        auto mini = std::numeric_limits<weight_t>::max();

        for (const auto &node : { from, tod })
        {
            if (parent != node)
            {
                dec.query_up(parent, node, min_op, mini);
            }
        }

        assert(mini < std::numeric_limits<weight_t>::max());

        return mini;
    }

    // Update in O(log(n)).
    constexpr void update(std::int32_t from, const std::vector<std::int32_t> &orders, const weight_t new_weight,
        const std::vector<std::int32_t> &depths, min_tree_t &min_tree, std::int32_t tod)
    {
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

        min_tree.set(max_order_rex - 1LL, new_weight);
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

        constexpr auto root = 1;
        dec.dfs(root);

        const auto &orders = dec.node_visit_orders();
        const auto &parents = dec.parents();
        const auto &depths = dec.depths();

        auto min_tree = build_min_tree(tree, orders, parents, depths);

        const auto min_op = [&min_tree](const std::int32_t start, const std::int32_t stop, weight_t &ans)
        {
            assert(0 < start && start < stop && static_cast<std::size_t>(stop) <= min_tree.max_index_exclusive());

            const auto cand = min_tree.get(start, stop);
            ans = std::min(ans, cand);
        };

        auto tree_slow = test.tree();

        const auto &opers = test.operations();

        // No constant. // NOLINTNEXTLINE
        for (std::size_t index{}; index < opers.size(); ++index)
        {
            const auto &oper = opers[index];

            try
            {
                if (oper.is_update)
                {
                    update(oper.from, orders, oper.weight, depths, min_tree, oper.to);

                    Standard::Algorithms::Trees::min_value_in_path_update_slow<edge_t, weight_t>(
                        oper.from, tree_slow, oper.to, oper.weight);
                }
                else
                {
                    const auto fast = select(oper.from, min_op, dec, oper.to);
                    if (oper.weight != weight_t{})
                    {
                        ::Standard::Algorithms::ert::are_equal(oper.weight, fast, "min_value_in_path_select");
                    }

                    const auto slow = Standard::Algorithms::Trees::min_value_in_path_select_slow<edge_t, weight_t>(
                        oper.from, tree_slow, oper.to);

                    ::Standard::Algorithms::ert::are_equal(fast, slow, "min_value_in_path_select_slow");
                }
            }
            catch (const std::exception &exc)
            {
                throw std::runtime_error(std::string(exc.what()) + " Error at operation " + std::to_string(index));
            }
        }
    }
} // namespace

void Standard::Algorithms::Trees::Tests::min_value_in_path_tests()
{
    Standard::Algorithms::Tests::test_utilities<test_case>::run_tests(run_test_case, generate_test_cases);
}
