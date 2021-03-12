#include <functional>
#include "PrefixTree.h"
#include "PrefixTreeTests.h"
#include "PrefixTreePerformanceTest.h"
#include "../Assert.h"

using namespace std;
using namespace Privet::Algorithms;

void ReadWords(vector< string >& words)
{
    //be
    //beach
    //beaches
    //cat
    //cats
    //crossing
    //cross-ing
    //spoon
    //tea
    //teach
    //teacher
    //teachers
    //teachers ok
    //teachers.ok
    //teachers.oks
    //teaching
    //team
    //tear
    //teaspoon
    //tempo
    //xenon
    //zoo
    //zulu
    words.push_back("zoo");

    words.push_back("tea");
    //A duplicate.
    words.push_back("tea");

    words.push_back("teach");
    words.push_back("tempo");
    words.push_back("spoon");

    //A duplicate.
    words.push_back("teach");

    words.push_back("teacher");
    words.push_back("team");
    words.push_back("tear");
    words.push_back("cross-ing");
    words.push_back("teaspoon");
    words.push_back("crossing");
    words.push_back("teachers");
    words.push_back("teachers.oks");
    words.push_back("teachers ok");
    words.push_back("teachers.ok");

    words.push_back("cats");
    words.push_back("cat");

    words.push_back("zulu");
    words.push_back("teaching");

    words.push_back("beach");
    words.push_back("beaches");
    words.push_back("be");
    words.push_back("xenon");
}

using PrefixAndLimit = pair<string, int>;
using ExpectedResultAndMessage = pair< vector< string >, string >;
using PrefixTestCase = pair< PrefixAndLimit, ExpectedResultAndMessage >;

void AddTestCases(vector< PrefixTestCase >& testCases)
{
    const unsigned int defaultCountLimitMax = 5;

    const vector< string > emptyResult;

    {
        vector< string > expectedResult;

        testCases.push_back(PrefixTestCase(
            PrefixAndLimit("xn", defaultCountLimitMax),
            ExpectedResultAndMessage(expectedResult, "Same first letter, second is not.")));
    }
    {
        vector< string > expectedResult;
        expectedResult.push_back("cats");

        testCases.push_back(PrefixTestCase(
            PrefixAndLimit("cats", defaultCountLimitMax),
            ExpectedResultAndMessage(expectedResult, "Cats.")));
    }

    {
        const unsigned int countLimitMax = 7;
        vector< string > expectedResult;
        expectedResult.push_back("teach");
        expectedResult.push_back("teacher");
        expectedResult.push_back("teachers");
        expectedResult.push_back("teachers ok");
        expectedResult.push_back("teachers.ok");
        expectedResult.push_back("teachers.oks");
        expectedResult.push_back("teaching");

        testCases.push_back(PrefixTestCase(
            PrefixAndLimit("teach", countLimitMax),
            ExpectedResultAndMessage(expectedResult, "Word => Few words.")));
    }

    {
        vector< string > expectedResult;
        expectedResult.push_back("teach");
        expectedResult.push_back("teacher");
        expectedResult.push_back("teachers");
        expectedResult.push_back("teachers ok");
        expectedResult.push_back("teaching");

        testCases.push_back(PrefixTestCase(
            PrefixAndLimit("teac", defaultCountLimitMax),
            ExpectedResultAndMessage(expectedResult, "Sub-word => Few words.")));
    }

    {
        vector< string > expectedResult;
        expectedResult.push_back("tea");
        expectedResult.push_back("teach");
        expectedResult.push_back("team");
        expectedResult.push_back("tempo");

        const unsigned int countLimitMax = 4;
        testCases.push_back(PrefixTestCase(
            PrefixAndLimit("t", countLimitMax),
            ExpectedResultAndMessage(expectedResult, "Many words with limit.")));
    }

    testCases.push_back(PrefixTestCase(
        PrefixAndLimit("a", defaultCountLimitMax),
        ExpectedResultAndMessage(emptyResult, "Non-existing prefix.")));

    testCases.push_back(PrefixTestCase(
        PrefixAndLimit("bazooka", defaultCountLimitMax),
        ExpectedResultAndMessage(emptyResult, "Non-existing word.")));

    testCases.push_back(PrefixTestCase(
        PrefixAndLimit("zoom", defaultCountLimitMax),
        ExpectedResultAndMessage(emptyResult, "Non-existing word with long initial prefix.")));

    {
        vector< string > expectedResult;
        expectedResult.push_back("beach");
        expectedResult.push_back("beaches");

        testCases.push_back(PrefixTestCase(
            PrefixAndLimit("beac", defaultCountLimitMax),
            ExpectedResultAndMessage(expectedResult, "2 matching words to a prefix.")));
    }

    {
        vector< string > expectedResult;
        expectedResult.push_back("teaspoon");

        testCases.push_back(PrefixTestCase(
            PrefixAndLimit("teasp", defaultCountLimitMax),
            ExpectedResultAndMessage(expectedResult, "One matching word to a prefix.")));
    }

    {
        vector< string > expectedResult;
        expectedResult.push_back("teaspoon");

        testCases.push_back(PrefixTestCase(
            PrefixAndLimit("teaspoon", defaultCountLimitMax),
            ExpectedResultAndMessage(expectedResult, "One word.")));
    }

    {
        vector< string > expectedResult;
        expectedResult.push_back("cat");
        expectedResult.push_back("cats");

        testCases.push_back(PrefixTestCase(
            PrefixAndLimit("cat", defaultCountLimitMax),
            ExpectedResultAndMessage(expectedResult, "Cat.")));
    }
}

void RunPrefixTreeTests(
    PrefixTree& prefixTree,
    vector< PrefixTestCase >& testCases)
{
    vector< string > actualResult;

    const auto itEnd = testCases.cend();
    for (auto it = testCases.cbegin(); itEnd != it; ++it)
    {
        const PrefixTestCase& testCase = *it;
        const PrefixAndLimit& prefixAndLimit = testCase.first;
        const string& prefix = prefixAndLimit.first;
        const unsigned int limit = prefixAndLimit.second;

        const ExpectedResultAndMessage& expectedResultAndMessage = testCase.second;

        const vector< string >& expected = expectedResultAndMessage.first;
        const string& messageS = expectedResultAndMessage.second;
        const char* message = messageS.c_str();

        prefixTree.Search(prefix, limit, actualResult);
        Assert::AreEqual<string>(expected, actualResult, message);
    }
}

void RunPrefixTreeTests4Words(PrefixTree& prefixTree, const vector< string >& words)
{
    vector< string > actualResult;

    //This must not be in any word.
    const char nonExistingSymbol = '_';

    for (auto it = words.cbegin(); words.cend() != it; ++it)
    {
        actualResult.clear();

        const string& word = *it;
        const unsigned int limit = 3;

        prefixTree.Search(word, limit, actualResult);

        bool found = false;
        for (auto actual_it = actualResult.cbegin(); actualResult.cend() != actual_it; ++actual_it)
        {
            const string& actualWord = *actual_it;
            if (word == actualWord)
            {
                found = true;
                break;
            }
        }

        if (!found)
        {
            const auto actualResultSize = actualResult.size();

            ostringstream ss;
            ss << "The word '" << word <<
                "' must present in the tree. The returned result size="
                << actualResultSize << ".";
            StreamUtilities::throw_exception(ss);
        }

        if (nonExistingSymbol == word[0] || nonExistingSymbol == word[word.size() - 1])
        {
            ostringstream ss;
            ss << "The word '" << word <<
                "' must not have this symbol: '" << nonExistingSymbol << "'.";
            StreamUtilities::throw_exception(ss);
        }

        ostringstream ss;
        ss << nonExistingSymbol << word;
        const string word2 = ss.str();

        prefixTree.Search(word2, limit, actualResult);
        Assert::AreEqual<size_t>(0, actualResult.size(), word2.c_str());

        ss << nonExistingSymbol;
        const string word3 = ss.str();

        prefixTree.Search(word3, limit, actualResult);
        Assert::AreEqual<size_t>(0, actualResult.size(), word3.c_str());
    }
}

void EmptyPrefix()
{
    PrefixTree prefixTree;
    vector< string > actualResult;
    prefixTree.Search("", 1, actualResult);
}

void ZeroLimit()
{
    PrefixTree prefixTree;
    vector< string > actualResult;
    prefixTree.Search("0", 0, actualResult);
}

void ExpectExceptionForPrefixTree()
{
    Assert::ExpectException<exception>(
        &EmptyPrefix,
        "The 'prefix' must be not empty.",
        "Empty prefix.");

    Assert::ExpectException<exception>(
        &ZeroLimit,
        "The 'countLimit' must be positive.",
        "Zero limit.");
}

void PrefixTreeTests()
{
    vector< string > words;
    ReadWords(words);

    PrefixTree prefixTree;
    PrefixTreePerformanceTest::AddWordsToTree(words, prefixTree);

    vector< PrefixTestCase > testCases;
    AddTestCases(testCases);

    RunPrefixTreeTests(prefixTree, testCases);
    RunPrefixTreeTests4Words(prefixTree, words);

    ExpectExceptionForPrefixTree();
}