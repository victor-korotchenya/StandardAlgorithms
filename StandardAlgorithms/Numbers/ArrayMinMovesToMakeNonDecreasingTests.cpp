#include "../test_utilities.h"
//#include "../Utilities/Random.h"
#include "../Utilities/PrintUtilities.h"
#include "ArrayMinMovesToMakeNonDecreasing.h"
#include "ArrayMinMovesToMakeNonDecreasingTests.h"

using namespace std;
using namespace Privet::Algorithms::Numbers::Tests;
using namespace Privet::Algorithms::Numbers;
using namespace Privet::Algorithms::Tests;
using namespace Privet::Algorithms;

namespace
{
    using Number = long long int;

    struct TestCase final : base_test_case
    {
        vector<Number> Data;
        Number ExpectedMoves;

        TestCase(
            string&& name,
            vector<Number>&& data,
            const Number expectedMoves)
            : base_test_case(forward<string>(name)),
            Data(forward<vector<Number>>(data)), ExpectedMoves(expectedMoves)
        {
        }

        //TODO: p3. Impl. //void Validate() const override
        //{
        //    base_test_case::Validate();
        //
        //}

        void Print(ostream& str) const override
        {
            base_test_case::Print(str);

            ::Print(", Data=", Data, str);

            str << " ExpectedMoves=" << ExpectedMoves;
        }
    };

    void GenerateTestCases(vector<TestCase>& testCases)
    {
        testCases.push_back({ "One element", {10}, 0 });
        testCases.push_back({ "Two increasing elements",{ 5, 10 }, 0 });
        testCases.push_back({ "Two decreasing elements",{ 10, 5 }, 5 });
        testCases.push_back({ "Simple", { 5, 9, 1, 4, 10, 8 }, 11 });
    }

    void RunTestCase(const TestCase& testCase)
    {
        const auto actual = ArrayMinMovesToMakeNonDecreasing(testCase.Data);
        Assert::AreEqual(testCase.ExpectedMoves, actual, testCase.get_Name());
    }
}

namespace Privet
{
    namespace Algorithms
    {
        namespace Numbers
        {
            namespace Tests
            {
                void ArrayMinMovesToMakeNonDecreasingTests()
                {
                    test_utilities<TestCase>::run_tests(RunTestCase, GenerateTestCases);
                }
            }
        }
    }
}