#include "BinaryTreeUtilities.h"
#include "BinarySearchTreeUtilities.h"
#include "TestNodeUtilities.h"
#include "DoubleLinkedListUtilities.h"
#include "BST_To_DoubleLinkedListTests.h"
#include "../Assert.h"

using namespace std;
using namespace Privet::Algorithms::Trees;
using namespace Privet::Algorithms;

namespace
{
    using Number = long;
    using Node = BinaryTreeNode<Number>;

    void BuildTree(vector<Node>& nodes)
    {
        nodes.clear();

        const int size = 50;
        nodes.resize(size);

        auto root = &nodes[0];
        root->Value = 20;

        auto left = root->Left = SetValueGetNode(nodes, 10);
        auto right = root->Right = SetValueGetNode(nodes, 30);

        //        20
        //    10            30
        //  5           25      40
        //   7      null  28
        left->Left = SetValueGetNode(nodes, 5);
        left->Left->Right = SetValueGetNode(nodes, 7);

        right->Left = SetValueGetNode(nodes, 25);
        right->Right = SetValueGetNode(nodes, 40);

        right->Left->Right = SetValueGetNode(nodes, 28);
    }

    void BuildResult(vector< Node >& nodes)
    {
        nodes.clear();

        const Number values[] = { 5, 7, 10, 20, 25, 28, 30, 40, };
        const size_t valuesSize = sizeof(values) / sizeof(Number);

        nodes.resize(valuesSize);

        for (size_t i = 0; i < valuesSize; ++i)
        {
            auto node = &nodes[i];

            node->Value = values[i];

            if (i)
            {
                node->Left = &nodes[i - 1];
            }

            if (i + 1 < valuesSize)
            {
                node->Right = &nodes[i + 1];
            }
        }
    }
}

void Privet::Algorithms::Trees::Tests::BST_To_DoubleLinkedListTests()
{
    vector< Node > nodes;
    BuildTree(nodes);
    Assert::NotEmpty(nodes, "nodes");

    Node* root = &nodes[0];
    Assert::AreEqual(true,
        BinarySearchTreeUtilities< Number, Node >::IsBSTWithoutDuplicates(root),
        "Is first tree BST?");

    vector< Node > nodes2;
    BuildTree(nodes2);
    Assert::NotEmpty(nodes2, "nodes2");
    Assert::AreEqual< Node >(nodes, nodes2, "nodes and its clone");

    Node* root2 = &nodes2[0];

    {
        const auto inputEqual = BinaryTreeUtilities< Number, Node >::AreIsomorphic(root, root2);

        RequireIsomorphic(inputEqual, "Input tree and its clone must be the same.");
    }

    BinarySearchTreeUtilities< Number, Node >::
        ConvertBST_To_DoubleLinkedListSlow(&root);

    vector< Node > nodesResult;
    BuildResult(nodesResult);
    Assert::NotEmpty(nodesResult, "nodesResult");

    {
        const auto outputEqual = DoubleLinkedListUtilities< Number, Node>::AreIsomorphic(&nodesResult[0], root);

        RequireIsomorphic(outputEqual, "Expected tree must be equal to slow tree.");
    }

    BinarySearchTreeUtilities< Number, Node >::ConvertBST_To_DoubleLinkedList(&root2);

    {
        const auto outputEqual = DoubleLinkedListUtilities<Number, Node>::AreIsomorphic(root, root2);

        RequireIsomorphic(outputEqual, "Slow tree must be equal to fast one.");
    }
}