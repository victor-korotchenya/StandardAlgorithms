#include"construct_from_distinct_numbers_tests.h"
#include"../Utilities/test_utilities.h"
#include"are_binary_trees_isomorphic.h"
#include"binary_search_tree_utilities.h"
#include"binary_tree_utilities.h"
#include"test_node_utilities.h"

namespace
{
    using int_t = std::int16_t;
    using node_t = Standard::Algorithms::Trees::binary_tree_node<int_t>;
    using unique_ptr_tree_t = std::unique_ptr<node_t, void (*)(node_t *)>;

    [[nodiscard]] auto create_empty_tree()
    {
        return unique_ptr_tree_t(nullptr, Standard::Algorithms::Trees::free_tree_33<node_t>);
    }

    [[nodiscard]] auto append_new_tree(const int_t value, std::vector<unique_ptr_tree_t> &trees) -> node_t *
    {
        constexpr auto ratio = 1.1;

        const auto new_size =
            static_cast<std::size_t>(static_cast<Standard::Algorithms::floating_point_type>(trees.size()) * ratio) + 1;
        trees.reserve(new_size);

        auto tree = create_empty_tree();
        {
            auto uniq = std::make_unique<node_t>();
            {
                auto *temp = uniq.get();
                temp->value = value;
                tree.reset(temp);
                // Now, the ownership has been transferred from uniq to tree.
            }

            // Ignore a false warning: the value returned by this function should be used [bugprone-unused-return-value]
            //
            // NOLINTNEXTLINE
            uniq.release();
        }

        auto *result = tree.get();
        trees.push_back(std::move(tree));

        return result;
    }

    inline constexpr void set_new(const int_t value, gsl::owner<node_t *> &dest)
    {
        assert(dest == nullptr);

        dest = new node_t{ value };
    }

    struct test_case final : Standard::Algorithms::Tests::base_test_case
    {
        constexpr test_case(
            std::string &&name, std::vector<int_t> &&data, std::vector<int_t> &&expected_parents, node_t *expected_tree)
            : base_test_case(std::move(name))
            , Expected_tree(Standard::Algorithms::throw_if_null("expectedTree", expected_tree))
            , Data(std::move(data))
            , Expected_parents(std::move(expected_parents))
        {
        }

        constexpr test_case(const test_case &) = default;

        auto operator= (const test_case &) & -> test_case & = delete;
        test_case(test_case &&) noexcept = default;
        auto operator= (test_case &&) &noexcept -> test_case & = delete;

        constexpr ~test_case() noexcept override = default;

        [[nodiscard]] constexpr auto expected_tree() const &noexcept -> const node_t *
        {
            return Expected_tree;
        }

        [[nodiscard]] constexpr auto data() const &noexcept -> const std::vector<int_t> &
        {
            return Data;
        }

        [[nodiscard]] constexpr auto expected_parents() const &noexcept -> const std::vector<int_t> &
        {
            return Expected_parents;
        }

        constexpr void validate() const override
        {
            base_test_case::validate();

            Standard::Algorithms::throw_if_null("Expected tree", Expected_tree);

            ::Standard::Algorithms::ert::are_equal(Data.size(), Expected_parents.size() + 1U, "Data.size");
        }

        void print(std::ostream &str) const override
        {
            ::Standard::Algorithms::print(", Data ", Data, str);
            ::Standard::Algorithms::print(", Expected parents ", Expected_parents, str);
        }

private:
        const node_t *const Expected_tree;
        std::vector<int_t> Data;
        std::vector<int_t> Expected_parents;
    };

    void generate_test_cases(std::vector<test_case> &test_cases, std::vector<unique_ptr_tree_t> &trees)
    {
        for (int_t value{}; value <= 1; ++value)
        {
            test_cases.emplace_back("Simple" + std::to_string(value), std::initializer_list<int_t>({ value }),
                std::initializer_list<int_t>(), append_new_tree(value, trees));
        }

        {
            auto *root = append_new_tree(2, trees);
            set_new(1, root->left);
            set_new(4, root->right);
            {
                constexpr auto val5 = 5;
                constexpr auto val6 = 6;

                set_new(val6, root->right->right);
                set_new(val5, root->right->right->left);
            }

            test_cases.emplace_back("Few nodes",
                // NOLINTNEXTLINE
                std::initializer_list<int_t>({ 2, 4, 1, 6, 5 }),
                // NOLINTNEXTLINE
                std::initializer_list<int_t>({ 2, 2, 4, 6 }), root);
        }
    }

    void run_test_case(const test_case &test)
    {
        auto tree = create_empty_tree();

        const auto parents =
            Standard::Algorithms::Trees::binary_search_tree_utilities<int_t>::construct_from_distinct_numbers(
                test.data(), tree);

        ::Standard::Algorithms::ert::are_equal(test.expected_parents(), parents, "Parents");

        const auto actual = Standard::Algorithms::Trees::are_binary_trees_isomorphic(test.expected_tree(), tree.get());
        require_isomorphic(actual, "Tree");
    }
} // namespace

void Standard::Algorithms::Trees::Tests::construct_from_distinct_numbers_tests()
{
    std::vector<unique_ptr_tree_t> trees;

    Standard::Algorithms::Tests::test_utilities<test_case, std::vector<unique_ptr_tree_t> &>::run_tests(
        run_test_case, generate_test_cases, trees);
}
