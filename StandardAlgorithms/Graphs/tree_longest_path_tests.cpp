#include "tree_longest_path.h"
#include "tree_longest_path_tests.h"
#include "BinaryTreeUtilities.h"
#include "../test_utilities.h"

using namespace std;
using namespace Privet::Algorithms::Trees;
using namespace Privet::Algorithms;

namespace
{
    void tree_longest_path_simple()
    {
        const vector<int> expected = { 2,3,3,2,3 };
        const vector<vector<int>> tree{
          {1,2,3},//0
          {0},
          {0},
          {0,4},
          {3},
        };
        {
            const auto actual = tree_longest_path_slow(tree);
            Assert::AreEqual(expected, actual, "tree_longest_path_slow");
        }
        {
            const auto actual = tree_longest_path(tree);
            Assert::AreEqual(expected, actual, "tree_longest_path");
        }
    }

    void tree_longest_path_random()
    {
        const short minSize = 1, maxSize = 20,
            size = static_cast<short>(UnsignedIntRandom(minSize, maxSize)());

        const auto tree = build_random_tree<int>(size);
        Assert::AreEqual(size, static_cast<int>(tree.size()), "tree.size");

        const auto slow = tree_longest_path_slow(tree),
            fast = tree_longest_path(tree);
        Assert::AreEqual(slow, fast, "tree_longest_path_random");
    }
}

namespace Privet::Algorithms::Trees::Tests
{
    void tree_longest_path_tests()
    {
        tree_longest_path_simple();
        tree_longest_path_random();
    }
}