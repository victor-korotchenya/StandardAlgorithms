#include <algorithm>
#include <vector>
#include <numeric> // accumulate
#include "../Utilities/Random.h"
#include "../Utilities/VectorUtilities.h"
#include "TestArrayUtility.h"
#include "ArraySumOfThreeElements.h"
#include "ArraySumOfThreeElementsTests.h"
#include "../Assert.h"

using namespace std;
using namespace Privet::Algorithms::Numbers::Tests;
using namespace Privet::Algorithms::Numbers;
using namespace Privet::Algorithms;

ArraySumOfThreeElementsTests::Number ArraySumOfThreeElementsTests::GetRandomSum(
    const vector<Number>& data)
{
    const size_t size = data.size();
    Assert::GreaterOrEqual(size, _MinSize, "The size");

    SizeRandom sizeRandom;

    const size_t third = size / _MinSize;
    const size_t index1 = sizeRandom(size_t(0), third - 1);
    const size_t index2 = sizeRandom(third, (third << 1) - 1);
    const size_t index3 = sizeRandom(third << 1, size - 1);

    const Number result = data[index1] + data[index2] + data[index3];
    return result;
}

void ArraySumOfThreeElementsTests::Test()
{
    const Number cutter = _MinSize << 4;
    const size_t size = SizeRandom(_MinSize, static_cast<size_t>(cutter - 1))();

    vector<Number> data;

    //Use int to have negative numbers.
    FillRandom<Number, int>(data, size, cutter);
    sort(data.begin(), data.end());

    const Number expectedSum = GetRandomSum(data);

    using Algorithm = Tuple(*)(const vector<Number>&, const Number&);

    const pair<string, Algorithm> subCases[] = {
      make_pair("Slow", ArraySumOfThreeElements::Slow<Number>),
      make_pair("Fast", ArraySumOfThreeElements::Fast<Number>),
    };

    for (const pair<string, Algorithm>& subCase : subCases)
    {
        const auto actual = subCase.second(data, expectedSum);

        //TODO: p1: does C++ lack flexibility?
        const vector<size_t> actualIndexes = { get<0>(actual), get<1>(actual), get<2>(actual) };
        VectorUtilities::RequireArrayIsSorted(actualIndexes, "actualIndexes", true);

        TestArrayUtility::CheckArrayIndex(size, actualIndexes[0], subCase.first + "_IndexFirst");
        TestArrayUtility::CheckArrayIndex(size, actualIndexes[_MinSize - 1],
            subCase.first + "_IndexLast");

        const Number actualSum = accumulate(actualIndexes.begin(), actualIndexes.end(),
            Number(0),
            [&](const Number& a, const size_t& indexB) {
                return a + data[indexB];
            });

        Assert::AreEqual(expectedSum, actualSum, subCase.first + "_Sum");
    }
}