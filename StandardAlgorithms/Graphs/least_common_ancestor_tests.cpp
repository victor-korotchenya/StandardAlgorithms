#include"least_common_ancestor_tests.h"
#include"../Numbers/arithmetic.h"
#include"../Numbers/shift.h"
#include"../Utilities/project_constants.h"
#include"../Utilities/test_utilities.h"
#include"binary_tree_node.h"
#include"binary_tree_utilities.h"
#include"least_common_ancestor.h"

namespace
{
    using node_t = Standard::Algorithms::Trees::binary_tree_node<std::int64_t>;

    constexpr auto nodes_count = ::Standard::Algorithms::Utilities::graph_max_nodes<std::int8_t>;
    static_assert(1 < nodes_count);

    constexpr void set_null_child(node_t &parent, node_t *child, const bool is_right)
    {
        auto &ptr = is_right ? parent.right : parent.left;
        if (ptr == nullptr)
        {
            ptr = child;
            return;
        }

        const auto *const name = is_right ? "right" : "left";

        auto str = ::Standard::Algorithms::Utilities::w_stream();
        str << "Error: the " << name << " value (" << ptr->value << ") has already been set for the parent " << parent
            << ".";
        Standard::Algorithms::throw_exception(str);
    }

    // Zero based numbering.
    constexpr auto common_ancestor_for_complete_tree_slow(std::size_t node1, std::size_t node2) -> std::size_t
    {
        for (;;)
        {
            // if (0U == node1 || 0U == node2)
            //{
            //   return 0;
            // }

            if (node1 == node2)
            {
                return node1;
            }

            auto &nod = node2 < node1 ? node1 : node2;
            nod = Standard::Algorithms::Trees::binary_tree_utilities<>::parent_node(nod);
        }
    }

    // One base numbering - extra 1 allows easier calculations.
    constexpr auto common_ancestor_for_complete_tree_fast(std::size_t node1, std::size_t node2) -> std::size_t
    {
        if constexpr (::Standard::Algorithms::is_debug)
        {
            if (0U == node1 || 0U == node2) [[unlikely]]
            {
                auto str = ::Standard::Algorithms::Utilities::w_stream();
                str << "The nodes (" << node1 << ", " << node2 << ") must be positive (1 based).";
                throw std::runtime_error(str.str());
            }
        }

        // It is rare in a big tree.
        // if (1U == node1 || 1U == node2)
        //{
        //  return 1;
        //}

        const auto level1 = static_cast<std::int32_t>(Standard::Algorithms::Numbers::most_significant_bit(node1));
        const auto level2 = static_cast<std::int32_t>(Standard::Algorithms::Numbers::most_significant_bit(node2));

        const auto level_delta = std::abs(level1 - level2);

        // Equate the same level.
        if (level1 < level2)
        {
            node2 = ::Standard::Algorithms::Numbers::shift_right(node2, level_delta);
        }
        else if (level2 < level1)
        {
            node1 = ::Standard::Algorithms::Numbers::shift_right(node1, level_delta);
        }

        if (node1 == node2)
        {
            return node1;
        }

        const auto column_distance = Standard::Algorithms::Numbers::most_significant_bit(node1 ^ node2);

        auto result = node1 >> (1U + column_distance);

        if constexpr (::Standard::Algorithms::is_debug)
        {
            if (0U == result)
            {
                auto str = ::Standard::Algorithms::Utilities::w_stream();
                str << "Error in CommonAncestorForCompleteTreeFast:"
                    << " got zero result. node1 " << node1 << ", node2 " << node2 << ".";

                Standard::Algorithms::throw_exception(str);
            }
        }

        return result;
    }

    constexpr void create_tree(std::vector<node_t> &nodes)
    {
        const auto max_parent_index = Standard::Algorithms::Trees::binary_tree_utilities<>::parent_node(nodes_count - 1);

        for (std::size_t parent_index{}; parent_index <= max_parent_index; ++parent_index)
        {
            auto &parent = nodes.at(parent_index);
            {
                const auto child_index = Standard::Algorithms::Trees::binary_tree_utilities<>::left_child(parent_index);
                auto &child = nodes.at(child_index);

                set_null_child(parent, &child, false);
            }
            {
                const auto child_index = Standard::Algorithms::Trees::binary_tree_utilities<>::right_child(parent_index);
                auto &child = nodes.at(child_index);

                set_null_child(parent, &child, true);
            }
        }
    }

    constexpr void check_ancestors(const std::vector<node_t> &nodes)
    {
        ::Standard::Algorithms::ert::are_equal(static_cast<std::size_t>(nodes_count), nodes.size(), "nodes size");

        const auto *const root = &nodes.at(0);

        for (std::int32_t index1{}; index1 < nodes_count; ++index1)
        {
            const auto *const node1 = &nodes[index1];

            for (std::int32_t index2{}; index2 < nodes_count; ++index2)
            {
                const auto *const node2 = &nodes[index2];

                const auto *const actual =
                    Standard::Algorithms::Trees::least_common_ancestor_without_parent(root, node1, node2);

                if (nullptr == actual) [[unlikely]]
                {
                    auto str = ::Standard::Algorithms::Utilities::w_stream();
                    str << "Error in least_common_ancestor_tests:"
                        << " null returned for index1 " << index1 << ", index2 " << index2 << ".";
                    throw std::runtime_error(str.str());
                }

                const auto expected_index = common_ancestor_for_complete_tree_slow(index1, index2);

                const auto expected_index_fast = common_ancestor_for_complete_tree_fast(1 + index1, 1 + index2);

                if (expected_index + 1U != expected_index_fast) [[unlikely]]
                {
                    auto str = ::Standard::Algorithms::Utilities::w_stream();
                    str << "Error in CommonAncestorForCompleteTreeFast:"
                        << " expectedIndex (" << expected_index << ") != expectedIndexFast ("
                        << expected_index_fast - 1U << ") when index1 " << index1 << ", index2 " << index2 << ".";
                    throw std::runtime_error(str.str());
                }

                const auto *const expected = &nodes.at(expected_index);
                if (expected != actual) [[unlikely]]
                {
                    auto str = ::Standard::Algorithms::Utilities::w_stream();
                    str << "Error in least_common_ancestor_tests:"
                        << " expect " << expected_index << " but got " << actual->value << " when index1 " << index1
                        << ", index2 " << index2 << ".";
                    throw std::runtime_error(str.str());
                }
            }
        }
    }
} // namespace

void Standard::Algorithms::Trees::Tests::least_common_ancestor_tests()
{
    std::vector<node_t> nodes(nodes_count);

    for (std::int32_t index{}; index < nodes_count; ++index)
    {
        nodes[index].value = index;
    }

    create_tree(nodes);
    check_ancestors(nodes);
}
