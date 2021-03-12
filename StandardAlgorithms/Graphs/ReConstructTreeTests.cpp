#include <vector>
#include "BinaryTreeNode.h"
#include "BinaryTreeUtilities.h"
#include "TestNodeUtilities.h"
#include "ReConstructTreeTests.h"
#include "../Assert.h"
#include "../Utilities/StreamUtilities.h"

using namespace std;
using namespace Privet::Algorithms::Trees::Tests;
using namespace Privet::Algorithms::Trees;
using namespace Privet::Algorithms;

namespace
{
    static void RegularTest();

    static void RandomTest();

    using Number = unsigned short;
    using Numbers = std::vector< Number >;

    using TNode = BinaryTreeNode< Number >;
    using Nodes = std::vector< TNode >;

    static void BuildVisitOrders(
        const size_t size,
        const TNode* const expectedRoot,
        Numbers& preOrder,
        Numbers& inOrder,
        Numbers& postOrder);

    static void TestPostOrder(
        const TNode* const expectedRoot,
        const Numbers& postOrderFast);

    static void VefifyVisitOrders(
        const TNode* const expectedRoot,
        const Numbers& preOrderOld,
        const Numbers& inOrderOld,
        const Numbers& postOrderOld);

    static void TestTemplate(
        const Numbers& preOrder,
        const Numbers& inOrder,
        const TNode* const expectedRoot);

    void RegularTest()
    {
        const Number size = 10;

        Nodes nodes;
        nodes.resize(size + 1);

        Numbers preOrder, inOrder, postOrder;
        preOrder.resize(size);
        inOrder.resize(size);
        postOrder.resize(size);

        for (Number i = 0; i < size; ++i)
        {
            preOrder[i] = i + 1;
        }

        TNode* expectedRoot = SetValueGetNode(nodes, Number(1));

        TNode* left = expectedRoot->Left = SetValueGetNode(nodes, Number(2));
        left->Right = SetValueGetNode(nodes, Number(3));
        left->Right->Left = SetValueGetNode(nodes, Number(4));
        //                 1
        //        2                  5
        // null      3           6        7
        //        4   null             8     10
        //                          9
        TNode* right = expectedRoot->Right = SetValueGetNode(nodes, Number(5));
        right->Left = SetValueGetNode(nodes, Number(6));
        TNode* rightRight = right->Right = SetValueGetNode(nodes, Number(7));
        rightRight->Left = SetValueGetNode(nodes, Number(8));
        rightRight->Left->Left = SetValueGetNode(nodes, Number(9));
        rightRight->Right = SetValueGetNode(nodes, Number(10));

        inOrder[0] = 2; inOrder[1] = 4; inOrder[2] = 3; inOrder[3] = 1;
        inOrder[4] = 6; inOrder[5] = 5;
        inOrder[6] = 9; inOrder[7] = 8; inOrder[8] = 7; inOrder[9] = 10;

        postOrder[0] = 4; postOrder[1] = 3; postOrder[2] = 2;
        postOrder[3] = 6; postOrder[4] = 9; postOrder[5] = 8;
        postOrder[6] = 10; postOrder[7] = 7; postOrder[8] = 5; postOrder[9] = 1;

        VefifyVisitOrders(expectedRoot, preOrder, inOrder, postOrder);

        TestTemplate(preOrder, inOrder, expectedRoot);
    }

    void BuildVisitOrders(
        const size_t size,
        const TNode* const expectedRoot,
        Numbers& preOrder,
        Numbers& inOrder,
        Numbers& postOrder)
    {
        preOrder.clear();
        inOrder.clear();
        postOrder.clear();

        preOrder.reserve(size);
        inOrder.reserve(size);
        postOrder.reserve(size);

        BinaryTreeUtilities< Number, TNode >::PreOrder<>(
            expectedRoot, [&](const TNode* const theNode) -> void
            {
                ThrowIfNull(theNode, "PreOrder.theNode");
                preOrder.push_back(theNode->Value);
            });
        Assert::AreEqual(size, preOrder.size(), "preOrder.size");

        BinaryTreeUtilities< Number, TNode >::InOrder<>(
            expectedRoot, [&](const TNode* const theNode) -> void
            {
                ThrowIfNull(theNode, "InOrder.theNode");
                inOrder.push_back(theNode->Value);
            });
        Assert::AreEqual(size, inOrder.size(), "inOrder.size");

        BinaryTreeUtilities< Number, TNode >::PostOrder<>(
            expectedRoot, [&](const TNode* const theNode) -> void
            {
                ThrowIfNull(theNode, "PostOrder.theNode");
                postOrder.push_back(theNode->Value);
            });
        Assert::AreEqual(size, postOrder.size(), "postOrder.size");

        TestPostOrder(expectedRoot, postOrder);
    }

    void TestPostOrder(
        const TNode* const expectedRoot,
        const Numbers& postOrderFast)
    {
        const size_t size = postOrderFast.size();

        Numbers postOrderSlow;
        postOrderSlow.reserve(size);

        BinaryTreeUtilities< Number, TNode >::PostOrderSlow<>(
            expectedRoot, [&](const TNode* const theNode) -> void
            {
                ThrowIfNull(theNode, "PostOrder.theNode");
                postOrderSlow.push_back(theNode->Value);
            });
        Assert::AreEqual(postOrderFast, postOrderSlow, "post-order slow and fast.");
    }

    void VefifyVisitOrders(
        const TNode* const expectedRoot,
        const Numbers& preOrderOld,
        const Numbers& inOrderOld,
        const Numbers& postOrderOld)
    {
        Assert::AreEqual(preOrderOld.size(), inOrderOld.size(),
            "Sizes of preOrderOld and inOrderOld");
        Assert::AreEqual(preOrderOld.size(), postOrderOld.size(),
            "Sizes of preOrderOld and postOrderOld");

        Numbers preOrderNew, inOrderNew, postOrderNew;

        BuildVisitOrders(preOrderOld.size(), expectedRoot,
            preOrderNew, inOrderNew, postOrderNew);

        const string sufffix = "-order old and new.";

        Assert::AreEqual(preOrderOld, preOrderNew, "pre" + sufffix);
        Assert::AreEqual(inOrderOld, inOrderNew, "in" + sufffix);
        Assert::AreEqual(postOrderOld, postOrderNew, "post" + sufffix);
    }

    void RandomTest()
    {
        const Number minSize = 10;
        const Number maxSize = 20;
        const Number size = Number(UnsignedIntRandom(minSize, maxSize)());

        TNode* expectedRoot = nullptr;
        try
        {
            const bool isUniqueOnly = true;
            BinaryTreeUtilities< Number, TNode >::BuildRandomTree(
                size, &expectedRoot, size << 2, isUniqueOnly);

            Numbers preOrder, inOrder, postOrder;
            BuildVisitOrders(size, expectedRoot, preOrder, inOrder, postOrder);

            TestTemplate(preOrder, inOrder, expectedRoot);
        }
        catch (...)
        {
            FreeTree(&expectedRoot);
            throw;
        }

        FreeTree(&expectedRoot);
    }

    void TestTemplate(
        const Numbers& preOrder,
        const Numbers& inOrder,
        const TNode* const expectedRoot)
    {
        const size_t size = preOrder.size();
        Assert::AreEqual(size, inOrder.size(), "Pre- and in-Order sizes");

        TNode* actualRoot = nullptr;
        try
        {
            BinaryTreeUtilities< Number, TNode >::ReConstructTree(
                preOrder, inOrder,
                size,
                &actualRoot);
            if (nullptr == actualRoot)
            {
                throw exception("The root must be not null after tree creation.");
            }
            Assert::AreEqual(preOrder[0], actualRoot->Value, "Root value.");

            const auto actual = BinaryTreeUtilities< Number >::AreIsomorphic(
                expectedRoot, actualRoot);

            RequireIsomorphic(actual);
        }
        catch (...)
        {
            FreeTree(&actualRoot);
            throw;
        }

        FreeTree(&actualRoot);
    }
}

void Privet::Algorithms::Trees::Tests::ReConstructTreeTests()
{
    using TPair = pair< void(*)(), string>;

    const TPair pairs[] = {
      TPair(RandomTest, "RandomTest"),
      TPair(RegularTest, "RegularTest"),
    };
    const size_t pairsSize = sizeof(pairs) / sizeof(TPair);

    for (size_t i = 0; i < pairsSize; ++i)
    {
        const TPair& p = pairs[i];

        //TODO: p2. use test library.
        try
        {
            p.first();
        }
        catch (const exception& ex)
        {
            ostringstream ss;
            ss << "Error in " << p.second << ": " << ex.what();
            StreamUtilities::throw_exception(ss);
        }
    }
}