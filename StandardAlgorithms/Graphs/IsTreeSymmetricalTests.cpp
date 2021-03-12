#include "BinaryTreeNode.h"
#include "BinaryTreeUtilities.h"
#include "IsTreeSymmetricalTests.h"
#include "../test_utilities.h"

using namespace std;
using namespace Privet::Algorithms::Trees;
using namespace Privet::Algorithms::Tests;
using namespace Privet::Algorithms;

namespace
{
    using Number = unsigned;

    using TNode = Privet::Algorithms::Trees::BinaryTreeNode< Number >;

    struct TestCase final : base_test_case
    {
        TNode* Root{};
        bool Expected{};

        TestCase(std::string&& name = "")
            : base_test_case(forward<string>(name))
        {
        }

        void Print(std::ostream& str) const override
        {
            base_test_case::Print(str);
            str << " Expected=" << Expected;
        }
    };

    void GenerateTestCases(vector<TestCase>& testCases, vector<TNode>& nodes)
    {
        const size_t size = 20;
        nodes.resize(size);

        unsigned index = 0;
        {
            TestCase testCase("Trivial");
            testCase.Root = &nodes[index];
            testCase.Expected = true;
            testCases.push_back(testCase);

            ++index;
        }
        {
            TestCase testCase("Not symmetric, 2 nodes");
            testCase.Root = &nodes[index];
            testCase.Root->Left = &nodes[index + 1llu];
            testCase.Expected = false;
            testCases.push_back(testCase);

            index += 2;
        }
        {
            TestCase testCase("Symmetric, 5 nodes");
            testCase.Root = &nodes[index];
            testCase.Root->Left = &nodes[index + 1llu];
            testCase.Root->Right = &nodes[index + 2llu];
            testCase.Root->Left->Right = &nodes[index + 3llu];
            testCase.Root->Right->Left = &nodes[index + 4llu];

            testCase.Root->Value = index;
            testCase.Root->Left->Value =
                testCase.Root->Right->Value = index + 2;
            testCase.Root->Left->Right->Value =
                testCase.Root->Right->Left->Value = index + 3;
            testCase.Expected = true;
            testCases.push_back(testCase);

            index += 5;
        }
        {
            TestCase testCase("Not symmetric, 5 nodes");
            testCase.Root = &nodes[index];
            testCase.Root->Left = &nodes[index + 1llu];
            testCase.Root->Right = &nodes[index + 2llu];
            testCase.Root->Left->Right = &nodes[index + 3llu];
            testCase.Root->Right->Left = &nodes[index + 4llu];

            testCase.Root->Value = index;
            testCase.Root->Left->Value =
                testCase.Root->Right->Value = index + 2;
            testCase.Root->Left->Right->Value = index + 3;
            testCase.Root->Right->Left->Value = index + 2;
            testCase.Expected = false;
            testCases.push_back(testCase);

            index += 5;
        }

        if (size <= index)
        {
            throw exception("Increase the size: size <= index");
        }
    }

    void RunTestCase(const TestCase& testCase)
    {
        const bool actual = BinaryTreeUtilities< Number, TNode >
            ::IsTreeSymmetrical(testCase.Root);

        Assert::AreEqual(testCase.Expected, actual, testCase.get_Name());
    }
}

void IsTreeSymmetricalTests()
{
    vector<TNode> nodes;

    test_utilities<TestCase, vector<TNode>&>::run_tests(RunTestCase, GenerateTestCases, nodes);
}