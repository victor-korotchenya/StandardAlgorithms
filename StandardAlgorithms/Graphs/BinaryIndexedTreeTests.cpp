#include <algorithm>
#include <tuple>
#include <vector>
#include "BinaryIndexedTree.h"
#include "../Assert.h"
#include "../Numbers/StandardOperations.h"
#include "../test_utilities.h"
#include "BinaryIndexedTreeTests.h"

using namespace std;
using namespace Privet::Algorithms::Trees;
using namespace Privet::Algorithms::Numbers;
using namespace Privet::Algorithms::Tests;
using namespace Privet::Algorithms;

namespace
{
    using Number = int64_t;
    using Operation_t = Number(*)(const Number&, const Number&);
    using Tree = BinaryIndexedTree<Number, Operation_t>;

    //leftIndex, rightIndex, before, after.
    using Request = tuple<size_t, size_t, Number, Number>;

    struct TestCase final : base_test_case
    {
        vector<Request> Requests;
        Operation_t Operation;
        Operation_t ReverseOperation;
        Number Zero;

        // ReSharper disable once CppNonExplicitConvertingConstructor
        TestCase(string&& name = "",
            vector<Request> requests = {},
            Operation_t operation = {},
            Operation_t reverseOperation = {},
            Number zero = {})
            :
            base_test_case(forward<string>(name)),
            Requests(forward<vector<Request>>(requests)),
            Operation(operation),
            ReverseOperation(reverseOperation),
            Zero(zero)
        {
        }

        void Print(ostream& str) const override
        {
            base_test_case::Print(str);

            str << ", Zero=" << Zero;
        }
    };

    void MakeChanges(Tree& tree)
    {
        tree.set(1, 31);
        tree.set(3, 522);
        tree.set(6, 7803);
    }

    void GenerateTestCases(vector<TestCase>& testCases)
    {
        constexpr Number moduloSum = 5;
        constexpr Number moduloProd = 23;

        //Indexes: 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12.
        //    Sum: 2, 1, 1, 1,  ,  , 1,  ,  ,   ,   ,  1.
        //    Xor: 0, 1, 1, 1,  ,  , 1,  ,  ,   ,   ,  1.
        // SumMod: 2, 1, 1, 1,  ,  , 1,  ,  ,   ,   ,  1.
        //ProdMod: 1, 1, 1, 1,  ,  , 1,  ,  ,   ,   ,  1.

        //Indexes: 1, 2,  3, 4, 5,   6, 7, 8, 9, 10, 11, 12.
        //   Sum: 33, 1,523, 1,  ,7803, 1,  ,  ,   ,   ,  1.
        //   Xor: 31, 1,523, 1,  ,7803, 1,  ,  ,   ,   ,  1.
        //ProdMod: 8, 1, 16, 1,  ,   6, 1,  ,  ,   ,   ,  1.

        testCases = {
          { "Sum",{
            Request(0, 1, 2, 33),
            Request(1, 1, 2, 33),
            Request(2, 3, 2, 524),
            Request(2, 6, 3, 8328),
            Request(5, 5, 0, 0),
            Request(5, 6, 0, 7803),
            Request(6, 6, 0, 7803),
            Request(7, 7, 1, 1),
            Request(1, 12, 7, 8363)
          }, Sum_unchecked<Number>, Subtract_unchecked<Number> },

          { "Xor",{
            Request(0, 1, 0, 31),
            Request(1, 1, 0, 31),
            Request(2, 3, 0, 522),
            Request(2, 6, 1, 7280),
            Request(5, 5, 0, 0),
            Request(5, 6, 0, 7803),
            Request(6, 6, 0, 7803),
            Request(7, 7, 1, 1),
            Request(1, 12, 1, 7279)
          }, Xor<Number>, Xor<Number> },

          { "SumModulo",{
            Request(0, 1, 2, 3),
            Request(1, 1, 2, 3),
            Request(2, 3, 2, 4),
            Request(2, 6, 3, 3),
            Request(5, 5, 0, 0),
            Request(5, 6, 0, 3),
            Request(6, 6, 0, 3),
            Request(7, 7, 1, 1),
            Request(1, 12, 2, 3)
          }, SumModulo_unchecked<Number, moduloSum>, SubtractModulo_unchecked<Number, moduloSum> },

          { "ProductModulo",{
            Request(0, 1, 1, 8),
            Request(1, 1, 1, 8),
            Request(2, 3, 1, 16),
            Request(2, 6, 1, 4),
            Request(5, 5, 1, 1),
            Request(5, 6, 1, 6),
            Request(6, 6, 1, 6),
            Request(7, 7, 1, 1),
            Request(1, 12, 1, 9)
          }, ProductModulo_unchecked<Number, moduloProd>,
          DivideModulo_unchecked<Number, moduloProd>, 1 }
        };
    }

    Tree CreateTree(const TestCase& testCase)
    {
        const vector<size_t> indexesToAdd{ 1, 2, 4, 1, 7, 3, 12 };
        const auto maxIndex = *max_element(indexesToAdd.cbegin(), indexesToAdd.cend());

        Tree result(maxIndex,
            testCase.Operation, testCase.ReverseOperation, testCase.Zero);

        for (const auto& index : indexesToAdd)
        {
            result.set(index);
        }

        return result;
    }

    void CheckIndexOutOfRange(const Tree& tree, const string& name)
    {
        const auto maxIndex = tree.max_index();
        const auto tooLargeIndex = 123456789;
        const auto expectedMessage = "The index (123456789) must be between 1 and "
            + to_string(maxIndex) + ".";
        const auto message = string(__FUNCTION__) + "_" + name;

        Assert::ExpectException<out_of_range>(
            [&]() -> void {
                tree.get(tooLargeIndex, tooLargeIndex);
            },
            expectedMessage, message);
    }

    void TestValueAt(const Tree& tree, const string& stepName)
    {
        const auto max_index = tree.max_index();
        Assert::Greater(max_index, Tree::InitialIndex, "max_index");

        const auto prefix = stepName + "_at_";

        for (auto i = Tree::InitialIndex; i <= max_index; ++i)
        {
            const auto expected = tree.value_at(i);
            const auto actual = tree.get(i, i);
            const auto name = prefix + to_string(i);
            Assert::AreEqual(expected, actual, name);
        }
    }

    void CheckRequest(const Tree& tree, const int id,
        const Request& request,
        const string& stepName)
    {
        const auto& left = get<0>(request);
        const auto& right = get<1>(request);
        const auto& expected = id ? get<3>(request) : get<2>(request);

        const string separator = ", ";
        const auto name = to_string(left) + separator + to_string(right)
            + separator + stepName;

        const auto actual = tree.get(left, right);
        Assert::AreEqual(expected, actual, name);
    }

    void CheckRequests(
        const Tree& tree,
        const vector<Request>& requests,
        const int id, const string& stepName)
    {
        Assert::NotEmpty(requests, stepName);

        for (const auto& request : requests)
        {
            CheckRequest(tree, id, request, stepName);
        }

        TestValueAt(tree, stepName);
    }

    void RunTestCase(const TestCase& testCase)
    {
        auto tree = CreateTree(testCase);
        const auto& name = testCase.get_Name();

        CheckIndexOutOfRange(tree, name);
        {
            CheckRequests(tree, testCase.Requests, 0, "before_" + name);
        }
        {
            MakeChanges(tree);

            CheckRequests(tree, testCase.Requests, 1, "after_" + name);
        }
    }
}

void Privet::Algorithms::Trees::Tests::BinaryIndexedTreeTests()
{
    test_utilities<TestCase>::run_tests(RunTestCase, GenerateTestCases);
}