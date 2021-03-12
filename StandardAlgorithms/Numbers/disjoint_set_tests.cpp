#include "disjoint_set.h"
#include "disjoint_set_tests.h"
#include "../Assert.h"

using namespace std;
using namespace Privet::Algorithms::Numbers::Tests;
using namespace Privet::Algorithms::Numbers;
using namespace Privet::Algorithms;

namespace
{
    using int_t = unsigned;

    void test_are_connected(
        const int_t a, const int_t b,
        disjoint_set<int_t>& ds,
        const bool expected, const char* const message)
    {
        assert(message && message[0]);

        const auto actual = ds.are_connected(a, b);
        Assert::AreEqual(expected, actual, "are_connected (" + to_string(a) + ", " + to_string(b) + ") " + message);
    }

    void first_union(
        const int_t size,
        disjoint_set<int_t>& ds,
        const int_t node2,
        const int_t node8,
        const int_t nodeOther)
    {
        test_are_connected(node2, node8, ds, false, "Before first union.");

        ds.unite(node2, node8);
        Assert::AreEqual(size - 1, ds.count(), "Count after first union.");

        test_are_connected(node2, node8, ds, true, "After first union.");
        test_are_connected(nodeOther, node8, ds, false, "After first union.");
    }

    void three_more_unions(
        const int_t size,
        disjoint_set<int_t>& ds,
        const int_t node2,
        const int_t node8)
    {
        ds.unite(node8, 4);
        ds.unite(4, node8);
        ds.unite(3, 6);
        ds.unite(6, node8);
        Assert::AreEqual(size - 4, ds.count(), "Count after 4 unions.");

        test_are_connected(6, 8, ds, true, "After 4 unions.");
        test_are_connected(node2, node8, ds, true, "After 4 unions.");
        test_are_connected(node2, node8, ds, true, "After 4 unions.");
        test_are_connected(node8, node2, ds, true, "After 4 unions.");
        test_are_connected(node2, node2, ds, true, "After 4 unions.");

        test_are_connected(node2, 4, ds, true, "After 4 unions.");
        test_are_connected(4, node8, ds, true, "After 4 unions.");
        test_are_connected(4, 6, ds, true, "After 4 unions.");
        test_are_connected(4, 3, ds, true, "After 4 unions.");

        test_are_connected(1, 3, ds, false, "After 4 unions.");
        test_are_connected(3, 1, ds, false, "After 4 unions.");
        test_are_connected(node2, 0, ds, false, "After 4 unions.");
    }

    void union_all(
        const int_t size,
        disjoint_set<int_t>& ds)
    {
        for (int_t i = 0; i < size - 1; ++i)
            ds.unite(i, i + 1);

        Assert::AreEqual(int_t(1), ds.count(), "Count after all unions.");

        for (int_t j = 0; j < size - 1; ++j)
            test_are_connected(j, j + 1, ds, true, "After all unions.");

        test_are_connected(int_t(), size - 1, ds, true, "After all unions.");
    }
}

namespace Privet::Algorithms::Numbers::Tests
{
    void disjoint_set_tests()
    {
        constexpr int_t size = 9;
        disjoint_set<int_t> ds(size);

        constexpr int_t node2 = 2, node8 = 8;
        try
        {
            ds.are_connected(node2, size);
            throw runtime_error("The DisjointSetException must have been thrown.");
        }
        catch (const DisjointSetException&)
        {//Ignore the expected error.
        }

        Assert::AreEqual(size, ds.count(), "Initial count.");

        constexpr int_t nodeOther = 4;
        first_union(size, ds, node2, node8, nodeOther);

        three_more_unions(size, ds, node2, node8);

        union_all(size, ds);
        union_all(size, ds);
    }
}