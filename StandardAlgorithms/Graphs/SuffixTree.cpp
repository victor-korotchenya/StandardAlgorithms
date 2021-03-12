#include "../Strings/string_utilities.h"
#include "SuffixTree.h"

using namespace std;
using namespace Privet::Algorithms::Trees;
using namespace Privet::Algorithms::Strings;
using namespace Privet::Algorithms;

void SuffixTree::SetWord(const string& word)
{
    const auto size = word.size();
    if (!size)
        throw runtime_error("The word must be not empty.");

    _PrefixTree.Clear();
    _Word = word;

    auto word_lower_case = word;
    ToLower(word_lower_case);
    _PrefixTree.AddWord(word_lower_case);

    //For now, use simple method.
    for (size_t i = 1; i < size; ++i)
    {
        const auto subString = word_lower_case.substr(i);
        _PrefixTree.AddWord(subString);
    }
}

void SuffixTree::Search(
    const string& suffix, const unsigned countLimit, vector<string>& result) const
{
    const auto suffixSize = suffix.size();
    if (!suffixSize)
        throw runtime_error("The suffix must be not empty.");

    if (!countLimit)
        throw exception("The 'countLimit' must be positive.");

    result.clear();
    if (_Word.empty() || _PrefixTree.empty())
        return;

    auto suffix_lower_case = suffix;
    ToLower(suffix_lower_case);

    vector<string> temp;
    _PrefixTree.Search(suffix_lower_case, countLimit, temp);

    if (!temp.empty())
        result.push_back(_Word);
}