#include"bst_to_doubly_linked_list_tests.h"
#include"../Utilities/ert.h"
#include"are_binary_trees_isomorphic.h"
#include"are_trees_isomorphic.h"
#include"binary_search_tree_utilities.h"
#include"binary_tree_node.h"
#include"test_node_utilities.h"

namespace
{
    using int_t = std::int64_t;
    using node_t = Standard::Algorithms::Trees::binary_tree_node<int_t>;

    constexpr void build_not_empty_tree(std::vector<node_t> &nodes)
    {
        nodes.clear();

        constexpr auto size = 50;
        nodes.resize(size);

        auto &root = nodes[0];
        // NOLINTNEXTLINE
        root.value = 20;

        auto *left = root.left = Standard::Algorithms::Trees::set_value_get_node(nodes,
            // NOLINTNEXTLINE
            10);

        auto *right = root.right = Standard::Algorithms::Trees::set_value_get_node(nodes,
            // NOLINTNEXTLINE
            30);

        //        20
        //    10            30
        //  5           25      40
        //   7      null  28
        left->left = Standard::Algorithms::Trees::set_value_get_node(nodes,
            // NOLINTNEXTLINE
            // NOLINTNEXTLINE
            5);

        left->left->right = Standard::Algorithms::Trees::set_value_get_node(nodes,
            // NOLINTNEXTLINE
            7);

        right->left = Standard::Algorithms::Trees::set_value_get_node(nodes,
            // NOLINTNEXTLINE
            25);

        right->right = Standard::Algorithms::Trees::set_value_get_node(nodes,
            // NOLINTNEXTLINE
            40);

        right->left->right = Standard::Algorithms::Trees::set_value_get_node(nodes,
            // NOLINTNEXTLINE
            28);
    }

    constexpr void build_result(std::vector<node_t> &nodes)
    {
        nodes.clear();

        const std::vector<int_t> values{// NOLINTNEXTLINE
            5, 7, 10, 20, 25, 28, 30, 40
        };

        nodes.resize(values.size());

        for (std::size_t index{}; index < values.size(); ++index)
        {
            auto &node = nodes[index];
            node.value = values[index];

            if (index != 0U)
            {
                node.left = &nodes[index - 1U];
            }

            if (index + 1U < values.size())
            {
                node.right = &nodes[index + 1U];
            }
        }
    }
} // namespace

void Standard::Algorithms::Trees::Tests::bst_to_doubly_linked_list_tests()
{
    std::vector<node_t> nodes{};
    build_not_empty_tree(nodes);
    ::Standard::Algorithms::ert::not_empty(nodes, "nodes");

    auto *root = nodes.data();

    {
        const auto actual =
            Standard::Algorithms::Trees::binary_search_tree_utilities<int_t, node_t>::is_bst_without_duplicates(root);

        ::Standard::Algorithms::ert::are_equal(true, actual, "Is first tree BST?");
    }

    std::vector<node_t> nodes2{};
    build_not_empty_tree(nodes2);
    ::Standard::Algorithms::ert::not_empty(nodes2, "nodes2");
    ::Standard::Algorithms::ert::are_equal(nodes, nodes2, "nodes and its clone");

    auto *root2 = nodes2.data();

    {
        const auto input_equal = Standard::Algorithms::Trees::are_binary_trees_isomorphic(root, root2);
        Standard::Algorithms::Trees::require_isomorphic(input_equal, "Input tree and its clone must be the same.");
    }

    Standard::Algorithms::Trees::binary_search_tree_utilities<int_t, node_t>::convert_bst_to_double_linked_list_slow(
        &root);

    std::vector<node_t> nodes_result{};
    build_result(nodes_result);
    ::Standard::Algorithms::ert::not_empty(nodes_result, "nodes_result");

    {
        const auto *root3 = &nodes_result.at(0);
        const auto output_equal = Standard::Algorithms::Trees::are_trees_isomorphic(root, root3);

        Standard::Algorithms::Trees::require_isomorphic(output_equal, "Expected tree must be equal to slow tree.");
    }

    Standard::Algorithms::Trees::binary_search_tree_utilities<int_t, node_t>::convert_bst_to_double_linked_list(&root2);

    {
        const auto output_equal = Standard::Algorithms::Trees::are_trees_isomorphic(root, root2);

        Standard::Algorithms::Trees::require_isomorphic(output_equal, "Slow tree must be equal to fast one.");
    }
}
