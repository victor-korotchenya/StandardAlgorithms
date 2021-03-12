#include <numeric> //accumulate
#include <vector>
#include "SegmentTree.h"
#include "SegmentTree_lazy.h"
#include "../Assert.h"
#include "../Numbers/StandardOperations.h"
#include "SegmentTreeTests.h"

using namespace std;
using namespace Privet::Algorithms::Trees;
using namespace Privet::Algorithms::Numbers;
using namespace Privet::Algorithms;

namespace
{
    using Number = unsigned short;
    using TLambda = Number(*)(const Number&, const Number&);

    template<typename TTree>
    void TestHelper(
        const vector<Number>& data, TLambda lambda,
        const size_t someIndex, const Number someValue,
        const Number expected, const string& name,
        const Number zero = {})
    {
        TTree tree(data, lambda, zero);
        const auto max_index = tree.max_index_exclusive();
        Assert::AreEqual(data.size(), max_index, "max_index_exclusive_" + name);

        tree.set(someIndex, someValue);
        const auto actual = tree.get(0, max_index);
        Assert::AreEqual(expected, actual, "get_" + name);
    }

    template<typename TTree>
    void Test(const string test_name)
    {
        const vector<Number> data{ 1, 3, 7, 9, 12 };

        const size_t someIndex = 2;
        const Number someValue = 101;

        //Sum, XOR, min/max, count, GCD/LCM.
        //2D, set/get a segment.
        {
            const string name = test_name + "Add";
            auto lambda = Sum_unchecked<Number>;
            const auto acc = accumulate(
                data.cbegin(), data.cend(), Number(0), lambda);
            const auto expected = Number(lambda(someValue, acc)
                - data[someIndex]);

            TestHelper<TTree>(data, lambda,
                someIndex, someValue, expected, name);
        }
        {
            const string name = test_name + "XOR";
            auto lambda = Xor<Number>;
            const auto acc = accumulate(
                data.cbegin(), data.cend(), Number(0), lambda);
            const auto expected = Number(lambda(lambda(someValue, acc),
                data[someIndex]));

            TestHelper<TTree>(data, lambda,
                someIndex, someValue, expected, name);
        }
        {
            const string name = test_name + "Max";
            auto lambda = Max<Number>;
            const auto acc = accumulate(
                data.cbegin(), data.cend(), data[0], lambda);

            TestHelper<TTree>(data, lambda,
                someIndex, data[someIndex + 1], acc, name);

            const auto expected = Number(lambda(lambda(someValue, acc),
                data[someIndex]));
            TestHelper<TTree>(data, lambda,
                someIndex, someValue, expected, name);
        }
        {
            const string name = test_name + "GCD";
            const vector<Number> data2{ 12 * 19, 36, 256 };
            auto lambda = Gcd<Number>;
            const Number someValue2 = 21 * 33;
            const Number expected = 3;

            TestHelper<TTree>(data2, lambda,
                someIndex, someValue2, expected, name);
        }
    }
}

namespace Privet::Algorithms::Trees::Tests
{
    void SegmentTreeTests()
    {
        Test<SegmentTree_lazy<Number, TLambda>>("SegmentTree_lazy_");
        Test<SegmentTree<Number, TLambda>>("SegmentTree_");
    }
}