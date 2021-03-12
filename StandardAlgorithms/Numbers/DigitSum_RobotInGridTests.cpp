#include <vector>
#include "../test_utilities.h"
#include "../Utilities/TestNameUtilities.h"
#include "DigitSum_RobotInGrid.h"
#include "DigitSum_RobotInGridTests.h"
#include "../Assert.h"

using namespace std;
using namespace Privet::Algorithms::Tests;
using namespace Privet::Algorithms::Numbers;
using namespace Privet::Algorithms;

namespace
{
    using Number = long long int;
    using TPair = std::pair< Number, Number >;

    static void MainTest();
    static void DigitCountTest();
    static void DigitSumTest();

    static void MaxDigitSumTest();
    static void MaxDigitSumAddAutoCases(std::vector< TPair >& tests);
    static Number SumFromOneToValue(const Number& value);

    struct TestCase final : base_test_case
    {
        Number Rows{};
        Number Columns{};
        Number DigitSumLimit{};

        Number Expected{};

        //TODO: p2. add another data members.
        explicit TestCase(std::string&& name = "")
            : base_test_case(forward<string>(name))
        {
        }

        void Print(std::ostream& str) const override
        {
            base_test_case::Print(str);

            str << ", Rows=" << Rows
                << ", Columns=" << Columns
                << ", DigitSumLimit=" << DigitSumLimit
                << ", Expected=" << Expected;
        }
    };

    void GenerateTestCases(vector<TestCase>& testCases)
    {
        const Number side = 10;
        const Number maxDigitSumLimit = 9 * 2;
        {
            TestCase testCase("All with extra");
            testCase.Rows = side;
            testCase.Columns = side;
            testCase.DigitSumLimit = maxDigitSumLimit + 200;
            testCase.Expected = testCase.Rows * testCase.Columns;
            testCases.push_back(testCase);
        }

        for (int i = 0; i < maxDigitSumLimit; ++i)
        {
            TestCase testCase = VectorUtilities::Last(testCases);
            testCase.set_Name(0 == i ? "All" : "All but " + to_string(i));
            testCase.DigitSumLimit = maxDigitSumLimit - i;

            testCase.Expected = i <= side
                ? side * side - SumFromOneToValue(i)
                : (testCase.DigitSumLimit + 1)
                * (testCase.DigitSumLimit + 2) / 2;

            testCases.push_back(testCase);
        }
        //{
        //  TestCase testCase = VectorUtilities::Last(testCases);
        //  testCase.set_Name("All but 1");
        //  testCase.DigitSumLimit--;//17
        //  testCase.Expected--;//(9,9)
        //  testCases.push_back(testCase);
        //}
        //{
        //  TestCase testCase = VectorUtilities::Last(testCases);
        //  testCase.set_Name("All but 2";
        //  testCase.DigitSumLimit--;//16
        //  testCase.Expected -= 2;//(8,9) and (9,8)
        //  testCases.push_back(testCase);
        //}
        //{
        //  TestCase testCase = VectorUtilities::Last(testCases);
        //  testCase.set_Name("All but 3";
        //  testCase.DigitSumLimit--;//15
        //  testCase.Expected -= 3;//(7,9), (8,8), (9,7)
        //  testCases.push_back(testCase);
        //}
        //{
        //  TestCase testCase = VectorUtilities::Last(testCases);
        //  testCase.set_Name("All but 4";
        //  testCase.DigitSumLimit--;//14
        //  testCase.Expected -= 4;//(6,9), (7,8), (8,7), (9,6)
        //  testCases.push_back(testCase);
        //}
        //{
        //  TestCase testCase = VectorUtilities::Last(testCases);
        //  testCase.set_Name("All but 5";
        //  testCase.DigitSumLimit--;//13
        //  testCase.Expected -= 5;//(5,9), (6,8), (7,7), (8,6), (9,5)
        //  testCases.push_back(testCase);
        //}
        //{
        //  TestCase testCase = VectorUtilities::Last(testCases);
        //  testCase.set_Name("All but 6";
        //  testCase.DigitSumLimit--;//12
        //  testCase.Expected -= 6;//(4,9), (5,8), (6,7), (7,6), (8,5), (9,4)
        //  testCases.push_back(testCase);
        //}
#ifndef _DEBUG
        {
            TestCase testCase;
            testCase.Rows = 54;
            testCase.Columns = 29;
            testCase.DigitSumLimit = 16;
            testCase.set_Name(to_string(testCase.Rows)
                + "_" + to_string(testCase.Columns)
                + "_" + to_string(testCase.DigitSumLimit));
            testCase.Expected = 1344;
            testCases.push_back(testCase);
        }
        {
            TestCase testCase = VectorUtilities::Last(testCases);
            testCase.DigitSumLimit--;
            testCase.set_Name(to_string(testCase.Rows)
                + "_" + to_string(testCase.Columns)
                + "_" + to_string(testCase.DigitSumLimit));
            testCase.Expected = 1251;
            testCases.push_back(testCase);
        }
#endif
    }

    void DigitCountTest()
    {
        using TPair = pair< Number, Number >;
        vector< TPair > tests;
        tests.push_back(make_pair(1, 1));
        tests.push_back(make_pair(2, 1));
        tests.push_back(make_pair(9, 1));
        tests.push_back(make_pair(10, 2));
        tests.push_back(make_pair(11, 2));
        tests.push_back(make_pair(19, 2));
        tests.push_back(make_pair(20, 2));
        tests.push_back(make_pair(21, 2));
        tests.push_back(make_pair(98, 2));
        tests.push_back(make_pair(99, 2));
        tests.push_back(make_pair(100, 3));
        tests.push_back(make_pair(101, 3));
        tests.push_back(make_pair(1000 * 1000 - 1, 6));
        tests.push_back(make_pair(1000 * 1000, 7));

        for (const TPair& pair : tests)
        {
            const Number actual = DigitSum_RobotInGrid< Number >::DigitCount(
                pair.first);
            Assert::AreEqual<Number>(pair.second, actual,
                "DigitCount_" + to_string(pair.first));
        }
        }

    void DigitSumTest()
    {
        using TPair = pair< Number, Number >;
        vector< TPair > tests;
        tests.push_back(make_pair(1, 1));
        tests.push_back(make_pair(2, 2));
        tests.push_back(make_pair(9, 9));
        tests.push_back(make_pair(10, 1));
        tests.push_back(make_pair(11, 2));
        tests.push_back(make_pair(19, 10));
        tests.push_back(make_pair(20, 2));
        tests.push_back(make_pair(21, 3));
        tests.push_back(make_pair(98, 17));
        tests.push_back(make_pair(99, 18));
        tests.push_back(make_pair(100, 1));
        tests.push_back(make_pair(101, 2));
        tests.push_back(make_pair(1000 * 1000 - 1, 9 * 6));
        tests.push_back(make_pair(1000 * 1000, 1));
        tests.push_back(make_pair(1000 * 1000 + 25, 8));

        for (const TPair& pair : tests)
        {
            const Number actual = DigitSum_RobotInGrid< Number >::DigitSum(
                pair.first);
            Assert::AreEqual<Number>(pair.second, actual,
                "DigitSum_" + to_string(pair.first));
        }
    }

    void MaxDigitSumAddAutoCases(vector<TPair>& tests)
    {
        Number maxValue = Number(1);

        for (Number i = Number(1); i < Number(
#ifdef _DEBUG
            12
#else
            123
#endif
        ); ++i)
        {
            const Number temp = DigitSum_RobotInGrid< Number >::DigitSum(i);
            if (maxValue < temp)
            {
                maxValue = temp;
            }

            tests.push_back(make_pair(-i, maxValue));
        }
    }

    void MaxDigitSumTest()
    {
        vector< TPair > tests;
        MaxDigitSumAddAutoCases(tests);

        //Manual tests.
        for (Number i = Number(1); i < Number(10); ++i)
        {
            tests.push_back(make_pair(i, i));
        }

        for (Number i = Number(10); i < Number(19); ++i)
        {
            tests.push_back(make_pair(i, 9));
        }

        for (Number i = Number(19); i < Number(29); ++i)
        {
            tests.push_back(make_pair(i, 10));
        }

        for (Number i = Number(89); i < Number(99); ++i)
        {
            tests.push_back(make_pair(i, 17));
        }

        for (Number i = Number(99); i < Number(199); ++i)
        {
            tests.push_back(make_pair(i, 18));
        }

        for (Number i = Number(199); i < Number(299); ++i)
        {
            tests.push_back(make_pair(i, 19));
        }

        for (const TPair& pair : tests)
        {
            const Number& inputRaw = pair.first;
            const bool isPositive = Number(0) < inputRaw;
            const Number& inputData = isPositive ? inputRaw : -inputRaw;

            const string name = "MaxDigitSum_" + to_string(inputData)
                + (isPositive ? "" : "_Auto");

            const Number actual = DigitSum_RobotInGrid< Number >::MaxDigitSum(
                inputData);
            Assert::AreEqual<Number>(pair.second, actual, name);
        }
    }

    Number SumFromOneToValue(const Number& value)
    {
        const Number result = value * (value + 1) / 2;
        return result;
    }

    void RunTestCase(const TestCase& testCase)
    {
        {
            const Number actual = DigitSum_RobotInGrid< Number >::Slow(
                testCase.Rows,
                testCase.Columns,
                testCase.DigitSumLimit);
            Assert::AreEqual<Number>(testCase.Expected, actual,
                testCase.get_Name() + "_Slow");
        }
        {
            //TODO: p3 finish fast test..
            //const Number actual = DigitSum_RobotInGrid< Number >::Fast(
            //  testCase.Rows,
            //  testCase.Columns,
            //  testCase.DigitSumLimit);
            //Assert::AreEqual<Number>(testCase.Expected, actual,
            //  testCase.Name + "_Fast");
        }
    }

    void MainTest()
    {
        test_utilities<TestCase>::run_tests(RunTestCase, GenerateTestCases);
    }
    }

void Privet::Algorithms::Numbers::Tests::DigitSum_RobotInGridTests()
{
    DigitCountTest();
    DigitSumTest();
    MaxDigitSumTest();
    MainTest();
}