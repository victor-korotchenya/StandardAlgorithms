#include <vector>
#include "../test_utilities.h"
#include "../Utilities/PrintUtilities.h"
#include "ThreeStringsInterleaving.h"
#include "ThreeStringsInterleavingTests.h"

using namespace std;
using namespace Privet::Algorithms::Strings;
using namespace Privet::Algorithms::Tests;
using namespace Privet::Algorithms;

namespace
{
    struct TestCase final : base_test_case
    {
        string A, B, AB;
        bool ExpectedResult;
        vector<bool> ExpectedPositions;

        TestCase(
            string&& name = {},
            string&& a = {}, string&& b = {}, string&& ab = {},
            bool expectedResult = {}, vector<bool>&& expectedPositions = {})
            : base_test_case(forward<string>(name)),
            A(forward<string>(a)),
            B(forward<string>(b)),
            AB(forward<string>(ab)),
            ExpectedResult(expectedResult),
            ExpectedPositions(forward<vector<bool>>(expectedPositions))
        {
        }

        void Validate() const override
        {
            base_test_case::Validate();

            if (ExpectedResult)
            {
                Assert::AreEqual(A.size() + B.size(), AB.size(), "AB.size()");

                Assert::AreEqual(ExpectedPositions.size(), AB.size(),
                    "ExpectedPositions.size");

                string actual{};
                actual.reserve(ExpectedPositions.size() + 1);

                for (size_t index = 0, pos1 = 0, pos2 = 0;
                    index < ExpectedPositions.size(); ++index)
                {
                    if (ExpectedPositions[index])
                    {
                        Assert::Greater(A.size(), pos1, "pos1");
                        actual += A[pos1++];
                    }
                    else
                    {
                        Assert::Greater(B.size(), pos2, "pos2");
                        actual += B[pos2++];
                    }
                }

                Assert::AreEqual(AB, actual, "Restored AB.");
            }
        }

        void Print(ostream& str) const override
        {
            base_test_case::Print(str);

            str << ", A=" << A
                << ", B=" << B
                << ", AB=" << AB
                << ", ExpectedResult=" << ExpectedResult;

            ::Print<>(", ExpectedPositions", ExpectedPositions.begin(), ExpectedPositions.end(), str);
        }
    };

    void add_reverted(vector<TestCase>& testCases)
    {
        Assert::Greater(testCases.size(), 0,
            "There must be testCases to add a reverted.");

        auto last = testCases.back();
        last.set_Name(last.get_Name() + "_reverted");
        swap(last.A, last.B);
        for (size_t i = 0; i < last.ExpectedPositions.size(); i++)
        {
            last.ExpectedPositions[i] = !last.ExpectedPositions[i];
        }

        testCases.push_back(last);
    }

    void GenerateTestCases(vector<TestCase>& testCases)
    {
        testCases.push_back({ "Simple",
          "ab", "cde", "cadbe",
          true,{ false, true, false, true, false } });
        add_reverted(testCases);

        testCases.push_back({ "Trivial",
          "", "", "",
          true, });

        testCases.push_back({ "One empty",
          "", "a", "a",
          true, {false} });
        add_reverted(testCases);

        testCases.push_back({ "Two same strings",
          "a", "a", "aa",
          true, {true, false} });
        //add_reverted(testCases);

        testCases.push_back({ "Inside",
          "ab", "cd", "acdb",
          true, {true, false, false, true} });
        add_reverted(testCases);

        testCases.push_back({ "Chess",
          "ab", "cd", "acbd",
          true,{ true, false, true, false } });
        add_reverted(testCases);

        //No.
        testCases.push_back({ "Size mismatch",
          "b", "a", "aab",
          false });
        add_reverted(testCases);

        testCases.push_back({ "Char Mismatch",
          "b", "a", "ac",
          false });
        add_reverted(testCases);
    }

    void RunTestCase(const TestCase& testCase)
    {
        vector<bool> chosen_positions;

        const auto actual = is_interleaving(testCase.A, testCase.B, testCase.AB,
            chosen_positions);
        Assert::AreEqual(testCase.ExpectedResult, actual, "Result");
        Assert::AreEqual(testCase.ExpectedPositions, chosen_positions, "Positions");
    }
}

namespace Privet
{
    namespace Algorithms
    {
        namespace Strings
        {
            namespace Tests
            {
                void ThreeStringsInterleavingTests()
                {
                    test_utilities<TestCase>::run_tests(RunTestCase, GenerateTestCases);
                }
            }
        }
    }
}