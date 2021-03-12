#include <vector>
#include "../test_utilities.h"
#include <numeric>
#include "../Utilities/VectorUtilities.h"
#include "../Utilities/PrintUtilities.h"
#include "DiceCounts.h"
#include "DiceCountsTests.h"

using namespace std;
using namespace Privet::Algorithms::Tests;
using namespace Privet::Algorithms::Numbers;
using namespace Privet::Algorithms;

namespace
{
    using Number = unsigned long long int;

    struct TestCase final : base_test_case
    {
        Number N = 0;
        std::vector<Number> Expected;

        //TODO: p2. add another data members.
        explicit TestCase(std::string&& name = "")
            : base_test_case(forward<string>(name))
        {
        }

        void Validate() const override
        {
            base_test_case::Validate();

            auto name = get_Name();
            Assert::Greater(N, 0, "N_" + name);

            const auto sides = DiceCounts<>::Sides;

            const Number expectedSize = N * sides + 1;
            const size_t actualSize = Expected.size();
            Assert::AreEqual(expectedSize, actualSize, "Expected_Size_" + name);

            //TODO: Is Bitonic sorted?  VectorUtilities::RequireArrayIsSorted<SumCount>(Expected, "Expected_" + Name, true);

            const Number minValue = 0;
            Assert::AreEqual(minValue, Expected[0], "MinValue_" + name);

            const Number maxValue = 1;
            const auto last = VectorUtilities::Last(Expected);
            Assert::AreEqual(maxValue, last, "MaxValue_" + name);

            Number sumCountsExpected = sides;
            for (Number i = Number(1); i < N; ++i)
            {
                sumCountsExpected = MultiplyUnsigned(sumCountsExpected, Number(sides));
            }

            const Number sumCountsActual = SumCounts();
            Assert::AreEqual(sumCountsExpected, sumCountsActual, "SumCounts_" + name);
        }

        void Print(std::ostream& str) const override
        {
            base_test_case::Print(str);

            ::Print(", Expected=", Expected, str);

            str << " N=" << N;
        }

        Number SumCounts() const
        {
            Number result = accumulate(Expected.begin(), Expected.end(), Number(0),
                [](const Number& result1, const Number& p)
                {
                    auto result2 = AddUnsigned(result1, p);
                    return result2;
                });

            return result;
        }
    };

    void GenerateTestCases(vector<TestCase>& testCases)
    {
        {
            TestCase testCase;
            testCase.N = 1;
            testCase.Expected = { 0, 1, 1, 1, 1, 1, 1, };

            testCase.set_Name(to_string(testCase.N));
            testCases.push_back(testCase);
        }
        {
            TestCase testCase;
            testCase.N = 2;
            //11 12 13 14 15 16
            //21 22 23 24 25 26
            //31 32 33 34 35 36
            //41 42 43 44 45 46
            //51 52 53 54 55 56
            //61 62 63 64 65 66
            // Go by diagonals starting from the left upper corner.
            testCase.Expected = { 0, 0, 1, 2, 3, 4, 5, 6, 5, 4, 3, 2, 1 };

            testCase.set_Name(to_string(testCase.N));
            testCases.push_back(testCase);
        }
        {
            TestCase testCase;
            testCase.N = 3;
            testCase.Expected = { 0, 0, 0,
              1, 3, 6, 10, 15, 21, 25, 27,
              27, 25, 21, 15, 10, 6, 3, 1,
            };

            testCase.set_Name(to_string(testCase.N));
            testCases.push_back(testCase);
        }
    }

    void RunTestCase(const TestCase& testCase)
    {
        const auto actual = DiceCounts<Number>::CalcCountPerEachSum(testCase.N);
        Assert::AreEqual(testCase.Expected, actual, testCase.get_Name());
    }
}

void  Privet::Algorithms::Numbers::Tests::DiceCountsTests()
{
    test_utilities<TestCase>::run_tests(RunTestCase, GenerateTestCases);
}