#include <utility>
#include "MaximumDifference2Items.h"
#include "MaximumDifference2ItemsTests.h"
#include "../Utilities/PrintUtilities.h"
#include "Arithmetic.h"
#include "../Utilities/Random.h"

using namespace std;
using namespace Privet::Algorithms::Numbers;
using namespace Privet::Algorithms::Tests;
using namespace Privet::Algorithms;

namespace
{
    using Number = short;

    struct TestCase final : base_test_case
    {
        std::vector< Number > Data;

        Number ExpectedDifference{};
        size_t ExpectedIndexMin{};
        size_t ExpectedIndexMax{};

        //TODO: p2. add another data members.
        explicit TestCase(std::string&& name);

        void Print(std::ostream& str) const override;
    };

    static void GenerateTestCases(std::vector<TestCase>& testCases);

    static void RunTestCase(const TestCase& testCase);

    void GenerateTestCases(vector<TestCase>& testCases)
    {
        MaximumDifference2Items::SubtractFunction < Number > subtractFunction =
            static_cast<MaximumDifference2Items::SubtractFunction < Number >>(SubtractSigned< Number >);

        {
            TestCase testCase("Simple1.");

            testCase.Data.push_back(-1548);
            testCase.Data.push_back(920);
            testCase.Data.push_back(-1685);
            testCase.Data.push_back(1376);
            testCase.Data.push_back(2148);

            testCase.ExpectedDifference = Number(3833);
            testCase.ExpectedIndexMin = 2;
            testCase.ExpectedIndexMax = 4;

            testCases.push_back(testCase);
        }
        {
            TestCase testCase("Simple2.");

            testCase.Data.push_back(6865);
            testCase.Data.push_back(8522);
            testCase.Data.push_back(12780);
            testCase.Data.push_back(14155);
            testCase.Data.push_back(290);
            testCase.Data.push_back(11779);
            testCase.Data.push_back(13995);
            testCase.Data.push_back(102);
            testCase.Data.push_back(12761);

            testCase.ExpectedDifference = Number(13705);
            testCase.ExpectedIndexMin = 4;
            testCase.ExpectedIndexMax = 6;

            testCases.push_back(testCase);
        }
        {
            TestCase testCase("Simple3.");

            testCase.Data.push_back(500);
            testCase.Data.push_back(500);
            testCase.Data.push_back(2);
            testCase.Data.push_back(3);
            testCase.Data.push_back(10);
            testCase.Data.push_back(10);
            testCase.Data.push_back(10);
            testCase.Data.push_back(7);
            testCase.Data.push_back(5);
            testCase.Data.push_back(9);
            testCase.Data.push_back(0);

            testCase.ExpectedDifference = Number(10) - Number(2);
            testCase.ExpectedIndexMin = 2;
            testCase.ExpectedIndexMax = 4;

            testCases.push_back(testCase);
        }
        {
            TestCase testCase("Many decreasing.");

            testCase.Data.push_back(12);
            testCase.Data.push_back(7);
            testCase.Data.push_back(10);
            testCase.Data.push_back(5);
            testCase.Data.push_back(6);
            testCase.Data.push_back(3);
            testCase.Data.push_back(2);
            testCase.Data.push_back(1);

            testCase.ExpectedDifference = Number(10 - 7);
            testCase.ExpectedIndexMin = 1;
            testCase.ExpectedIndexMax = 2;

            testCases.push_back(testCase);
        }
        {
            TestCase testCase("All decreasing by 2.");

            const Number size = 5;
            for (Number i = 0; i < size; ++i)
            {
                testCase.Data.push_back((size - i) << 1);
            }

            testCase.ExpectedDifference = Number(Number(0) - Number(2));
            testCase.ExpectedIndexMin = 0;
            testCase.ExpectedIndexMax = 1;

            testCases.push_back(testCase);
        }
        {
            TestCase testCase("Decreasing by 7, 6, 5, 4, 3.");

            const Number big = 100;
            testCase.Data.push_back(big);

            Number previous = big;
            Number delta = 7;
            do
            {
                previous -= delta;
                testCase.Data.push_back(previous);
            } while (3 <= --delta);

            testCase.ExpectedDifference = Number(Number(0) - Number(3));
            testCase.ExpectedIndexMin = 4;
            testCase.ExpectedIndexMax = 5;

            testCases.push_back(testCase);
        }
        {
            TestCase testCase("Random test.");

            const size_t size = 10;
            const Number cutter = SHRT_MAX >> 1;
            FillRandom(testCase.Data, size, cutter);

            testCase.ExpectedDifference = MaximumDifference2Items::Slow(
                subtractFunction,
                testCase.Data.data(), size, testCase.ExpectedIndexMin, testCase.ExpectedIndexMax);

            testCases.push_back(testCase);
            //testCases.insert(testCases.begin(), testCase);
        }
    }

    TestCase::TestCase(string&& name)
        : base_test_case(forward<string>(name))
    {
    }

    void TestCase::Print(ostream& str) const
    {
        base_test_case::Print(str);

        ::Print(", Data=", Data, str);

        str << " ExpectedDifference=" << ExpectedDifference
            << ", ExpectedIndexMin=" << ExpectedIndexMin
            << ", ExpectedIndexMax=" << ExpectedIndexMax;
    }

    void RunTestCase(const TestCase& testCase)
    {
        MaximumDifference2Items::SubtractFunction < Number > subtractFunction =
            static_cast<MaximumDifference2Items::SubtractFunction < Number >>(SubtractSigned< Number >);

        const Number* const numbers = testCase.Data.data();
        const size_t numbersSize = testCase.Data.size();

        using runFunction = Number(*)(
            MaximumDifference2Items::SubtractFunction < Number >,
            const Number*,
            const size_t,
            size_t&,
            size_t&);

        using TSubPair = pair < string, runFunction >;

        const TSubPair subCases[] = {
          make_pair("_Slow", static_cast<runFunction>(MaximumDifference2Items::Slow)),
          make_pair("_Fast", static_cast<runFunction>(MaximumDifference2Items::Fast))
        };
        const size_t subCasesSize = sizeof(subCases) / sizeof(TSubPair);

        for (size_t j = 0; j < subCasesSize; j++)
        {
            const TSubPair& subCase = subCases[j];
            const string& subCaseName = subCase.first;

            size_t indexMin, indexMax;

            const Number actual = subCase.second(subtractFunction, numbers, numbersSize, indexMin, indexMax);

            Assert::Greater(indexMax, indexMin, "Min_Max_Indexes_" + subCaseName);
            Assert::AreEqual(testCase.ExpectedDifference, actual, "Difference_" + subCaseName);
            Assert::AreEqual(testCase.ExpectedIndexMin, indexMin, "Min_" + subCaseName);
            Assert::AreEqual(testCase.ExpectedIndexMax, indexMax, "Max_" + subCaseName);
        }
    }
}

void MaximumDifference2ItemsTests::Test()
{
    test_utilities<TestCase>::run_tests(RunTestCase, GenerateTestCases);
}