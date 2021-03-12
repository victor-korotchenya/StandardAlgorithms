#include <vector>
#include "../test_utilities.h"
#include "../Utilities/PrintUtilities.h"
#include "MaxSumNonDecreasingSubsequence.h"
#include "MaxSumNonDecreasingSubsequenceTests.h"

using namespace std;
using namespace Privet::Algorithms::Tests;
using namespace Privet::Algorithms::Numbers::Tests;
using namespace Privet::Algorithms::Numbers;
using namespace Privet::Algorithms;

namespace
{
    using Number = unsigned short;
    using Data = std::vector<Number>;

    using LongNumber = unsigned long long int;

    struct TestCase final : base_test_case
    {
        Data Input;
        Data Expected;

        TestCase(
            std::string&& name = "", Data&& inputData = {}, Data&& expected = {});

        void Validate() const override;

        void Print(std::ostream& str) const override;

    private:

        void CheckExpectedWithinLimits(const size_t index) const;

        void RequireIncreasing(const size_t index, Number& lastExpected) const;
    };

    static void GenerateTestCases(std::vector<TestCase>& testCases);

    static void RunTestCase(const TestCase& testCase);

    void GenerateTestCases(vector<TestCase>& testCases)
    {
        {
            TestCase testCase("SecondZero", { 5, 2 }, { 5, 0 });
            testCases.push_back(testCase);
        }
        {
            TestCase testCase("BothEqual", { 5, 3 }, { 3, 3 });
            testCases.push_back(testCase);
        }
        {
            TestCase testCase("MiddleZero", { 65533, 1234, 65535, }, { 65533, 0, 65535 });
            testCases.push_back(testCase);
        }
        {
            TestCase testCase("MiddleZero2", { 65535, 1234, 65533, }, { 65533, 0, 65533 });
            testCases.push_back(testCase);
        }
        {
            TestCase testCase("Trivial", { 1 }, { 1 });
            testCases.push_back(testCase);
        }
        {
            TestCase testCase("Trivial2", { 20 }, { 20 });
            testCases.push_back(testCase);
        }
        {
            TestCase testCase("Increasing", { 10, 20 }, { 10, 20 });
            testCases.push_back(testCase);
        }
        {
            TestCase testCase("TwoSame", { 20, 20 }, { 20, 20 });
            testCases.push_back(testCase);
        }
        {
            TestCase testCase("ThreeSame", { 20, 20, 20 }, { 20, 20, 20 });
            testCases.push_back(testCase);
        }
        {
            TestCase testCase("Long",
                { 40, 20, 30, 10, 20, 50, 30, 100, 20 },
                { 20, 20, 20, 0, 20, 30, 30, 100, 0 });
            testCases.push_back(testCase);
        }
    }

    TestCase::TestCase(
        std::string&& name,
        Data&& inputData,
        Data&& expected)
        : base_test_case(forward<string>(name)),
        Input(forward<Data>(inputData)),
        Expected(forward<Data>(expected))
    {
    }

    void TestCase::Validate() const
    {
        base_test_case::Validate();
        ThrowIfEmpty(Expected, "Expected", get_Name());

        const auto size = Input.size();
        Assert::AreEqual(size, Expected.size(), "Sizes must be equal for " + get_Name());

        Number lastExpected = Expected[0];
        for (size_t i = 0; i < size; ++i)
        {
            CheckExpectedWithinLimits(i);
            RequireIncreasing(i, lastExpected);
        }

        Assert::Greater(lastExpected, Number(0), "Last must be non-negative, name=" + get_Name());
    }

    void TestCase::CheckExpectedWithinLimits(
        const size_t index) const
    {
        if (Expected[index] < 0)
        {
            ostringstream ss;
            ss << "Expected[" << index << "] = " << Expected[index] << " must be non-negative.";
            StreamUtilities::throw_exception(ss);
        }

        if (Input[index] < Expected[index])
        {
            ostringstream ss;
            ss << "Expected[" << index << "] = " << Expected[index]
                << " must not exceed " << "Input[" << index << "] = " << Input[index] << ".";
            StreamUtilities::throw_exception(ss);
        }
    }

    void TestCase::RequireIncreasing(
        const size_t index, Number& lastExpected) const
    {
        if (0 == Expected[index])
        {
            return;
        }

        if (Expected[index] < lastExpected)
        {
            ostringstream ss;
            ss << "Expected[" << index << "] = " << Expected[index]
                << " must be equal to or greater than the previous value = " << lastExpected << ".";
            StreamUtilities::throw_exception(ss);
        }

        lastExpected = Expected[index];
    }

    void TestCase::Print(ostream& str) const
    {
        base_test_case::Print(str);

        AppendSeparator(str);
        ::Print("Input", Input, str);
        ::Print("Expected", Expected, str);
    }

    void RunTestCase(const TestCase& testCase)
    {
        const auto actual = MaxSumNonDecreasingSubsequence<Number, LongNumber>::Compute(testCase.Input);
        Assert::AreEqual(testCase.Expected, actual, testCase.get_Name());
    }
}

void MaxSumNonDecreasingSubsequenceTests::Test()
{
    test_utilities<TestCase>::run_tests(RunTestCase, GenerateTestCases);
}