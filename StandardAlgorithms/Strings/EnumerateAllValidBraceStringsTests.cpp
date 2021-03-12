#include <vector>
#include "../test_utilities.h"
#include "../Utilities/PrintUtilities.h"
#include "EnumerateAllValidBraceStrings.h"
#include "EnumerateAllValidBraceStringsTests.h"

using namespace std;
using namespace Privet::Algorithms::Strings::Tests;
using namespace Privet::Algorithms::Strings;
using namespace Privet::Algorithms::Tests;
using namespace Privet::Algorithms;

namespace
{
    struct TestCase final : base_test_case
    {
        int Length;
        vector<string> ExpectedStrings;

        TestCase(
            string&& name = {},
            int length = {},
            vector<string>&& expectedStrings = {})
            : base_test_case(forward<string>(name)),
            Length(length),
            ExpectedStrings(forward<vector<string>>(expectedStrings))
        {
            sort(ExpectedStrings.begin(), ExpectedStrings.end());
        }

        void Validate() const override
        {
            base_test_case::Validate();

            if (0 == Length)
            {
                if (1 == ExpectedStrings.size()
                    && (*ExpectedStrings.begin()).empty())
                {
                    return;
                }

                throw runtime_error("0 length must generate only an empty string.");
            }

            Assert::Greater(ExpectedStrings.size(), 0, "ExpectedStrings.size()");
            for (const auto& s : ExpectedStrings)
            {
                Assert::GreaterOrEqual(Length, s.size(), s);
            }
        }

        void Print(ostream& str) const override
        {
            base_test_case::Print(str);

            str << ", Length=" << Length;
            ::Print<>("ExpectedStrings", ExpectedStrings.begin(), ExpectedStrings.end(), str);
        }
    };

    void GenerateTestCases(vector<TestCase>& testCases)
    {
        testCases.push_back({ "Zero", 0, {""} });
        testCases.push_back({ "2", 2, {"", "()"} });
        testCases.push_back({ "4", 4, {"", "()", "()()", "(())"} });
        testCases.push_back({ "6", 6, {"", "()", "()()", "(())",
          "()()()", "()(())", "(()())", "(())()", "((()))"
        } });
    }

    void RunTestCase(const TestCase& testCase)
    {
        vector<string> actual;

        auto callback = [&](const string& s) -> void {
            actual.push_back(s);
        };
        EnumerateAllValidBraceStrings(testCase.Length, callback);

        sort(actual.begin(), actual.end());
        Assert::AreEqual(testCase.ExpectedStrings, actual, "Generated strings");
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
                void EnumerateAllValidBraceStringsTests()
                {
                    test_utilities<TestCase>::run_tests(RunTestCase, GenerateTestCases);
                }
            }
        }
    }
}