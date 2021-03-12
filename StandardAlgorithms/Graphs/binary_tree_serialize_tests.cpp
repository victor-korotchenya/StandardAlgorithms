#include"binary_tree_serialize_tests.h"
#include"are_binary_trees_isomorphic.h"
#include"binary_tree_node.h"
#include"binary_tree_serialize.h"
#include"test_node_utilities.h"

namespace
{
    using key_t = std::uint16_t;
    using node_t = Standard::Algorithms::Trees::binary_tree_node<key_t>;

    constexpr auto vertex_size = 5U;

    constexpr auto build_nodes() -> std::vector<node_t>
    {
        std::vector<node_t> nodes(vertex_size);

        for (std::size_t index{}; index < vertex_size; ++index)
        {
            nodes[index].value = static_cast<key_t>(index);
        }

        return nodes;
    }

    constexpr void serialize_check(const std::string &name, const std::vector<bool> &expected_storage,
        const node_t *root, std::vector<bool> &storage)
    {
        assert(!name.empty());

        storage.clear();
        ::Standard::Algorithms::Trees::binary_tree_serialize<node_t>(root, storage);

        ::Standard::Algorithms::ert::are_equal(expected_storage, storage, name);
    }

    constexpr auto last_file_contents()
    {
        return std::vector<bool>{ true, true, false, false, true, false, true, false };
    }

    constexpr void gradually_serialize(std::vector<node_t> &nodes)
    {
        const auto &root = nodes.at(0);
        std::vector<bool> storage;

        {
            constexpr auto bits = 0U;
            serialize_check("predefined serialize nullptr", std::vector<bool>(bits), nullptr, storage);
        }
        {
            constexpr auto bits = 8U;
            serialize_check("predefined serialize 0", std::vector<bool>(bits), &root, storage);
        }

        auto &a_node = nodes.at(0);
        auto &b_node = nodes.at(1);
        auto &c_node = nodes.at(2);
        auto &d_node = nodes.at(3);
        auto &e_node = nodes.at(4);
        //        A
        //      /
        //    B
        //   /  \;
        // C      D
        //           \;
        //             E
        a_node.left = &b_node;
        serialize_check(
            "predefined serialize 1", { true, false, false, false, false, false, false, false }, &root, storage);

        b_node.left = &c_node;
        serialize_check(
            "predefined serialize 2", { true, true, false, false, false, false, false, false }, &root, storage);

        b_node.right = &d_node;
        serialize_check(
            "predefined serialize 3", { true, true, false, false, true, false, false, false }, &root, storage);

        d_node.right = &e_node;
        serialize_check("predefined serialize 4", last_file_contents(), &root, storage);
    }

    constexpr void deserialize_test(const std::vector<node_t> &original_nodes, std::vector<node_t> &nodes)
    {
        assert(&original_nodes != &nodes);

        ::Standard::Algorithms::ert::are_equal(vertex_size, original_nodes.size(), "original nodes size");

        std::size_t index{};

        auto build_node = [&nodes, &index]() -> node_t *
        {
            if (!(index < nodes.size())) [[unlikely]]
            {
                throw std::runtime_error("All nodes have been used in deserialize test.");
            }

            auto &node = nodes.at(index);
            ++index;
            return &node;
        };

        using build_node_t = decltype(build_node);

        const auto storage = last_file_contents();
        const auto *actual_root =
            ::Standard::Algorithms::Trees::binary_tree_deserialize<node_t, build_node_t>(storage, build_node);

        ::Standard::Algorithms::ert::are_equal(vertex_size, index, "deserialized nodes count");
        ::Standard::Algorithms::ert::is_not_null_ptr(actual_root, "deserialized root node");

        if (const auto &nodes_front = nodes.at(0); &nodes_front != actual_root) [[unlikely]]
        {
            throw std::runtime_error("The deserialized root node is not at the front.");
        }

        const auto &root = original_nodes.at(0);
        const auto actual = Standard::Algorithms::Trees::are_binary_trees_isomorphic(&root, actual_root);
        ::Standard::Algorithms::Trees::require_isomorphic(actual, "deserialize test");
    }

    constexpr void predefined_test()
    {
        auto original_nodes = build_nodes();
        auto nodes = original_nodes;
        gradually_serialize(original_nodes);

        deserialize_test(original_nodes, nodes);
    }
} // namespace

void Standard::Algorithms::Trees::Tests::binary_tree_serialize_tests()
{
    predefined_test();
}
