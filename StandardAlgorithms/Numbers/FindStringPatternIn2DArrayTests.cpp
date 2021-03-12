#include <algorithm>

#include "FindStringPatternIn2DArrayTests.h"
//#include "..\Random.h"
#include "../Assert.h"
#include "../Utilities/PrintUtilities.h"

using namespace std;
using namespace Privet::Algorithms::Tests;
using namespace Privet::Algorithms::Numbers::Tests;
using namespace Privet::Algorithms::Numbers;
using namespace Privet::Algorithms;

namespace
{
    using TPoint = FindStringPatternIn2DArray::TPoint;

    struct TestCase final : base_test_case
    {
        std::vector<std::string > Data;
        std::string Pattern;

        bool ExpectedResult{};
        std::vector< TPoint > ExpectedPoints;

        //TODO: p2. add another data members.
        explicit TestCase(std::string&& name);

        void Print(std::ostream& str) const override;

        void FixUp();
    };

    TestCase::TestCase(string&& name)
        : base_test_case(forward<string>(name))
    {
    }

    void TestCase::Print(ostream& str) const
    {
        base_test_case::Print(str);

        str << ", Pattern=" << Pattern;

        ::Print(", Data=", Data, str);

        ::Print(", ExpectedPoints=", ExpectedPoints, str);

        str << " ExpectedResult=" << ExpectedResult;
    }

    void TestCase::FixUp()
    {
        Assert::AreEqual(
            ExpectedResult ? Pattern.size() : 0,
            ExpectedPoints.size(),
            "Pattern and points sizes.");

        sort(this->ExpectedPoints.begin(), this->ExpectedPoints.end());
    }

    void GenerateTestCases(vector<TestCase>& testCases)
    {
        {
            TestCase testCase("Simple1.");

            testCase.Data.push_back("nbed");
            testCase.Data.push_back("aacm");
            testCase.Data.push_back("ebad");
            testCase.Data.push_back("macn");

            testCase.Pattern = "name";
            testCase.ExpectedResult = true;
            testCase.ExpectedPoints.push_back(TPoint(0, 2));
            testCase.ExpectedPoints.push_back(TPoint(1, 3));
            testCase.ExpectedPoints.push_back(TPoint(2, 2));
            testCase.ExpectedPoints.push_back(TPoint(3, 3));

            testCase.FixUp();
            testCases.push_back(testCase);
        }
        //{
        //  TestCase testCase("Random test.");
        //
        //  const size_t size = 10;
        //  const Number cutter = SHRT_MAX >> 1;
        //  FillR andom(testCase.Data, size, cutter);
        //
        //  testCase.ExpectedDifference = MaximumDifference2Items::Slow(
        //    subtractFunction,
        //    testCase.Data.data(), size, testCase.ExpectedIndexMin, testCase.ExpectedIndexMax);
        //
        //  testCases.push_back(testCase);
        //  //testCases.insert(testCases.begin(), testCase);
        //}
    }

    void RunTestCase(const TestCase& testCase)
    {
        vector< TPoint > actualPoints;

        const bool actual = FindStringPatternIn2DArray::Find(
            testCase.Data, testCase.Pattern, actualPoints);
        Assert::AreEqual(testCase.ExpectedResult, actual, "ExpectedResult");

        //Simple verification.
        sort(actualPoints.begin(), actualPoints.end());
        Assert::AreEqual(testCase.ExpectedPoints, actualPoints, "ExpectedPoints");
    }
}

void FindStringPatternIn2DArrayTests::Test()
{
    test_utilities<TestCase>::run_tests(RunTestCase, GenerateTestCases);
}