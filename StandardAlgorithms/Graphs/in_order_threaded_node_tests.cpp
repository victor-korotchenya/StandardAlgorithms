#include"in_order_threaded_node_tests.h"
#include"../Utilities/ert.h"
#include"binary_tree_utilities.h"
#include"in_order_threaded_node.h"
#include<vector>

namespace
{
    using int_t = std::int16_t;
    using node_t = Standard::Algorithms::Trees::in_order_threaded_node<int_t>;

    constexpr void build_binary_tree(
        const std::size_t size, std::vector<node_t> &nodes, std::vector<std::int32_t> &inorder_traversal)
    {
        nodes.assign(size, node_t());

        for (std::size_t index{}; index < size; ++index)
        {
            nodes[index].datum() = static_cast<int_t>(index);

            const auto left_index = Standard::Algorithms::Trees::binary_tree_utilities<>::left_child(index);

            if (left_index < size)
            {
                nodes[index].set_left(&nodes[left_index]);

                const auto right_index = Standard::Algorithms::Trees::binary_tree_utilities<>::right_child(index);

                if (right_index < size)
                {
                    nodes[index].set_right(&nodes[right_index]);
                }
            }
        }

        //       0
        //    1       2
        //  3   4    5 6
        // 7 8 9 10
        inorder_traversal.clear();
        inorder_traversal.reserve(size);
        inorder_traversal.insert(inorder_traversal.begin(), // NOLINTNEXTLINE
            { 7, 3, 8, 1, 9, 4, 0, 5, 2, 6 });

        ::Standard::Algorithms::ert::are_equal(
            static_cast<std::size_t>(size), inorder_traversal.size(), "Fix the In-order traversal.");
    }

    constexpr void check_threaded_binary_tree(
        const std::size_t size, const std::vector<node_t> &nodes, const std::vector<std::int32_t> &inorder_traversal)
    {
        constexpr std::int32_t absent_index = -1;
        auto previous_index = absent_index;

        for (std::size_t index{}; index < size; ++index)
        {
            {
                const auto casti = static_cast<int_t>(index);
                ::Standard::Algorithms::ert::are_equal(casti, nodes[index].datum(), "Datum after construction.");
            }

            const auto index_as_string = std::to_string(index);
            {// Left
                const auto child_index = Standard::Algorithms::Trees::binary_tree_utilities<>::left_child(index);
                const auto *expected = child_index < size ? &nodes[child_index] : nullptr;

                const auto *actual = nodes[index].get_left();
                const auto error = index_as_string + "->left.";
                ::Standard::Algorithms::ert::are_equal_by_ptr<node_t>(expected, actual, error);
            }
            {// Right
                const auto child_index = Standard::Algorithms::Trees::binary_tree_utilities<>::right_child(index);
                const auto *expected = child_index < size ? &nodes[child_index] : nullptr;

                const auto *actual = nodes[index].get_right();
                const auto error = index_as_string + "->right.";
                ::Standard::Algorithms::ert::are_equal_by_ptr<node_t>(expected, actual, error);
            }

            const auto current_index = inorder_traversal[index];
            const auto current_index_as_string = std::to_string(current_index);
            {
                const auto *expected = absent_index == previous_index || nullptr != nodes[current_index].get_left()
                    ? nullptr
                    : &nodes[previous_index];

                const auto *actual = nodes[current_index].predecessor();
                const auto error = current_index_as_string + "->predecessor.";
                ::Standard::Algorithms::ert::are_equal_by_ptr<node_t>(expected, actual, error);

                previous_index = current_index;
            }
            {
                const auto *expected = (index + 1U) < size && nullptr == nodes[current_index].get_right()
                    ? &nodes[inorder_traversal[index + 1U]]
                    : nullptr;

                const auto *actual = nodes[current_index].successor();
                const auto error = current_index_as_string + "->successor.";
                ::Standard::Algorithms::ert::are_equal_by_ptr<node_t>(expected, actual, error);
            }
        }
    }

    constexpr void visit(
        const node_t *const previous, const node_t *const current, std::vector<std::int32_t> &threaded_visited)
    {
        if (nullptr == current) [[unlikely]]
        {
            throw std::runtime_error("nullptr == current in in_order_threaded_node_tests::visit.");
        }

        threaded_visited.push_back(current->datum());

        if (nullptr != previous)
        {
            if (nullptr == previous->get_right())
            {
                const auto *const next_expected = previous->successor();
                ::Standard::Algorithms::ert::are_equal_by_ptr(next_expected, current, "successor in visit test.");
            }
        }

        if (nullptr == current->get_left())
        {
            const auto *const previous_expected = current->predecessor();
            ::Standard::Algorithms::ert::are_equal_by_ptr(previous_expected, previous, "predecessor in visit test.");
        }
    }

    constexpr void test_visits(const std::size_t size, const std::vector<node_t> &nodes,
        const std::vector<std::int32_t> &inorder_traversal, std::vector<std::int32_t> &threaded_visited)
    {
        threaded_visited.clear();

        ::Standard::Algorithms::ert::are_equal(size, inorder_traversal.size(), "inorderTraversal.size()");

        const auto &root = nodes.at(0);
        root.in_order_visit_slow(visit, std::ref(threaded_visited));
        ::Standard::Algorithms::ert::are_equal(inorder_traversal, threaded_visited, "in_order_visit_slow.");

        threaded_visited.clear();
        root.in_order_visit_fast(visit, std::ref(threaded_visited));

        ::Standard::Algorithms::ert::are_equal(inorder_traversal, threaded_visited, "in_order_visit_fast.");
    }
} // namespace

void Standard::Algorithms::Trees::Tests::in_order_threaded_node_tests()
{
    constexpr auto size = 10U;
    std::vector<node_t> nodes;
    std::vector<std::int32_t> inorder_traversal;
    std::vector<std::int32_t> threaded_visited;

    build_binary_tree(size, nodes, inorder_traversal);

    auto &root = nodes.at(0);
    root.in_order_visit_and_set_tags();

    check_threaded_binary_tree(size, nodes, inorder_traversal);

    test_visits(size, nodes, inorder_traversal, threaded_visited);
}
