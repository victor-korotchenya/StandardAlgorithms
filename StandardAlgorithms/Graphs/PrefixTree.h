#pragma once
#include <stdexcept>
#include <queue>
#include <string>
#include <vector>

#ifdef _DEBUG
#include <unordered_set>
#endif

struct PrefixNode final
{
    std::string Value;
    bool HasWord;

    std::vector<PrefixNode> Children;

    inline bool operator <(const PrefixNode& b) const
    {
        //The nodes must differ by the first symbol only.
        bool result = FirstSymbol() < b.FirstSymbol();
        return result;
    }

    inline char FirstSymbol() const
    {
#ifdef _DEBUG
        if (Value.empty())
        {
            throw std::runtime_error("The value is empty in FirstSymbol.");
        }
#endif
        return Value[0];
    }

    inline static char FirstSymbol(const PrefixNode& node)
    {
        return node.FirstSymbol();
    }
};

struct FindNodeResult final
{
    //Not nullptr.
    PrefixNode* Node;

    //The beginning of last used position in the "search prefix".
    size_t WordPosition;

    //How many symbols were partially matched in the "Node".
    size_t MatchedSymbolCount;

    bool IsPartialMatch;

    bool HasMatch;

    inline void Validate() const
    {
        if (nullptr == Node)
        {
            throw std::runtime_error("The 'Node' is null in FindNodeResult.");
        }

        if (IsPartialMatch)
        {
            if (0 == MatchedSymbolCount)
            {
                throw std::runtime_error("The MatchedCount is 0 for partially matched FindNodeResult.");
            }
        }
    }
};

//Auto-complete auto complete (Trie): Allows searching for the words starting from the given prefix.
//The words are case-sensitive.
class PrefixTree final
{
    std::vector<PrefixNode> _Roots;

public:
    PrefixTree();
    ~PrefixTree();

    inline bool empty() const
    {
        const bool result = _Roots.empty();
        return result;
    }

    //Clears the words.
    void Clear() noexcept;

    //The words are case-sensitive.
    //Duplicates are ignored.
    void AddWord(const std::string& word);

    //Searches for the words starting from the given prefix.
    //The prefix is case-sensitive.
    void Search(
        const std::string& prefix, const unsigned int countLimit,
        std::vector<std::string>& result) const;

private:
    static inline PrefixNode* FindNodeByFirstChar(
        const std::vector< PrefixNode >& children, const char firstSymbol);

    static inline void AddWordAsChild(
        const std::string& word, std::vector< PrefixNode >& children);

    static inline void AddWordToNode(
        const std::string& word, PrefixNode* startNode);

    static inline FindNodeResult FindLowestNode(
        const std::string& word, PrefixNode* startNode);

    static inline size_t FirstMatchingSymbolsCount(
        const std::string& word, const size_t leftToMatch,
        const std::string& nodeValue, const size_t nodeValueSize,
        const size_t wordPosition);

    static void SplitPartialMatch(
        const std::string& word,
        const size_t wordPosition,
        PrefixNode* node, const size_t matchedSymbolCount);

    static inline void FormRootWord(
        const std::string& prefix, const size_t lastWordPosition,
        const std::string& nodeValue,
        std::string& rootWord);

    static inline void AppendChildrenToResult(
        const unsigned int countLimit,
        const std::string& rootWord,
        const PrefixNode* const node,
        std::vector< std::string >& result);

    using StringNodePair = std::pair<std::string, PrefixNode>;
#ifdef _DEBUG

    static void CheckUniqueWords(const std::vector< PrefixNode >& roots);

    static void InsertUniqueWord(
        const std::string& word,
        const std::string& prefix,
        std::unordered_set<std::string>& words);

    static void CheckUniquePrefixes(const std::vector< PrefixNode >& children);

    static void CheckSearchResult(
        const std::string& prefix,
        const unsigned int countLimit,
        const std::vector< std::string >& result);
#endif

    static inline void EnqueueChildren(
        const std::string& parentValue,
        const std::vector< PrefixNode >& children,
        std::queue< StringNodePair >& q);
};