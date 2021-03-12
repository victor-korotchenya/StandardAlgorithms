#include"segment_tree_add_bool_tests.h"
#include"../Utilities/test_utilities.h"
#include"segment_tree_add_bool.h"

namespace
{
    using int_t = std::uint32_t;

    using tree_t = Standard::Algorithms::Trees::segment_tree_add_bool<int_t>;

    using tree_t_slow = Standard::Algorithms::Trees::segment_tree_add_bool_slow<int_t>;
} // namespace

void Standard::Algorithms::Trees::Tests::segment_tree_add_bool_tests()
{
    constexpr int_t size = 4;

    tree_t tree(std::vector<int_t>(size, 1U));
    tree_t_slow tree_slow(std::vector<int_t>(size, 1U));

    const auto *const prefix = " at ";

    for (int_t index{};; ++index)
    {
        const auto name = prefix + std::to_string(index);

        {
            const auto expected = index + (0U < index ? 1U : 0U);

            {
                const auto actual = tree.get(0U, size);
                ::Standard::Algorithms::ert::are_equal(expected, actual, "get" + name);
            }
            {
                const auto actual = tree_slow.get(0U, tree_slow.size());
                ::Standard::Algorithms::ert::are_equal(expected, actual, "slow get" + name);
            }
        }

        if (index == size - 1U)
        {
            break;
        }

        {
            const auto expected = 1U + (index == 0U ? 1U : 0U);
            const auto rex = index + 2U;

            {
                const auto actual = tree.add(index, rex);
                ::Standard::Algorithms::ert::are_equal(expected, actual, "add" + name);
            }
            {
                const auto actual = tree_slow.add(index, rex);
                ::Standard::Algorithms::ert::are_equal(expected, actual, "slow add" + name);
            }
        }
    }

    for (int_t index{};; ++index)
    {
        const auto name = prefix + std::to_string(index);
        {
            const auto expected = size - index - (index == size - 1U ? 1U : 0U);

            {
                const auto actual = tree.get(0U, size);
                ::Standard::Algorithms::ert::are_equal(expected, actual, "rem get" + name);
            }
            {
                const auto actual = tree_slow.get(0U, size);
                ::Standard::Algorithms::ert::are_equal(expected, actual, "slow rem get" + name);
            }
        }

        if (index == size - 1U)
        {
            break;
        }

        {
            const auto expected = 1U + (index == size - 2U ? 1U : 0U);
            const auto rex = index + 2U;

            {
                const auto actual = tree.remove(index, rex);
                ::Standard::Algorithms::ert::are_equal(expected, actual, "remove" + name);
            }
            {
                const auto actual = tree_slow.remove(index, rex);
                ::Standard::Algorithms::ert::are_equal(expected, actual, "slow remove" + name);
            }
        }
    }
}
