#pragma once
#include <vector>
#include "PrefixTree.h"

class PrefixTreePerformanceTest final
{
    PrefixTreePerformanceTest() = delete;

public:
    static void Test();

    static void AddWordsToTree(const std::vector<std::string >& words, PrefixTree& prefixTree);

private:

    static void ReadWords(std::vector<std::string >& words, const size_t limit = 0);
    static void RunSomeTests(const std::vector<std::string >& words, const PrefixTree& prefixTree);
};