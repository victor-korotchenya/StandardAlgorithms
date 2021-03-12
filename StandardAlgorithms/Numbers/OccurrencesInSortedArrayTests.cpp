#include <algorithm>
#include "../Utilities/VectorUtilities.h"
#include "../Utilities/Random.h"
#include "OccurrencesInSortedArrayTests.h"
#include "../Assert.h"

using namespace std;
using namespace Privet::Algorithms::Numbers::Tests;
using namespace Privet::Algorithms::Numbers;

inline OccurrencesInSortedArrayTests::Pair OccurrencesInSortedArrayTests::NotFoundPair()
{
    const size_t minusOne = size_t(0) - size_t(1);

    const Pair result = Pair(minusOne, minusOne);
    return result;
}

void OccurrencesInSortedArrayTests::StandardTest()
{
    const Data data{ 1,1,1,1,1, 3,4, 37,37,37,// 10 items
      89,// 11-th element
      98,98,98,98,98,98,98,
      100,
      1000000, 1000000 };
    VectorUtilities::RequireArrayIsSorted(data, "IsSorted");

    {
        const Number first = VectorUtilities::First(data);
        const auto expected = Pair(0, 4);
        Helper(data, first, "First element", expected);
    }
    {
        const Number middle = 89;
        const auto expected = Pair(10, 10);
        Helper(data, middle, "Middle element", expected);
    }
    {
        const Number mostOften = 98;
        const auto expected = Pair(11, 17);
        Helper(data, mostOften, "Most often element", expected);
    }
    {
        const Number last = VectorUtilities::Last(data);
        const auto expected = Pair(data.size() - 2, data.size() - 1);
        Helper(data, last, "Last element", expected);
    }
    BeforeAfter(data, "Standard element");
}

void OccurrencesInSortedArrayTests::AllSameTest()
{
    const Number number = 500;
    const Data data{ number,number,number,number,number,
      number,number,number, };
    VectorUtilities::RequireArrayIsSorted(data, "allSame IsSorted");

    {
        const auto expected = Pair(0, data.size() - 1);
        Helper(data, number, "allSame", expected);
    }

    BeforeAfter(data, "All same");
}

void OccurrencesInSortedArrayTests::RandomTest()
{
    SizeRandom sizeRandom;

    const size_t size = sizeRandom(1,
#ifdef _DEBUG
        50
#else
        1024
#endif
    );
    Data data(size);

    //Create a lot of conflicts to have possible repetitions.
    const Number cutter = 10;
    FillRandom(data, size, cutter);

    sort(data.begin(), data.end());
    VectorUtilities::RequireArrayIsSorted(data, "Random IsSorted");

    {
        const size_t index = 0 < size - 1
            ? sizeRandom(0, size - 1)
            : 0;
        const Number element = data[index];
        Helper(data, element, "Random element test", Pair(), false);
    }

    BeforeAfter(data, "Random element");
}

void OccurrencesInSortedArrayTests::BeforeAfter(const Data& data, const string& name)
{
    const Pair expected = NotFoundPair();

    const Number before = data[0] - 1;
    Helper(data, before, "Before_" + name, expected);

    const Number last = VectorUtilities::Last(data);
    const Number after = last + 1;
    Helper(data, after, "After_" + name, expected);
}

void OccurrencesInSortedArrayTests::Helper(
    const Data& data, const Number& value,
    const string& name, const Pair& expected,
    const bool hasExpected)
{
    const auto actualSlow = OccurrencesInSortedArray::FindOccurrencesSlow(data, value);
    if (hasExpected)
    {
        Assert::AreEqual(expected, actualSlow, name + "_Slow");
    }

    const auto actual = OccurrencesInSortedArray::FindOccurrences(data, value);
    Assert::AreEqual(actualSlow, actual, name + "_slow vs fast");
}

void OccurrencesInSortedArrayTests::Test()
{
    StandardTest();
    AllSameTest();
    RandomTest();
}