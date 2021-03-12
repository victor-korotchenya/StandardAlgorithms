#include"next_inorder_successor_tests.h"
#include"../Utilities/test_utilities.h"
#include"binary_search_tree_utilities.h"
#include"binary_tree_node_with_parent.h"
#include"binary_tree_utilities.h"

namespace
{
    using node_t = Standard::Algorithms::Trees::binary_tree_node_with_parent<std::int64_t>;

    using int_t = node_t::value_type;

    constexpr auto nodes_count = 25;

    constexpr void set_left_with_parent(
        const std::int32_t parent, std::vector<node_t *> &nodes, const std::int32_t left)
    {
        Standard::Algorithms::require_non_negative(left, "left");
        Standard::Algorithms::require_less_equal(left, parent, "left vs parent");
        Standard::Algorithms::require_greater(nodes_count, parent, "size vs parent");

        const auto name = ::Standard::Algorithms::Utilities::zu_string(parent);
        auto *const parent_ptr = Standard::Algorithms::throw_if_null("nodes at " + name, nodes.at(parent));

        auto *&dest = parent_ptr->left;
        Standard::Algorithms::throw_if_not_null("left of parent " + name, dest);

        auto *const child_ptr = Standard::Algorithms::throw_if_null("nodes[left] of parent " + name, nodes.at(left));

        {
            const auto good = child_ptr->parent == nullptr || child_ptr->parent == parent_ptr;

            ::Standard::Algorithms::ert::are_equal(
                true, good, "parent " + name + " left is being set to a different value");
        }

        dest = child_ptr;
        child_ptr->parent = parent_ptr;
    }

    constexpr void set_right_with_parent(
        const std::int32_t parent, std::vector<node_t *> &nodes, const std::int32_t right)
    {
        Standard::Algorithms::require_non_negative(parent, "parent");
        Standard::Algorithms::require_less_equal(parent, right, "right vs parent");
        Standard::Algorithms::require_greater(nodes_count, right, "size vs right");

        const auto name = ::Standard::Algorithms::Utilities::zu_string(parent);
        auto *const parent_ptr = Standard::Algorithms::throw_if_null("nodes at " + name, nodes.at(parent));

        auto *&dest = parent_ptr->right;
        Standard::Algorithms::throw_if_not_null("right of parent " + name, dest);

        auto *const child_ptr = Standard::Algorithms::throw_if_null("nodes[right] of parent " + name, nodes.at(right));

        {
            const auto good = child_ptr->parent == nullptr || child_ptr->parent == parent_ptr;

            ::Standard::Algorithms::ert::are_equal(
                true, good, "parent " + name + " right is being set to a different value");
        }

        dest = child_ptr;
        child_ptr->parent = parent_ptr;
    }

    constexpr void create_tree(std::vector<node_t *> &nodes)
    {
        // NOLINTNEXTLINE
        set_left_with_parent(10, nodes, 5);
        // NOLINTNEXTLINE
        set_right_with_parent(10, nodes, 17);

        // NOLINTNEXTLINE
        set_left_with_parent(5, nodes, 3);
        // NOLINTNEXTLINE
        set_right_with_parent(5, nodes, 7);

        // NOLINTNEXTLINE
        set_left_with_parent(17, nodes, 14);
        // NOLINTNEXTLINE
        set_right_with_parent(17, nodes, 20);

        // NOLINTNEXTLINE
        set_left_with_parent(3, nodes, 1);
        // NOLINTNEXTLINE
        set_right_with_parent(3, nodes, 4);

        // NOLINTNEXTLINE
        set_left_with_parent(7, nodes, 6);
        // NOLINTNEXTLINE
        set_right_with_parent(7, nodes, 8);

        // NOLINTNEXTLINE
        set_left_with_parent(14, nodes, 12);
        // NOLINTNEXTLINE
        set_right_with_parent(14, nodes, 15);

        // NOLINTNEXTLINE
        set_left_with_parent(20, nodes, 19);
        // NOLINTNEXTLINE
        set_right_with_parent(20, nodes, 23);

        //                        10
        //            5                            17
        //      3           7            14                20
        //  1     4       6   8       12    15         19       23
        // 0   2                 9   11  13    16     18       22  24
        //                                                  21

        // NOLINTNEXTLINE
        set_left_with_parent(1, nodes, 0);
        // NOLINTNEXTLINE
        set_right_with_parent(1, nodes, 2);

        // NOLINTNEXTLINE
        set_right_with_parent(8, nodes, 9);

        // NOLINTNEXTLINE
        set_left_with_parent(12, nodes, 11);
        // NOLINTNEXTLINE
        set_right_with_parent(12, nodes, 13);

        // NOLINTNEXTLINE
        set_right_with_parent(15, nodes, 16);

        // NOLINTNEXTLINE
        set_left_with_parent(19, nodes, 18);

        // NOLINTNEXTLINE
        set_left_with_parent(23, nodes, 22);
        // NOLINTNEXTLINE
        set_right_with_parent(23, nodes, 24);

        // NOLINTNEXTLINE
        set_left_with_parent(22, nodes, 21);
    }

    constexpr auto expected_successor(const int_t &value, const int_t &last_node_value) -> int_t
    {
        constexpr int_t one = 1;

        auto result = static_cast<int_t>(value < last_node_value ? one + value : -one);

        return result;
    }

    constexpr void check_successors(const std::vector<node_t *> &nodes)
    {
        const auto size = nodes.size();
        ::Standard::Algorithms::ert::greater(size, 1U, "nodes size");

        const auto last_node_value = static_cast<int_t>(size - 1U);

        for (std::size_t index{}; index < size; ++index)
        {
            const auto expected = expected_successor(static_cast<int_t>(index), last_node_value);

            const auto *const node = Standard::Algorithms::throw_if_null("nodes[index]", nodes[index]);

            const auto *const successor =
                Standard::Algorithms::Trees::binary_search_tree_utilities<int_t, node_t>::next_in_order_successor(node);

            if (nullptr == successor)
            {
                ::Standard::Algorithms::ert::are_equal(
                    last_node_value, static_cast<int_t>(index), "null returned for node");
            }
            else
            {
                ::Standard::Algorithms::ert::are_equal(expected, successor->value,
                    "successor value for node index " + ::Standard::Algorithms::Utilities::zu_string(index));
            }
        }
    }
} // namespace

void Standard::Algorithms::Trees::Tests::next_inorder_successor_tests()
{
    std::vector<node_t> nodes(nodes_count);
    std::vector<node_t *> pointers(nodes_count);

    for (std::int32_t index{}; index < nodes_count; ++index)
    {
        auto &node = nodes[index];
        node.value = index;
        pointers[index] = &node;
    }

    create_tree(pointers);
    check_successors(pointers);
}
