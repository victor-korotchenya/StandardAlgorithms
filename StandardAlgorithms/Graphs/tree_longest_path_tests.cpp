#include"tree_longest_path_tests.h"
#include"../Utilities/test_utilities.h"
#include"tree_longest_path.h"

namespace
{
    template<class t>
    constexpr auto min_max_or_default(const std::vector<t> &vec) noexcept -> std::pair<t, t>
    {
        if (vec.empty())
        {
            return {};
        }

        const auto [min_it, max_it] = std::minmax_element(vec.cbegin(), vec.cend());

        return { *min_it, *max_it };
    }

    constexpr void test_tree(const std::vector<std::vector<std::int32_t>> &tree)
    {
        try
        {
            const auto slow = Standard::Algorithms::Trees::tree_longest_path_slow(tree);
            {
                const auto fast = Standard::Algorithms::Trees::tree_longest_path(tree);
                ::Standard::Algorithms::ert::are_equal(slow, fast, "tree_longest_path");
            }

            const auto min_max = min_max_or_default(slow);
            const auto expected_diameter = min_max.second;
            {
                const auto diameter = Standard::Algorithms::Trees::tree_diameter(tree);
                ::Standard::Algorithms::ert::are_equal(expected_diameter, diameter, "tree_diameter");
            }

            const auto expected_radius = min_max.first;
            const auto radius = (expected_diameter + 1) / 2;
            ::Standard::Algorithms::ert::are_equal(expected_radius, radius, "radius");
        }
        catch (const std::exception &exc)
        {
            auto str = ::Standard::Algorithms::Utilities::w_stream();
            ::Standard::Algorithms::print("tree", tree, str);

            str << "\nError: " << exc.what();

            throw std::runtime_error(str.str());
        }
    }

    constexpr void tree_longest_path_simple()
    {
        const std::vector<std::vector<std::int32_t>> tree{ { 1, 2, 3 }, { 0 }, { 0 }, { 0, 4 }, { 3 } };

        {
            const std::vector<std::int32_t> expected = { 2, 3, 3, 2, 3 };

            const auto actual = Standard::Algorithms::Trees::tree_longest_path(tree);
            ::Standard::Algorithms::ert::are_equal(expected, actual, "tree_longest_path");
        }

        test_tree(tree);
    }

    void tree_longest_path_random()
    {
        constexpr auto min_size = 1;
        constexpr auto max_size = 20;

        const auto size = Standard::Algorithms::Utilities::random_t<std::int32_t>()(min_size, max_size);

        const auto tree = Standard::Algorithms::Trees::build_random_tree<std::int32_t>(size);

        ::Standard::Algorithms::ert::are_equal(static_cast<std::size_t>(size), tree.size(), "tree size");

        test_tree(tree);
    }
} // namespace

void Standard::Algorithms::Trees::Tests::tree_longest_path_tests()
{
    constexpr auto max_attempts = 1;

    for (std::int32_t att{}; att < max_attempts; ++att)
    {
        tree_longest_path_random();
    }

    tree_longest_path_simple();
}
