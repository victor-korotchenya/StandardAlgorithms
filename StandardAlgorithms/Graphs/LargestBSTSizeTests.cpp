#include "../Numbers/Arithmetic.h"
#include "../Utilities/TestNameUtilities.h"
#include "../Utilities/StreamUtilities.h"
#include "BinarySearchTreeUtilities.h"
#include "LargestBSTSizeTests.h"

#include <vector>
#include "BinaryTreeNode.h"
#include "../test_utilities.h"

using namespace std;
using namespace Privet::Algorithms::Tests;
using namespace Privet::Algorithms::Trees;
using namespace Privet::Algorithms;

namespace
{
    using TData = short;
    using TNode = BinaryTreeNode< TData >;
    using TSizeIndex = std::pair<size_t, int>;

    constexpr auto MaxAllowedSize = 100;

    struct TestCase final : base_test_case
    {
        std::vector< TNode >* Nodes;
        TNode* Root;
        TSizeIndex Expected;

        TestCase(
            std::string&& name = "",
            std::vector< TNode >* nodes = nullptr,
            TNode* root = nullptr,
            TSizeIndex expected = {})
            : base_test_case(forward<string>(name)),
            Nodes(nodes), Root(root), Expected(expected)
        {
        }

        void Print(std::ostream& str) const override
        {
            base_test_case::Print(str);
            str << ", Expected=" << Expected;
        }
    };

    void FixTestCases(vector<TNode>& nodes, vector<TestCase>& testCases)
    {
        for (TestCase& testCase : testCases)
        {
            testCase.Nodes = &nodes;
        }
    }

    void IncreaseIndex(int& index, const int numberOfUsedNodes)
    {
        if (numberOfUsedNodes <= 0)
        {
            ostringstream ss;
            ss << "The numberOfUsedNodes(" << numberOfUsedNodes
                << ") must be positive.";
            StreamUtilities::throw_exception(ss);
        }

        index = AddSigned(index, numberOfUsedNodes);

        if (MaxAllowedSize <= index)
        {
            ostringstream ss;
            ss << "The index(" << index <<
                ") must not exceed " << MaxAllowedSize
                << " - please increase the size.";
            StreamUtilities::throw_exception(ss);
        }
    }

    void GenerateTestCases(vector<TestCase>& testCases, vector<TNode>& nodes)
    {
        nodes.clear();
        nodes.resize(MaxAllowedSize);
        int index = 0;

        {
            TestCase testCase("8 nodes, root");
            //             2
            //        -10        345
            //   -25     1     10  509
            //null -15
            testCase.Root = &nodes[index];
            TNode* leftNode = &nodes[index + 1];
            TNode* rightNode = &nodes[index + 2];
            testCase.Root->Left = leftNode;
            testCase.Root->Right = rightNode;
            testCase.Root->Value = 2;
            leftNode->Value = -10;
            rightNode->Value = 345;

            leftNode->Left = &nodes[index + 3];
            leftNode->Right = &nodes[index + 4];
            leftNode->Left->Value = -25;
            leftNode->Right->Value = 1;

            rightNode->Left = &nodes[index + 5];
            rightNode->Right = &nodes[index + 6];
            rightNode->Left->Value = 10;
            rightNode->Right->Value = 509;

            leftNode->Left->Right = &nodes[index + 7];
            leftNode->Left->Right->Value = -15;

            testCase.Expected = TSizeIndex(8, index);

            testCases.push_back(testCase);
            IncreaseIndex(index, 8);
        }
        {
            TestCase testCase("Root is not BST");
            //         2
            //    -10      45
            // -25   7   -1
            testCase.Root = &nodes[index];
            TNode* leftNode = &nodes[index + 1];
            TNode* rightNode = &nodes[index + 2];
            testCase.Root->Left = leftNode;
            testCase.Root->Right = rightNode;
            testCase.Root->Value = 2;
            leftNode->Value = -10;
            rightNode->Value = 45;

            leftNode->Left = &nodes[index + 3];
            leftNode->Right = &nodes[index + 4];
            leftNode->Left->Value = -25;
            leftNode->Right->Value = testCase.Root->Value + 5;

            rightNode->Left = &nodes[index + 5];
            rightNode->Left->Value = testCase.Root->Value - 3;

            testCase.Expected = TSizeIndex(3, index + 1);

            testCases.push_back(testCase);
            IncreaseIndex(index, 6);
        }

        //Must be last.
        FixTestCases(nodes, testCases);
    }

    TSizeIndex ConvertToSizeIndex(const vector< TNode >& nodes, const pair< size_t, TNode* >& value)
    {
        int index = 0;

        for (const TNode& node : nodes)
        {
            if (&node == value.second)
            {
                break;
            }

            ++index;
        }

        if (nodes.size() == index)
        {
            index = -1;
        }

        const TSizeIndex result = TSizeIndex(value.first, index);
        return result;
    }

    void RunTestCase(const TestCase& testCase)
    {
        const pair< size_t, TNode* > actualRaw = BinarySearchTreeUtilities< TData, TNode >::LargestBinarySearchTreeSize(testCase.Root);

        const TSizeIndex actual = ConvertToSizeIndex(*(testCase.Nodes), actualRaw);
        Assert::AreEqual(testCase.Expected, actual, testCase.get_Name());
    }
}

void Privet::Algorithms::Trees::Tests::LargestBSTSizeTests()
{
    vector<TNode> nodes;

    test_utilities<TestCase, vector<TNode>&>::run_tests(RunTestCase, GenerateTestCases, nodes);
}