#pragma once
#include <climits>
#include <stdexcept>
#include <sstream>
#include <stack>
#include <type_traits>
#include <tuple>
#include <unordered_map>
#include <unordered_set>
#include "BinaryTreeNode.h"
#include "../Numbers/Arithmetic.h"
#include "../Utilities/StreamUtilities.h"
#include "../Utilities/VectorUtilities.h"
#include "../Utilities/Random.h"

namespace
{
    template<typename t>
    typename std::enable_if<!std::is_pointer<t>::value, void>::type
        null_check(t, const char*)
    {//do nothing when t is not a pointer.
    }

    template<typename t>
    typename std::enable_if<std::is_pointer<t>::value, void>::type
        null_check(t value, const char*& argument_name)
    {
        ThrowIfNull<t>(value, argument_name);
    }

    //Unlike "PostOrder", it deals with double pointers.
    template <typename TNode,
        typename VisitFunction = void(*)(TNode**)>
        void PostOrder2(TNode** pRoot, VisitFunction visitFunction)
    {
        null_check<VisitFunction>(visitFunction, "visitFunction");
        if (nullptr == pRoot || nullptr == *pRoot)
        {
            return;
        }

        //Assign some not null value.
        TNode* lastVisited = *pRoot;

        std::stack<TNode**> s;
        s.push(pRoot);

        for (;;)
        {
            auto pTop = s.top();
#ifdef _DEBUG
            ThrowIfNull(pTop, "PostOrder2.pTop");
            ThrowIfNull(*pTop, "*(PostOrder2.pTop)");
#endif
            auto top = *pTop;
            const auto shallVisit =
                //Leaf
                nullptr == top->Left && nullptr == top->Right
                || //After both sub-trees.
                top->Right == lastVisited
                //No right subtree, and left is visited.
                || //nullptr == top->Right &&
                top->Left == lastVisited;

            if (shallVisit)
            {
                visitFunction(pTop);
                lastVisited = top;

                s.pop();
                if (s.empty())
                {
                    break;
                }
            }
            else
            {
                if (top->Right)
                {
                    s.push(&(top->Right));
                }

                if (top->Left)
                {
                    s.push(&(top->Left));
                }
            }
        }
    }
}

namespace Privet::Algorithms::Trees
{
    //Frees the memory to the pointer of the tree.
    template <typename TNode>
    void FreeTree(TNode** pRoot)
    {
        if (nullptr == pRoot || nullptr == *pRoot)
        {//Nothing to delete.
            return;
        }

        PostOrder2(
            pRoot, [](TNode** pNode) -> void
            {
#ifdef _DEBUG
                ThrowIfNull(pNode, "FreeTree.pNode");
                ThrowIfNull(*pNode, "*(FreeTree.pNode)");
#endif

                TNode* node = *pNode;

#ifdef _DEBUG
                if (node->Left)
                {
                    throw std::runtime_error("FreeTree.node->Left must be null.");
                }
                if (node->Right)
                {
                    throw std::runtime_error("FreeTree.node->Right must be null.");
                }
#endif
                delete node;
                *pNode = nullptr;
            });

#ifdef _DEBUG
        if (*pRoot)
        {
            throw std::runtime_error("*(FreeTree.pRoot) must be null after deletion.");
        }
#endif
    }

    //Frees the memory of the tree.
    template <typename TNode>
    void FreeTree(TNode* root)
    {
        if (nullptr == root)
        {//Nothing to delete.
            return;
        }

        FreeTree(&root);
    }

    //See also "NodeUtilities".
    template <typename Number = int,
        typename TNode = BinaryTreeNode < Number >
    >
        class BinaryTreeUtilities final
    {
        BinaryTreeUtilities() = delete;

    public:

        static inline size_t parent(const size_t index)
        {
#ifdef _DEBUG
            if (0 == index)
            {
                throw std::out_of_range("The index is zero in the BinaryTreeUtilities::parent.");
            }
#endif
            return index >> 1;
        }

        static inline size_t left_child(const size_t index)
        {
            return index << 1;
        }

        static inline size_t right_child(const size_t index)
        {
            return (index << 1) | 1;
        }

        static size_t calc_tree_size(const size_t size)
        {
            check_size(size);

            const auto result = size << 1;
            return result;
        }

        static unsigned calc_tree_height(const size_t size)
        {
            check_size(size);

            const auto result = MostSignificantBit64(size) + 1;
            return result;
        }

        static inline void check_index(
            const size_t index, const size_t max_index_exclusive,
            const std::string& name)
        {
            if (max_index_exclusive <= index)
            {
                std::ostringstream ss;
                ss << "The index (" << index
                    << ") must be smaller than " << max_index_exclusive
                    << " in " << name << ".";
                StreamUtilities::throw_exception<std::out_of_range>(ss);
            }
        }

        static inline void check_indexes(
            size_t leftInclusive, size_t rightExclusive,
            const size_t max_index_exclusive, const std::string& name)
        {
            BinaryTreeUtilities<>::check_index(
                leftInclusive, max_index_exclusive, name);

            BinaryTreeUtilities<>::check_index(
                rightExclusive - 1, max_index_exclusive, name);
#ifdef _DEBUG
            if (rightExclusive <= leftInclusive)
            {
                std::ostringstream ss;
                ss << "Error: rightExclusive (" << rightExclusive
                    << ") <= leftInclusive(" << leftInclusive
                    << ") in " << name << ".";
                StreamUtilities::throw_exception<std::out_of_range>(ss);
            }
#endif
        }

        //Memory usage must be minimized - tree can be big.
        //There is no "parent" link.
        static const TNode* LeastCommonAncestorWithoutParent(
            const TNode* const root,
            const TNode* const node1, const TNode* const node2);

        //"maxNodeCount" allows protecting against
        // a broken tree when there is a cycle.

        static bool IsTreeSymmetrical(const TNode* const root);

        //This is only for binary trees.
        //
        //If yes, returns the pair of nulls.
        //Else, the returned nodes:
        // - either have different values,
        // - or one pointer is null.
        static std::pair< const TNode* const, const TNode* const > AreIsomorphic(
            const TNode* const root1, const TNode* const root2);

        //The tree must have unique elements.
        template <typename Data = std::vector< Number > >
        static void ReConstructTree(
            const Data& preOrder,
            const Data& inOrder,
            const size_t size,
            TNode** head);

        // See also build_random_tree.
        template <typename NewFunction = TNode * (*)() >
        static void BuildRandomTree(
            const size_t size,
            TNode** head,
            const Number cutter = Number(0),
            const bool isUniqueOnly = false,
            NewFunction newFunction = &NodeConstructor)
        {
            ThrowIfNull(head, "head");
            *head = nullptr;

            if (0 == size)
                return;

            if ((SIZE_MAX - 1) <= size)
            {
                throw std::runtime_error("The size is too big.");
            }

            std::vector< Number > data;
            FillRandom(data, size, cutter, isUniqueOnly);

            NewNode(data[0], head, newFunction);
            if (1 == size)
            {
                return;
            }

            using Temp = std::tuple< TNode*, size_t, size_t >;

            SizeRandom intRandom;

            std::stack< Temp > s;
            s.push(Temp(*head, 0, size));

            do
            {
                const Temp& top = s.top();

                TNode* root = std::get<0>(top);

                //Both indexes are exclusive.
                const size_t firstIndex = std::get<1>(top);
                const size_t secondIndex = std::get<2>(top);

                s.pop();

                const size_t split = intRandom(firstIndex, secondIndex - 1);

#ifdef _DEBUG
                int subTreeCount = 0;
#endif
                //range = (0..2)
                //split = 0, left is none; right = 1.
                //split = 1, left is 1; right is none.
                //
                //range = (0..10)
                //split = 0, left is none; right is 1: next (1..10).
                //split = 1, left is 1: stop; right is 2: next (2..10).
                // split = 4, left is 4: next (0..4); right is 5: next (5..10).
                //split = 9, left is 9: next (0..9); right none.

                if (firstIndex < split)
                {//Left sub-tree.
                    NewNode(data[split], &(root->Left), newFunction);

#ifdef _DEBUG
                    ++subTreeCount;
#endif
                    if (firstIndex < split - 1)
                    {
                        s.push(Temp(root->Left, firstIndex, split));
                    }
                }

                if (split + 1 < secondIndex)
                {//Right sub-tree.
                    NewNode(data[split + 1], &(root->Right), newFunction);

#ifdef _DEBUG
                    ++subTreeCount;
#endif
                    if (split + 1 < secondIndex - 1)
                    {
                        s.push(Temp(root->Right, split + 1, secondIndex));
                    }
                }

#ifdef _DEBUG
                if (0 == subTreeCount)
                {
                    std::ostringstream ss;
                    ss << "Error: 0 == subTreeCount"
                        << ", split=" << split
                        << ", firstIndex=" << firstIndex
                        << ", secondIndex=" << secondIndex
                        << ".";
                    StreamUtilities::throw_exception(ss);
                }
#endif
            } while (!s.empty());
        }

        template <typename VisitFunction = void(*)(const TNode* const) >
        static void PreOrder(const TNode* const root,
            VisitFunction visitFunction);

        template <typename VisitFunction = void(*)(const TNode* const) >
        static void InOrder(const TNode* const root,
            VisitFunction visitFunction);

        template <typename VisitFunction = void(*)(const TNode* const) >
        static void PostOrder(const TNode* const root,
            VisitFunction visitFunction);

        //Use the "PostOrder" which uses less memory.
        template <typename VisitFunction = void(*)(const TNode* const) >
        static void PostOrderSlow(const TNode* const root,
            VisitFunction visitFunction);

    private:
        using TLevels = std::unordered_map< TNode*, std::pair<size_t, TNode*> >;

        //Pre-index, in-index, length, node.
        using ConstructTuple = std::tuple<size_t, size_t, size_t, TNode*>;

        static void check_size(const size_t size)
        {
            constexpr auto maxSize = std::numeric_limits<size_t>::max() >> 1;
            if (0 == size || maxSize < size)
            {
                std::ostringstream ss;
                ss << "The tree size (" << size
                    << ") must positive and smaller than or equal to " << maxSize << ".";
                StreamUtilities::throw_exception<std::out_of_range>(ss);
            }
        }

        static void FindNodeLevels(
            const TNode* const root,
            const std::vector< TNode*>& sourceNodes,
            TLevels& levels,
            //Limits the search to the given depth.
            const size_t depthMax = 0);

        static void ExploreLevel(
            const size_t depth,
            const std::vector< TNode*>& currentList,
            std::vector< TNode*>& nextList,
            std::vector< TNode*>& sourceNodesLeft,
            TLevels& levels);

        //Returns true if no more nodes left to match.
        static bool MatchAndUnmark(
            const size_t depth,
            TNode* current,
            std::vector< TNode*>& sourceNodesLeft,
            TLevels& levels);

        static bool AddFoundNode(
            const size_t depth,
            TNode* parent, TNode* node,
            std::vector< TNode*>& sourceNodesLeft,
            TLevels& levels,
            const size_t index,
            size_t& leftSize);

        static std::pair<size_t, TNode*> CheckNodeExists(
            const TNode* const root,
            const TLevels& levels,
            TNode* node);

        static bool MatchSubtree(
            const std::vector< TNode* >& sourceNodes,
            TNode** result,
            TNode* tryNode, TNode* otherNode,
            size_t& finds, size_t& levelMax,
            TLevels& levels,
            const char* const  currentBranchName,
            const char* const  otherBranchName);

        template <typename Data = std::vector<Number>>
        static void ReConstructTreeStep(
            const Data& preOrder,
            const Data& inOrder,
#ifdef _DEBUG
            const size_t size,
#endif
            std::stack<ConstructTuple>& s);

        template <typename Data = std::vector<Number>>
        static size_t FindInOrderValue(
            const Data& inOrder, const Number& value,
            const size_t inIndex, const size_t length);

        inline static TNode* NodeConstructor()
        {
            return new TNode();
        }

        template <typename NewFunction = TNode * (*)() >
        static void NewNode(const Number& value, TNode** result,
            NewFunction newFunction = &NodeConstructor);

#ifdef _DEBUG
        static void ValidateConstructTuple(
            const size_t preIndex,
            const size_t inIndex,
            const size_t length,
            const TNode* const root,
            const size_t size);
#endif

        template <typename Data = std::vector<Number>>
        static void RequireUniquePermuted(
            const Data& preOrder,
            const Data& inOrder,
            const size_t size);

        //inline static void CheckStackSize(const size_t size, const size_t maxNodeCount);
    };

    template <typename Number,
        typename TNode >
        const TNode*
        BinaryTreeUtilities< Number, TNode >
        ::LeastCommonAncestorWithoutParent(
            const TNode* const root,
            const TNode* const node1, const TNode* const node2)
    {
        if (nullptr == root)
        {
            throw std::runtime_error("The root must be not null.");
        }

        if (nullptr == node1)
        {
            throw std::runtime_error("The node1 must be not null.");
        }

        if (nullptr == node2)
        {
            throw std::runtime_error("The node2 must be not null.");
        }

        //Some shortcuts.
        if (node1 == root
            || node1 == node2
            || node1->Left == node2 || node1->Right == node2)
        {
            return node1;
        }
        else if (node2 == root
            || node2->Left == node1 || node2->Right == node1)
        {
            return node2;
        }

        std::vector< TNode* > sourceNodes;
        sourceNodes.push_back(const_cast<TNode*>(node1));
        sourceNodes.push_back(const_cast<TNode*>(node2));

        TLevels levels;

        FindNodeLevels(root, sourceNodes, levels);

        const std::pair<size_t, TNode*> pair1 = CheckNodeExists(root, levels, const_cast<TNode*>(node1));
        const std::pair<size_t, TNode*> pair2 = CheckNodeExists(root, levels, const_cast<TNode*>(node2));

        ////These shortcuts rarely happen.
        //if (nullptr != pair1.second)
        //{
        //  if (pair1.second == pair2.second)
        //  {//The same parent.
        //    return pair1.second;
        //  }
        //
        //  if (node2->Left == pair1.second || node2->Right == pair1.second)
        //  {
        //    return node2;
        //  }
        //}
        //
        //if (nullptr != pair2.second
        //  &&(node1->Left == pair2.second || node1->Right == pair2.second))
        //{
        //  return node1;
        //}

        TNode* result = const_cast<TNode*>(root);
        size_t levelMax = std::max(pair1.first, pair2.first);

        for (;;)
        {
            if (node1 == result || node2 == result)
            {
                break;
            }

            size_t leftFinds = 0;
            size_t rightFinds = 0;

            const char* const  leftBranchName = "left";
            const char* const  rightBranchName = "right";

            if (MatchSubtree(
                sourceNodes,
                &result, result->Left, result->Right,
                leftFinds, levelMax,
                levels,
                leftBranchName, rightBranchName))
            {
                continue;
            }

            if (MatchSubtree(
                sourceNodes,
                &result, result->Right, result->Left,
                rightFinds, levelMax,
                levels,
                rightBranchName, leftBranchName))
            {
                continue;
            }

            if (1 == leftFinds && 1 == rightFinds)
            {
                break;
            }

            {
                std::ostringstream ss;
                ss << "Internal error: Should never get here."
                    << " leftFinds=" << leftFinds
                    << ", rightFinds=" << rightFinds
                    << ", result->Value='"
                    << result->Value << "'.";
                const auto s = ss.str();
                throw std::runtime_error(s.c_str());
            }
        }

        return result;
    }

    template <typename Number,
        typename TNode >
        void
        BinaryTreeUtilities<Number, TNode>
        ::FindNodeLevels(
            const TNode* const root,
            const std::vector<TNode*>& sourceNodes,
            TLevels& levels,
            const size_t depthMax)
    {
        if (nullptr == root)
        {
            throw std::runtime_error(
                "The root must be not null in Find Node Levels.");
        }

        const size_t size = sourceNodes.size();
        if (0 == size)
        {
            throw std::runtime_error(
                "There must be sourceNodes in Find Node Levels.");
        }

        levels.clear();

        //Do not search more than necessary.
        const size_t depthMaxVerified = (0 == depthMax) ? (SIZE_MAX) : depthMax;

        //Nodes will be deleted once they are found
        //to make search faster.
        std::vector< TNode*> sourceNodesLeft = sourceNodes;

        std::vector< TNode*> currentList;
        currentList.push_back(const_cast<TNode*>(root));

        std::vector< TNode*> nextList;

        size_t depth = 0;

        //Run BFS.
        do
        {
            nextList.clear();

            ExploreLevel(
                depth,
                currentList, nextList,
                sourceNodesLeft,
                levels);

            swap(currentList, nextList);
        } while (!currentList.empty()
            && ((++depth) <= depthMaxVerified));
    }

    template <typename Number,
        typename TNode >
        void
        BinaryTreeUtilities<Number, TNode>
        ::ExploreLevel(
            const size_t depth,
            const std::vector< TNode*>& currentList,
            std::vector< TNode*>& nextList,
            std::vector< TNode*>& sourceNodesLeft,
            TLevels& levels)
    {
        const size_t currentSize = currentList.size();

        for (size_t index = 0; index < currentSize; ++index)
        {
            TNode* current = currentList[index];

            const bool hasFoundEverything = MatchAndUnmark(
                depth,
                current,
                sourceNodesLeft,
                levels);
            if (hasFoundEverything)
            {//Clear to stop the search.
                nextList.clear();
                return;
            }

            if (current->Left)
            {
                nextList.push_back(current->Left);
            }

            if (current->Right)
            {
                nextList.push_back(current->Right);
            }
        }
    }

    template <typename Number,
        typename TNode >
        bool
        BinaryTreeUtilities<Number, TNode>
        ::MatchAndUnmark(
            const size_t depth,
            TNode* current,
            std::vector< TNode*>& sourceNodesLeft,
            TLevels& levels)
    {
        const TNode* const left = const_cast<const TNode* const>(current->Left);
        const TNode* const right = const_cast<const TNode* const>(current->Right);

        size_t leftSize = sourceNodesLeft.size();

        for (size_t index = 0; index < leftSize; )
        {
            TNode* node = sourceNodesLeft[index];
            if (left == node || right == node)
            {
                if (AddFoundNode(1 + depth, current, node,
                    sourceNodesLeft, levels,
                    index, leftSize))
                {
                    return true;
                }
            }
            else if (current == node)
            {
                TNode* unknownParent = nullptr;
                if (AddFoundNode(depth, unknownParent, node,
                    sourceNodesLeft, levels,
                    index, leftSize))
                {
                    return true;
                }
            }
            else
            {
                ++index;
            }
        }

        return false;
    }

    template <typename Number,
        typename TNode >
        bool
        BinaryTreeUtilities<Number, TNode>
        ::AddFoundNode(
            const size_t depth,
            TNode* parent, TNode* node,
            std::vector< TNode*>& sourceNodesLeft,
            TLevels& levels,
            const size_t index,
            size_t& leftSize)
    {
        const std::pair<size_t, TNode*> p(depth, parent);
        const std::pair<TNode*, std::pair<size_t, TNode*> > levelEntry(node, p);
        const auto isInserted = levels.insert(levelEntry);
        if (!isInserted.second)
        {
            std::ostringstream ss;
            ss << "Error: cannot insert a node (" << node->Value
                << ") a second time."
                << " Probably sourceNodes has repeating values.";
            const auto s = ss.str();
            throw std::runtime_error(s.c_str());
        }

        if (1 == leftSize)
        {
            return true;
        }

        VectorUtilities::SwapAndEraseFast< TNode >(sourceNodesLeft, index);
        --leftSize;

        return false;
    }

    template <typename Number,
        typename TNode>

        std::pair<size_t, TNode*>

        BinaryTreeUtilities<Number, TNode>
        ::CheckNodeExists(
            const TNode* const root,
            const TLevels& levels,
            TNode* node)
    {
        const auto end = levels.end();
        const auto found = levels.find(node);
        if (end == found)
        {
            std::ostringstream ss;
            ss << "The node (" << node->Value
                << ") was not found in the tree rooted at (" << root->Value
                << ").";
            const auto s = ss.str();
            throw std::runtime_error(s.c_str());
        }

        return found->second;
    }

    template <typename Number,
        typename TNode >
        bool
        BinaryTreeUtilities<Number, TNode>
        ::MatchSubtree(
            const std::vector< TNode* >& sourceNodes,
            TNode** result,
            TNode* tryNode, TNode* otherNode,
            size_t& finds, size_t& levelMax,
            TLevels& levels,
            const char* const  currentBranchName,
            const char* const  otherBranchName)
    {
        if (tryNode)
        {
            FindNodeLevels(tryNode, sourceNodes, levels, levelMax);
            finds = levels.size();
            if (2 == finds)
            {
                --levelMax;
                *result = tryNode;
                return true;
            }

            if (0 == finds)
            {
                if (nullptr == otherNode)
                {
                    std::ostringstream ss;
                    ss << "Internal error: node (" << (*result)->Value
                        << ") has no '" << otherBranchName
                        << "' node, and nodes to find and not in the '"
                        << currentBranchName << "' subtree.";
                    const auto s = ss.str();
                    throw std::runtime_error(s.c_str());
                }

                --levelMax;
                *result = otherNode;
                return true;
            }
        }

        return false;
    }

    template <typename Number, typename TNode >
    template <typename Data >
    void BinaryTreeUtilities< Number, TNode >::RequireUniquePermuted(
        const Data& preOrder, const Data& inOrder, const size_t size)
    {
        std::unordered_set< Number > preSet;

        for (size_t i = 0; i < size; ++i)
        {
            const auto inserted = preSet.insert(preOrder[i]);
            if (!inserted.second)
            {
                std::ostringstream ss;
                ss << "The value (" << preOrder[i]
                    << ") has already been added to the first set.";
                StreamUtilities::throw_exception(ss);
            }
        }

        for (size_t i = 0; i < size; ++i)
        {
            const auto deleted = preSet.erase(inOrder[i]);
            if (1 != deleted)
            {
                std::ostringstream ss;
                ss << "The value (" << inOrder[i]
                    << ") must belong to the second subset, deleted="
                    << deleted << ".";
                StreamUtilities::throw_exception(ss);
            }
        }
    }

    template <typename Number, typename TNode >
    bool BinaryTreeUtilities< Number, TNode >::IsTreeSymmetrical(
        const TNode* const root)
    {
        if (nullptr == root)
        {
            return true;
        }

        using TPair = std::pair < TNode*, TNode* >;
        std::stack < TPair > pairs;

        TNode* rootEditable = const_cast<TNode*>(root);
        pairs.push(TPair(rootEditable, rootEditable));

        do
        {
            const TPair top = pairs.top();

            if (top.first->Value != top.second->Value
                || (nullptr == top.first->Left) != (nullptr == top.second->Right)
                || (nullptr == top.first->Right) != (nullptr == top.second->Left))
            {
                return false;
            }

            pairs.pop();

            if (nullptr != top.first->Left)
                //Skip unnecessary check for second node.
            {
                pairs.push(TPair(top.first->Left, top.second->Right));
            }

            if (nullptr != top.first->Right)
            {
                pairs.push(TPair(top.first->Right, top.second->Left));
            }
        } while (!pairs.empty());

        return true;
    }

    template <typename Number, typename TNode >
    std::pair< const TNode* const, const TNode* const >
        BinaryTreeUtilities< Number, TNode >::AreIsomorphic(
            const TNode* const root1,
            const TNode* const root2)
    {
        using TPair = std::pair< TNode*, TNode* >;
        using TPairResult = std::pair< const TNode* const, const TNode* const >;

        if (root1 != root2)
        {
            std::stack< TPair > s;
            s.push(TPair(const_cast<TNode*>(root1), const_cast<TNode*>(root2)));

            do
            {
                const TPair top = s.top();
                s.pop();

                if (nullptr == top.first && nullptr == top.second)
                {
                    continue;
                }

                if (nullptr == top.first && nullptr != top.second
                    || nullptr != top.first && nullptr == top.second
                    || top.first->Value != top.second->Value)
                {
                    const TPairResult result = TPairResult(
                        const_cast<const TNode* const>(top.first),
                        const_cast<const TNode* const>(top.second));
                    return result;
                }

                s.push(TPair(top.first->Left, top.second->Left));
                s.push(TPair(top.first->Right, top.second->Right));
            } while (!s.empty());
        }

        return TPairResult(nullptr, nullptr);
    }

    template <typename Number, typename TNode >
    template <typename Data >
    void BinaryTreeUtilities< Number, TNode >::ReConstructTree(
        const Data& preOrder, const Data& inOrder,
        const size_t size,
        TNode** head)
    {
        ThrowIfNull(head, "head");

        *head = nullptr;
        if (0 == size)
        {
            return;
        }

        RequireUniquePermuted(preOrder, inOrder, size);

        *head = new TNode();
        (*head)->Left = nullptr;
        (*head)->Right = nullptr;
        (*head)->Value = preOrder[0];

        if (1 == size)
        {
            return;
        }

        std::stack< ConstructTuple > s;
        s.push(ConstructTuple(0, 0, size, *head));

        do
        {
            ReConstructTreeStep(preOrder, inOrder,
#ifdef _DEBUG
                size,
#endif
                s);
        } while (!s.empty());
    }

    template <typename Number, typename TNode >
    template <typename Data >
    void BinaryTreeUtilities< Number, TNode >::ReConstructTreeStep(
        const Data& preOrder, const Data& inOrder,
#ifdef _DEBUG
        const size_t size,
#endif
        std::stack<ConstructTuple>& s)
    {
        const ConstructTuple& t = s.top();

        const size_t preIndex = std::get<0>(t);
        const size_t inIndex = std::get<1>(t);
        const size_t length = std::get<2>(t);
        TNode* root = std::get<3>(t);

#ifdef _DEBUG
        ValidateConstructTuple(preIndex, inIndex, length, root, size);
#endif
        s.pop();

        if (preOrder[preIndex] == inOrder[inIndex])
        {//Right sub-tree only.
            NewNode(preOrder[preIndex + 1], &(root->Right));

            const bool goRight = 2 < length;
            if (goRight)
            {
                s.push(ConstructTuple(preIndex + 1,
                    inIndex + 1, length - 1, root->Right));
            }
        }
        else
        {//There must be left subtree.
            NewNode(preOrder[preIndex + 1], &(root->Left));

            const size_t rootIndex = FindInOrderValue(
                inOrder, preOrder[preIndex], inIndex, length);

            const size_t rootDelta = rootIndex - inIndex;

            const bool goLeft = 1 < rootDelta;
            if (goLeft)
            {
                s.push(ConstructTuple(preIndex + 1,
                    inIndex, rootDelta, root->Left));
            }

            const bool hasRight = rootDelta + 1 < length;
            if (hasRight)
            {
                NewNode(preOrder[preIndex + rootDelta + 1], &(root->Right));

                const bool goRight = rootDelta + 2 < length;
                if (goRight)
                {
                    s.push(ConstructTuple(preIndex + rootDelta + 1,
                        rootIndex + 1,
                        length - (rootDelta + 1), root->Right));
                }
            }
        }
    }

    template <typename Number, typename TNode >
    template <typename Data >
    size_t BinaryTreeUtilities< Number, TNode >::FindInOrderValue(
        const Data& inOrder, const Number& value,
        const size_t inIndex, const size_t length)
    {
        const size_t maxIndexInclusive = inIndex + length - 1;

        size_t result = inIndex + 1;

        while (result <= maxIndexInclusive
            && value != inOrder[result])
        {
            ++result;
        }

        if (maxIndexInclusive < result)
        {
            std::ostringstream ss;
            ss << "The item value=" << value
                << " is not found in the in-order list.";
            StreamUtilities::throw_exception(ss);
        }

        return result;
    }

    template <typename Number, typename TNode >
    template <typename NewFunction >
    inline void BinaryTreeUtilities<Number, TNode>::NewNode(
        const Number& value, TNode** result,
        NewFunction newFunction)
    {
        //ThrowIfNull(newFunction, "newFunction");
#ifdef _DEBUG
        ThrowIfNull(result, "NewNode**");
        if (*result)
        {
            std::ostringstream ss;
            ss << "The node tor value \""
                << value << "\" to create is not null: "
                << (*result) << ".";
            StreamUtilities::throw_exception(ss);
        }
#endif
        TNode* node = (*result) = newFunction();
        node->Left = nullptr;
        node->Right = nullptr;
        node->Value = value;
    }

#ifdef _DEBUG
    template <typename Number, typename TNode >
    void BinaryTreeUtilities< Number, TNode >::ValidateConstructTuple(
        const size_t preIndex,
        const size_t inIndex,
        const size_t length,
        const TNode* const root,
        const size_t size)
    {
        bool hasError = false;
        std::ostringstream ss;

        if (length <= 1)
        {
            hasError = true;
            ss << " Bad length=" << length << ".";
        }
        if (!is_within(size_t(0), size - 1, preIndex))
        {
            hasError = true;
            ss << " Bad preIndex=" << preIndex << ".";
        }
        if (!is_within(size_t(0), size - 1, inIndex))
        {
            hasError = true;
            ss << " Bad inIndex=" << inIndex << ".";
        }
        if (nullptr == root)
        {
            hasError = true;
            ss << "The root is null.";
        }

        if (hasError)
        {
            ss << " preIndex=" << preIndex
                << ", inIndex=" << inIndex
                << ", length=" << length
                << ", size=" << size;

            if (root)
            {
                ss << ", root->Value=" << root->Value << ".";
            }
            else
            {
                ss << ", root->Value=null.";
            }

            StreamUtilities::throw_exception(ss);
        }
    }
#endif

    template <typename Number, typename TNode >
    template <typename VisitFunction >
    void BinaryTreeUtilities< Number, TNode >::PreOrder(
        const TNode* const root,
        VisitFunction visitFunction)
    {
        //ThrowIfNull(visitFunction, "visitFunction");
        if (nullptr == root)
        {
            return;
        }

        std::stack< TNode* > s;
        s.push(const_cast<TNode*>(root));

        do
        {
            TNode* node = s.top();
            s.pop();

            visitFunction(node);

            if (node->Right)
            {
                s.push(node->Right);
            }

            if (node->Left)
            {
                s.push(node->Left);
            }
        } while (!s.empty());
    }

    template <typename Number, typename TNode >
    template <typename VisitFunction >
    void BinaryTreeUtilities< Number, TNode >::InOrder(
        const TNode* const root,
        VisitFunction visitFunction)
    {
        //ThrowIfNull(visitFunction, "visitFunction");

        std::stack< TNode* > s;
        TNode* current = const_cast<TNode*>(root);

        while (current || !s.empty())
        {
            if (nullptr == current)
            {
                current = s.top();
                s.pop();

                visitFunction(current);

                current = current->Right;
            }
            else
            {
                s.push(current);
                current = current->Left;
            }
        }
    }

    template <typename Number, typename TNode >
    template <typename VisitFunction >
    void BinaryTreeUtilities< Number, TNode >::PostOrderSlow(
        const TNode* const root,
        VisitFunction visitFunction)
    {
        //ThrowIfNull(visitFunction, "visitFunction");
        if (nullptr == root)
        {
            return;
        }

        std::stack< TNode* > tempStack, resultStack;

        tempStack.push(const_cast<TNode*>(root));
        do
        {
            TNode* top = tempStack.top();
            tempStack.pop();

            resultStack.push(top);

            if (nullptr != top->Left)
            {
                tempStack.push(top->Left);
            }

            if (nullptr != top->Right)
            {
                tempStack.push(top->Right);
            }
        } while (!tempStack.empty());

        do
        {
            TNode* node = resultStack.top();
            visitFunction(node);
            resultStack.pop();
        } while (!resultStack.empty());
    }

    template <typename Number, typename TNode >
    template <typename VisitFunction >
    void BinaryTreeUtilities< Number, TNode >::PostOrder(
        const TNode* const root,
        VisitFunction visitFunction)
    {
        TNode* rootEditable = const_cast<TNode*>(root);
        TNode** pRoot = &rootEditable;

        PostOrder2(pRoot,
            [&](TNode** pNode) -> void
            {
#ifdef _DEBUG
                ThrowIfNull(pNode, "PostOrder.pNode");
                ThrowIfNull(*pNode, "*(PostOrder.pNode)");
#endif
                TNode* nodeTemp = *pNode;
                const TNode* const node = const_cast<const TNode* const>(nodeTemp);
                visitFunction(node);
            });
    }

    //template <typename Number,
    //    typename TNode >
    //    void
    //    BinaryTreeUtilities< Number, TNode >
    //    ::CheckStackSize(const size_t size, const size_t maxNodeCount)
    //{
    //    if (size > maxNodeCount)
    //    {
    //        ostringstream ss;
    //        ss << "The tree might be circular: stack size (" << size
    //            << ") has exceeded maximum value (" << maxNodeCount << ").";
    //        const string s = ss.str();
    //        throw runtime_error(s.c_str());
    //    }
    //}
}