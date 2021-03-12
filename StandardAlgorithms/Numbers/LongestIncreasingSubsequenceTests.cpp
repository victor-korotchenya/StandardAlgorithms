#include "LongestIncreasingSubsequence.h"
#include "LongestIncreasingSubsequenceTests.h"
#include "../Utilities/PrintUtilities.h"
#include "../test_utilities.h"

using namespace std;
using namespace Privet::Algorithms::Numbers;
using namespace Privet::Algorithms::Tests;
using namespace Privet::Algorithms;

namespace
{
    using Number = int;
    using TContainer = vector<Number>;
    using Indexes = LongestIncreasingSubsequence<Number>::Indexes;

    class TestCase final : public base_test_case
    {
        TContainer _Data;
        Indexes _Expected;

    public:

        TestCase(
            string&& name,
            TContainer&& data,
            Indexes&& expected)
            : base_test_case(forward<string>(name)),
            _Data(forward<TContainer>(data)), _Expected(forward<Indexes>(expected))
        {
        }

        inline const TContainer& get_Data() const { return _Data; }
        inline const Indexes& get_Expected() const { return _Expected; }

        void Print(ostream& str) const override
        {
            base_test_case::Print(str);

            AppendSeparator(str);
            ::Print("Data", _Data, str);
            ::Print("Expected", _Expected, str);
        }
    };

    void GenerateTestCases(
        vector<TestCase>& testCases)
    {
        testCases.push_back({ "Increasing",{ 5, 10, 20, 500 },{ 0, 1, 2, 3 } });
        testCases.push_back({ "NonIncreasing",{ 50, 10 },{ 1 } });
        testCases.push_back({ "Trivial",{ -97864 },{ 0 } });
        testCases.push_back({ "Many",{ -50, 10, 90, 25, -3, 1000, 50, 500 },{ 0, 1, 3, 6, 7 } });
    }

    void RunTestCase(const TestCase& testCase)
    {
        using TComparer = less<Number>;
        using Alg = Indexes(*)(const TContainer& source, TComparer);
        using NameAlg = pair<string, Alg>;

        const vector<NameAlg> subTests{
          { "Slow",LongestIncreasingSubsequence<Number>::Slow },
          { "Fast",LongestIncreasingSubsequence<Number>::Fast },
        };

        const string separator = "_";
        for (const auto& subTest : subTests)
        {
            const string name = testCase.get_Name() + separator + subTest.first;
            const auto actual = subTest.second(testCase.get_Data(), TComparer());
            Assert::AreEqual(testCase.get_Expected(), actual, name);
        }
    }
}

void Privet::Algorithms::Numbers::Tests::LongestIncreasingSubsequenceTests()
{
    test_utilities<TestCase>::run_tests(RunTestCase, GenerateTestCases);
}