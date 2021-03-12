#include "../Utilities/Random.h"
#include "../Utilities/VectorUtilities.h"
#include "ArrayInversionCount.h"
#include "ArrayInversionCountTests.h"
#include "../Assert.h"

using namespace std;
using namespace Privet::Algorithms::Numbers::Tests;
using namespace Privet::Algorithms::Numbers;

void ArrayInversionCountTests::SimpleTest()
{
    const Data data{ 1297, 127, 41, 400, 1, 14, 4, };

    const size_t expected = 6 + 4 + 3 + 3 + 1;

    TestHelper(data, "Simple", true, expected);
}

void ArrayInversionCountTests::RandomTest()
{
    Data data;

    const size_t size = UnsignedIntRandom(1,
#ifdef _DEBUG
        9
#else
        99
#endif
    )();
    FillRandom(data, size);

    TestHelper(data, "Random", false);
}

void ArrayInversionCountTests::TestHelper(
    const Data& data, const string& name,
    bool hasExpected, size_t expected)
{
    using Pair = pair<string, size_t(*)(Data&) >;

    const Pair tests[] = {
      Pair("Slow_" + name, [](Data& dat) {return ArrayInversionCount::Slow<Number>(dat); }),
      Pair("Fast_" + name, [](Data& dat)
      {
        const size_t result1 = ArrayInversionCount::FastUsingMergeSort<Number>(dat);
        VectorUtilities::RequireArrayIsSorted(dat, "dat");
        return result1;
      }),
    };

    for (const Pair& test : tests)
    {
        Data copy(data);

        size_t actual = 0;
        try
        {
            actual = test.second(copy);
        }
        catch (const exception& ex)
        {
            ostringstream ss;
            ss << "Error in '" << test.first << "': " << ex.what();

            StreamUtilities::throw_exception(ss);
        }

        if (hasExpected)
        {
            Assert::AreEqual(expected, actual, test.first);
        }
        else
        {
            expected = actual;
            hasExpected = true;
        }
    }
}

void Privet::Algorithms::Numbers::Tests::ArrayInversionCountTests::Test()
{
    SimpleTest();
    RandomTest();
}