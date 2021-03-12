#include <vector>
#include "../test_utilities.h"
#include "PrintFrequenciesInEditableArray.h"
#include "PrintFrequenciesInEditableArrayTests.h"
#include "../Utilities/Random.h"
#include "../Utilities/PrintUtilities.h"

using namespace std;
using namespace Privet::Algorithms::Numbers::Tests;
using namespace Privet::Algorithms::Numbers;
using namespace Privet::Algorithms::Tests;
using namespace Privet::Algorithms;

namespace
{
    using Number = short;

    struct TestCase final : base_test_case
    {
        std::vector< Number > Data;

        std::vector< Number > ExpectedFrequencies;

        void Print(std::ostream& str) const override;

        //TODO: p2. add another data members.
        explicit TestCase(std::string&& name);

        void FixUp();
    };

    static void RunTestCase(const TestCase& testCase);

    void GenerateTestCases(vector<TestCase>& testCases)
    {
        {
            TestCase testCase("Simple1.");

            testCase.Data.push_back(1);

            testCase.FixUp();

            testCases.push_back(testCase);
        }
        {
            TestCase testCase("Simple2.");

            testCase.Data.push_back(2);
            testCase.Data.push_back(3);
            testCase.Data.push_back(2);
            testCase.Data.push_back(3);
            testCase.Data.push_back(2);

            testCase.FixUp();

            testCases.push_back(testCase);
        }
        {
            TestCase testCase("All same.");

            const size_t length = 8;
            for (size_t i = 0; i < length; i++)
            {
                testCase.Data.push_back(5);
            }

            testCase.FixUp();

            testCases.push_back(testCase);
        }
        {
            TestCase testCase("Random test.");

            const size_t size = 10;
            const Number cutter = Number(size);
            FillRandom(testCase.Data, size, cutter);

            testCase.FixUp();

            testCases.push_back(testCase);
            //testCases.insert(testCases.begin(), testCase);
        }
    }

    void TestCase::Print(ostream& str) const
    {
        base_test_case::Print(str);

        ::Print(", Data=", Data, str);

        ::Print(", ExpectedFrequencies=", ExpectedFrequencies, str);
    }

    TestCase::TestCase(std::string&& name)
        : base_test_case(forward<string>(name))
    {
    }

    void TestCase::FixUp()
    {
        const size_t size = Data.size();

        ExpectedFrequencies.clear();
        ExpectedFrequencies.resize(size, 0);

        for (size_t i = 0; i < size; ++i)
        {
            if (Data[i] <= 0 || size <= Data[i])
            {
                Data[i] = 1;
            }

            ExpectedFrequencies[Data[i] - 1]++;
        }
    }

    void RunTestCase(const TestCase& testCase)
    {
        try
        {
            vector< Number > actualFrequencies = testCase.Data;

            PrintFrequenciesInEditableArray::PrintFrequencies(
                actualFrequencies.data(), actualFrequencies.size());

            Assert::AreEqual(testCase.ExpectedFrequencies, actualFrequencies, testCase.get_Name());
        }
        catch (...)
        {
            ostringstream ss;
            ss << testCase.get_Name();
            Print(" The failed testCase.Data=", testCase.Data, ss);

            StreamUtilities::throw_exception(ss);
        }
    }
}

void PrintFrequenciesInEditableArrayTests::Test()
{
    test_utilities<TestCase>::run_tests(RunTestCase, GenerateTestCases);
}