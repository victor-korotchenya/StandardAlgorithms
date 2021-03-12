#include"disjoint_set_tests.h"
#include"../Utilities/ert.h"
#include"disjoint_set.h"
#include<initializer_list>

namespace
{
    using int_t = std::uint32_t;

    constexpr int_t node0{};
    constexpr int_t node1 = 1;
    constexpr int_t node2 = 2;
    constexpr int_t node3 = 3;
    constexpr int_t node4 = 4;
    constexpr int_t node5 = 5;
    constexpr int_t node6 = 6;
    constexpr int_t node7 = 7;
    constexpr int_t node8 = 8;
    constexpr int_t size = 9;

    constexpr void test_are_connected(const int_t &one, const Standard::Algorithms::Numbers::disjoint_set<int_t> &dsu,
        const int_t &two, const bool expected, const char *const message)
    {
        assert(message != nullptr);

        const auto actual = dsu.are_connected(one, two);

        const auto name = "are_connected (" + ::Standard::Algorithms::Utilities::zu_string(one) + ", " +
            ::Standard::Algorithms::Utilities::zu_string(two) + ") " + message;

        ::Standard::Algorithms::ert::are_equal(expected, actual, name);

        if (one == two)
        {
            ::Standard::Algorithms::ert::are_equal(true, actual, name);
            return;
        }

        const auto name_2 = "are_connected (" + ::Standard::Algorithms::Utilities::zu_string(two) + ", " +
            ::Standard::Algorithms::Utilities::zu_string(one) + ") " + message;

        const auto &one_rev = two; // Pacify the compiler.
        const auto &two_rev = one;

        const auto actual_2 = dsu.are_connected(one_rev, two_rev);

        ::Standard::Algorithms::ert::are_equal(expected, actual_2, name_2);
    }

    constexpr void test_are_connected_identity(
        const Standard::Algorithms::Numbers::disjoint_set<int_t> &dsu, const char *const message)
    {
        for (int_t index{}; index < size; ++index)
        {
            test_are_connected(index, dsu, index, true, message);
        }
    }

    constexpr void first_union(Standard::Algorithms::Numbers::disjoint_set<int_t> &dsu)
    {
        {
            const auto *const name = "Before the first union.";

            test_are_connected(node2, dsu, node8, false, name);

            test_are_connected_identity(dsu, name);
        }

        dsu.unite(node2, node8);
        ::Standard::Algorithms::ert::are_equal(size - 1U, dsu.count(), "Count after the first union.");

        {
            const auto *const name = "After the first union.";

            for (int_t index{}; index < size; ++index)
            {
                const auto connected = node2 == index || node8 == index;

                test_are_connected(node2, dsu, index, connected, name);
            }

            test_are_connected(node3, dsu, node6, false, name);
        }
    }

    constexpr void three_more_unions(Standard::Algorithms::Numbers::disjoint_set<int_t> &dsu)
    {
        dsu.unite(node8, node4);
        // Changing the order must have no effect.
        dsu.unite(node4, node8);

        dsu.unite(node3, node6);
        dsu.unite(node6, node8);

        ::Standard::Algorithms::ert::are_equal(size - 4U, dsu.count(), "Count after 4 unions.");

        const auto *const name = "After 4 unions.";

        test_are_connected_identity(dsu, name);

        const std::vector<int_t> united{ node2, node3, node4, node6, node8 };

        for (std::size_t index{}; index < united.size(); ++index)
        {
            const auto &node = united[index];

            for (auto index_2 = index + 1U; index_2 < united.size(); ++index_2)
            {
                const auto &node_2 = united[index_2];

                test_are_connected(node, dsu, node_2, true, name);
            }
        }

        const std::initializer_list<int_t> singles{ node0, node1, node5, node7 };

        ::Standard::Algorithms::ert::are_equal(size, singles.size() + united.size(), "Size after 4 unions.");

        for (const auto &single : singles)
        {
            for (std::size_t index{}; index < size; ++index)
            {
                const auto connected = single == index;

                test_are_connected(single, dsu, index, connected, name);
            }
        }
    }

    constexpr void union_all(Standard::Algorithms::Numbers::disjoint_set<int_t> &dsu)
    {
        for (int_t index = 1; index < size; ++index)
        {
            dsu.unite(index - 1U, index);
        }

        ::Standard::Algorithms::ert::are_equal(1U, dsu.count(), "Count after all unions.");

        const auto *const name = "After all unions.";

        const auto root = dsu.parent(0U);

        for (int_t index = 1; index < size; ++index)
        {
            test_are_connected(index - 1U, dsu, index, true, name);

            const auto parent = dsu.parent(index);

            ::Standard::Algorithms::ert::are_equal(root, parent, "Parent after all unions.");
        }

        test_are_connected(int_t{}, dsu, size - 1U, true, name);
    }
} // namespace

void Standard::Algorithms::Numbers::Tests::disjoint_set_tests()
{
    Standard::Algorithms::Numbers::disjoint_set<int_t> dsu(size);

    try
    {
        const auto conn = dsu.are_connected(node2, size);

        // NOLINTNEXTLINE NOLINT
        if (conn)
        {
        }

        throw std::runtime_error("The disjoint_set_exception must have been thrown.");
    }
    catch (const disjoint_set_exception &)
    {// Ignore the expected error.
    }

    ::Standard::Algorithms::ert::are_equal(size, dsu.count(), "Initial count.");

    first_union(dsu);

    three_more_unions(dsu);

    union_all(dsu);
    union_all(dsu);
}
