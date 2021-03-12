#include <vector>
#include "../test_utilities.h"
#include "../Utilities/Random.h"
#include "../Utilities/PrintUtilities.h"
#include "CoinLineMaxSum.h"
#include "CoinLineMaxSumTests.h"

using namespace std;
using namespace Privet::Algorithms::Tests;
using namespace Privet::Algorithms::Numbers;
using namespace Privet::Algorithms;

namespace
{
    using Number = unsigned long int;

    struct TestCase final : base_test_case
    {
        std::vector<Number> Values;
        Number ExpectedMinValue;

        explicit TestCase(
            std::string&& name = "",
            const std::vector<Number>& values = std::vector<Number>(),
            const Number expectedvalue = Number(0))
            : base_test_case(forward<string>(name)),
            Values(values), ExpectedMinValue(expectedvalue)
        {
        }

        void Validate() const override
        {
            base_test_case::Validate();

            Assert::Greater(ExpectedMinValue, Number(0), "Expectedvalue_" + get_Name());
        }

        void Print(std::ostream& str) const override
        {
            base_test_case::Print(str);

            ::Print(", Values=", Values, str);

            str << " ExpectedMinValue=" << ExpectedMinValue;
        }
    };

    void GenerateTestCases(std::vector<TestCase>& testCases);
    void AddTestsRandom(std::vector<TestCase>& testCases);

    //Reversing the values must give the same result;
    void AddTestsReversed(std::vector<TestCase>& testCases);
    void AddTestsSameSize(std::vector<TestCase>& testCases);

    void RunTestCase(const TestCase& testCase);

    void GenerateTestCases(vector<TestCase>& testCases)
    {
        const Number value = Number(20);

        //Size 5
        {
            TestCase testCase("Size5", { 5, 8, 2, 1, 4 }, 11);
            testCases.push_back(testCase);
        }
        //Size 2
        {
            TestCase testCase("Trivial", { value, value << 1 }, value << 1);
            testCases.push_back(testCase);
        }
        //Size 4
        {
            //All unique permutations of {1,2,3,4}:
            {
                TestCase testCase("Permute4_1", { 1, 2, 3, 4 }, 6);
                testCases.push_back(testCase);
            }
            {
                TestCase testCase("Permute4_2", { 1, 2, 4, 3 }, 5);
                testCases.push_back(testCase);
            }
            {
                TestCase testCase("Permute4_3", { 1, 3, 2, 4 }, 7);
                testCases.push_back(testCase);
            }
            {
                TestCase testCase("Permute4_4", { 1, 3, 4, 2 }, 5);
                testCases.push_back(testCase);
            }
            {
                TestCase testCase("Permute4_5", { 1, 4, 2, 3 }, 7);
                testCases.push_back(testCase);
            }
            {
                TestCase testCase("Permute4_6", { 1, 4, 3, 2 }, 6);
                testCases.push_back(testCase);
            }
            {
                TestCase testCase("Permute4_7", { 2, 1, 3, 4 }, 6);
                testCases.push_back(testCase);
            }
            {
                TestCase testCase("Permute4_8", { 2, 1, 4, 3 }, 6);
                testCases.push_back(testCase);
            }
            {
                TestCase testCase("Permute4_9", { 2, 3, 1, 4 }, 7);
                testCases.push_back(testCase);
            }
            {
                TestCase testCase("Permute4_10", { 2, 4, 1, 3 }, 7);
                testCases.push_back(testCase);
            }
            {
                TestCase testCase("Permute4_11", { 3, 1, 2, 4 }, 6);
                testCases.push_back(testCase);
            }
            {
                TestCase testCase("Permute4_12", { 3, 2, 1, 4 }, 6);
                testCases.push_back(testCase);
            }
        }
        {
            TestCase testCase("Size4", { 5, 8, 2, 1 }, 9);
            testCases.push_back(testCase);
        }
        //Size 3
        {
            TestCase testCase("Size3", { 5, 8, 2 }, 7);
            testCases.push_back(testCase);
        }
        {
            TestCase testCase("Size3_2", { 8, 2, 1 }, 9);
            testCases.push_back(testCase);
        }

        //These must be the last lines.
        AddTestsRandom(testCases);
        AddTestsReversed(testCases);
        AddTestsSameSize(testCases);
    }

    void AddTestsRandom(vector<TestCase>& testCases)
    {
        TestCase testCase("Random");

        const size_t sizeMin = 1;
        const size_t sizeMax = 50;
        const size_t evenSize = SizeRandom(sizeMin, sizeMax)() << 1;

        const Number cutter = 1024 * 1024;
        FillRandom(testCase.Values, evenSize, cutter);

        testCase.ExpectedMinValue = CoinLineMaxSum<Number>::MinGuaranteedValue(testCase.Values);

        testCases.push_back(testCase);
    }

    void AddTestsReversed(vector<TestCase>& testCases)
    {
        const size_t size = testCases.size();
        testCases.resize(size << 1);

        const string postFix = "_rev";

        for (size_t i = 0; i < size; ++i)
        {
            const TestCase& oldTestCase = testCases[i];
            TestCase& newTestCase = testCases[i + size];

            newTestCase.set_Name(oldTestCase.get_Name() + postFix);

            newTestCase.Values.insert(newTestCase.Values.end(),
                oldTestCase.Values.rbegin(), oldTestCase.Values.rend());

            newTestCase.ExpectedMinValue = oldTestCase.ExpectedMinValue;
        }
    }

    void AddTestsSameSize(vector<TestCase>& testCases)
    {
        const string prefix = "Same";

        const Number minSize = 6;
        const Number maxSize = 10;
        const Number value = minSize + maxSize + 5;

        for (Number i = minSize; i <= maxSize; ++i)
        {
            const Number expectedvalue = value * ((i + 1) >> 1);

            TestCase testCase(prefix + to_string(i), {}, expectedvalue);
            testCase.Values.assign(i, value);

            testCases.push_back(testCase);
        }
    }

    void RunTestCase(const TestCase& testCase)
    {
        using alg = Number(*)(const vector<Number>&);

        const vector<pair<string, alg>> subTests = {
          { "Usual", &CoinLineMaxSum<Number>::MinGuaranteedValue },
          { "Method2", &CoinLineMaxSum<Number>::MinGuaranteedValue_Fast }
        };

        for (const auto& sub : subTests)
        {
            const auto actual = sub.second(testCase.Values);
            Assert::AreEqual(testCase.ExpectedMinValue, actual, testCase.get_Name() + "_" + sub.first);
        }
    }
}

void Privet::Algorithms::Numbers::Tests::CoinLineMaxSumTests()
{
    test_utilities<TestCase>::run_tests<0>(RunTestCase, GenerateTestCases);
}