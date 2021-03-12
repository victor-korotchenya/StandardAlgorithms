#pragma once
#include <string>
#include <sstream>
#include <vector>
#include <algorithm>
#include "../Utilities/StreamUtilities.h"

namespace Privet::Algorithms::Trees
{
    //See also "BinaryTreeUtilities".
    class NodeUtilities final
    {
        NodeUtilities() = delete;

    public:

        template <typename TNode >
        static void CreateNodes(
            std::vector< TNode*>& nodes, const int nodesCount);

        template <typename TNode >
        static void SetLeftWithParent(
            std::vector< TNode*>& nodes, const size_t parent, const size_t left);

        template <typename TNode >
        static void SetRightWithParent(
            std::vector< TNode*>& nodes, const size_t parent, const size_t right);

        template <typename TNode >
        static void SetLeft(
            std::vector< TNode*>& nodes, const size_t parent, const size_t left);

        template <typename TNode >
        static void SetRight(
            std::vector< TNode*>& nodes, const size_t parent, const size_t right);

        //0 has 1 and 2.
        //3 has 7 and 8.
        //6 has 13 and 14.
        static size_t LeftChild(const size_t parentIndex) noexcept;

        static size_t RightChild(const size_t parentIndex) noexcept;

        //The childIndex must not be 0.
        static size_t ParentNode(const size_t childIndex);
    };

    template <typename TNode >
    void
        NodeUtilities::CreateNodes(
            std::vector<TNode*>& nodes, const int nodesCount)
    {
        nodes.reserve(nodesCount);

        for (auto i = 0; i < nodesCount; )
        {
            nodes.push_back(new TNode(i++));
        }
    }

    template <typename TNode >
    void NodeUtilities
        ::SetLeftWithParent(
            std::vector< TNode*>& nodes, const size_t parent, const size_t left)
    {
        if (parent < left)
        {
            std::ostringstream ss;
            ss << "Error: parent (" << parent << ") < left (" << left << ").";
            StreamUtilities::throw_exception(ss);
        }

        TNode* pParent = nodes[parent];
        if (pParent->Left)
        {
            std::ostringstream ss;
            ss << "Error: the left (" << pParent->Left->Value <<
                ") has already been set for parent=" << parent << ".";
            StreamUtilities::throw_exception(ss);
        }

        TNode* pLeft = nodes[left];
        if (nullptr != pLeft->Parent && pLeft->Parent != pParent)
        {
            std::ostringstream ss;
            ss << "Error: the parent (" << pLeft->Parent->Value
                << ") of left node (" << pLeft->Value
                << ") is being set to a different value (" << pParent->Value << ").";
            StreamUtilities::throw_exception(ss);
        }

        pParent->Left = pLeft;
        pLeft->Parent = pParent;
    }

    template <typename TNode >
    void NodeUtilities
        ::SetRightWithParent(
            std::vector< TNode*>& nodes, const size_t parent, const size_t right)
    {
        if (right < parent)
        {
            std::ostringstream ss;
            ss << "Error: right (" << right << ") < parent (" << parent << ").";
            StreamUtilities::throw_exception(ss);
        }

        TNode* pParent = nodes[parent];
        if (pParent->Right)
        {
            std::ostringstream ss;
            ss << "Error: the right (" << pParent->Right->Value <<
                ") has already been set for parent=" << parent << ".";
            StreamUtilities::throw_exception(ss);
        }

        TNode* pRight = nodes[right];
        if (nullptr != pRight->Parent && pRight->Parent != pParent)
        {
            std::ostringstream ss;
            ss << "Error: the parent (" << pRight->Parent->Value
                << ") of right node (" << pRight->Value
                << ") is being set to a different value (" << pParent->Value << ").";
            StreamUtilities::throw_exception(ss);
        }

        pParent->Right = pRight;
        pRight->Parent = pParent;
    }

    template <typename TNode >
    void NodeUtilities
        ::SetLeft(
            std::vector< TNode*>& nodes, const size_t parent, const size_t left)
    {
        //if (parent < left)
        //{
        //  ostringstream ss;
        //  ss << "Error: parent (" << parent << ") < left (" << left << ").";
        //  StreamUtilities::throw_exception(ss);
        //}

        TNode* pParent = nodes[parent];
        if (pParent->Left)
        {
            std::ostringstream ss;
            ss << "Error: the left (" << pParent->Left->Value <<
                ") has already been set for parent=" << parent << ".";
            StreamUtilities::throw_exception(ss);
        }

        TNode* pLeft = nodes[left];
        pParent->Left = pLeft;
    }

    template <typename TNode >
    void NodeUtilities
        ::SetRight(
            std::vector< TNode*>& nodes, const size_t parent, const size_t right)
    {
        //if (right < parent)
        //{
        //  ostringstream ss;
        //  ss << "Error: right (" << right << ") < parent (" << parent << ").";
        //  StreamUtilities::throw_exception(ss);
        //}
        TNode* pParent = nodes[parent];
        if (pParent->Right)
        {
            std::ostringstream ss;
            ss << "Error: the right (" << pParent->Right->Value <<
                ") has already been set for parent=" << parent << ".";
            StreamUtilities::throw_exception(ss);
        }

        TNode* pRight = nodes[right];
        pParent->Right = pRight;
    }
}