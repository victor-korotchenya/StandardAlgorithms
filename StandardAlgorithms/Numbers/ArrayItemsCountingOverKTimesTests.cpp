#include <vector>
#include "ArrayItemsCountingOverKTimes.h"
#include "ArrayItemsCountingOverKTimesTests.h"
#include "ArrayItemsCountingOverHalfTimes_BoyerMoore_majority_vote_alg.h"
#include "../test_utilities.h"
#include "../Utilities/Random.h"
#include "../Utilities/PrintUtilities.h"

using namespace std;
using namespace Privet::Algorithms::Tests;
using namespace Privet::Algorithms::Numbers;
using namespace Privet::Algorithms;

namespace
{
    using Number = int;

    struct TestCase final : base_test_case
    {
        std::vector<Number> Data;
        size_t MinimumCount;

        std::vector<size_t> Expected;

        explicit TestCase(
            std::string&& name = "",
            const std::vector<Number>& data = {},
            const size_t minimumCount = 0,
            const std::vector<size_t>& expected = {})
            : base_test_case(forward<string>(name)),
            Data(data), MinimumCount(minimumCount),
            Expected(expected)
        {
        }

        void Print(std::ostream& str) const override
        {
            base_test_case::Print(str);

            ::Print(", Data=", Data, str);

            ::Print(", Expected=", Expected, str);

            str << " MinimumCount=" << MinimumCount;
        }
    };

    void GenerateTestCases(vector<TestCase>& testCases)
    {
        {
            TestCase testCase("Major",
                { 1, 1, 7, 2, 7, 7, 7 },
                4,
                { 2 });
            testCases.push_back(testCase);
        }
        {
            TestCase testCase("Major7",
                { 1, 7, 7, 2, 7, 7, 7 },
                4,
                { 1 });
            testCases.push_back(testCase);
        }
        {
            TestCase testCase("Same",
                { 7, 7, 7, 7 },
                3,
                { 0 });
            testCases.push_back(testCase);
        }
        {
            TestCase testCase("Simple",
                { 8, 3, 8, 2 },
                2,
                { 0 });
            testCases.push_back(testCase);
        }
        {
            TestCase testCase("Simple2",
                { 1, 2, 3, 4, 3, 2 },
                2,
                { 1, 2 });
            testCases.push_back(testCase);
        }
        {
            TestCase testCase("None",
                { 1, 2, 3, 4, 2, 3 },
                3,
                {});
            testCases.push_back(testCase);
        }
        {
            TestCase testCase("One",
                { 1, 3, 3, 1, 2, 2, 3 },
                3,
                { 1 });
            testCases.push_back(testCase);
        }
        {
            TestCase testCase("All items",
                { 2, 3, 3, 2, 2, 3 },
                3,
                { 0, 1 });
            testCases.push_back(testCase);
        }
        {
            TestCase testCase("Everything for 1",
                { 1, 2, 3, 1, 7 },
                1,
                { 0, 1, 2, 4 });
            testCases.push_back(testCase);
        }
        {
            TestCase testCase("Nine",
                { 1, 2, 3,
                3, 2, 1,
                2, 3, 3 },
                4,
                { 2 });
            testCases.push_back(testCase);
        }
        {
            const size_t minValue = 1;

            SizeRandom sizeRandom;
            const size_t size = sizeRandom(minValue << 1,
#ifdef _DEBUG
                20
#else
                99
#endif
            );

            TestCase testCase("Random");
            FillRandom(testCase.Data, size);

            testCase.MinimumCount = sizeRandom(minValue, minValue + (size >> 1));

            testCase.Expected = ArrayItemsCountingOverKTimes<Number>::MajorElementsIndexes_MuchMemory(
                testCase.Data, testCase.MinimumCount);

            sort(testCase.Expected.begin(), testCase.Expected.end());

            testCases.push_back(testCase);
        }
    }

    void RunTestCase(const TestCase& testCase)
    {
        using alg = vector<size_t>(*)(const vector<Number>&, const size_t);

        const vector<pair<string, alg>> subTests = {
          { "Slow" , &ArrayItemsCountingOverKTimes<Number>::MajorElementsIndexes_MuchMemory },
          { "Fast" , &ArrayItemsCountingOverKTimes<Number>::MajorElementsIndexesFast },
        };

        for (const auto& sub : subTests)
        {
            auto actual = sub.second(
                testCase.Data, testCase.MinimumCount);

            //To compare results easily.
            sort(actual.begin(), actual.end());

            Assert::AreEqual(testCase.Expected, actual, testCase.get_Name() + "_" + sub.first);
            if (testCase.MinimumCount <= testCase.Data.size() >> 1)
                continue;

            const auto actual_it = Boyer_Moore_majority_vote_algorithm(testCase.Data.begin(), testCase.Data.end());
            vector<Number> expected2;
            if (!testCase.Expected.empty())
                expected2.push_back(testCase.Data[testCase.Expected[0]]);

            vector<Number> actual2;
            if (actual_it != testCase.Data.end())
                actual2.push_back(*actual_it);

            Assert::AreEqual(expected2, actual2, "Boyer_Moore_majority_vote_algorithm" + testCase.get_Name() + "_" + sub.first);
        }
    }
}

void Privet::Algorithms::Numbers::Tests::ArrayItemsCountingOverKTimesTests()
{
    test_utilities<TestCase>::run_tests(RunTestCase, GenerateTestCases);
}