#include <vector>
#include "InOrderThreadedNode.h"
#include "InOrderThreadedNodeTests.h"
#include "NodeUtilities.h"
#include "../Assert.h"

using namespace std;
using namespace Privet::Algorithms::Trees;
using namespace Privet::Algorithms;

namespace
{
    using TData = short;
    using TNode = InOrderThreadedNode< TData >;

    std::vector< int > Visited;

    void BuildBinaryTree(const int size, vector< TNode >& nodes, vector< int >& inorderTraversal)
    {
        nodes.clear();
        nodes.resize(size);

        for (int i = 0; i < size; i++)
        {
            nodes[i].Datum = static_cast<TData>(i);

            const auto leftIndex = NodeUtilities::LeftChild(i);
            if (leftIndex < size)
            {
                nodes[i].SetLeft(&nodes[leftIndex]);

                const auto rightIndex = NodeUtilities::RightChild(i);
                if (rightIndex < size)
                {
                    nodes[i].SetRight(&nodes[rightIndex]);
                }
            }
        }

        Assert::AreEqual(10, size, "Fix the In-order traversal.");

        //       0
        //    1       2
        //  3   4    5 6
        // 7 8 9 10
        inorderTraversal.clear();
        inorderTraversal.reserve(size);
        inorderTraversal.push_back(7);
        inorderTraversal.push_back(3);
        inorderTraversal.push_back(8);
        inorderTraversal.push_back(1);

        inorderTraversal.push_back(9);
        inorderTraversal.push_back(4);
        inorderTraversal.push_back(0);

        inorderTraversal.push_back(5);
        inorderTraversal.push_back(2);
        inorderTraversal.push_back(6);
    }

    void CheckThreadedBinaryTree(const int size, const vector< TNode >& nodes, const vector< int >& inorderTraversal)
    {
        const int absentIndex = -1;
        int previousIndex = absentIndex;

        for (int i = 0; i < size; i++)
        {
            Assert::AreEqual<int>(i, nodes[i].Datum, "Datum after construction.");

            const string indexAsString = to_string(i);
            {//Left
                const auto childIndex = NodeUtilities::LeftChild(i);
                TNode* expected = childIndex < size
                    ? const_cast<TNode*>(&nodes[childIndex]) : nullptr;

                TNode* actual = nodes[i].GetLeft();
                const string error = indexAsString + "->Left.";
                Assert::AreEqualByPtr< TNode >(expected, actual, error);
            }
            {//Right
                const auto childIndex = NodeUtilities::RightChild(i);
                TNode* expected = childIndex < size
                    ? const_cast<TNode*>(&nodes[childIndex]) : nullptr;

                TNode* actual = nodes[i].GetRight();
                const string error = indexAsString + "->Right.";
                Assert::AreEqualByPtr< TNode >(expected, actual, error);
            }

            const int currentIndex = inorderTraversal[i];
            const string currentIndexAsString = to_string(currentIndex);
            {
                TNode* expected = absentIndex == previousIndex
                    || nullptr != nodes[currentIndex].GetLeft()
                    ? nullptr : const_cast<TNode*>(&nodes[previousIndex]);

                TNode* actual = nodes[currentIndex].Predecessor();
                const string error = currentIndexAsString + "->Predecessor.";
                Assert::AreEqualByPtr< TNode >(expected, actual, error);

                previousIndex = currentIndex;
            }
            {
                TNode* expected = (i + 1) < size
                    && nullptr == nodes[currentIndex].GetRight()
                    ? const_cast<TNode*>(&nodes[inorderTraversal[i + 1]])
                    : nullptr;

                TNode* actual = nodes[currentIndex].Successor();
                const string error = currentIndexAsString + "->Successor.";
                Assert::AreEqualByPtr< TNode >(expected, actual, error);
            }
        }
    }

    void Visit(const TNode* const previous, const TNode* const current)
    {
        if (nullptr == current)
        {
            throw exception("nullptr == current in InOrderThreadedNodeTests::Visit.");
        }

        Visited.push_back(current->Datum);

        if (nullptr != previous)
        {
            if (nullptr == previous->GetRight())
            {
                TNode* const nextExpected = previous->Successor();
                Assert::AreEqualByPtr(nextExpected, current, "Successor in Visit test.");
            }
        }

        if (nullptr == current->GetLeft())
        {
            TNode* const previousExpected = current->Predecessor();
            Assert::AreEqualByPtr(previousExpected, previous, "Predecessor in Visit test.");
        }
    }

    void TestVisits(const int size, const vector< TNode >& nodes, const vector< int >& inorderTraversal)
    {
        Visited.clear();
        Assert::AreEqual<size_t>(size, inorderTraversal.size(), "inorderTraversal.size()");

        const TNode& root = nodes[0];

        root.InOrderVisitSlow(Visit);
        Assert::AreEqual(inorderTraversal, Visited, "InOrderVisitSlow.");

        Visited.clear();
        root.InOrderVisitFast(Visit);
        Assert::AreEqual(inorderTraversal, Visited, "InOrderVisitFast.");
    }
}

void Privet::Algorithms::Trees::Tests::InOrderThreadedNodeTests()
{
    const int size = 10;
    vector< TNode > nodes;
    vector< int > inorderTraversal;

    BuildBinaryTree(size, nodes, inorderTraversal);

    TNode& root = nodes[0];
    root.InOrderVisitAndSetTags();

    CheckThreadedBinaryTree(size, nodes, inorderTraversal);

    TestVisits(size, nodes, inorderTraversal);
}