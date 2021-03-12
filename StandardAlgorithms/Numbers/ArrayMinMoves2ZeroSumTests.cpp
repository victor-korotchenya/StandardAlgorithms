#include <numeric>
#include "../test_utilities.h"
//#include "../Utilities/Random.h"
#include "../Utilities/PrintUtilities.h"
#include "ArrayMinMoves2ZeroSum.h"
#include "ArrayMinMoves2ZeroSumTests.h"

using namespace std;
using namespace Privet::Algorithms::Numbers::Tests;
using namespace Privet::Algorithms::Numbers;
using namespace Privet::Algorithms::Tests;
using namespace Privet::Algorithms;

namespace
{
    using Number = long long int;

    struct TestCase final : base_test_case
    {
        vector<Number> Ring;
        size_t ExpectedMoves;

        TestCase(
            string&& name,
            vector<Number>&& ring,
            const size_t expectedMoves)
            : base_test_case(forward<string>(name)),
            Ring(forward<vector<Number>>(ring)), ExpectedMoves(expectedMoves)
        {
        }

        void Validate() const override
        {
            base_test_case::Validate();

            const auto s = accumulate(Ring.cbegin(), Ring.cend(), static_cast<Number>(0));
            Assert::AreEqual(0, s, "Total_sum_" + get_Name());
        }

        void Print(ostream& str) const override
        {
            base_test_case::Print(str);

            ::Print(", Ring=", Ring, str);

            str << " ExpectedMoves=" << ExpectedMoves;
        }
    };

    void GenerateTestCases(vector<TestCase>& testCases)
    {
        testCases.push_back({ "Empty", {}, 0 });
        testCases.push_back({ "One element", {0}, 0 });
        testCases.push_back({ "Two elements", {-34536, 34536 }, 1 });
        testCases.push_back({ "Two zeros", {0, 0}, 0 });
        testCases.push_back({ "With zero in the middle", {1, 0, -1}, 1 });
        testCases.push_back({ "Simple", { 10,
          1, -4, 2, 4, -3,
          -6, -4 }, 6 });

        /*{
          const auto sizeMin = 2;
          const auto sizeMax = 50;
          const auto size = UnsignedIntRandom()(sizeMin, sizeMax);

          vector<Number> data;
          FillRandom(data, size);

          const auto index = UnsignedIntRandom()(0, size - 1);
          const auto s = accumulate(data.cbegin(), data.cend(), static_cast<Number>(0));
          data[index] -= s;

          const auto result_fast = ArrayMinMoves2ZeroSum(data);
          testCases.push_back({ "Random", move(data), result_fast });
        }*/
    }

    void RunTestCase(const TestCase& testCase)
    {
        using alg = size_t(*)(const vector<Number>& ring);

        const vector<pair<string, alg>> subTests = {
          { "Fast", &ArrayMinMoves2ZeroSum<Number> },
          //{ "Slow", &ArrayMinMoves2ZeroSum_slow<Number> }
        };

        for (const auto& sub : subTests)
        {
            const auto actual = sub.second(testCase.Ring);
            Assert::AreEqual(testCase.ExpectedMoves, actual, testCase.get_Name() + "_" + sub.first);
        }
    }
}

namespace Privet
{
    namespace Algorithms
    {
        namespace Numbers
        {
            namespace Tests
            {
                void ArrayMinMoves2ZeroSumTests()
                {
                    test_utilities<TestCase>::run_tests(RunTestCase, GenerateTestCases);
                }
            }
        }
    }
}