#include "KnapsackDynamo.h"
#include "KnapsackDynamoTests.h"
#include "../Assert.h"

using namespace std;
using namespace Privet::Algorithms::Numbers;
using namespace Privet::Algorithms;

namespace
{
    using Value = int;
    using Weight = size_t;
    using TKnapsackItem = knapsack_item_t<Value, Weight>;

    struct TestCase_Knapsack final
    {
        vector<TKnapsackItem> Items;
        Weight MaximumWeight{};

        Value ExpectedValue{};
        vector<size_t> ExpectedIndexes;
    };
}

void Privet::Algorithms::Numbers::Tests::knapsack_dynamo_tests()
{
    TestCase_Knapsack test_0;
    {
        test_0.MaximumWeight = 8;
        test_0.ExpectedValue = 25;

        test_0.Items.emplace_back(15, 2);
        test_0.Items.emplace_back(10, 2);
        test_0.Items.emplace_back(9, 5);
        test_0.Items.emplace_back(100, test_0.MaximumWeight << 1);

        test_0.ExpectedIndexes = { 0,1, };
    }

    TestCase_Knapsack test_0a;
    {
        test_0a.MaximumWeight = 8;
        test_0a.ExpectedValue = 115;

        test_0a.Items.emplace_back(15, 2);
        test_0a.Items.emplace_back(10, 2);
        test_0a.Items.emplace_back(100, 5);
        test_0.Items.emplace_back(1000, test_0.MaximumWeight << 1);

        test_0a.ExpectedIndexes = { 0,2, };
    }

    TestCase_Knapsack test_1;
    {
        test_1.ExpectedValue = 0;
        test_1.MaximumWeight = 8;

        test_1.Items.emplace_back(1, test_1.MaximumWeight + 1);
    }

    TestCase_Knapsack test_2;
    {
        test_2.ExpectedValue = 2;
        test_2.MaximumWeight = 8;

        test_2.Items.emplace_back(test_2.ExpectedValue, test_2.MaximumWeight);
        test_2.ExpectedIndexes.push_back(0);
    }

    auto test_3 = test_2;
    {
        const Value value = 107;
        test_3.ExpectedValue += value;

        const Weight weight = 204;
        test_3.MaximumWeight += weight + 1000;

        test_3.Items.emplace_back(value, weight);

        test_3.ExpectedIndexes.push_back(1);
    }

    TestCase_Knapsack test_4;
    {
        test_4.MaximumWeight = 8;
        test_4.ExpectedValue = 29;

        test_4.Items.emplace_back(10000, test_4.MaximumWeight << 3);
        test_4.Items.emplace_back(15, 1);
        test_4.Items.emplace_back(10, 5);
        test_4.Items.emplace_back(9, 3);
        test_4.Items.emplace_back(1000, test_4.MaximumWeight << 2);
        test_4.Items.emplace_back(5, 4);
        test_4.Items.emplace_back(100, test_4.MaximumWeight << 1);

        test_4.ExpectedIndexes = { 1,3,5 };
    }

    const TestCase_Knapsack tests[] = {
        test_0,
        test_0a,
        test_1,
        test_2,
        test_3,
        test_4,
    };

    const auto testsCount = (sizeof tests) / sizeof(TestCase_Knapsack);
    vector<size_t> chosen_indexes;
    for (size_t i = 0; i < testsCount; ++i)
    {
        chosen_indexes.clear();
        const auto& test = tests[i];
        const auto value = KnapsackDynamo<Value, Weight>(test.Items, test.MaximumWeight, chosen_indexes);
        Assert::AreEqual(test.ExpectedValue, value, "Value");
        Assert::AreEqual(test.ExpectedIndexes, chosen_indexes, "chosenIndexes");

        chosen_indexes.clear();
        const auto still = KnapsackDynamo_slow_still<Value, Weight>(test.Items, test.MaximumWeight, chosen_indexes);
        Assert::AreEqual(test.ExpectedValue, still, "Value_slow_still");
        Assert::AreEqual(test.ExpectedIndexes, chosen_indexes, "chosenIndexes_slow_still");

        chosen_indexes.clear();
        const auto slow = KnapsackDynamo_slow<Value, Weight>(test.Items, test.MaximumWeight, chosen_indexes);
        Assert::AreEqual(test.ExpectedValue, slow, "Value_slow");
        Assert::AreEqual(test.ExpectedIndexes, chosen_indexes, "chosenIndexes_slow");
    }
}