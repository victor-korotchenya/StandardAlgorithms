#include <string>
#include "BinaryTreeNode.h"
#include <memory>
#include "../Utilities/ExceptionUtilities.h"
#include "BinaryTreeUtilities.h"
#include "BinarySearchTreeUtilities.h"
#include "IsBinarySearchTreeTests.h"
#include "../Assert.h"

using namespace std;
using namespace Privet::Algorithms::Trees;
using namespace Privet::Algorithms;

namespace
{
    using Number = long int;

    using TNode = Privet::Algorithms::Trees::BinaryTreeNode< Number >;

    static void CreateTree(TNode* root);

    static void RunTest(
        const std::string& name,
        const TNode* const root,
        const bool expected);

    static void LeftRightGreater(TNode* const root);
    static void RightLeftLeftSmaller(TNode* const root);

    static void BreakNode(TNode* const root);

    void CreateTree(TNode* root)
    {
        ThrowIfNull(root, "root");

        root->Value = 5;

        //left subtree.
        root->Left = new TNode(2);
        root->Left->Left = new TNode(1);
        root->Left->Right = new TNode(4);

        //
        //            5
        //       2           10
        //     1   4       7       12
        //               6   8   11   20
        //                           18

        //right subtree.
        TNode* rightNode = new TNode(10);
        root->Right = rightNode;

        rightNode->Left = new TNode(7);
        rightNode->Left->Left = new TNode(6);
        rightNode->Left->Right = new TNode(8);

        rightNode->Right = new TNode(12);
        rightNode->Right->Left = new TNode(11);
        rightNode->Right->Right = new TNode(20);
        rightNode->Right->Right->Left = new TNode(18);
    }

    void RunTest(
        const string& name,
        const TNode* const root,
        const bool expected)
    {
        {
            const bool actual = BinarySearchTreeUtilities< Number >
                ::IsBSTWithoutDuplicates(root);
            Assert::AreEqual(expected, actual, name);
        }
        {
            const bool actual = BinarySearchTreeUtilities< Number >
                ::IsBSTDuplicatesAllowed(root);
            Assert::AreEqual(expected, actual, name);
        }
    }

    void LeftRightGreater(TNode* const root)
    {
        TNode* const node = root->Left->Right;

        const auto old = 4;
        if (old != node->Value)
        {
            throw runtime_error("LeftRightGreater: precondition failed.");
        }

        node->Value = 6;
        RunTest("LeftRightGreater0", root, false);

        node->Value = old;
        RunTest("LeftRightGreater1", root, true);
    }

    void RightLeftLeftSmaller(TNode* const root)
    {
        TNode* const node = root->Right->Left->Left;

        const auto old = 6;
        if (old != node->Value)
        {
            throw runtime_error("RightLeftLeftSmaller: precondition failed.");
        }

        node->Value = 3;
        RunTest("RightLeftLeftSmaller0", root, false);

        node->Value = old;
        RunTest("RightLeftLeftSmaller1", root, true);
    }

    void BreakNode(TNode* const root)
    {
        TNode* const node = root->Right->Right->Right->Left;

        const auto old = 18;
        if (old != node->Value)
        {
            throw runtime_error("BreakNode: precondition failed.");
        }

        node->Value = 22;
        RunTest("BreakNode0", root, false);

        node->Value = old;
        RunTest("BreakNode1", root, true);
    }
}

void Privet::Algorithms::Trees::Tests::IsBinarySearchTreeTests()
{
    unique_ptr< TNode, void(*)(TNode*) > uniqueRoot(new TNode(),
        FreeTree<TNode>);
    if (!uniqueRoot)
    {
        throw exception("Cannot create root node.");
    }

    TNode* root = uniqueRoot.get();

    CreateTree(root);
    RunTest("After creation", root, true);

    LeftRightGreater(root);
    RightLeftLeftSmaller(root);

    BreakNode(root);
}