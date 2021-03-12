#include <algorithm>
#include <string>
#include <vector>
#include "../Utilities/Random.h"
#include "../Utilities/VectorUtilities.h"
#include "TestArrayUtility.h"
#include "SortedArraySumOfTwoElements.h"
#include "SortedArraySumOfTwoElementsTests.h"
#include "../Assert.h"

using namespace std;
using namespace Privet::Algorithms::Numbers::Tests;
using namespace Privet::Algorithms::Numbers;
using namespace Privet::Algorithms;

void SortedArraySumOfTwoElementsTests::Test()
{
    const Number cutter = 100;

    SizeRandom sizeRandom;
    const size_t size = sizeRandom(size_t(2), static_cast<size_t>(cutter - 1));

    vector<Number> data;

    //Use int to have negative numbers.
    FillRandom<Number, int>(data, size, cutter);

    sort(data.begin(), data.end());

    data[size - 1]++;
    const Number tooLargeSum = VectorUtilities::Last(data) << 1;

    const size_t middle = size >> 1;
    const size_t lowIndex = sizeRandom(size_t(0), middle - 1);
    const size_t highIndex = sizeRandom(middle, size - 1);
    const Number expectedSum = data[lowIndex] + data[highIndex];

    using Algorithm = Pair(*)(const vector<Number>&, const Number&);

    const pair<string, Algorithm> subCases[] = {
      make_pair("Slow", SortedArraySumOfTwoElements::Slow<Number>),
      make_pair("Fast",
        [](const vector<Number>& data2, const Number& sum2)
        -> Pair {
      const auto result1 = SortedArraySumOfTwoElements::Fast<Number>(data2, sum2);
      return result1;
    }),
    };

    for (const pair<string, Algorithm>& subCase : subCases)
    {
        {
            const auto actual = subCase.second(data, expectedSum);

            TestArrayUtility::CheckArrayIndex(size, actual.first, subCase.first + "_First");
            TestArrayUtility::CheckArrayIndex(size, actual.second, subCase.first + "_Second");

            Assert::NotEqual(actual.first, actual.second, subCase.first + "_Indexes");

            const Number actualSum = data[actual.first] + data[actual.second];
            Assert::AreEqual(expectedSum, actualSum, subCase.first + "_Sum");
        }
        {
            const auto actual = subCase.second(data, tooLargeSum);
            Assert::AreEqual(SortedArraySumOfTwoElements::NotFoundPair(),
                actual, subCase.first + "_TooLargeSum");
        }
    }
}