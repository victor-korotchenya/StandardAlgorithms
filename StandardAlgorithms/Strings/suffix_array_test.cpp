#include "../Utilities/Random.h"
#include "../test_utilities.h"
#include "suffix_array.h"
#include "ukkonen_suffix_tree_utilities.h"

using namespace std;
using namespace Privet::Algorithms::Strings;
using namespace Privet::Algorithms;

namespace
{
    void sa_test(const string& s, const vector<int>& expected, const bool skip_fast = false)
    {
        Assert::AreEqual(s.size(), expected.size(), "size " + s);
        {
            const auto actual = suffix_array_slow(s);
            Assert::AreEqual(expected, actual, "suffix_array_slow '" + s + "'.");
        }
        {
            const ukkonen_suffix_tree<string> tree(s);
            const auto& nodes = tree.nodes;
            const auto actual = build_suffix_array<int>(s, nodes);
            Assert::AreEqual(expected, actual, "build_suffix_array '" + s + "'.");
        }
        if (skip_fast)
            return;
        {
            const auto actual = suffix_array(s);
            Assert::AreEqual(expected, actual, "suffix_array '" + s + "'.");
        }
    }
}

namespace Privet
{
    namespace Algorithms
    {
        namespace Strings
        {
            namespace Tests
            {
                void suffix_array_test()
                {
                    const string last_unique = "#";

                    sa_test("cccacc" + last_unique, { 6,3,5,2,4,1,0 });
                    sa_test("cccabcc" + last_unique, { 7,3,4,6,2,5,1,0 });
                    sa_test("abcabx", { 0,3,1,4,2,5 });
                    sa_test("ABAACBAB" + last_unique, { 8,2,6,0,3,7,1,5,4 });
                    sa_test("banana" + last_unique, { 6,5,3,1,0,4,2 });

                    IntRandom r;
                    for (auto att = 0, att_max = 1; att < att_max; ++att)
                    {
                        const auto s = random_string(r) + last_unique;
                        const auto fast = suffix_array(s);
                        sa_test(s, fast, true);
                    }
                }
            }
        }
    }
}