#include <iostream>
#include <fstream>
#include "PrefixTreePerformanceTest.h"
#include "../Assert.h"

using namespace std;

void PrefixTreePerformanceTest::Test()
{
    vector< string > words;
#ifdef _DEBUG
    const size_t wordLimit = 10;
    const int attemptMax = 4;
#else
    const size_t wordLimit = 0;
    const int attemptMax = 10;
#endif
    ReadWords(words, wordLimit);
    const size_t wordsSize = words.size();
    if (0 == wordsSize)
    {
        throw exception("There are no read words in PrefixTreePerformanceTest.");
    }

    //LARGE_INTEGER Frequency;
    //QueryPerformanceFrequency(&Frequency);

    //LARGE_INTEGER totalTime0, totalTime1;
    //totalTime0.QuadPart = 0;
    //totalTime1.QuadPart = 0;

    PrefixTree prefixTree;

    for (int attempt = 0; attempt < attemptMax; ++attempt)
    {
        prefixTree.Clear();

        //LARGE_INTEGER time0, time1, time2;

        //QueryPerformanceCounter(&time0);
        AddWordsToTree(words, prefixTree);
        //QueryPerformanceCounter(&time1);

        RunSomeTests(words, prefixTree);
        //QueryPerformanceCounter(&time2);

        //LARGE_INTEGER elapsedMicroseconds0;
        //elapsedMicroseconds0.QuadPart = time1.QuadPart - time0.QuadPart;
        //elapsedMicroseconds0.QuadPart *= 1000000;
        //elapsedMicroseconds0.QuadPart /= Frequency.QuadPart;

        //totalTime0.QuadPart = totalTime0.QuadPart + elapsedMicroseconds0.QuadPart;

        //LARGE_INTEGER elapsedMicroseconds1;
        //elapsedMicroseconds1.QuadPart = time2.QuadPart - time1.QuadPart;
        //elapsedMicroseconds1.QuadPart *= 1000000;
        //elapsedMicroseconds1.QuadPart /= Frequency.QuadPart;

    //    ..totalTime1.QuadPart = totalTime1.QuadPart + elapsedMicroseconds1.QuadPart;
    //
    ////#ifdef SHALL_PRINT_TEST_RESULTS
    //    const double averageSearchTime = static_cast<double>(totalTime1.QuadPart) / wordsSize / (1 + attempt);
    //
    //    cout << attempt << " AddWordsToTree=" << elapsedMicroseconds0.QuadPart
    //      << ", RunSomeTests=" << elapsedMicroseconds1.QuadPart
    //      << ", Ave Add=" << (totalTime0.QuadPart / (1 + attempt))
    //      << ", Ave Run=" << (totalTime1.QuadPart / (1 + attempt))
    //      << ", Ave Search micro.sec=" << averageSearchTime
    //      << ", total words=" << wordsSize << ".\n";
    ////#endif
    }
}

void PrefixTreePerformanceTest::ReadWords(
    vector< string >& words, const size_t limit)
{
    const string filename = "M:\\Projects\\English_Word_List__corncob_lowercase.txt";

    ifstream readStream;

    readStream.open(filename);
    if (!readStream.good())
    {
        throw exception("Cannot read words in PrefixTreePerformanceTest.");
    }

    const bool hasLimit = limit > 0;

    while (readStream.good() && !readStream.eof())
    {
        string line;
        readStream >> line;

        // TODO: p2. line.trim();

        if (!line.empty())
        {
            words.push_back(line);

            if (hasLimit && limit <= words.size())
            {
                break;
            }
        }
    }

    readStream.close();
}

void PrefixTreePerformanceTest::AddWordsToTree(
    const vector< string >& words, PrefixTree& prefixTree)
{
    size_t index = 0;
    try
    {
        for (auto it = words.cbegin(); words.cend() != it; ++it, ++index)
        {
            const string& word = *it;
            prefixTree.AddWord(word);
        }
    }
    catch (const exception& ex)
    {
        ostringstream ss;
        ss << "Error adding word '" << words[index]
            << "' with index=" << index << " to the prefix tree.";
        ss << " ex = " << ex.what();

        StreamUtilities::throw_exception(ss);
    }
}

void PrefixTreePerformanceTest::RunSomeTests(
    const vector< string >& words,
    const PrefixTree& prefixTree)
{
    vector< string > result;

    for (auto it = words.cbegin(); words.cend() != it; ++it)
    {
        const string& word = *it;

        const unsigned int countLimit = 2;
        prefixTree.Search(word, countLimit, result);
        if (result.empty())
        {
            ostringstream ss;
            ss << "The word '" << word
                << "' was not found in the prefix tree.";
            StreamUtilities::throw_exception(ss);
        }
    }
}