#include <memory>//unique_ptr
#include <type_traits>
#include <vector>
#include "../test_utilities.h"
#include "BinarySearchTreeUtilities.h"
#include "BinaryTreeUtilities.h"
#include "../Utilities/PrintUtilities.h"
#include "ConstructFromDistinctNumbersTests.h"
#include "TestNodeUtilities.h"

using namespace std;
using namespace Privet::Algorithms::Trees;
using namespace Privet::Algorithms::Tests;
using namespace Privet::Algorithms;

namespace
{
    using Number = short;
    using node_t = BinaryTreeNode<Number>;
    using unique_ptr_tree = unique_ptr<node_t, void(*)(node_t*)>;

    unique_ptr_tree CreateEmptyTree()
    {
        return unique_ptr_tree(nullptr, FreeTree<node_t>);
    }

    node_t* AddNew(const Number value, vector<unique_ptr_tree>& trees)
    {
        const auto new_size = static_cast<size_t>(trees.size() * 1.1) + 1;
        trees.reserve(new_size);

        auto result = new node_t(value);
        trees.emplace_back(unique_ptr_tree(result, FreeTree<node_t>));

        return result;
    }

    struct TestCase final : base_test_case
    {
        node_t* ExpectedTree;
        vector<Number> Data;
        vector<Number> ExpectedParents;

        TestCase(string&& name,
            vector<Number>&& data,
            vector<Number>&& expectedParents,
            node_t* expectedTree)
            :
            base_test_case(forward<string>(name)),
            ExpectedTree(ThrowIfNull(expectedTree, "expectedTree")),
            Data(forward<vector<Number>>(data)),
            ExpectedParents(forward<vector<Number>>(expectedParents))
        {
        }

        void Validate() const override
        {
            base_test_case::Validate();
            ThrowIfNull(ExpectedTree, "ExpectedTree");
            Assert::AreEqual(Data.size(), ExpectedParents.size() + 1, "Data.size " + get_Name());
        }

        void Print(ostream& str) const override
        {
            base_test_case::Print(str);
            ::Print(", Data=", Data, str);
            ::Print(", ExpectedParents=", ExpectedParents, str);
        }
    };

    void GenerateTestCases(vector<TestCase>& testCases, vector<unique_ptr_tree>& trees)
    {
        for (Number value = 0; value <= 1; ++value)
        {
            testCases.emplace_back("Simple" + to_string(value),
                initializer_list<Number>({ value }), initializer_list<Number>(),
                AddNew(value, trees));
        }

        {
            const auto root = AddNew(2, trees);
            root->Left = new node_t(1);
            root->Right = new node_t(4);
            root->Right->Right = new node_t(6);
            root->Right->Right->Left = new node_t(5);

            testCases.emplace_back("Few nodes"s,
                initializer_list<Number>({ 2, 4, 1, 6, 5 }),
                initializer_list<Number>({ 2, 2, 4, 6 }),
                root);
        }
    }

    void RunTestCase(const TestCase& testCase)
    {
        auto tree = CreateEmptyTree();

        const auto parents = BinarySearchTreeUtilities<Number>::ConstructFromDistinctNumbers(testCase.Data, tree);
        Assert::AreEqual(testCase.ExpectedParents, parents, "Parents_" + testCase.get_Name());

        const auto actual = BinaryTreeUtilities<Number>::AreIsomorphic(testCase.ExpectedTree, tree.get());
        RequireIsomorphic(actual, "Tree_" + testCase.get_Name());
    }
}

void Privet::Algorithms::Trees::Tests::ConstructFromDistinctNumbersTests()
{
    vector<unique_ptr_tree> trees;
    test_utilities<TestCase, vector<unique_ptr_tree>&>::run_tests(RunTestCase, GenerateTestCases, trees);
}