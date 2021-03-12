#include <string>
#include <vector>

#include "LongestCommonSubsequence.h"
#include "LongestCommonSubsequenceTests.h"
#include "../test_utilities.h"

using namespace std;
using namespace Privet::Algorithms::Strings::Tests;
using namespace Privet::Algorithms::Strings;
using namespace Privet::Algorithms;

namespace
{
    struct LCSTestCase final
    {
        string A;
        string B;

        vector<char> Expected;

        LCSTestCase(
            const string& a = {},
            const string& b = {},
            const vector<char>& expected = {})
            : A(a), B(b), Expected(expected)
        {
        }
    };

    void LongestCommonSubsequenceTests_String()
    {
        vector<LCSTestCase> testCases;

        testCases.push_back({ "A", "A",{ 'A' } });
        testCases.push_back({ "AB", "AB",{ 'A', 'B' } });
        testCases.push_back({ "CAb", "bCAd",{ 'C', 'A' } });
        testCases.push_back({ "c_e+xyz", "dcbae pqr",{ 'c', 'e' } });
        testCases.push_back({ "tarbkcnfhiccb", "ezacfnib",{ 'a', 'c', 'f', 'i', 'b' } });

        vector<char> actual;

        const auto size = testCases.size();
        for (size_t i = 0; i < size; ++i)
        {
            const auto& testCase = testCases[i];

            const auto actual_length = LongestCommonSubsequence(testCase.A, testCase.B, actual);
            Assert::AreEqual(testCase.Expected, actual, "Case_" + to_string(i));
            Assert::AreEqual(testCase.Expected.size(), actual_length, "Case_" + to_string(i) + "_length");
        }
    }

    void LongestCommonSubsequenceTests_Int()
    {
        const vector<int> s1{ 10, 23, 59, 97, 672, -3245450, 34976 };
        const vector<int> s2{ -34976, 23, 23, 23, 23, 234867, 23, 672, 23, 672, 672, 345, 97, 97, 724 };
        const vector<int> expected{ 23, 672 };

        vector<int> actual;
        const auto actual_length = LongestCommonSubsequence(s1, s2, actual);

        Assert::AreEqual<int>(
            expected, actual, "Error in LongestCommonSubsequenceTests_Int.");
        Assert::AreEqual(expected.size(), actual_length, "LongestCommonSubsequenceTests_Int.length.");
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
                void LongestCommonSubsequenceTests()
                {
                    LongestCommonSubsequenceTests_String();
                    LongestCommonSubsequenceTests_Int();
                }
            }
        }
    }
}