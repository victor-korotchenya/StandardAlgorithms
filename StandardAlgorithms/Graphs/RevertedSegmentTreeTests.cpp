#include <vector>
#include "SegmentTreeReverted.h"
#include "SegmentTree_lazy.h"
#include "../Assert.h"
#include "../Numbers/StandardOperations.h"
#include "RevertedSegmentTreeTests.h"

using namespace std;
using namespace Privet::Algorithms::Trees;
using namespace Privet::Algorithms::Numbers;
using namespace Privet::Algorithms;

namespace
{
    using Number = unsigned short;
    using TLambda = Number(*)(const Number&, const Number&);

    template<typename TTreeFactory>
    void TestHelper(
        TTreeFactory treeFactory,
        const vector<Number>& data,
        TLambda lambda,
        const size_t leftInclusive, const size_t rightExclusive,
        const Number someValue,
        const Number index, const Number expected,
        const string& name, const Number zero = {})
    {
        auto tree = treeFactory(data, lambda, zero);
        const auto max_index = tree.max_index_exclusive();
        Assert::AreEqual(data.size(), max_index, "max_index_exclusive_" + name);

        tree.set(leftInclusive, rightExclusive, someValue);
        {
            const auto actual = tree.get(index);
            Assert::AreEqual(expected, actual, "get_" + name);
        }

        //const auto leaves = tree.push_changes_to_leaves();
        //{
        //    const auto actual = tree.get(index);
        //    Assert::AreEqual(expected, actual, "push_get_" + name);
        //}
        //{
        //    const auto actual = leaves[index + tree.max_index_exclusive()];
        //    Assert::AreEqual(expected, actual, "leaves_" + name);
        //}
    }

    template<typename TTreeFactory>
    void Test(const string test_name, TTreeFactory treeFactory)
    {
        const vector<Number> data{ 1, 3, 7, 9, 12 };

        const size_t leftInclusive = 0, rightExclusive = 3;
        const size_t someIndex = 1;
        const Number someValue = 101;
        {
            const string name = test_name + "Add";
            auto lambda = Sum_unchecked<Number>;
            const auto expected = Number(lambda(someValue, data[someIndex]));

            TestHelper<TTreeFactory>(treeFactory, data, lambda,
                leftInclusive, rightExclusive, someValue,
                someIndex, expected, name);
        }
        {
            const string name = test_name + "XOR";
            auto lambda = Xor<Number>;
            const auto expected = Number(lambda(someValue, data[someIndex]));

            TestHelper<TTreeFactory>(treeFactory, data, lambda,
                leftInclusive, rightExclusive, someValue,
                someIndex, expected, name);
        }
        {
            const string name = test_name + "Max";
            auto lambda = Max<Number>;
            const auto expected = Number(lambda(someValue, data[someIndex]));

            TestHelper<TTreeFactory>(treeFactory, data, lambda,
                leftInclusive, rightExclusive, someValue,
                someIndex, expected, name);
        }
        {
            const string name = test_name + "GCD";
            auto lambda = Gcd<Number>;
            const vector<Number> data2{ 12 * 19, 36, 256 * 3 * 3 };

            const size_t someIndex2 = 2;
            const Number someValue2 = 21 * 33;
            const Number expected = 3 * 3;

            TestHelper<TTreeFactory>(treeFactory, data2, lambda,
                leftInclusive, rightExclusive, someValue2,
                someIndex2, expected, name);
        }
    }
}

void Privet::Algorithms::Trees::Tests::RevertedSegmentTreeTests()
{
    {
        auto factory = [](const vector<Number>& data,
            TLambda lambda,
            const Number zero) -> auto {
            SegmentTree_lazy<Number, TLambda> result1(data,
                lambda,
                zero,
                lambda);
            return result1;
        };
        Test("SegmentTree_lazy_", factory);
    }
    {
        auto factory = [](const vector<Number>& data,
            TLambda lambda,
            const Number zero) -> auto {
            SegmentTreeReverted<Number, TLambda> result1(data, lambda, zero);
            return result1;
        };
        Test("SegmentTreeReverted_", factory);
    }
}