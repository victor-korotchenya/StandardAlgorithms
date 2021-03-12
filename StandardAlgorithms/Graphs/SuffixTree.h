#pragma once
#include "PrefixTree.h"

namespace Privet::Algorithms::Trees
{
    // Todo: p2. make it linear, not n*n.
    //The words are case-insensitive.
    class SuffixTree final
    {
        PrefixTree _PrefixTree;
        std::string _Word;

    public:
        //The words are case-insensitive.
        void SetWord(const std::string& word);

        //The suffix is case-insensitive.
        void Search(const std::string& suffix,
            const unsigned int countLimit,
            std::vector<std::string>& result) const;
    };
}