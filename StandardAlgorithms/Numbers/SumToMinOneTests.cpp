#include "SumToMinOne.h"
#include "SumToMinOneTests.h"
//#include "..\Assert.h"

using namespace std;
using namespace Privet::Algorithms::Numbers;
using namespace Privet::Algorithms;

using TInteger = unsigned int;
using TSumItem = SumItem< TInteger >;
using TSubSequence = vector< size_t >;

struct TestCase_SumToMinOne final
{
    vector< TSumItem > Items;

    TInteger ExpectedValue;

    vector< TSubSequence > ExpectedPartitions;
};

void SumToMinOneTests::Test()
{
    /*
    TestCase_SumToMinOne test_0;
    {
      test_0.ExpectedValue = 0;

      {
        const TSumItem item = {5, 7};
        test_0.Items.push_back(item);
      }
    }

    TestCase_SumToMinOne test_simple_1;
    {
      test_simple_1.ExpectedValue = 1;

      {
        const TInteger value = 758971;
        const TSumItem item = {value, value};
        test_simple_1.Items.push_back(item);
      }
    }

    TestCase_SumToMinOne test_simple_2 = test_simple_1;
    {
      test_simple_2.ExpectedValue = 2;

      {
        const TInteger value = 9754212;
        const TSumItem item = {value, value};
        test_simple_2.Items.push_back(item);
      }
    }

    TestCase_SumToMinOne test_1;
    {
      test_1.ExpectedValue = 1;

      {
        const TSumItem item = {5, 7};
        test_1.Items.push_back(item);
      }
      {
        const TSumItem item = {4, 11};
        test_1.Items.push_back(item);
      }
      {
        const TSumItem item = {12, 13};
        test_1.Items.push_back(item);
      }
    }

    TestCase_SumToMinOne test_2 = test_1;
    {
      test_2.ExpectedValue = 3;

      {
        const TSumItem item = {1001, 1001};
        test_2.Items.push_back(item);
      }
    }

    const TestCase_SumToMinOne tests[] = {
      test_0,
      test_simple_1,
      test_simple_2,
      test_1,
      test_2,
    };

    const size_t testsCount = (sizeof tests) / sizeof(TestCase_SumToMinOne);
    vector< TSubSequence > chosenPartitions;

    for (size_t i = 0; i < testsCount; ++i)
    {
      const TestCase_SumToMinOne &test = tests[i];

      const TInteger value = SumToMinOne<TInteger>::Solve(
        test.Items,
        chosenPartitions);

      Assert::AreEqual(test.ExpectedValue, value, "Value");

      //TODO: p2. Assert::AreEqual(test.ExpectedPartitions, chosenPartitions, "chosenPartitions");
    }
    */
}