#include"lowest_common_ancestor_tests.h"
#include"../Utilities/test_utilities.h"
#include"euler_tour.h"
#include"lowest_common_ancestor.h"
#include"lowest_common_ancestor_offline_fast.h"
#include"simple_binary_lca.h"
#include<compare>

namespace
{
    template<class item_t>
    struct tree_node final
    {
        // NOLINTNEXTLINE
        item_t datum{};

        // NOLINTNEXTLINE
        tree_node *left{};

        // NOLINTNEXTLINE
        tree_node *right{};

        // Must be in.
        [[nodiscard]] inline constexpr auto operator<=> (const tree_node &) const noexcept = default;
    };

    template<class item_t>
    auto operator<< (std::ostream &str, const tree_node<item_t> &node) -> std::ostream &
    {
        str << "datum " << node.datum;
        return str;
    }

    using data_t = std::int32_t;
    using node_t = tree_node<data_t>;

    struct lca_test_case final
    {
        std::string name{};
        const node_t *root{};
        const node_t *a{};
        const node_t *b{};
        const node_t *expected{};
    };

    [[nodiscard]] inline auto to_string(const node_t *node) -> std::string
    {
        auto result = nullptr == node ? "nullptr" : std::to_string(node->datum);
        return result;
    }

    [[nodiscard]] auto to_string(const lca_test_case &test) -> std::string
    {
        auto result = to_string(test.a) + "_" + to_string(test.b) + "_" + to_string(test.expected);
        return result;
    }

    [[nodiscard]] constexpr auto build_tree(std::vector<std::vector<std::int32_t>> &tree, const std::int32_t size)
        -> std::vector<node_t>
    {
        Standard::Algorithms::require_positive(size, "size");

        std::vector<node_t> nodes(size);
        tree.resize(size + 1LL);

        for (std::int32_t index{}; index < size; ++index)
        {
            nodes[index].datum = index;

            const auto left_index = index * 2 + 1;
            if (!(left_index < size))
            {
                continue;
            }

            nodes[index].left = &nodes[left_index];

            tree[index + 1LL].push_back(left_index + 1);
            tree[left_index + 1LL].push_back(index + 1);

            const auto right_index = (index + 1) * 2;
            if (!(right_index < size))
            {
                continue;
            }

            nodes[index].right = &nodes[right_index];

            tree[index + 1LL].push_back(right_index + 1);
            tree[right_index + 1LL].push_back(index + 1);
        }

        return nodes;
    }

    constexpr void add_test_case(const data_t one, std::vector<lca_test_case> &tests, const data_t two,
        const node_t *const root, const data_t expected)
    {
        lca_test_case test;
        test.root = root;

        // NOLINTNEXTLINE
        test.a = root + one;
        // NOLINTNEXTLINE
        test.b = root + two;
        // NOLINTNEXTLINE
        test.expected = root + expected;

        tests.push_back(test);
    }

    [[nodiscard]] constexpr auto build_test_cases(const std::vector<node_t> &nodes) -> std::vector<lca_test_case>
    {
        std::vector<lca_test_case> tests;
        const auto *const root = nodes.data();

        add_test_case(0, tests, 0, root, 0);
        add_test_case(1, tests, 2, root, 0);
        add_test_case(3, tests, 4, root, 1);

        // NOLINTNEXTLINE
        add_test_case(3, tests, 9, root, 1);
        // NOLINTNEXTLINE
        add_test_case(3, tests, 10, root, 1);
        // NOLINTNEXTLINE
        add_test_case(3, tests, 8, root, 3);
        // NOLINTNEXTLINE
        add_test_case(8, tests, 3, root, 3);
        // NOLINTNEXTLINE
        add_test_case(15, tests, 16, root, 7);

        return tests;
    }
} // namespace

void Standard::Algorithms::Trees::Tests::lowest_common_ancestor_tests()
{
    constexpr auto size = 32;

    Standard::Algorithms::Trees::lowest_common_ancestor<std::int32_t, std::int32_t (*)(const std::int32_t &)> lca2(
        &node_to<std::int32_t>);

    auto &tree = lca2.reset(size);

    const auto nodes = build_tree(tree, size);
    ::Standard::Algorithms::ert::not_empty(nodes, "nodes");

    constexpr auto root = 1;
    lca2.dfs(root);

    auto tests = build_test_cases(nodes);
    ::Standard::Algorithms::ert::not_empty(tests, "tests");

    std::vector<std::vector<std::pair<std::int32_t, std::int32_t>>> lca_requests(nodes.size() + 1);

    for (auto &test : tests)
    {
        if (test.name.empty())
        {
            test.name = to_string(test);
        }

        const auto from = test.a->datum + 1;
        const auto tod = test.b->datum + 1;
        const auto expected = test.expected->datum + 1;

        assert(0 < expected);
        {
            if (from != tod)
            {// Either should work.
                constexpr auto garbage = -1;
                lca_requests[from].emplace_back(tod, garbage);
                lca_requests[tod].emplace_back(from, garbage);
            }
            else
            {
                lca_requests[tod].emplace_back(tod, tod);
            }
        }

        const auto *actual = Standard::Algorithms::Trees::simple_binary_lca(test.root, test.a, test.b);
        ::Standard::Algorithms::ert::are_equal_by_ptr(test.expected, actual, test.name);

        const auto actual2 = lca2.lca(from, tod);
        ::Standard::Algorithms::ert::are_equal(expected, actual2, test.name + " lca");
    }

    const Standard::Algorithms::Trees::lowest_common_ancestor_offline_fast fast(
        tree, lca_requests, node_to<std::int32_t>);

    const auto finder = [&lca_requests](const std::pair<std::int32_t, std::int32_t> &from_to)
    {
        const auto &from1 = from_to.first;
        const auto &tod1 = from_to.second;
        const auto &reqs = lca_requests[from1];

        // A few values - O(n) time.
        const auto ite = std::find_if(reqs.cbegin(), reqs.cend(),
            [tod1](const auto &par)
            {
                return tod1 == par.first;
            });

        constexpr auto nothing = -2;
        const auto val = ite == reqs.cend() ? nothing : ite->second;
        return val;
    };

    for (const auto &test : tests)
    {
        const auto from = test.a->datum + 1;
        const auto tod = test.b->datum + 1;
        const auto expected = test.expected->datum + 1;

        auto actual = finder({ from, tod });
        if (actual < 0)
        {
            actual = finder({ tod, from });
        }

        ::Standard::Algorithms::ert::are_equal(expected, actual, test.name + " fast_lca");
    }
}

void Standard::Algorithms::Trees::Tests::lca_euler_tour_tests()
{
    const std::vector<std::vector<std::int32_t>> tree{ {
        { 1, 2, 3 }, // 0
        // NOLINTNEXTLINE
        {
            0,
            5,
            6,
            7,
            8,
        },
        { 0, 4 }, // 2
        // NOLINTNEXTLINE
        { 0, 9, 12 }, { 2 }, // 4
        // NOLINTNEXTLINE
        { 1, 10, 11 }, { 1 }, // 6
        { 1 }, { 1 }, // 8
        { 3 },
        // NOLINTNEXTLINE
        { 5 }, // 10
        // NOLINTNEXTLINE
        { 5 }, { 3 }, // 12
    } };
    //        0
    //     1     2  3
    //   5___678   4 9_12
    // 10_11
    const std::vector<std::int32_t> expected_tour{// NOLINTNEXTLINE
        0, 1, 5, 10, 5, 11, 5, 1, 6, 1, 7, 1, 8, 1,
        // NOLINTNEXTLINE
        0, 2, 4, 2,
        // NOLINTNEXTLINE
        0, 3, 9, 3, 12, 3, 0
    };

    const std::vector<std::int32_t> expected_times{// NOLINTNEXTLINE
        0, 1, 15, 19, 16, 2, 8, 10, 12, 20, 3, 5, 22
    };

    ::Standard::Algorithms::ert::are_equal(true, Standard::Algorithms::Trees::is_tree(tree), "is tree.");

    ::Standard::Algorithms::ert::are_equal(tree.size(), expected_times.size(), "expected_times.size");

    std::vector<std::int32_t> actual_times(tree.size());
    std::vector<std::int32_t> actual_tour(tree.size() * 2 - 1);

    auto *first_time = actual_times.data();
    auto *tour = actual_tour.data();

    Standard::Algorithms::Graphs::to_check_euler_tour(tree, first_time, tour);

    ::Standard::Algorithms::ert::are_equal(expected_tour, actual_tour, "to_check_euler_tour");
    ::Standard::Algorithms::ert::are_equal(expected_times, actual_times, "first_times");

    Standard::Algorithms::Trees::lowest_common_ancestor<std::int32_t, std::int32_t (*)(const std::int32_t &)> lca2(
        &node_to<std::int32_t>);

    auto &tree2 = lca2.reset(static_cast<std::int32_t>(tree.size()));

    for (std::int32_t from{}; from < static_cast<std::int32_t>(tree.size()); ++from)
    {
        const auto &src = tree[from];
        auto &dst = tree2[from + 1LL];

        const auto old_size = static_cast<std::ptrdiff_t>(dst.size());
        dst.resize(old_size + src.size());

        std::transform(src.cbegin(), src.cend(), dst.begin() + old_size,
            [] [[nodiscard]] (const auto &val)
            {
                return val + 1;
            });
    }

    constexpr auto root = 1;
    lca2.dfs(root);

    auto &tour2 = lca2.tour();

    for (auto &val : tour2)
    {
        --val;
    }

    ::Standard::Algorithms::ert::are_equal(expected_tour, tour2, "euler_tour2");
}
