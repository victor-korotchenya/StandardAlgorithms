#include "segment_tree_add_bool_tests.h"
#include "segment_tree_add_bool.h"
#include "../test_utilities.h"

using namespace std;
using namespace Privet::Algorithms::Trees;
using namespace Privet::Algorithms::Tests;
using namespace Privet::Algorithms;

namespace
{
    using tree_t = segment_tree_add_bool<int>;
    using tree_t_slow = segment_tree_add_bool_slow<int>;
}

void Privet::Algorithms::Trees::Tests::segment_tree_add_bool_tests()
{
    constexpr auto size = 4;
    vector<int> widths(size, 1);
    tree_t tree(widths);
    tree_t_slow tree_slow(move(widths));

    for (auto i = 0; ; ++i)
    {
        const auto is = to_string(i);
        const auto get = tree.get(0, tree.size()),
            get_slow = tree_slow.get(0, tree_slow.size()),
            get_expected = i + (i > 0);
        Assert::AreEqual(get_expected, get, "get" + is);
        Assert::AreEqual(get_expected, get_slow, "get_slow" + is);

        if (i == size - 1)
            break;

        const auto add = tree.add(i, i + 2),
            add_slow = tree_slow.add(i, i + 2),
            add_expected = 1 + !i;
        Assert::AreEqual(add_expected, add, "add" + is);
        Assert::AreEqual(add_expected, add_slow, "add_slow" + is);
    }

    for (auto i = 0; ; ++i)
    {
        const auto is = to_string(i);
        const auto get = tree.get(0, tree.size()),
            get_slow = tree_slow.get(0, tree_slow.size()),
            get_expected = size - i - (i == size - 1);
        Assert::AreEqual(get_expected, get, "rem_get" + is);
        Assert::AreEqual(get_expected, get_slow, "rem_get_slow" + is);

        if (i == size - 1)
            break;

        const auto actual = tree.remove(i, i + 2),
            actual_slow = tree_slow.remove(i, i + 2),
            remove_expected = 1 + (i == size - 2);
        Assert::AreEqual(remove_expected, actual, "remove" + is);
        Assert::AreEqual(remove_expected, actual_slow, "remove_slow" + is);
    }
}