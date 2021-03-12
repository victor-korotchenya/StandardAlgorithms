#include <sstream>
#include "../Numbers/Sort.h"
#include "../Numbers/BinarySearch.h"
#include "../Utilities/StreamUtilities.h"
#include "PrefixTree.h"

using namespace std;
using namespace Privet::Algorithms::Numbers;

PrefixTree::PrefixTree()
{
}

PrefixTree::~PrefixTree()
{
    Clear();
}

void PrefixTree::Clear() noexcept
{
    //TODO: p2. memory leaks.
    _Roots.clear();
}

void PrefixTree::AddWord(const string& word)
{
    if (word.empty())
    {
        throw exception("The word must be not empty.");
    }

    PrefixNode* startNode = FindNodeByFirstChar(_Roots, word[0]);
    if (nullptr == startNode)
    {
        AddWordAsChild(word, _Roots);
    }
    else
    {
        AddWordToNode(word, startNode);
    }

#ifdef _DEBUG
    CheckUniqueWords(_Roots);
#endif
}

PrefixNode* PrefixTree::FindNodeByFirstChar(
    const vector< PrefixNode >& children, const char firstSymbol)
{
    if (children.empty())
    {
        return nullptr;
    }

    PrefixNode* left = const_cast<PrefixNode*>(children.data());
    PrefixNode* rightInclusive = left + children.size() - 1;

    using NodeToCharCast = char(*)(const PrefixNode& node);

    // InterpolationSearch seems to be 3% slower than BinarySearch on 58K words.
    PrefixNode* result = binary_search_inclusive_with_cast<PrefixNode, char, NodeToCharCast>
        (left, rightInclusive,
            firstSymbol, &PrefixNode::FirstSymbol);

    return result;
}

void PrefixTree::AddWordAsChild(const string& word, vector< PrefixNode >& children)
{
#ifdef _DEBUG
    if (word.empty())
    {
        throw exception("word.empty() in AddWordAsChild.");
    }
#endif
    PrefixNode prefixNode;
    prefixNode.Value = word;
    prefixNode.HasWord = true;

    children.push_back(prefixNode);

    InsertionSortStep<PrefixNode>(children.data(), children.size());
}

void PrefixTree::AddWordToNode(const string& word, PrefixNode* startNode)
{
    const FindNodeResult findNodeResult = FindLowestNode(word, startNode);
#ifdef _DEBUG
    findNodeResult.Validate();
#endif

    if (findNodeResult.IsPartialMatch)
    {
        SplitPartialMatch(
            word,
            findNodeResult.WordPosition,
            findNodeResult.Node,
            findNodeResult.MatchedSymbolCount);

        return;
    }

    if (findNodeResult.HasMatch)
    {
        findNodeResult.Node->HasWord = true;
        return;
    }

    const size_t offset = findNodeResult.WordPosition + findNodeResult.MatchedSymbolCount;
    const string subWord = word.substr(offset);

    AddWordAsChild(subWord, findNodeResult.Node->Children);
}

FindNodeResult PrefixTree::FindLowestNode(const string& word, PrefixNode* startNode0)
{
    PrefixNode* startNode = startNode0;
#ifdef _DEBUG
    if (nullptr == startNode)
    {
        throw exception("nullptr == node in FindLowestNode.");
    }
#endif
    const size_t wordSize = word.size();
    size_t wordPosition = 0;

    for (;;)
    {
        const string& nodeValue = startNode->Value;
        const size_t nodeValueSize = nodeValue.size();
#ifdef _DEBUG
        if (0 == nodeValueSize)
        {
            throw exception("0 == nodeValueSize");
        }
        if (word[wordPosition] != nodeValue[0])
        {
            throw exception("The first symbol must match, but word[wordPosition] != nodeValue[0].");
        }
#endif
        const size_t leftToMatch = wordSize - wordPosition;

        const size_t matchedSymbolCount = FirstMatchingSymbolsCount(
            word, leftToMatch,
            nodeValue, nodeValueSize,
            wordPosition);

        const bool isPartialMatch = matchedSymbolCount != nodeValueSize;
        if (isPartialMatch)
        {
            FindNodeResult result;
            result.Node = startNode;

            result.WordPosition = wordPosition;
            result.MatchedSymbolCount = matchedSymbolCount;

            result.IsPartialMatch = true;

            result.HasMatch = leftToMatch == matchedSymbolCount;
#ifdef _DEBUG
            result.Validate();
#endif
            return result;
        }

        const bool isCompleteMatch = wordSize == wordPosition + nodeValueSize;
        if (isCompleteMatch)
        {
            FindNodeResult result;
            result.Node = startNode;

            result.WordPosition = wordPosition;
            result.MatchedSymbolCount = nodeValueSize;

            result.IsPartialMatch = false;
            result.HasMatch = true;
#ifdef _DEBUG
            result.Validate();
#endif
            return result;
        }

        //Continue searching in the node's children.
        PrefixNode* childNode = FindNodeByFirstChar(
            startNode->Children, word[wordPosition + nodeValueSize]);
        if (nullptr == childNode)
        {
            FindNodeResult result;
            result.Node = startNode;

            result.WordPosition = wordPosition;
            result.MatchedSymbolCount = nodeValueSize;

            result.IsPartialMatch = false;
            result.HasMatch = false;
#ifdef _DEBUG
            result.Validate();
#endif
            return result;
        }

        startNode = childNode;
        wordPosition += nodeValueSize;
    }
}

size_t PrefixTree::FirstMatchingSymbolsCount(
    const string& word, const size_t leftToMatch,
    const string& nodeValue, const size_t nodeValueSize,
    const size_t wordPosition)
{
    const size_t minLength = min(leftToMatch, nodeValueSize);

    const size_t skipFirstMatchingSymbol = 1;
    size_t result = skipFirstMatchingSymbol;

    while (result < minLength
        && nodeValue[result] == word[wordPosition + result])
    {
        ++result;
    }

    return result;
}

void PrefixTree::SplitPartialMatch(
    const string& word,
    const size_t wordPosition,
    PrefixNode* node, const size_t matchedSymbolCount)
{
    PrefixNode middlePrefixNode;
    middlePrefixNode.Value = node->Value.substr(matchedSymbolCount);
    middlePrefixNode.HasWord = node->HasWord;

    node->Value = node->Value.substr(0, matchedSymbolCount);

    swap(middlePrefixNode.Children, node->Children);

    //No need to sort as there is one child node.
    node->Children.push_back(middlePrefixNode);

    const size_t wordSize = word.size();
    const bool isCompleteMatch = wordSize == wordPosition + matchedSymbolCount;

    node->HasWord = isCompleteMatch;

    if (!isCompleteMatch)
    {
        node->HasWord = false;

        const string reminder = word.substr(wordPosition + matchedSymbolCount);
        AddWordAsChild(reminder, node->Children);
    }
}

#ifdef _DEBUG
void PrefixTree::CheckUniqueWords(const vector< PrefixNode >& roots)
{
    unordered_set<string> words;

    queue< StringNodePair > q;

    {
        PrefixNode root;
        root.HasWord = false;
        root.Children = roots;

        const string empty = "";
        q.push(make_pair(empty, root));
    }

    do
    {
        const StringNodePair top = q.front();
        q.pop();

        const PrefixNode& prefixNode = top.second;

        string parentValue = top.first;
        parentValue.append(prefixNode.Value);

        if (prefixNode.HasWord)
        {
            InsertUniqueWord(parentValue, prefixNode.Value, words);
        }

        const vector< PrefixNode >& children = prefixNode.Children;

        CheckUniquePrefixes(children);
        EnqueueChildren(parentValue, children, q);
    } while (!q.empty());
}

void PrefixTree::InsertUniqueWord(
    const string& word,
    const string& prefix,
    unordered_set<string>& words)
{
    if (word.empty() || prefix.empty())
    {
        ostringstream ss;
        ss << "Either the prefix '" << prefix
            << "' or the word '" << word << "' is empty.";
        StreamUtilities::throw_exception(ss);
    }

    const auto insertResult = words.insert(word);
    if (!insertResult.second)
    {
        ostringstream ss;
        ss << "The prefix '" << prefix
            << "' is repeated in PrefixTree when adding the word '" << word << "'.";
        StreamUtilities::throw_exception(ss);
    }
}

void PrefixTree::CheckUniquePrefixes(const vector< PrefixNode >& children)
{
    const size_t length = children.size();
    const size_t skipFirstItem = 1;

    for (size_t index = skipFirstItem; index < length; ++index)
    {
        const PrefixNode& previousNode = children[index - skipFirstItem];
        const char previousSymbol = previousNode.FirstSymbol();

        const PrefixNode& currentNode = children[index];
        const char currentSymbol = currentNode.FirstSymbol();

        if (currentSymbol <= previousSymbol)
        {
            ostringstream ss;
            ss << "The terms are not sorted properly at index=" << index
                << ", previous value='" << previousNode.Value
                << "', current value='" << currentNode.Value
                << "' in the prefix tree.";
            StreamUtilities::throw_exception(ss);
        }
    }
}

void PrefixTree::CheckSearchResult(
    const string& prefix,
    const unsigned int countLimit,
    const vector< string >& result)
{
    if (result.empty())
    {
        return;
    }

    if (result.size() > countLimit)
    {
        ostringstream ss;
        ss << result.size() << "=result.size() > countLimit=" << countLimit
            << " in the PrefixTree.Search. prefix='" << prefix << "'.";
        StreamUtilities::throw_exception(ss);
    }

    const size_t prefixSize = prefix.size();
    if (0 == prefixSize)
    {
        throw exception("0 == prefixSize");
    }

    unordered_set<string> words;

    const auto end = result.cend();
    for (auto it = result.cbegin(); end != it; ++it)
    {
        const string& word = *it;

        const auto insertResult = words.insert(word);
        if (!insertResult.second)
        {
            ostringstream ss;
            ss << "The word '" << word <<
                "' is repeated in the result of PrefixTree.Search. prefix='" << prefix << "'.";
            StreamUtilities::throw_exception(ss);
        }

        const size_t wordSize = word.size();
        if (wordSize < prefixSize)
        {
            ostringstream ss;
            ss << "The word '" << word <<
                "' cannot have a prefix='" << prefix << "'.";
            StreamUtilities::throw_exception(ss);
        }

        const string wordSubstring = word.substr(0, prefixSize);
        if (wordSubstring != prefix)
        {
            ostringstream ss;
            ss << "The resulting word '" << word <<
                "' must have a prefix='" << prefix << "'.";
            StreamUtilities::throw_exception(ss);
        }
    }
}
#endif

void PrefixTree::EnqueueChildren(
    const string& parentValue,
    const vector< PrefixNode >& children,
    queue< StringNodePair >& q)
{
    for (auto it = children.begin(); children.end() != it; ++it)
    {
        const PrefixNode& node2 = *it;
        q.push(make_pair(parentValue, node2));
    }
}

void PrefixTree::Search(
    const string& prefix,
    const unsigned int countLimit,
    vector< string >& result) const
{
    if (prefix.empty())
    {
        throw exception("The 'prefix' must be not empty.");
    }

    if (0 == countLimit)
    {
        throw exception("The 'countLimit' must be positive.");
    }

    result.clear();

    PrefixNode* startNode = FindNodeByFirstChar(_Roots, prefix[0]);
    if (nullptr == startNode)
    {
        return;
    }

    const FindNodeResult findNodeResult = FindLowestNode(prefix, startNode);
#ifdef _DEBUG
    findNodeResult.Validate();
#endif
    PrefixNode* lowestNode = findNodeResult.Node;

    const bool notFound = !(findNodeResult.HasMatch) || nullptr == lowestNode;
    if (notFound)
    {
        return;
    }

    string rootWord;
    FormRootWord(prefix, findNodeResult.WordPosition, lowestNode->Value, rootWord);

    AppendChildrenToResult(countLimit, rootWord, lowestNode, result);

    //BFS can get unsorted data.
    InsertionSort<>(result.data(), result.size());

#ifdef _DEBUG
    CheckSearchResult(prefix, countLimit, result);
#endif
}

void PrefixTree::FormRootWord(
    const string& prefix, const size_t lastWordPosition,
    const string& nodeValue,
    string& rootWord)
{
    if (lastWordPosition)
    {
        rootWord = prefix.substr(0, lastWordPosition);
    }

    rootWord.append(nodeValue);
}

void PrefixTree::AppendChildrenToResult(
    const unsigned int countLimit,
    const string& rootWord,
    const PrefixNode* const node,
    vector< string >& result)
{
    PrefixNode root;
    root.HasWord = node->HasWord;
    root.Children = node->Children;

    queue< StringNodePair > q;
    q.push(make_pair(rootWord, root));

    do
    {
        const StringNodePair top = q.front();
        q.pop();

        const PrefixNode& prefixNode = top.second;

        string parentValue = top.first;
        parentValue.append(prefixNode.Value);

        if (prefixNode.HasWord)
        {
            result.push_back(parentValue);
            if (countLimit <= result.size())
            {
                return;
            }
        }

        const vector< PrefixNode >& children = prefixNode.Children;
#ifdef _DEBUG
        CheckUniquePrefixes(children);
#endif
        EnqueueChildren(parentValue, children, q);
    } while (!q.empty());
}