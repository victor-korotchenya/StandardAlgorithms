#include"centroid_decomposition_tests.h"
#include"../Utilities/test_utilities.h"
#include"centroid_decomposition.h"

namespace
{
    using int_t = std::int32_t;

    struct test_case : Standard::Algorithms::Tests::base_test_case
    {
        constexpr test_case(std::string &&name, std::vector<std::vector<int_t>> &&tree,
            std::vector<std::vector<std::int32_t>> &&expected_centroid_tree, std::int32_t root) noexcept
            : base_test_case(std::move(name))
            , Tree(std::move(tree))
            , Expected_centroid_tree(std::move(expected_centroid_tree))
            , Root(root)
        {
        }

        [[nodiscard]] constexpr auto tree() const &noexcept -> const std::vector<std::vector<int_t>> &
        {
            return Tree;
        }

        [[nodiscard]] constexpr auto expected_centroid_tree() const &noexcept
            -> const std::vector<std::vector<std::int32_t>> &
        {
            return Expected_centroid_tree;
        }

        [[nodiscard]] constexpr auto root() const noexcept -> std::int32_t
        {
            return Root;
        }

        constexpr void validate() const override
        {
            base_test_case::validate();

            const auto size = Standard::Algorithms::require_positive(static_cast<std::int32_t>(Tree.size()), "tree size");

            {
                const auto istr = Standard::Algorithms::Trees::is_tree(Tree);
                ::Standard::Algorithms::ert::are_equal(true, istr, "is tree");
            }

            ::Standard::Algorithms::ert::are_equal(
                size, static_cast<std::int32_t>(Expected_centroid_tree.size()), "expected centroid tree size");

            {
                const auto isr = Standard::Algorithms::Trees::is_tree(Expected_centroid_tree);
                ::Standard::Algorithms::ert::are_equal(true, isr, "is expected centroid tree");
            }

            ::Standard::Algorithms::ert::greater_or_equal(Root, 0, "root");
            ::Standard::Algorithms::ert::greater(size, Root, "root");
        }

        void print(std::ostream &str) const override
        {
            ::Standard::Algorithms::print("tree", Tree, str);
            ::Standard::Algorithms::print("expected centroid tree", Expected_centroid_tree, str);
            ::Standard::Algorithms::print_value("root", str, Root);
        }

private:
        std::vector<std::vector<int_t>> Tree;
        std::vector<std::vector<std::int32_t>> Expected_centroid_tree;
        std::int32_t Root;
    };

    constexpr void generate_test_cases(std::vector<test_case> &tests)
    {
        tests.push_back({ "One", { {} }, { {} }, 0 });

        tests.push_back({ "Edge", { { 1 }, { 0 } }, { { 1 }, { 0 } }, 0 });

        tests.push_back({ "three_0", { { 1, 2 }, { 0 }, { 0 } }, { { 1, 2 }, { 0 }, { 0 } }, 0 });

        tests.push_back({ "three_1", { { 1 }, { 0, 2 }, { 1 } }, { { 1 }, { 0, 2 }, { 1 } }, 1 });

        tests.push_back({ "three_2", { { 2 }, { 2 }, { 0, 1 } }, { { 2 }, { 2 }, { 0, 1 } }, 2 });

        tests.push_back({ "path7",
            // NOLINTNEXTLINE
            { { 1 }, { 0, 2 }, { 1, 3 }, { 2, 4 }, // 3
                                                   // NOLINTNEXTLINE
                { 3, 5 }, { 4, 6 }, { 5 } },
            // NOLINTNEXTLINE
            { { 1 }, { 0, 2, 3 }, { 1 }, { 1, 5 }, // 3
                                                   // NOLINTNEXTLINE
                { 5 }, { 4, 6, 3 }, { 5 } },
            3 });

        tests.push_back({ "path8",
            { { 1 }, { 0, 2 }, { 1, 3 }, { 2, 4 }, // 3
                                                   // NOLINTNEXTLINE
                { 3, 5 }, { 4, 6 }, { 5, 7 }, { 6 } },
            // NOLINTNEXTLINE
            { { 1 }, { 0, 2, 3 }, { 1 }, { 1, 5 }, // 3
                                                   // NOLINTNEXTLINE
                { 5 }, { 4, 6, 3 }, { 7, 5 }, { 6 } },
            3 });

        tests.push_back({ "base_8",
            // NOLINTNEXTLINE
            { { 1 }, { 0, 2, 3 }, { 1, 4, 5, 6 }, { 1 }, // 3
                                                         // NOLINTNEXTLINE
                { 2, 7 }, { 2 }, { 2 }, { 4 } },
            /* 0 - 1 - 3
                   |
                   2
                 / | \
           7 - 4 - 5 - 6
            */
            // NOLINTNEXTLINE
            { { 1 }, { 0, 3, 2 }, { 1, 4, 5, 6 }, { 1 }, // 3
                                                         // NOLINTNEXTLINE
                { 7, 2 }, { 2 }, { 2 }, { 4 } },
            2 });
    }

    constexpr void run_test_case(const test_case &test)
    {
        Standard::Algorithms::Trees::centroid_decomposition<int_t> dec(test.tree());

        const auto &centroid_tree = dec.centroid_tree();
        ::Standard::Algorithms::ert::are_equal(test.expected_centroid_tree(), centroid_tree, "centroid tree");

        const auto root = dec.root();
        ::Standard::Algorithms::ert::are_equal(test.root(), root, "root");
    }
} // namespace

void Standard::Algorithms::Trees::Tests::centroid_decomposition_tests()
{
    Standard::Algorithms::Tests::test_utilities<test_case>::run_tests(run_test_case, generate_test_cases);
}
