#include <set>
#include "BinaryTreeUtilities.h"
#include "avl_tree_t.h"
#include "avl_tree_tests.h"
#include "../test_utilities.h"
#include "../Utilities/Random.h"

using namespace std;
using namespace Privet::Algorithms::Trees;
using namespace Privet::Algorithms::Tests;
using namespace Privet::Algorithms;

// todo: p1. end
namespace
{
    using value_t = short;
    using node_t = avl_node_t<value_t>;
    using tree_t = avl_tree_t<value_t>;
}

void Privet::Algorithms::Trees::Tests::avl_tree_tests()
{
    tree_t tree;
    set<value_t> s;
    vector<value_t> ve;
    IntRandom r;
    constexpr auto cutter = 10;
    constexpr auto size = cutter << 1;
    for (auto i = 0; i < size; ++i)
    {
        {
            const auto v = static_cast<value_t>(r(0, size));
            const auto del = s.erase(v);
            const auto del2 = tree.erase(v);
            Assert::AreEqual(!!del, del2, "del and del2");
            if (del)
            {
                sort(ve.begin(), ve.end());
                auto fi = find(ve.begin(), ve.end(), v);
                assert(fi != ve.end());
                ve.erase(fi);
            }
        }
        {
            const auto v = static_cast<value_t>(r(0, size));
            const auto find = s.find(v);
            const auto find2 = tree.lower_bound(v);
            const auto has = find != s.end(),
                has2 = find2.first && find2.first->key == v;
            Assert::AreEqual(has, has2, "find and find2");
            {
                sort(ve.begin(), ve.end());
                const auto ind = static_cast<int>(lower_bound(ve.begin(), ve.end(), v) - ve.begin());
                const auto find4 = tree.lower_bound(v);
                Assert::AreEqual(ind, find4.second, "lb2");
                Assert::AreEqual(!s.empty(), static_cast<bool>(find4.first), "first lb2");
                if (has)
                    Assert::AreEqual(v, find4.first->key, "has lb2");

                if (ind < static_cast<int>(s.size()))
                {
                    const auto cur = tree.select(ind);
                    Assert::AreEqual(true, !!cur, "select");
                    Assert::AreEqual(cur->key, ve[ind], "select key");

                    if (has)
                        Assert::AreEqual(cur->key, v, "select v");
                }
            }
        }
        {
            const auto v = static_cast<value_t>(r(0, size));
            const auto added = s.insert(v).second;
            const auto added2 = tree.insert(v).second;
            Assert::AreEqual(added, added2, "added and added2");
            if (added)
                ve.push_back(v);
        }
    }
}