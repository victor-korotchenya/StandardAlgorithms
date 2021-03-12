#include <sstream>
#include "NumberSplitter.h"
#include "../Utilities/PrintUtilities.h"
#include "../Utilities/Random.h"
#include "NumberSplitterTests.h"
#include "../Assert.h"
#include "../Utilities/TestNameUtilities.h"
#include "../Utilities/StreamUtilities.h"

using namespace std;
using namespace Privet::Algorithms::Numbers;
using namespace Privet::Algorithms;

ostream&
operator <<(
    ostream& str, NumberSplitterTests::TestCase const& a)
{
    str << "Name=" << a.Name << '\n';

    Print(
        "Numbers: ", a.Numbers, str, NumberSplitterTests::MaxSeparatorCounter());

    Print(
        "ExpectedNumbers: ", a.ExpectedNumbers, str, NumberSplitterTests::MaxSeparatorCounter());

    return str;
}

void NumberSplitterTests::GenerateTestCases(vector<TestCase>& testCases)
{
    {
        TestCase testCase;
        testCase.Name = "Simple1";

        const Number arraySource[] = { 9, -1, 5, -5, 3, 4, -7, -8 };
        const size_t arraySourceSize = sizeof(arraySource) / sizeof(Number);

        testCase.Numbers.insert(testCase.Numbers.begin(),
            arraySource, arraySource + arraySourceSize);

        {
            const Number arrayTarget[] = { -1, -5, -7, -8, 9, 5, 3, 4 };
            const size_t arrayTargetSize = sizeof(arrayTarget) / sizeof(Number);
            Assert::AreEqual(arraySourceSize, arrayTargetSize, "arrayTargetSize");

            testCase.ExpectedNumbers.insert(testCase.ExpectedNumbers.begin(),
                arrayTarget, arrayTarget + arrayTargetSize);
        }

        testCases.push_back(testCase);
    }
    {
        TestCase testCase;
        testCase.Name = "Simple2 with 0-s";

        const Number arraySource[] = { 9, -1, 0, 5, -5, -7, 0, 3, 4, -8, 0 };
        const size_t arraySourceSize = sizeof(arraySource) / sizeof(Number);

        testCase.Numbers.insert(testCase.Numbers.begin(),
            arraySource, arraySource + arraySourceSize);

        {
            const Number arrayTarget[] = { -1, -5, -7, -8, 0, 0, 0, 9, 5, 3, 4 };
            const size_t arrayTargetSize = sizeof(arrayTarget) / sizeof(Number);
            Assert::AreEqual(arraySourceSize, arrayTargetSize, "arrayTargetSize");

            testCase.ExpectedNumbers.insert(testCase.ExpectedNumbers.begin(),
                arrayTarget, arrayTarget + arrayTargetSize);
        }

        testCases.push_back(testCase);
    }
    {
        TestCase testCase;
        testCase.Name = "All negative";

        const Number arraySource[] = { -5, -3, -8, };
        const size_t arraySourceSize = sizeof(arraySource) / sizeof(Number);

        testCase.Numbers.insert(testCase.Numbers.begin(),
            arraySource, arraySource + arraySourceSize);

        testCase.ExpectedNumbers = testCase.Numbers;

        testCases.push_back(testCase);
    }
    {
        TestCase testCase;
        testCase.Name = "All zeros";

        const Number arraySource[] = { 0, 0, 0, 0, 0, 0, };
        const size_t arraySourceSize = sizeof(arraySource) / sizeof(Number);

        testCase.Numbers.insert(testCase.Numbers.begin(),
            arraySource, arraySource + arraySourceSize);

        testCase.ExpectedNumbers = testCase.Numbers;

        testCases.push_back(testCase);
    }
    {
        TestCase testCase;
        testCase.Name = "All positive";

        const Number arraySource[] = { 7, 5, 3, 8, };
        const size_t arraySourceSize = sizeof(arraySource) / sizeof(Number);

        testCase.Numbers.insert(testCase.Numbers.begin(),
            arraySource, arraySource + arraySourceSize);

        testCase.ExpectedNumbers = testCase.Numbers;

        testCases.push_back(testCase);
    }
    {
        TestCase testCase;
        testCase.Name = "Random";

#ifdef _DEBUG
        const size_t minInclusive = 5, maxInclusive = 10;
#else
        const size_t minInclusive = 50, maxInclusive = 102;
#endif
        const size_t size = SizeRandom(minInclusive, maxInclusive)();

        const bool uniqueNumbersSimplifyDebugging = true;
        FillRandom(testCase.Numbers, size, Number(0), uniqueNumbersSimplifyDebugging);

        testCase.ExpectedNumbers = testCase.Numbers;
        NumberSplitter< Number >::Fast(testCase.ExpectedNumbers);

        testCases.push_back(testCase);
    }
}

void NumberSplitterTests::Test()
{
    vector<TestCase> testCases;
    GenerateTestCases(testCases);
    TestNameUtilities::RequireUniqueNames(testCases, "testCases");

    const size_t size = testCases.size();

    using TLess = less< Number >;
    using TFunction = void(*)(vector< Number >& data, const TLess& lessArg);
    using TPair = pair< string, TFunction>;

    const TLess lessComparer = TLess();

    const TPair subTests[] = {
      TPair("_Simple", NumberSplitter< Number >::Slow),
      //TODO: fix it:  TPair("_Fast", NumberSplitter< Number >::Fast),
    };
    const size_t subTestsSize = sizeof(subTests) / sizeof(TPair);

    for (size_t index = 0; index < size; ++index)
    {
        const TestCase& testCase = testCases[index];

        for (size_t subIndex = 0; subIndex < subTestsSize; ++subIndex)
        {
            const TPair& subTestPair = subTests[subIndex];
            const string name = testCase.Name + subTestPair.first;

            //Copy in order not to corrupt the data for debugging.
            vector< Number > actualNumbers = testCase.Numbers;

            try
            {
                subTestPair.second(actualNumbers, lessComparer);

                Assert::AreEqual(testCase.ExpectedNumbers, actualNumbers, name);
            }
            catch (...)
            {
                ostringstream ss;
                ss << "Failed " << subTestPair.first << " test case: " << testCase << '\n';

                Print(
                    "Actual numbers: ", actualNumbers, ss, MaxSeparatorCounter());

                StreamUtilities::throw_exception(ss);
            }
        }
    }
}