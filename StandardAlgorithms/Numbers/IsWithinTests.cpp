#include <limits>
#include <vector>
#include "../Numbers/Arithmetic.h"
#include "../test_utilities.h"
#include "IsWithinTests.h"

using namespace std;
using namespace Privet::Algorithms::Numbers::Tests;
using namespace Privet::Algorithms::Numbers;
using namespace Privet::Algorithms::Tests;
using namespace Privet::Algorithms;

namespace
{
    struct TestCase final : base_test_case
    {
        unsigned Min;
        unsigned Max;
        unsigned Value;
        bool Expected;

        //TODO: p2. add another data members.
        TestCase(std::string&& name = "")
            : base_test_case(forward<string>(name))
        {
        }

        void Print(ostream& str) const override
        {
            base_test_case::Print(str);

            str << " Min=" << Min
                << ", Max=" << Max
                << ", Value=" << Value
                << ", Expected=" << Expected;
        }
    };

    void GenerateTestCases(vector<TestCase>& testCases)
    {
        const unsigned minValue = '0';
        const unsigned maxValue = '9';

        TestCase testCase;
        testCase.Min = minValue;
        testCase.Max = maxValue;

        for (unsigned i = 0; i <= (CHAR_MAX); ++i)
        {
            testCase.set_Name(to_string(i));
            testCase.Value = i;
            testCase.Expected = minValue <= i && i <= maxValue;

            testCases.push_back(testCase);
        }
    }

    void RunTestCase(const TestCase& testCase)
    {
        const auto actual = is_within(
            testCase.Min, testCase.Max, testCase.Value);

        Assert::AreEqual(testCase.Expected, actual, testCase.get_Name());
    }
}

void Privet::Algorithms::Numbers::Tests::IsWithinTests()
{
    test_utilities<TestCase>::run_tests(RunTestCase, GenerateTestCases);
}