#include"largest_bst_size_tests.h"
#include"../Numbers/arithmetic.h"
#include"../Utilities/test_utilities.h"
#include"binary_search_tree_utilities.h"
#include"binary_tree_node.h"

namespace
{
    using int_t = std::int16_t;
    using node_t = Standard::Algorithms::Trees::binary_tree_node<int_t>;
    using size_index_t = std::pair<std::size_t, std::int32_t>;

    constexpr auto max_allowed_size = 100;

    struct test_case final : Standard::Algorithms::Tests::base_test_case
    {
        constexpr test_case(std::string &&name, std::vector<node_t> &nodes, const node_t *root, const std::size_t siz,
            const std::int32_t index)
            : Standard::Algorithms::Tests::base_test_case(std::move(name))
            , Nodes(nodes)
            , Root(root)
            , Expected({ siz, index })
        {
        }

        constexpr test_case(const test_case &) = default;

        auto operator= (const test_case &) & -> test_case & = delete;
        test_case(test_case &&) noexcept = default;
        auto operator= (test_case &&) &noexcept -> test_case & = delete;

        constexpr ~test_case() noexcept override = default;

        [[nodiscard]] constexpr auto nodes() const &noexcept -> const std::vector<node_t> &
        {
            return Nodes;
        }

        [[nodiscard]] constexpr auto root() const &noexcept -> const node_t *
        {
            return Root;
        }

        [[nodiscard]] constexpr auto expected() const &noexcept -> const size_index_t &
        {
            return Expected;
        }

        void print(std::ostream &str) const override
        {
            ::Standard::Algorithms::print(", Expected ", Expected, str);
        }

private:
        std::vector<node_t> &Nodes;
        const node_t *Root;
        size_index_t Expected;
    };

    constexpr void increase_index(std::int32_t &index, const std::int32_t number_of_used_nodes)
    {
        Standard::Algorithms::require_positive(number_of_used_nodes, "numberOfUsedNodes");

        index = Standard::Algorithms::Numbers::add_signed(index, number_of_used_nodes);

        Standard::Algorithms::require_greater(max_allowed_size, index, "MaxAllowedSize");
    }

    constexpr void generate_test_cases(std::vector<test_case> &test_cases, std::vector<node_t> &nodes)
    {
        nodes.assign(max_allowed_size, node_t{});

        std::int32_t index{};

        {
            constexpr auto node_count = 8;
            auto *const root = &nodes.at(index);
            test_cases.emplace_back("8 nodes, root", nodes, root, node_count, index);
            //             2
            //        -10        345
            //   -25     1     10  509
            // null -15
            auto *left_node = &nodes.at(index + 1);
            auto *right_node = &nodes.at(index + 2);
            root->left = left_node;
            root->right = right_node;
            root->value = 2;

            // NOLINTNEXTLINE
            left_node->value = -10;
            // NOLINTNEXTLINE
            right_node->value = 345;

            left_node->left = &nodes.at(index + 3);
            left_node->right = &nodes.at(index + 4);

            // NOLINTNEXTLINE
            left_node->left->value = -25;
            left_node->right->value = 1;

            // NOLINTNEXTLINE
            right_node->left = &nodes.at(index + 5);
            // NOLINTNEXTLINE
            right_node->right = &nodes.at(index + 6);

            // NOLINTNEXTLINE
            right_node->left->value = 10;
            // NOLINTNEXTLINE
            right_node->right->value = 509;

            // NOLINTNEXTLINE
            left_node->left->right = &nodes.at(index + 7);
            // NOLINTNEXTLINE
            left_node->left->right->value = -15;

            increase_index(index, node_count);
        }
        {
            constexpr auto node_count = 6;
            auto *const root = &nodes.at(index);
            test_cases.emplace_back("Root is not BST", nodes, root, 3, index + 1);
            //         2
            //    -10      45
            // -25   7   -1
            auto *left_node = root->left = &nodes.at(index + 1);
            auto *right_node = root->right = &nodes.at(index + 2);
            root->value = 2;

            // NOLINTNEXTLINE
            left_node->value = -10;
            // NOLINTNEXTLINE
            right_node->value = 45;

            left_node->left = &nodes.at(index + 3);
            left_node->right = &nodes.at(index + 4);

            // NOLINTNEXTLINE
            left_node->left->value = -25;
            // NOLINTNEXTLINE
            left_node->right->value = root->value + 5;

            // NOLINTNEXTLINE
            right_node->left = &nodes.at(index + 5);
            right_node->left->value = static_cast<int_t>(root->value - 3);

            increase_index(index, node_count);
        }
    }

    constexpr auto convert_to_size_index(
        const std::vector<node_t> &nodes, const std::pair<std::size_t, const node_t *> &value) -> size_index_t
    {
        std::int32_t index{};

        for (const node_t &node : nodes)
        {
            if (&node == value.second)
            {
                break;
            }

            ++index;
        }

        if (nodes.size() == static_cast<std::size_t>(index))
        {
            index = -1;
        }

        return { value.first, index };
    }

    constexpr void run_test_case(const test_case &test)
    {
        const auto actual_raw =
            Standard::Algorithms::Trees::binary_search_tree_utilities<int_t, node_t>::largest_binary_search_tree_size(
                test.root());

        const auto actual = convert_to_size_index(test.nodes(), actual_raw);
        ::Standard::Algorithms::ert::are_equal(test.expected(), actual, "largest_binary_search_tree_size");
    }
} // namespace

void Standard::Algorithms::Trees::Tests::largest_bst_size_tests()
{
    std::vector<node_t> nodes;

    Standard::Algorithms::Tests::test_utilities<test_case, std::vector<node_t> &>::run_tests(
        run_test_case, generate_test_cases, std::ref(nodes));
}
