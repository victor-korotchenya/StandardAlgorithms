#include <string>
#include <vector>

#include "StableMatchingTests.h"
#include "StableMatching.h"
#include "../Assert.h"
#include "../Utilities/TestNameUtilities.h"

using namespace std;
using namespace Privet::Algorithms::Graphs;
using namespace Privet::Algorithms;

namespace
{
    using Vertex = int;
    using Preference = vector< Vertex >;
    using PreferenceList = vector< Preference >;

    struct SMTestCase final
    {
        string Name;
        inline const std::string& get_Name() const
        {
            return Name;
        }
        PreferenceList A;
        PreferenceList B;

        Preference Expected;
    };

    void BuildTestCases(vector< SMTestCase >& testCases)
    {
        {//4 men, 4 women:
            SMTestCase testCase;
            testCase.Name = "Four";

            Preference preference1;
            preference1.push_back(2);
            preference1.push_back(1);
            preference1.push_back(0);
            preference1.push_back(3);

            Preference preference2;
            preference2.push_back(1);
            preference2.push_back(3);
            preference2.push_back(0);
            preference2.push_back(2);

            Preference preference3;
            preference3.push_back(2);
            preference3.push_back(0);
            preference3.push_back(3);
            preference3.push_back(1);

            Preference preference4;
            preference4.push_back(0);
            preference4.push_back(1);
            preference4.push_back(2);
            preference4.push_back(3);

            testCase.A.push_back(preference1);
            testCase.A.push_back(preference2);
            testCase.A.push_back(preference3);
            testCase.A.push_back(preference4);

            Preference preference5;
            preference5.push_back(0);
            preference5.push_back(2);
            preference5.push_back(1);
            preference5.push_back(3);

            Preference preference6;
            preference6.push_back(2);
            preference6.push_back(3);
            preference6.push_back(1);
            preference6.push_back(0);

            Preference preference7;
            preference7.push_back(1);
            preference7.push_back(2);
            preference7.push_back(3);
            preference7.push_back(0);

            Preference preference8;
            preference8.push_back(3);
            preference8.push_back(1);
            preference8.push_back(0);
            preference8.push_back(2);

            testCase.B.push_back(preference5);
            testCase.B.push_back(preference6);
            testCase.B.push_back(preference7);
            testCase.B.push_back(preference8);

            testCase.Expected.push_back(0);
            testCase.Expected.push_back(3);
            testCase.Expected.push_back(2);
            testCase.Expected.push_back(1);

            testCases.push_back(testCase);
        }
        {//4 men, 4 women:
            SMTestCase testCase;
            testCase.Name = "FourWeeks";

            Preference preference1;
            preference1.push_back(3);
            preference1.push_back(1);
            preference1.push_back(2);
            preference1.push_back(0);

            Preference preference2;
            preference2.push_back(1);
            preference2.push_back(0);
            preference2.push_back(2);
            preference2.push_back(3);

            Preference preference3;
            preference3.push_back(0);
            preference3.push_back(1);
            preference3.push_back(2);
            preference3.push_back(3);

            testCase.B.push_back(preference1);
            testCase.B.push_back(preference2);
            testCase.B.push_back(preference3);
            testCase.B.push_back(preference3);

            testCase.A.push_back(preference3);
            testCase.A.push_back(preference3);
            testCase.A.push_back(preference3);
            testCase.A.push_back(preference3);

            testCase.Expected.push_back(2);
            testCase.Expected.push_back(1);
            testCase.Expected.push_back(3);
            testCase.Expected.push_back(0);

            testCases.push_back(testCase);
        }
        {//2 men, 2 women:
          //0: (0, 1)
          //1: (1, 0)
            SMTestCase testCase;
            testCase.Name = "Symmetric";

            Preference preference;
            preference.push_back(0);
            preference.push_back(1);

            Preference preference2;
            preference2.push_back(1);
            preference2.push_back(0);

            testCase.A.push_back(preference);
            testCase.A.push_back(preference2);

            testCase.B = testCase.A;

            testCase.Expected = preference;

            testCases.push_back(testCase);
        }
        {
            SMTestCase testCase;
            testCase.Name = "Trivial";

            Preference preference;
            preference.push_back(0);
            testCase.A.push_back(preference);

            testCase.B.push_back(preference);

            testCase.Expected.push_back(0);

            testCases.push_back(testCase);
        }
    }
}

void Privet::Algorithms::Trees::Tests::StableMatchingTests()
{
    vector< SMTestCase > testCases;
    BuildTestCases(testCases);
    TestNameUtilities::RequireUniqueNames(testCases, "testCases");

    vector< Vertex > result;

    const size_t length = testCases.size();
    for (size_t i = 0; i < length; ++i)
    {
        const SMTestCase& testCase = testCases[i];

        StableMatching<Vertex>::Find(
            testCase.A,
            testCase.B,
            result);

        string errorMessage = testCase.Name + "_" + to_string(i);
        const char* const message = errorMessage.c_str();

        Assert::AreEqual(testCase.Expected, result, message);
    }
}