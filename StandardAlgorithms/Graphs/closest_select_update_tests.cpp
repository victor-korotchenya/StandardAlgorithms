#include"closest_select_update_tests.h"
#include"../Utilities/test_utilities.h"
#include"closest_select_update.h"
#include<queue>
#include<unordered_set>

namespace
{
    using int_t = std::int32_t;
    using node_is_update_t = std::pair<std::int32_t, bool>;
    using distance_node_t = std::pair<std::int32_t, std::int32_t>;

    struct test_case : Standard::Algorithms::Tests::base_test_case
    {
        constexpr test_case(std::string &&name, std::vector<std::vector<int_t>> &&tree,
            std::vector<node_is_update_t> &&operations, std::vector<distance_node_t> &&select_answers) noexcept
            : base_test_case(std::move(name))
            , Tree(std::move(tree))
            , Operations(std::move(operations))
            , Select_answers(std::move(select_answers))
        {
        }

        [[nodiscard]] constexpr auto tree() const &noexcept -> const std::vector<std::vector<int_t>> &
        {
            return Tree;
        }

        [[nodiscard]] constexpr auto operations() const &noexcept -> const std::vector<node_is_update_t> &
        {
            return Operations;
        }

        [[nodiscard]] constexpr auto select_answers() const &noexcept -> const std::vector<distance_node_t> &
        {
            return Select_answers;
        }

        constexpr void validate() const override
        {
            base_test_case::validate();

            const auto size = Standard::Algorithms::require_positive(static_cast<std::int32_t>(Tree.size()), "tree size");

            {
                const auto istr = Standard::Algorithms::Trees::is_tree(Tree);
                ::Standard::Algorithms::ert::are_equal(true, istr, "is tree");
            }

            Standard::Algorithms::require_positive(Operations.size(), "operations size");
            Standard::Algorithms::require_positive(Select_answers.size(), "select answers size");

            auto min_node = size;
            auto max_node = -1;
            auto selects = 0U;

            for (const auto &oper : Operations)
            {
                min_node = std::min(min_node, oper.first);
                max_node = std::max(max_node, oper.first);
                selects += oper.second ? 0 : 1;
            }

            ::Standard::Algorithms::ert::greater_or_equal(min_node, 0, "min node");
            ::Standard::Algorithms::ert::greater_or_equal(max_node, min_node, "max node");
            ::Standard::Algorithms::ert::greater(size, max_node, "max node");
            ::Standard::Algorithms::ert::are_equal(selects, Select_answers.size(), "select answers size2");
        }

        void print(std::ostream &str) const override
        {
            ::Standard::Algorithms::print("tree", Tree, str);
            ::Standard::Algorithms::print("operations", Operations, str);
            ::Standard::Algorithms::print("select answers", Select_answers, str);
        }

private:
        std::vector<std::vector<int_t>> Tree;
        std::vector<node_is_update_t> Operations;
        std::vector<distance_node_t> Select_answers;
    };

    void generate_test_case(std::vector<test_case> &tests, Standard::Algorithms::Utilities::random_t<std::int32_t> &rnd,
        const std::int32_t att)
    {
        constexpr auto min_size = 2;
        constexpr auto max_size_tree = 20;
        const auto tree_size = rnd(min_size, max_size_tree);

        auto tree = Standard::Algorithms::Trees::build_random_tree<int_t>(tree_size);

        constexpr auto max_size_ops = 25;
        const auto op_size = rnd(min_size, max_size_ops);

        std::vector<node_is_update_t> operations(op_size);
        std::int32_t selects{};

        for (std::int32_t index{}; index < op_size; ++index)
        {
            const auto is_update = index == 0 || (index + 1 != op_size && static_cast<bool>(rnd));

            auto &operation = operations[index];
            operation = { rnd(0, tree_size - 1), is_update };
            selects += operation.second ? 0 : 1;
        }

        tests.emplace_back("Random" + std::to_string(att), std::move(tree), std::move(operations),
            std::vector<distance_node_t>(selects, { -1, -1 }));
    }

    void generate_test_cases(std::vector<test_case> &tests)
    {
        Standard::Algorithms::Utilities::random_t<std::int32_t> rnd{};

        constexpr auto max_att = 1;

        for (std::int32_t att{}; att < max_att; ++att)
        {
            generate_test_case(tests, rnd, att);
        }

        tests.push_back({ "One", { {} }, { { 0, true }, { 0, false } }, { { 0, 0 } } });

        tests.push_back({ "Edge", { { 1 }, { 0 } },
            { { 0, true }, { 0, false }, { 1, false }, { 1, true }, { 1, false }, { 0, false } },
            { { 0, 0 }, { 1, 0 }, { 0, 1 }, { 0, 0 } } });

        tests.push_back({ "three_1", { { 1 }, { 0, 2 }, { 1 } }, // 0-1-2
            { { 2, true }, { 0, false }, { 1, false }, { 2, false }, { 0, true }, { 0, false }, { 1, false },
                { 2, false }, { 1, true }, { 0, false }, { 1, false }, { 2, false } },
            { { 2, 2 }, { 1, 2 }, { 0, 2 }, { 0, 0 }, { 1, 2 }, { 0, 2 }, { 0, 0 }, { 0, 1 }, { 0, 2 } } });

        tests.push_back({ "three_0", { { 1, 2 }, { 0 }, { 0 } }, // 1-0-2
            { { 2, true }, { 0, false }, { 1, false }, { 2, false }, { 0, true }, { 0, false }, { 1, false },
                { 2, false }, { 1, true }, { 0, false }, { 1, false }, { 2, false } },
            { { 1, 2 }, { 2, 2 }, { 0, 2 }, { 0, 0 }, { 1, 0 }, { 0, 2 }, { 0, 0 }, { 0, 1 }, { 0, 2 } } });

        tests.push_back({ "three_2", { { 2 }, { 2 }, { 0, 1 } }, // 0-2-1
            { { 2, true }, { 0, false }, { 1, false }, { 2, false }, { 0, true }, { 0, false }, { 1, false },
                { 2, false }, { 1, true }, { 0, false }, { 1, false }, { 2, false } },
            { { 1, 2 }, { 1, 2 }, { 0, 2 }, { 0, 0 }, { 1, 2 }, { 0, 2 }, { 0, 0 }, { 0, 1 }, { 0, 2 } } });
    }

    void bfs_update(const std::vector<std::vector<int_t>> &tree, std::queue<std::int32_t> &que,
        std::vector<std::int32_t> &dists, std::vector<std::unordered_set<std::int32_t>> &closests, std::int32_t node)
    {
        {
            auto &clo = closests[node];
            clo.clear();
            clo.insert(node);
            dists[node] = 0;
        }

        que.push(node);
        do
        {
            const auto par = que.front();
            que.pop();

            const auto cand = dists[par] + 1;
            const auto &edges = tree[par];

            for (const auto &tod : edges)
            {
                auto &dis = dists[tod];
                if (dis < cand)
                {
                    continue;
                }

                auto &clo = closests[tod];

                if (cand < dis)
                {
                    clo.clear();
                }

                clo.insert(node);
                dis = cand;
                que.push(tod);
            }
        } while (!que.empty());
    }

    void run_test_case(const test_case &test)
    {
        std::vector<distance_node_t> select_answers;

        Standard::Algorithms::Trees::closest_select_update<int_t>(test.tree(), test.operations(), select_answers);

        if (!(test.select_answers()[0].first < 0))
        {
            ::Standard::Algorithms::ert::are_equal(test.select_answers(), select_answers, "select answers");
        }

        std::queue<std::int32_t> que;

        std::vector<std::int32_t> dists(test.tree().size(), std::numeric_limits<std::int32_t>::max() / 2);

        std::vector<std::unordered_set<std::int32_t>> closests(test.tree().size());

        std::int32_t selects{};

        for (const auto &oper : test.operations())
        {
            const auto &node = oper.first;

            if (const auto &is_update = oper.second; is_update)
            {
                bfs_update(test.tree(), que, dists, closests, node);
                continue;
            }

            const auto &ans = select_answers[selects];
            const auto &actual = dists[node];
            const auto name = "select " + std::to_string(selects);
            ::Standard::Algorithms::ert::are_equal(ans.first, actual, name + " distance");

            {
                const auto &clo = closests[node];
                const auto actual_is = clo.contains(ans.second);

                ::Standard::Algorithms::ert::are_equal(true, actual_is, name + " node");
            }

            ++selects;
        }
    }
} // namespace

void Standard::Algorithms::Trees::Tests::closest_select_update_tests()
{
    Standard::Algorithms::Tests::test_utilities<test_case>::run_tests(run_test_case, generate_test_cases);
}
