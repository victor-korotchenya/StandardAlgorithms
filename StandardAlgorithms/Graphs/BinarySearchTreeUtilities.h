#pragma once

#include <map>
#include <sal.h>
#include <tuple>
#include <unordered_set>
#include <vector>
#include "../Utilities/ExceptionUtilities.h"
#include "BinaryTreeNode.h"

namespace Privet::Algorithms::Trees
{
    template <typename Number = int, typename TNode = BinaryTreeNode<Number>>
    class BinarySearchTreeUtilities final
    {
        BinarySearchTreeUtilities() = delete;

    public:

        _Ret_maybenull_ static const TNode* Find(
            _In_opt_ const TNode* const root,
            _In_ const Number& value);

        static Number MinimumValue(_In_ const TNode* const root);

        static Number MaximumValue(_In_ const TNode* const root);

        _Ret_maybenull_ static const TNode* MinimumValueNode(_In_opt_ const TNode* const root);

        _Ret_maybenull_ static const TNode* MaximumValueNode(_In_opt_ const TNode* const root);

        //Whether the tree is a binary search tree WITHOUT duplicates.
        //Assume that left sub-tree value(s)
        // must be strictly less than current node value;
        // right sub-tree value(s) - strictly greater.
        static bool IsBSTWithoutDuplicates(_In_opt_ const TNode* const root);

        //Whether the tree is a binary search tree WITH duplicates.
        //Assume that left sub-tree value(s)
        // must be not greater than current node value;
        // right sub-tree value(s) - not less.
        static bool IsBSTDuplicatesAllowed(_In_opt_ const TNode* const root);

        using TSizeNode = std::pair< size_t, TNode* >;
        using TSizeMinMaxNode = std::tuple< size_t, Number, Number, const TNode* const >;

        enum IndexKind final : int
        {
            Size = 0,
            Min = 1,
            Max = 2,
            Node = 3
        };

        //Find the largest BST, WITH duplicates,
        //having all its descendants, if any exists.
        static TSizeNode LargestBinarySearchTreeSize(_In_opt_ const TNode* const root);

        //Assume the "node" has a link to its parent.
        _Ret_maybenull_ static const TNode* NextInOrderSucessor(_In_opt_ const TNode* const node);

        //Use "ConvertBST_To_DoubleLinkedList" which is more reliable.
        static void ConvertBST_To_DoubleLinkedListSlow(_In_opt_ TNode** pRoot);

        static void ConvertBST_To_DoubleLinkedList(_In_opt_ TNode** pRoot);

        //The (*pRoot)->Right must be not null!
        static void RotateLeft(
            _Inout_opt_ TNode* parent,
            _Inout_ TNode** pRoot);

        //The (*pRoot)->Left must be not null!
        static void RotateRight(
            _Inout_opt_ TNode* parent,
            _Inout_ TNode** pRoot);

        //Given Distinct (!) numbers, construct a BST:
        // 0-th element becomes the root;
        // if smaller, go left, else go right;
        // nowhere to go - create a (left or right) node.
        //
        //Return an array, having (size - 1) numbers,
        // specifying, for each number, its parent number.
        //E.g. given {2,4,1,6,5}, the tree is:
        //    2
        //  1   4
        //        6
        //      5
        //  Parent values are:
        //  2   2
        //        4
        //      6
        template<typename unique_ptr_tree>
        static std::vector<Number> ConstructFromDistinctNumbers(
            const std::vector<Number>& distinct_data, unique_ptr_tree& tree);

    private:

        // It is pre-order traversal.
        // Note: In-order uses one fewer parameter.
        static bool IsBSTNoDuplicatesHelper(
            const TNode* const node,
            const Number minValue, const Number maxValue);

        // Use in-order traversal: items must be sorted.
        static bool IsBSTDuplicatesAllowedHelper(
            const TNode* const node,
            Number& minValue);

        //Post-order.
        static TSizeMinMaxNode LargestBSTSizeHelper(
            const TNode* const root);

        static void To_DoubleLinkedListSlowHelper(
            TNode* const root, TNode** pLastVisited);
    };

    template <typename Number, typename TNode >
    const TNode* BinarySearchTreeUtilities< Number, TNode >::Find(
        const TNode* const root,
        const Number& value)
    {
        auto node = const_cast<TNode*>(root);
        while (node && value != node->Value)
            node = value < node->Value ? node->Left : node->Right;

        return const_cast<const TNode*>(node);
    }

    template <typename Number, typename TNode >
    Number BinarySearchTreeUtilities< Number, TNode >::MinimumValue(
        const TNode* const root)
    {
        ThrowIfNull(root, "root");

        auto node = const_cast<TNode*>(root);
        while (node->Left)
            node = node->Left;
        return node->Value;
    }

    template <typename Number, typename TNode >
    const TNode* BinarySearchTreeUtilities< Number, TNode >
        ::MinimumValueNode(const TNode* const root)
    {
        if (nullptr == root)
        {
            return nullptr;
        }

        TNode* result = const_cast<TNode*>(root);

        while (result->Left)
        {
            result = result->Left;
        }

        return result;
    }

    template <typename Number, typename TNode >
    const TNode* BinarySearchTreeUtilities< Number, TNode >
        ::MaximumValueNode(const TNode* const root)
    {
        if (nullptr == root)
            return nullptr;

        auto result = const_cast<TNode*>(root);
        while (result->Right)
            result = result->Right;
        return result;
    }

    template <typename Number, typename TNode >
    Number BinarySearchTreeUtilities< Number, TNode >::MaximumValue(
        const TNode* const root)
    {
        ThrowIfNull(root, "root");

        auto node = MaximumValueNode(root);
        ThrowIfNull(node, "MaximumValueNode");

        auto result = node->Value;
        return result;
    }

    template <typename Number,
        typename TNode >
        bool BinarySearchTreeUtilities< Number, TNode >
        ::IsBSTWithoutDuplicates(const TNode* const root)
    {
        if (nullptr == root)
            return true;

        const Number actualMin = MinimumValue(root);
        const Number minValue = actualMin - Number(1);
        if (actualMin < minValue)
        {//TODO: p3. fix "min(max) -(+) Number(1)" issue.
            std::ostringstream ss;
            ss << "Left-most node has minimum value (" << actualMin
                << ") that is smaller than that minus one ("
                << minValue << ") .";
            StreamUtilities::throw_exception(ss);
        }

        const Number actualMax = MaximumValue(root);
        const Number maxValue = actualMax + Number(1);
        if (maxValue < actualMax)
        {//TODO: p3. fix "min(max) -(+) Number(1)" issue.
            std::ostringstream ss;
            ss << "Right-most node has maximum value (" << actualMax
                << ") that is smaller than that plus one ("
                << maxValue << ") .";
            StreamUtilities::throw_exception(ss);
        }

        const bool result = IsBSTNoDuplicatesHelper(root, minValue, maxValue);
        return result;
    }

    template <typename Number,
        typename TNode >
        bool BinarySearchTreeUtilities< Number, TNode >
        ::IsBSTNoDuplicatesHelper(
            const TNode* const node,
            const Number minValue, const Number maxValue)
    {
        const Number& current = node->Value;

        const bool hasMatch = (minValue < current) && (current < maxValue);
        if (!hasMatch)
        {
            return false;
        }

        const bool leftSubtreeMatch = nullptr == node->Left
            || IsBSTNoDuplicatesHelper(node->Left, minValue, current);
        if (!leftSubtreeMatch)
        {
            return false;
        }

        const bool rightSubtreeMatch = nullptr == node->Right
            || IsBSTNoDuplicatesHelper(node->Right, current, maxValue);
        return rightSubtreeMatch;
    }

    template <typename Number,
        typename TNode >
        bool BinarySearchTreeUtilities< Number, TNode >
        ::IsBSTDuplicatesAllowed(const TNode* const root)
    {
        if (nullptr == root)
        {
            return true;
        }

        Number minValue = MinimumValue(root);
        const bool result = IsBSTDuplicatesAllowedHelper(root, minValue);
        return result;
    }

    template <typename Number,
        typename TNode >
        bool BinarySearchTreeUtilities< Number, TNode >
        ::IsBSTDuplicatesAllowedHelper(
            const TNode* const node,
            Number& minValue)
    {
        const bool leftSubtreeMatch = nullptr == node->Left
            || IsBSTDuplicatesAllowedHelper(node->Left, minValue);
        if (!leftSubtreeMatch)
        {
            return false;
        }

        //In-order.
        if (node->Value < minValue)
        {
            return false;
        }

        minValue = node->Value;

        const bool rightSubtreeMatch = nullptr == node->Right
            || IsBSTDuplicatesAllowedHelper(node->Right, minValue);
        return rightSubtreeMatch;
    }

    template <typename Number, typename TNode >
    typename BinarySearchTreeUtilities< Number, TNode >::TSizeNode
        BinarySearchTreeUtilities< Number, TNode >
        ::LargestBinarySearchTreeSize(const TNode* const root)
    {
        if (nullptr == root)
        {
            return TSizeNode(0, nullptr);
        }

        const TSizeMinMaxNode temp = LargestBSTSizeHelper(root);

        const TSizeNode result = TSizeNode(
            std::get<IndexKind::Size>(temp),
            const_cast<TNode*>(std::get<IndexKind::Node>(temp)));

        return result;
    }

    template <typename Number, typename TNode >
    typename BinarySearchTreeUtilities< Number, TNode >::TSizeMinMaxNode
        BinarySearchTreeUtilities< Number, TNode >
        ::LargestBSTSizeHelper(
            const TNode* const root)
    {
#ifdef _DEBUG
        if (nullptr == root)
        {
            throw std::runtime_error("Null root in LargestBSTSizeHelper.");
        }
#endif
        if (nullptr == root->Left)
        {
            if (nullptr == root->Right)
            {//Leaf node.
                return std::make_tuple(size_t(1), root->Value, root->Value, root);
            }

            //Only right.
            auto  rightResult = LargestBSTSizeHelper(root->Right);

            const bool isBst = root->Right == std::get<IndexKind::Node>(rightResult)
                && root->Value <= std::get<IndexKind::Min>(rightResult);
            if (isBst)
                return std::make_tuple(
                    std::get<IndexKind::Size>(rightResult) + size_t(1),
                    root->Value,
                    std::get<IndexKind::Max>(rightResult),
                    root);

            return rightResult;
        }
        else
        {
            if (nullptr == root->Right)
            {//Only left.
                auto leftResult = LargestBSTSizeHelper(root->Left);

                const bool isBst = root->Left == std::get<IndexKind::Node>(leftResult)
                    && std::get<IndexKind::Max>(leftResult) <= root->Value;
                if (isBst)
                    return std::make_tuple(
                        std::get<IndexKind::Size>(leftResult) + size_t(1),
                        std::get<IndexKind::Min>(leftResult),
                        root->Value,
                        root);
                return leftResult;
            }

            //Both
            auto  leftResult = LargestBSTSizeHelper(root->Left);
            auto  rightResult = LargestBSTSizeHelper(root->Right);

            const bool isBst = root->Left == std::get<IndexKind::Node>(leftResult)
                && std::get<IndexKind::Max>(leftResult) <= root->Value
                //
                && root->Right == std::get<IndexKind::Node>(rightResult)
                && root->Value <= std::get<IndexKind::Min>(rightResult);

            auto result =
                isBst
                ? TSizeMinMaxNode(
                    std::get<IndexKind::Size>(leftResult)
                    + std::get<IndexKind::Size>(rightResult) + size_t(1),
                    std::get<IndexKind::Min>(leftResult),
                    std::get<IndexKind::Max>(rightResult),
                    root)
                : (std::get<IndexKind::Size>(leftResult)
                    < std::get<IndexKind::Size>(rightResult)
                    ? rightResult : leftResult
                    );
            return result;
        }
    }

    template <typename Number,
        typename TNode >
        const TNode*
        BinarySearchTreeUtilities< Number, TNode >
        ::NextInOrderSucessor(const TNode* const node)
    {
        if (nullptr == node)
        {
            return nullptr;
        }

        TNode* current = const_cast<TNode*>(node);
        if (current->Right)
        {
            auto result = current->Right;
            while (result->Left)
            {
                result = result->Left;
            }

            return result;
        }

        for (;;)
        {
            if (nullptr == current->Parent)
            {
                return nullptr;
            }

            if (current == current->Parent->Left)
            {
                return current->Parent;
            }

            current = current->Parent;
        }
    }

    template <typename Number, typename TNode >
    void BinarySearchTreeUtilities< Number, TNode >::
        ConvertBST_To_DoubleLinkedListSlow(TNode** pRoot)
    {
        if (nullptr == pRoot || nullptr == *pRoot)
        {
            return;
        }

        TNode* lastVisited = nullptr;
        To_DoubleLinkedListSlowHelper(*pRoot, &lastVisited);

        TNode* head = const_cast<TNode*>(MinimumValueNode(*pRoot));
#ifdef _DEBUG
        ThrowIfNull(head, "MinimumValueNode Slow");
#endif
        * pRoot = head;
    }

    template <typename Number, typename TNode >
    void BinarySearchTreeUtilities< Number, TNode >::
        To_DoubleLinkedListSlowHelper(TNode* const root, TNode** pLastVisited)
    {
        if (root->Left)
        {
            To_DoubleLinkedListSlowHelper(root->Left, pLastVisited);
#ifdef _DEBUG
            ThrowIfNull(*pLastVisited, "*pLastVisited at left node");
#endif
        }

        root->Left = *pLastVisited;
        if (*pLastVisited)
        {
            (*pLastVisited)->Right = root;
        }

        *pLastVisited = root;

        if (root->Right)
        {
            To_DoubleLinkedListSlowHelper(root->Right, pLastVisited);
#ifdef _DEBUG
            ThrowIfNull(root->Right, "right node");
#endif
        }
    }

    template <typename Number, typename TNode >
    void BinarySearchTreeUtilities< Number, TNode >::
        ConvertBST_To_DoubleLinkedList(TNode** pRoot)
    {
        if (nullptr == pRoot || nullptr == *pRoot)
        {
            return;
        }

        TNode* root = *pRoot;
        TNode* leftMost = const_cast<TNode*>(MinimumValueNode(root));
#ifdef _DEBUG
        ThrowIfNull(leftMost, "ConvertBST_To_DoubleLinkedList.leftMost");
#endif
        TNode* parent = nullptr;
        do
        {
            while (root->Left)
            {
                RotateRight(parent, &root);
            }

            parent = root;
            root = root->Right;
        } while (root);

        *pRoot = root = leftMost;

        while (root->Right)
        {//Repair the backward links.
            (root->Right)->Left = root;
            root = root->Right;
        }
    }

    template <typename Number, typename TNode >
    void BinarySearchTreeUtilities< Number, TNode >::RotateLeft(
        TNode* parent, TNode** pRoot)
    {
#ifdef _DEBUG
        ThrowIfNull(pRoot, "RotateLeft.pRoot");
        ThrowIfNull(*pRoot, "RotateLeft.*pRoot");
#endif
        //Note that parent can have R as right node as well.
        //
        //                             parent
        //              R                        x
        //        left           right
        //                  rightLeft rightRight
        //
        //Left rotation:
        //Before: parent(R-, x), R(left, right-), right(rightLeft-, rightRight).
        //After: parent(right+, x), R(left, rightLeft+); right(R+, rightRight) is new root.
        //
        //                             parent
        //               right                   x
        //         R           rightRight
        //     left rightLeft
        TNode* const root = *pRoot;
        TNode* const right = root->Right;
#ifdef _DEBUG
        ThrowIfNull(right, "RotateLeft.right");
#endif
        root->Right = right->Left;
        right->Left = root;

        if (parent)
        {
            if (root == parent->Left)
            {
                parent->Left = right;
            }
            else
            {
#ifdef _DEBUG
                if (root != parent->Right)
                {
                    throw std::runtime_error("Inner error in RotateLeft: root != parent->Right.");
                }
#endif
                parent->Right = right;
            }
        }

        *pRoot = right;
    }

    template <typename Number, typename TNode >
    void BinarySearchTreeUtilities< Number, TNode >::RotateRight(
        TNode* parent, TNode** pRoot)
    {
#ifdef _DEBUG
        ThrowIfNull(pRoot, "RotateRight.pRoot");
        ThrowIfNull(*pRoot, "RotateRight.*pRoot");
#endif
        //Note that parent can have R as left node as well.
        //
        //      parent
        // x              R
        //        left           right
        // leftLeft leftRight
        //
        //Right rotation:
        //Before: parent(x, R-), R(left-, right), left(leftLeft, leftRight-).
        //After: parent(x, left+), R(leftRight+, right); left(leftLeft, R+) is new root.
        //
        //    parent
        // x             left
        //       leftLeft             R
        //                   leftRight right
        TNode* const root = *pRoot;
        TNode* const left = root->Left;
#ifdef _DEBUG
        ThrowIfNull(left, "RotateRight.left");
#endif
        root->Left = left->Right;
        left->Right = root;

        if (parent)
        {
            if (root == parent->Right)
            {
                parent->Right = left;
            }
            else
            {
#ifdef _DEBUG
                if (root != parent->Left)
                {
                    throw std::runtime_error("Inner error in RotateRight: root != parent->Left.");
                }
#endif
                parent->Left = left;
            }
        }

        *pRoot = left;
    }

    template <typename Number, typename TNode>
    template<typename unique_ptr_tree>
    std::vector<Number>
        BinarySearchTreeUtilities<Number, TNode>::ConstructFromDistinctNumbers(
            const std::vector<Number>& distinct_data, unique_ptr_tree& tree)
    {
        ThrowIfEmpty(distinct_data, "distinct_data");
        const auto size = distinct_data.size();

        std::map<Number, TNode*> numbers;
        {
            auto root = new TNode(distinct_data[0]);
            tree.reset(root);

            numbers.emplace(distinct_data[0], root);
        }

        std::vector<Number> parents(size - 1);
        std::unordered_set<Number> left;

        for (size_t i = 1; i < size; ++i)
        {
            auto it = numbers.upper_bound(distinct_data[i]);
            //if (numbers.end() != it &&left.end() == left.find(*it))
            //{
            //  //left[*it] = distinct_data[i];
            //  left.insert(*it);
            //}
            //else
            //{
            //  --it;
            //  //right[*it] = distinct_data[i];
            //}
            TNode** parent_node;
            if (numbers.end() == it || !(left.insert(it->first).second))
            {
                --it;
                parent_node = &(it->second->Right);
            }
            else
            {
                parent_node = &(it->second->Left);
            }

            auto node = (*parent_node) = new TNode(distinct_data[i]);

            parents[i - 1] = it->first;
            numbers.emplace(distinct_data[i], node);
        }

        return parents;
    }
}