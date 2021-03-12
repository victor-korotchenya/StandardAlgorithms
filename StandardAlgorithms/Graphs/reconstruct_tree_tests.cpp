#include"reconstruct_tree_tests.h"
#include"../Utilities/ert.h"
#include"are_binary_trees_isomorphic.h"
#include"binary_tree_node.h"
#include"binary_tree_utilities.h"
#include"reconstruct_tree.h"
#include"test_node_utilities.h"
#include<array>

namespace
{
    using int_t = std::uint16_t;
    using vec_int_t = std::vector<int_t>;

    using node_t = Standard::Algorithms::Trees::binary_tree_node<int_t>;
    using vec_node_t = std::vector<node_t>;

    // For 'const node_t', a double pointer should be 'const node_t*const*'.
    using pointer_to_constant_pointer = const node_t *const *;

    constexpr void test_post_order(const node_t *const expected_root, const vec_int_t &post_order_fast)
    {
        const auto size = post_order_fast.size();

        vec_int_t post_order_slow{};
        post_order_slow.reserve(size);

        Standard::Algorithms::Trees::binary_tree_utilities<int_t, const node_t>::post_order_slow<>(expected_root,
            [&](const node_t *const the_node) -> void
            {
                Standard::Algorithms::throw_if_null("post_order_slow.theNode", the_node);

                post_order_slow.push_back(the_node->value);
            });

        ::Standard::Algorithms::ert::are_equal(post_order_fast, post_order_slow, "post-order slow and fast.");
    }

    constexpr void build_visit_orders(vec_int_t &pre_order, const std::size_t size, vec_int_t &in_order,
        const node_t *const expected_root, vec_int_t &post_order)
    {
        pre_order.clear();
        in_order.clear();
        post_order.clear();

        pre_order.reserve(size);
        in_order.reserve(size);
        post_order.reserve(size);

        Standard::Algorithms::Trees::binary_tree_utilities<int_t, node_t>::pre_order<>(expected_root,
            [&](const node_t *const the_node) -> void
            {
                Standard::Algorithms::throw_if_null("pre_order.theNode", the_node);

                pre_order.push_back(the_node->value);
            });

        ::Standard::Algorithms::ert::are_equal(size, pre_order.size(), "preOrder.size");

        auto vis = [&](const node_t *const the_node) -> void
        {
            Standard::Algorithms::throw_if_null("in_order.theNode", the_node);

            in_order.push_back(the_node->value);
        };

        Standard::Algorithms::Trees::binary_tree_utilities<int_t, node_t>::in_order_slow(expected_root, vis);
        ::Standard::Algorithms::ert::are_equal(size, in_order.size(), "in_order_slow.size");

        auto visitor = [&post_order](const node_t *const the_node) -> void
        {
            Standard::Algorithms::throw_if_null("post order.theNode", the_node);

            post_order.push_back(the_node->value);
        };

        Standard::Algorithms::Trees::binary_tree_utilities<int_t, const node_t>::post_order<decltype(visitor),
            pointer_to_constant_pointer>(expected_root, visitor);

        ::Standard::Algorithms::ert::are_equal(size, post_order.size(), "postOrder.size");

        test_post_order(expected_root, post_order);
    }

    constexpr void vefify_visit_orders(const vec_int_t &pre_order_old, const node_t *const expected_root,
        const vec_int_t &in_order_old, const vec_int_t &post_order_old)
    {
        ::Standard::Algorithms::ert::are_equal(
            pre_order_old.size(), in_order_old.size(), "Sizes of preOrderOld and inOrderOld");
        ::Standard::Algorithms::ert::are_equal(
            pre_order_old.size(), post_order_old.size(), "Sizes of preOrderOld and postOrderOld");

        vec_int_t pre_order_new{};
        vec_int_t in_order_new{};
        vec_int_t post_order_new{};

        build_visit_orders(pre_order_new, pre_order_old.size(), in_order_new, expected_root, post_order_new);

        const std::string sufffix = "-order old and new.";

        ::Standard::Algorithms::ert::are_equal(pre_order_old, pre_order_new, "pre" + sufffix);

        ::Standard::Algorithms::ert::are_equal(in_order_old, in_order_new, "in" + sufffix);

        ::Standard::Algorithms::ert::are_equal(post_order_old, post_order_new, "post" + sufffix);
    }

    constexpr void test_template(
        const vec_int_t &pre_order, const node_t *const expected_root, const vec_int_t &in_order)
    {
        const auto size = pre_order.size();
        ::Standard::Algorithms::ert::are_equal(size, in_order.size(), "Pre- and in-Order sizes");

        node_t *actual_root{};

        try
        {
            Standard::Algorithms::Trees::reconstruct_tree<int_t, node_t>(pre_order, in_order, size, &actual_root);

            Standard::Algorithms::throw_if_null("The root must be not null after the tree creation.", actual_root);

            ::Standard::Algorithms::ert::are_equal(pre_order.at(0), actual_root->value, "Root value.");

            const auto actual = Standard::Algorithms::Trees::are_binary_trees_isomorphic(expected_root, actual_root);

            Standard::Algorithms::Trees::require_isomorphic(actual);
        }
        catch (...)
        {
            Standard::Algorithms::Trees::free_tree_33(&actual_root);
            throw;
        }

        Standard::Algorithms::Trees::free_tree_33(&actual_root);
    }

    constexpr void regular_test()
    {
        constexpr int_t size = 10;
        constexpr int_t mag9 = 9;
        constexpr int_t mag8 = 8;
        constexpr int_t mag7 = 7;
        constexpr int_t mag6 = 6;
        constexpr int_t mag5 = 5;

        vec_node_t nodes(size + 1LL);

        vec_int_t pre_order(size);
        vec_int_t in_order(size);
        vec_int_t post_order(size);

        for (int_t index{}; index < size; ++index)
        {
            pre_order[index] = index + 1;
        }

        auto *expected_root = set_value_get_node(nodes, static_cast<int_t>(1));

        auto *left = expected_root->left = set_value_get_node(nodes, static_cast<int_t>(2));

        left->right = set_value_get_node(nodes, static_cast<int_t>(3));

        left->right->left = set_value_get_node(nodes, static_cast<int_t>(4));
        //                 1
        //        2                  5
        // null      3           6        7
        //        4   null             8     10
        //                          9
        auto *right = expected_root->right = set_value_get_node(nodes, mag5);

        right->left = set_value_get_node(nodes, mag6);

        auto *right_right = right->right = set_value_get_node(nodes, mag7);

        right_right->left = set_value_get_node(nodes, mag8);

        right_right->left->left = set_value_get_node(nodes, mag9);

        right_right->right = set_value_get_node(nodes, static_cast<int_t>(size));

        in_order[0] = 2;
        in_order[1] = 4;
        in_order[2] = 3;
        in_order[3] = 1;
        in_order[4] = mag6;
        in_order[mag5] = mag5;
        in_order[mag6] = mag9;
        in_order[mag7] = mag8;
        in_order[mag8] = mag7;
        in_order[mag9] = size;

        post_order[0] = 4;
        post_order[1] = 3;
        post_order[2] = 2;
        post_order[3] = mag6;
        post_order[4] = mag9;
        post_order[mag5] = mag8;
        post_order[mag6] = size;
        post_order[mag7] = mag7;
        post_order[mag8] = mag5;
        post_order[mag9] = 1;

        vefify_visit_orders(pre_order, expected_root, in_order, post_order);

        test_template(pre_order, expected_root, in_order);
    }

    void random_test()
    {
        constexpr int_t min_size = 10;
        constexpr int_t max_size = 20;

        const auto size =
            static_cast<int_t>(Standard::Algorithms::Utilities::random_t<std::uint32_t>(min_size, max_size)());

        gsl::owner<node_t *> expected_root{};
        try
        {
            constexpr auto is_unique_only = true;

            Standard::Algorithms::Trees::binary_tree_utilities<int_t, node_t>::build_random_tree(
                size, &expected_root, size << 2U, is_unique_only);

            vec_int_t pre_order{};
            vec_int_t in_order{};
            vec_int_t post_order{};

            build_visit_orders(pre_order, size, in_order, expected_root, post_order);

            test_template(pre_order, expected_root, in_order);
        }
        catch (...)
        {
            free_tree_33(&expected_root);
            throw;
        }

        free_tree_33(&expected_root);
    }
} // namespace

void Standard::Algorithms::Trees::Tests::reconstruct_tree_tests()
{
    using pair_t = std::pair<void (*)(), std::string>;

    const std::array pairs{ pair_t{ random_test, "RandomTest" }, pair_t{ regular_test, "RegularTest" } };

    for (const auto &par : pairs)
    {
        try
        {
            par.first();
        }
        catch (const std::exception &exc)
        {
            auto str = ::Standard::Algorithms::Utilities::w_stream();
            str << "Error in " << par.second << ": " << exc.what();
            throw_exception(str);
        }
    }
}
