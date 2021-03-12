#include <limits>
#include "SuffixTree.h"
#include "SuffixTreeTests.h"
#include "../Assert.h"

using namespace std;
using namespace Privet::Algorithms::Trees;
using namespace Privet::Algorithms;

void Privet::Algorithms::Trees::Tests::SuffixTreeTests()
{
    const string word = "Bananas";

    SuffixTree tree;
    tree.SetWord(word);

    constexpr auto countLimit = numeric_limits<unsigned>::max();
    vector<string> actual;
    {
        const string suffix = "BN";
        tree.Search(suffix, countLimit, actual);
        Assert::Empty(actual, suffix);
    }
    {
        const string suffix = "Ananas";
        tree.Search(suffix, countLimit, actual);

        const vector<string> expected = { word };
        Assert::AreEqual(expected, actual, suffix);
    }
}