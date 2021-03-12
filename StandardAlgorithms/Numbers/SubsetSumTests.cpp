#include <numeric>
#include "SubsetSum.h"
#include "SubsetSumTests.h"
#include "../Assert.h"

using namespace std;
using namespace Privet::Algorithms::Numbers;

namespace
{
    using Number = unsigned int;

    struct TestCase_SubsetSum final
    {
        vector<Number> Numbers;
        size_t MaximumWeight{};

        size_t ExpectedWeight{};
        vector<size_t> ExpectedIndexes;
    };
}

void Privet::Algorithms::Numbers::Tests::SubsetSumTests()
{
    TestCase_SubsetSum test_1;
    {
        test_1.MaximumWeight = 6;
        test_1.Numbers = { 2,2,3 };
        test_1.ExpectedWeight = 5;
        test_1.ExpectedIndexes = { 2, 1 };
    }

    TestCase_SubsetSum test_2;
    {
        test_2.MaximumWeight = 10;
        test_2.Numbers.push_back(2);
        test_2.Numbers.push_back(11);
        test_2.Numbers.push_back(4);//6
        test_2.Numbers.push_back(20);
        test_2.Numbers.push_back(3);//9
        test_2.Numbers.push_back(20);
        test_2.Numbers.push_back(11);
        test_2.Numbers.push_back(110);
        test_2.Numbers.push_back(9);
        test_2.Numbers.push_back(10010);

        test_2.ExpectedWeight = 9;

        test_2.ExpectedIndexes.push_back(8);
    }

    auto test_ExactMatch = test_2;
    {
        test_ExactMatch.Numbers.push_back(10);
        test_ExactMatch.ExpectedWeight = 10;
        test_ExactMatch.ExpectedIndexes = { 10 };
    }

    TestCase_SubsetSum test_4;
    {
        test_4.Numbers = { 1,500,7,20 };
        test_4.MaximumWeight = test_4.ExpectedWeight = accumulate(
            test_4.Numbers.begin(),
            test_4.Numbers.end(), 0);
        test_4.ExpectedIndexes = { 3,2,1,0 };
    }

    auto test_5 = test_4;
    {
        test_4.Numbers = { 1,500,7,20 };
        test_5.MaximumWeight = test_5.ExpectedWeight = 28;
        test_5.ExpectedIndexes = { 3,2,0 };
    }

    const vector<TestCase_SubsetSum> tests{
      test_1,
      test_2,
      test_ExactMatch,
      test_4,
      test_5,
    };

    vector<size_t> chosenIndexes;
    for (const auto& test : tests)
    {
        const auto weight = FindMaximumSum<Number>(
            test.Numbers,
            test.MaximumWeight,
            chosenIndexes);

        Assert::AreEqual(test.ExpectedWeight, weight, "Weight");

        Assert::AreEqual(test.ExpectedIndexes, chosenIndexes, "chosenIndexes");
    }
}