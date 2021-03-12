#include"is_bst_tests.h"
#include"../Utilities/ert.h"
#include"../Utilities/require_utilities.h"
#include"binary_search_tree_utilities.h"
#include"binary_tree_node.h"
#include"binary_tree_utilities.h"
#include<initializer_list>
#include<memory>

namespace
{
    template<class node_t>
    requires(std::is_pointer_v<node_t>)
    [[nodiscard]] constexpr auto get_binary_node(node_t node, const std::initializer_list<bool> &go_rights) -> node_t
    {
        Standard::Algorithms::throw_if_null("root node", node);

        for (const auto &flag : go_rights)
        {
            assert(node != nullptr);

            node = flag ? node->right : node->left;
            Standard::Algorithms::throw_if_null("node", node);
        }

        assert(node != nullptr);
        return node;
    }

    using int_t = std::int64_t;

    using node_t = Standard::Algorithms::Trees::binary_tree_node<int_t>;

    constexpr auto val5 = 5;
    constexpr auto val6 = 6;
    constexpr auto val7 = 7;
    constexpr auto val8 = 8;
    constexpr auto val10 = 10;
    constexpr auto val11 = 11;
    constexpr auto val12 = 12;
    constexpr auto val18 = 18;
    constexpr auto val20 = 20;
    constexpr auto val22 = 22;

    constexpr void produce_node(gsl::owner<node_t *> &node, const int_t value)
    {
        Standard::Algorithms::throw_if_not_null("node", node);

        node = new node_t{ value };
    }

    constexpr void create_tree(node_t *root)
    {
        Standard::Algorithms::throw_if_null("root", root);

        root->value = val5;

        produce_node(root->left, 2);
        produce_node(root->left->left, 1);
        produce_node(root->left->right, 4);

        //            5
        //       2           10
        //     1   4       7       12
        //               6   8   11   20
        //                           18
        produce_node(root->right, val10);
        produce_node(root->right->left, val7);
        produce_node(root->right->left->left, val6);
        produce_node(root->right->left->right, val8);

        produce_node(root->right->right, val12);
        produce_node(root->right->right->left, val11);
        produce_node(root->right->right->right, val20);
        produce_node(root->right->right->right->left, val18);
    }

    constexpr void run_test(const std::string &name, const node_t *const root, const bool expected)
    {
        {
            const auto actual =
                Standard::Algorithms::Trees::binary_search_tree_utilities<int_t>::is_bst_without_duplicates(root);
            ::Standard::Algorithms::ert::are_equal(expected, actual, name);
        }
        {
            const auto actual =
                Standard::Algorithms::Trees::binary_search_tree_utilities<int_t>::is_bst_duplicates_allowed(root);
            ::Standard::Algorithms::ert::are_equal(expected, actual, name + " dups");
        }
    }

    constexpr void left_right_greater(node_t *const root)
    {
        auto *const node = get_binary_node(root, { false, true });
        assert(node != nullptr);

        constexpr auto old = 4;

        if (old != node->value) [[unlikely]]
        {
            throw std::runtime_error("LeftRightGreater: precondition failed.");
        }

        node->value = val6;
        run_test("LeftRightGreater6", root, false);

        node->value = old;
        run_test("LeftRightGreater4", root, true);
    }

    constexpr void right_left_left_smaller(node_t *const root)
    {
        auto *const node = get_binary_node(root, { true, false, false });

        if (val6 != node->value) [[unlikely]]
        {
            throw std::runtime_error("RightLeftLeftSmaller: precondition failed.");
        }

        node->value = 3;
        run_test("RightLeftLeftSmaller0", root, false);

        node->value = val6;
        run_test("RightLeftLeftSmaller1", root, true);
    }

    constexpr void break_node(node_t *const root)
    {
        auto *const node = get_binary_node(root, { true, true, true, false });

        if (val18 != node->value) [[unlikely]]
        {
            throw std::runtime_error("BreakNode: precondition failed.");
        }

        node->value = val22;
        run_test("BreakNode0", root, false);

        node->value = val18;
        run_test("BreakNode1", root, true);
    }
} // namespace

void Standard::Algorithms::Trees::Tests::is_bst_tests()
{
    std::unique_ptr<node_t, void (*)(node_t *)> uniq(nullptr, free_tree_33<node_t>);

    {
        auto uni2 = std::make_unique<node_t>();
        uniq.reset(uni2.release());
    }

    auto *root = uniq.get();
    if (root == nullptr) [[unlikely]]
    {
        throw std::runtime_error("Cannot create the root node.");
    }

    create_tree(root);
    run_test("After creation", root, true);

    left_right_greater(root);
    right_left_left_smaller(root);

    break_node(root);
}
