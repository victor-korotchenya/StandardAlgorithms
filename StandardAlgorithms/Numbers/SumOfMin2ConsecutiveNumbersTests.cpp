#include <algorithm> // max_element
#include <utility>
#include "../Utilities/Random.h"
#include "../Utilities/VectorUtilities.h"
#include "SumOfMin2ConsecutiveNumbersTests.h"
#include "../Utilities/PrintUtilities.h"

using namespace std;
using namespace Privet::Algorithms::Numbers::Tests;
using namespace Privet::Algorithms::Numbers;
using namespace Privet::Algorithms::Tests;
using namespace Privet::Algorithms;

namespace
{
    using Number = unsigned long long;
    using Pair = SumOfMin2ConsecutiveNumbers<Number>::Pair;
    using Result = SumOfMin2ConsecutiveNumbers<Number>::Result;

    struct TestCase final : base_test_case
    {
        Number Input;

        Result Expected;

        TestCase(
            std::string&& name = "",
            const Number& input = Number(0),
            const Result& expected = Result());

        void Validate() const override;

        void Print(std::ostream& str) const override;

        void fix_name();
    };

    static void GenerateTestCases(std::vector<TestCase>& testCases);

    static void AddTestCasesRegular(std::vector<TestCase>& testCases);

    static Number GetNextNumber(const std::vector<TestCase>& testCases);

    static void AddTestCaseRandom(std::vector<TestCase>& testCases);

    static void AddTestCasesPowerOfTwo(std::vector<TestCase>& testCases);

    static void RunTestCase(const TestCase& testCase);

    void GenerateTestCases(vector<TestCase>& testCases)
    {
        AddTestCasesRegular(testCases);

        AddTestCaseRandom(testCases);

        //It must be after Random tests.
        AddTestCasesPowerOfTwo(testCases);

        //It must be the last line.
        fix_names(testCases);
    }

    void AddTestCasesRegular(vector<TestCase>& testCases)
    {
#ifndef _DEBUG
        {
            TestCase testCase;
            testCase.Input = 3;
            testCase.Expected.push_back(Pair(1, 2));
            testCases.push_back(testCase);
        }
        {
            TestCase testCase;
            testCase.Input = 5;
            testCase.Expected.push_back(Pair(2, 3));
            testCases.push_back(testCase);
        }
        {
            TestCase testCase;
            testCase.Input = 6;
            testCase.Expected.push_back(Pair(1, 3));
            testCases.push_back(testCase);
        }
        {
            TestCase testCase;
            testCase.Input = 7;
            testCase.Expected.push_back(Pair(3, 4));
            testCases.push_back(testCase);
        }
        {
            TestCase testCase;
            testCase.Input = 9;
            testCase.Expected.push_back(Pair(2, 4));
            testCase.Expected.push_back(Pair(4, 5));
            testCases.push_back(testCase);
        }
        {
            TestCase testCase;
            testCase.Input = 10;
            testCase.Expected.push_back(Pair(1, 4));
            testCases.push_back(testCase);
        }
        {
            TestCase testCase;
            testCase.Input = 15;
            testCase.Expected.push_back(Pair(1, 5));
            testCase.Expected.push_back(Pair(4, 6));
            testCase.Expected.push_back(Pair(7, 8));
            testCases.push_back(testCase);
        }
#endif
        {
            TestCase testCase;
            testCase.Input = 105;
            testCase.Expected = { Pair(1, 14),
              Pair(6, 15),
              Pair(12, 18),
              Pair(15, 20),
              Pair(19, 23),
              Pair(34, 36),
              Pair(52, 53),
            };
            testCases.push_back(testCase);
        }
#ifndef _DEBUG
        {
            TestCase testCase;
            testCase.Input = 127;
            testCase.Expected.push_back(Pair(63, 64));
            testCases.push_back(testCase);
        }
        {
            TestCase testCase;
            testCase.Input = 129;
            testCase.Expected.push_back(Pair(19, 24));
            testCase.Expected.push_back(Pair(42, 44));
            testCase.Expected.push_back(Pair(64, 65));
            testCases.push_back(testCase);
        }
        {
            TestCase testCase;
            testCase.Input = 4095;
            testCase.Expected = { Pair(1, 90),
              Pair(14, 91),
              Pair(24, 93),
              Pair(31, 95),
              Pair(34, 96),
              Pair(69, 113),
              Pair(77, 118),
              Pair(86, 124),
              Pair(100, 134),
              Pair(122, 151),
              Pair(145, 170),
              Pair(185, 205),
              Pair(219, 236),
              Pair(266, 280),
              Pair(286, 299),
              Pair(309, 321),
              Pair(405, 414),
              Pair(451, 459),
              Pair(582, 588),
              Pair(680, 685),
              Pair(817, 821),
              Pair(1364, 1366),
              Pair(2047, 2048),
            };
            testCases.push_back(testCase);
        }
#endif
    }

    void AddTestCasesPowerOfTwo(
        vector<TestCase>& testCases)
    {
        const Number minValue = Number(1) << 31;
        const Number maxValue = Number(1) << 35;

        for (Number i = minValue; i < maxValue; i = i << 1)
        {
            TestCase testCase;
            testCase.Input = i;
            //Empty expected.
            testCases.push_back(testCase);
        }
    }

    Number
        GetNextNumber(const vector<TestCase>& testCases)
    {
        Assert::NotEmpty(testCases, "GetNextNumber");

        const auto maxTest = max_element(testCases.begin(), testCases.end(),
            [](const TestCase& a, const TestCase& b) {return a.Input < b.Input; }
        );

        const Number result = (*maxTest).Input + Number(1);
        return result;
    }

    void AddTestCaseRandom(vector<TestCase>& testCases)
    {
        const Number minValue = GetNextNumber(testCases);

        const Number someNumber = Number(100);
        const Number maxValue = minValue + someNumber;

        TestCase testCase;

        SizeRandom sizeRandom;

        for (;;)
        {
            testCase.Input = sizeRandom(minValue, maxValue);

            if (IsPowerOfTwo(testCase.Input))
            {
                continue;
            }

            testCase.set_Name("Random_" + to_string(testCase.Input));
            testCase.Expected = SumOfMin2ConsecutiveNumbers<Number>::Slow(testCase.Input);
            testCases.push_back(testCase);
            return;
        }
    }

    TestCase::TestCase(
        string&& name,
        const Number& input,
        const Result& expected)
        : base_test_case(forward<string>(name)),
        Input(input), Expected(expected)
    {
    }

    void TestCase::Validate() const
    {
        base_test_case::Validate();

        if (!Expected.empty())
        {
            const bool requireUnique = true;
            auto name = get_Name();
            VectorUtilities::RequireArrayIsSorted(Expected, name, requireUnique);

            for (const Pair& p : Expected)
            {
                const Number actual = SumToN(p.first, p.second);
                Assert::Greater(p.second, p.first, "High and low_" + name);
                Assert::AreEqual(Input, actual, "Sum_" + name);
            }
        }
    }

    void TestCase::Print(ostream& str) const
    {
        base_test_case::Print(str);

        ::Print(", Expected=", Expected, str);

        str << " Input=" << Input;
    }

    void TestCase::fix_name()
    {
        if (get_Name().empty())
        {
            set_Name(to_string(Input));
        }
    }

    void RunTestCase(const TestCase& testCase)
    {
        using Alg = Result(*)(const Number&);

        const pair<string, Alg> subTests[] = {
          make_pair("Slow", SumOfMin2ConsecutiveNumbers<Number>::Slow),
          make_pair("OverOptimized", SumOfMin2ConsecutiveNumbers<Number>::OverOptimized),
        };

        for (const auto& subTest : subTests)
        {
            const auto actual = subTest.second(testCase.Input);
            Assert::AreEqual(testCase.Expected, actual, testCase.get_Name() + "_" + subTest.first);
        }
    }
}

void SumOfMin2ConsecutiveNumbersTests::Test()
{
    test_utilities<TestCase>::run_tests(RunTestCase, GenerateTestCases);
}