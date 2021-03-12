#include <vector>
#include "ArrayProductDividingCurrentIndex.h"
#include "ArrayProductDividingCurrentIndexTests.h"
#include "../test_utilities.h"
#include "../Utilities/Random.h"
#include "../Utilities/PrintUtilities.h"

using namespace std;
using namespace Privet::Algorithms::Tests;
using namespace Privet::Algorithms::Numbers::Tests;
using namespace Privet::Algorithms::Numbers;
using namespace Privet::Algorithms;

namespace
{
    using Number = long long int;

    struct TestCase final : base_test_case
    {
        std::vector<Number> Data;
        std::vector<Number> Expected;

        //TODO: p2. add another data members.
        explicit TestCase(std::string&& name)
            : base_test_case(forward<string>(name))
        {
        }

        void Validate() const override
        {
            base_test_case::Validate();

            Assert::AreEqual(Data.size(), Expected.size(), "Size_" + get_Name());
            Assert::Greater(Data.size(), size_t(1), "Size_" + get_Name());
        }

        void Print(std::ostream& str) const override
        {
            base_test_case::Print(str);

            ::Print(", Data=", Data, str);

            ::Print(", Expected=", Expected, str);
        }
    };

    void GenerateTestCases(vector<TestCase>& testCases)
    {
        {
            TestCase testCase("With zero");
            testCase.Data = { 0, 1, 2, 4, 5, };
            testCase.Expected = { 40, 0, 0, 0, 0, };

            testCases.push_back(testCase);
        }
        {
            TestCase testCase("Simple");
            testCase.Data = { -3, 1, 2, 4, 5, };
            testCase.Expected = { 40, -120, -60, -30, -24, };

            testCases.push_back(testCase);
        }
        {
            TestCase testCase("Random");

            const size_t size = SizeRandom(2, 10)();
            FillRandom<Number, int>(testCase.Data, size);

            testCase.Expected = ArrayProductDividingCurrentIndex<Number>::CalcSlow(testCase.Data);

            testCases.push_back(testCase);
        }
    }

    void RunTestCase(const TestCase& testCase)
    {
        const auto actual = ArrayProductDividingCurrentIndex<Number>::Calc(testCase.Data);
        Assert::AreEqual(testCase.Expected, actual, testCase.get_Name());
    }
}

void Privet::Algorithms::Numbers::Tests::ArrayProductDividingCurrentIndexTests()
{
    test_utilities<TestCase>::run_tests(RunTestCase, GenerateTestCases);
}