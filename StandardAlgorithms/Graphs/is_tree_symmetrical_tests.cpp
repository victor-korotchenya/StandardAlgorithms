#include"is_tree_symmetrical_tests.h"
#include"../Utilities/test_utilities.h"
#include"binary_tree_node.h"
#include"is_tree_symmetrical.h"

namespace
{
    using int_t = std::uint32_t;

    using node_t = Standard::Algorithms::Trees::binary_tree_node<int_t>;

    struct test_case final : Standard::Algorithms::Tests::base_test_case
    {
        constexpr test_case(std::string &&name, const node_t *root, bool expected = {}) noexcept
            : base_test_case(std::move(name))
            , Root(root)
            , Expected(expected)
        {
        }

        constexpr test_case(const test_case &) = default;

        auto operator= (const test_case &) & -> test_case & = delete;
        test_case(test_case &&) noexcept = default;
        auto operator= (test_case &&) &noexcept -> test_case & = delete;

        constexpr ~test_case() noexcept override = default;

        [[nodiscard]] constexpr auto root() const &noexcept -> const node_t *
        {
            return Root;
        }

        [[nodiscard]] constexpr auto expected() const &noexcept -> bool
        {
            return Expected;
        }

        void print(std::ostream &str) const override
        {
            str << " Expected " << Expected;
        }

private:
        const node_t *Root{};
        bool Expected{};
    };

    constexpr void generate_test_cases(std::vector<test_case> &test_cases, std::vector<node_t> &nodes)
    {
        // Must pre-allocate to have constant pointers.
        constexpr std::size_t size = 20;
        nodes.assign(size, node_t{});

        std::uint32_t index{};
        {
            constexpr auto node_count = 1;
            auto *root = &nodes.at(index);
            test_cases.emplace_back("Trivial 1 node", root, true);

            index += node_count;
        }
        {
            constexpr auto node_count = 2;
            auto *root = &nodes.at(index);
            test_cases.emplace_back("Not symmetric, 2 nodes", root);

            root->left = &nodes.at(index + 1LLU);

            index += node_count;
        }
        {
            constexpr auto node_count = 5;
            auto *root = &nodes.at(index);
            test_cases.emplace_back("Symmetric, 5 nodes", root, true);

            root->left = &nodes.at(index + 1LLU);
            root->right = &nodes.at(index + 2LLU);
            root->left->right = &nodes.at(index + 3LLU);
            root->right->left = &nodes.at(index + 4LLU);

            root->value = index;
            root->left->value = root->right->value = index + 2;
            root->left->right->value = root->right->left->value = index + 3;

            index += node_count;
        }
        {
            constexpr auto node_count = 5;
            auto *root = &nodes.at(index);
            test_cases.emplace_back("Not symmetric, 5 nodes", root);

            root->left = &nodes.at(index + 1LLU);
            root->right = &nodes.at(index + 2LLU);
            root->left->right = &nodes.at(index + 3LLU);
            root->right->left = &nodes.at(index + 4LLU);

            root->value = index;
            root->left->value = root->right->value = index + 2;
            root->left->right->value = index + 3;
            root->right->left->value = index + 2;

            index += node_count;
        }

        Standard::Algorithms::require_less_equal(index, size, "Increase the node vector size");
    }

    constexpr void run_test_case(const test_case &test)
    {
        const auto actual = Standard::Algorithms::Trees::is_tree_symmetrical<node_t>(test.root());

        ::Standard::Algorithms::ert::are_equal(test.expected(), actual, "is_tree_symmetrical");
    }
} // namespace

void Standard::Algorithms::Trees::Tests::is_tree_symmetrical_tests()
{
    std::vector<node_t> nodes;

    Standard::Algorithms::Tests::test_utilities<test_case, std::vector<node_t> &>::run_tests(
        run_test_case, generate_test_cases, std::ref(nodes));
}
