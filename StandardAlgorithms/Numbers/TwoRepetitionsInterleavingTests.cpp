#include <string>
#include <vector>
#include "TwoRepetitionsInterleaving.h"
#include "TwoRepetitionsInterleavingTests.h"
#include "../Assert.h"
#include "../Utilities/TestNameUtilities.h"

using namespace std;
using namespace Privet::Algorithms::Numbers;
using namespace Privet::Algorithms;

struct InterleavingTestCase final
{
    string Name;
    inline const std::string& get_Name() const
    {
        return Name;
    }
    vector< bool > SourceSignal, Sequence1, Sequence2;

    bool ExpectedResult;
    vector< size_t > ExpectedIndexes;
};

void TwoRepetitionsInterleavingTests::Test()
{
    vector< InterleavingTestCase > testCases;
    {
        InterleavingTestCase testCase;
        testCase.Name = "Simple";
        //100010101 == 10 00 1 0 101 == 101 + 101 + 00 + 0
        testCase.SourceSignal.push_back(1);
        testCase.SourceSignal.push_back(0);
        testCase.SourceSignal.push_back(0);
        testCase.SourceSignal.push_back(0);
        testCase.SourceSignal.push_back(1);
        testCase.SourceSignal.push_back(0);
        testCase.SourceSignal.push_back(1);
        testCase.SourceSignal.push_back(0);
        testCase.SourceSignal.push_back(1);

        //101
        testCase.Sequence1.push_back(1);
        testCase.Sequence1.push_back(0);
        testCase.Sequence1.push_back(1);

        //00
        testCase.Sequence2.push_back(0);
        testCase.Sequence2.push_back(0);

        testCase.ExpectedResult = true;

        testCase.ExpectedIndexes.push_back(0);
        testCase.ExpectedIndexes.push_back(1);
        testCase.ExpectedIndexes.push_back(4);
        testCase.ExpectedIndexes.push_back(6);
        testCase.ExpectedIndexes.push_back(7);
        testCase.ExpectedIndexes.push_back(8);

        testCases.push_back(testCase);
    }
    {
        InterleavingTestCase testCase;
        testCase.Name = "No";
        testCase.SourceSignal.push_back(1);
        testCase.SourceSignal.push_back(1);
        testCase.SourceSignal.push_back(0);

        testCase.Sequence1.push_back(1);
        testCase.Sequence1.push_back(1);
        testCase.Sequence1.push_back(1);

        testCase.Sequence2.push_back(1);
        testCase.Sequence2.push_back(1);
        testCase.Sequence2.push_back(1);
        testCase.Sequence2.push_back(0);

        testCase.ExpectedResult = false;

        testCases.push_back(testCase);
    }
    {
        InterleavingTestCase testCase;
        testCase.Name = "Trivial";

        for (size_t i = 0; i < 7; ++i)
        {
            testCase.SourceSignal.push_back(1);
            testCase.ExpectedIndexes.push_back(i);
        }

        testCase.Sequence1.push_back(1);
        testCase.Sequence1.push_back(1);
        testCase.Sequence1.push_back(1);

        testCase.Sequence2.push_back(0);
        testCase.Sequence2.push_back(0);

        testCase.ExpectedResult = true;

        testCases.push_back(testCase);
    }

    TestNameUtilities::RequireUniqueNames(testCases, "testCases");

    vector< size_t > actualIndexes;

    const size_t size = testCases.size();
    for (size_t i = 0; i < size; ++i)
    {
        const InterleavingTestCase& testCase = testCases[i];
        const bool actual = TwoRepetitionsInterleaving::FindInterleaving(
            testCase.SourceSignal,
            testCase.Sequence1,
            testCase.Sequence2,
            actualIndexes);

        const string resultName = testCase.Name + "_Result";
        Assert::AreEqual(testCase.ExpectedResult, actual, resultName);

        const string indexesName = testCase.Name + "_Indexes";
        Assert::AreEqual(testCase.ExpectedIndexes, actualIndexes, indexesName);
    }
}