#include "Find2RepeatingDistinctItems.h"
#include "Find2RepeatingDistinctItemsTests.h"
#include "../Utilities/Random.h"
#include "../Assert.h"

using namespace Privet::Algorithms::Numbers;

void Find2RepeatingDistinctItemsTests::Test()
{
    using Number = unsigned short;

    {
        Number data[] = { 1,1, 2,2 };
        const size_t dataSize = sizeof(data) / sizeof(Number);
        ShuffleArray(data, dataSize);

        Number a, b;
        Find2RepeatingDistinctItems::Slow(data, dataSize, a, b);

        Assert::AreEqual<Number>(1, a, "a");
        Assert::AreEqual<Number>(2, b, "b");
    }
    {
        Number data[] = { 1, 2,2, 3, 4,4, 5 };
        const size_t dataSize = sizeof(data) / sizeof(Number);
        ShuffleArray(data, dataSize);

        Number a, b;
        Find2RepeatingDistinctItems::Slow(data, dataSize, a, b);

        Assert::AreEqual<Number>(2, a, "a");
        Assert::AreEqual<Number>(4, b, "b");
    }
    {
        Number data[] = { 1, 2, 3,3, 4, 5, 6, 7, 8,8 };
        const size_t dataSize = sizeof(data) / sizeof(Number);
        ShuffleArray(data, dataSize);

        Number a, b;
        Find2RepeatingDistinctItems::Slow(data, dataSize, a, b);

        Assert::AreEqual<Number>(3, a, "a");
        Assert::AreEqual<Number>(8, b, "b");
    }
}