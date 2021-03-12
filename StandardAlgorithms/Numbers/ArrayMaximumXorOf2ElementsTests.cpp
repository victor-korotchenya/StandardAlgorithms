#include "../test_utilities.h"
#include "../Utilities/PrintUtilities.h"
#include "../Utilities/Random.h"
#include "../Utilities/RandomGenerator.h"
#include "../Assert.h"
#include "ArrayMaximumXorOf2Elements.h"
#include "ArrayMaximumXorOf2ElementsTests.h"

using namespace std;
using namespace Privet::Algorithms::Numbers::Tests;
using namespace Privet::Algorithms::Numbers;
using namespace Privet::Algorithms::Tests;
using namespace Privet::Algorithms;

namespace
{
    using Number = unsigned
#ifdef _DEBUG
        short;
    //char;
#else
        long;
#endif

    struct TestCase final : base_test_case
    {
        vector<Number> Data;
        Number Expected;

        TestCase(string&& name, vector<Number>&& data, Number expected)
            : base_test_case(forward<string>(name)),
            Data(forward<vector<Number>>(data)),
            Expected(expected)
        {
        }

        void Print(ostream& str) const override
        {
            base_test_case::Print(str);

            Privet::Algorithms::Print<Number>(", Data=", Data, str);
            str << ", Expected=" << Expected;
        }
    };

    void GenerateTestCases(vector<TestCase>& testCases)
    {
        testCases = {
          { "Trivial close",{ 5,6 }, 3 },
          { "Trivial close symmetric",{ 6,5 }, 3 },

          { "Many",{ 2,8,13,7,10,12 }, 15 },

          { "Smallest",{ 0,1 }, 1 },
          { "Smallest symmetric",{ 1,0 }, 1 },
          { "Smallest with repetitions",{ 0,1,0,0,1,1,1,1 }, 1 },

          {"Trivial", {5,2}, 7},
          {"Trivial symmetric", {2,5}, 7},

          {"Trivial large", {127,16}, 111},
          {"Trivial large symmetric", {16,127}, 111 },

          {"Equal 0",{ 0,0 }, 0},
          { "Equal 1",{ 1,1 }, 0 },
          { "Equal 5",{ 5,5 }, 0 },
        };

        const auto max_attempts =
#ifdef _DEBUG
            1;
#else
            5;
#endif

        for (auto attempt = 0; attempt < max_attempts; ++attempt)
        {
            const size_t minValue = 2;
            const size_t maxValue =
#ifdef _DEBUG
                10;
#else
                33;
#endif

            SizeRandom sizeRandom;
            const auto data_size = sizeRandom(minValue, maxValue);

            vector<Number> data;
            FillRandom<Number, unsigned>(data, data_size
#ifdef _DEBUG
                , 64
#endif
                );

            const auto expected = ArrayMaximumXorOf2Elements_slow<>(data);

            testCases.push_back(
                TestCase("Random test_" + std::to_string(attempt),
                    forward<vector<Number>>(data), expected));
        }
    }

    void RunTestCase(const TestCase& testCase)
    {
        const auto actual_slow = ArrayMaximumXorOf2Elements_slow<>(testCase.Data);
        Assert::AreEqual(testCase.Expected, actual_slow,
            "slow_" + testCase.get_Name());

        const auto actual = ArrayMaximumXorOf2Elements<>(testCase.Data);
        Assert::AreEqual(testCase.Expected, actual,
            "fast_" + testCase.get_Name());
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
                void ArrayMaximumXorOf2ElementsTests()
                {
                    test_utilities<TestCase>::run_tests(RunTestCase, GenerateTestCases);
                }
            }
        }
    }
}