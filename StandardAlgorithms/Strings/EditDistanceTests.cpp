#include <string>
#include <vector>
#include "EditDistance.h"
#include "../Utilities/VectorUtilities.h"
#include "../Assert.h"
#include "../test_utilities.h"
#include "EditDistanceTests.h"

using namespace std;
using namespace Privet::Algorithms::Strings::Tests;
using namespace Privet::Algorithms::Strings;
using namespace Privet::Algorithms::Tests;
using namespace Privet::Algorithms;

namespace
{
    struct TestCase final : base_test_case
    {
        size_t AddCost = 0;
        size_t ChangeCost = 0;
        size_t DeleteCost = 0;

        size_t Expected = 0;

        string Source;
        string Target;

        //TODO: p2. add another data members.
        TestCase(string&& name)
            : base_test_case(forward<string>(name)),
            Source(), Target()
        {
        }

        void Print(ostream& str) const override
        {
            base_test_case::Print(str);

            str << ", AddCost=" << AddCost
                << ", ChangeCost=" << ChangeCost
                << ", DeleteCost=" << DeleteCost
                << ", Source=" << Source
                << ", Target=" << Target
                << ", Expected=" << Expected;
        }
    };

    void GenerateTestCases(vector<TestCase>& testCases)
    {
        {
            TestCase testCase("Simple");
            testCase.AddCost = 1;
            testCase.ChangeCost = 2;
            testCase.DeleteCost = 4;

            // "a bc  fNM";
            // "a cdf fNM";
            testCase.Source = "abcfNM";
            testCase.Target = "acdffNM";
            testCase.Expected = testCase.ChangeCost * 2 + testCase.AddCost;

            testCases.push_back(testCase);
        }
        {
            TestCase testCase("Same");
            testCase.AddCost = 1;
            testCase.ChangeCost = 2;
            testCase.DeleteCost = 4;

            testCase.Source = "abcf";
            testCase.Target = "abcf";
            testCase.Expected = 0;

            testCases.push_back(testCase);
        }
        {
            TestCase testCase("Delete");
            testCase.AddCost = 1;
            testCase.ChangeCost = 2;
            testCase.DeleteCost = 4;

            testCase.Source = "abcf";
            testCase.Target = "abf";
            testCase.Expected = testCase.DeleteCost;

            testCases.push_back(testCase);
        }
        {
            TestCase testCase("Delete2");
            testCase.AddCost = 1;
            testCase.ChangeCost = 2;
            testCase.DeleteCost = 4;

            // "ab c eee c f";
            // "ab   eee   f";
            testCase.Source = "abceeecf";
            testCase.Target = "abeeef";

            testCase.Expected = testCase.DeleteCost * 2;

            testCases.push_back(testCase);
        }
        {
            TestCase testCase("Delete is too expensive.");
            testCase.AddCost = 1;
            testCase.ChangeCost = 2;
            testCase.DeleteCost = 4;

            // "sd   f s mj dhh";
            // "lj e f a mj    ";//1 add, 3 deletes, 3 changes = 1 + 12 + 6 = 19.
            //vs
            // "sdfsmjd hh";
            // "ljefamj   ";//2 deletes, 7 changes = 8 + 14 = 22.
            //vs
            // "  sd f s mj dhh";
            // "l je f a mj   ";//1 add, 3 deletes, 3 changes = 1 + 12 + 6 = 19.
            testCase.Source = "AK sdfsmjdhh FSD";
            testCase.Target = "AK ljefamj FSD";
            testCase.Expected = testCase.AddCost + 3 * testCase.ChangeCost + testCase.DeleteCost * 3;

            testCases.push_back(testCase);
        } {//It must follow the previous case.
            const TestCase& previousTestCase = VectorUtilities::Last(testCases);

            TestCase testCase = previousTestCase;
            testCase.set_Name("All same cost.");
            testCase.ChangeCost = 1;
            testCase.DeleteCost = 1;

            // "sd   f s mj dhh";
            // "lj e f a mj    ";//1 add, 3 deletes, 3 changes = 7.
            //vs
            // "sdfsmjd hh";
            // "ljefamj   ";//2 deletes, 7 changes = 9.
            //vs
            // "  sd f s mj dhh";
            // "l je f a mj   ";//1 add, 3 deletes, 3 changes = 7.
            testCase.Expected = testCase.AddCost + 3 * testCase.ChangeCost + testCase.DeleteCost * 3;

            testCases.push_back(testCase);
        } {//It must follow the previous case.
            const TestCase& previousTestCase = VectorUtilities::Last(testCases);

            TestCase testCase = previousTestCase;
            testCase.set_Name("Delete, Add are expensive.");
            testCase.AddCost = 10;
            testCase.ChangeCost = 1;
            testCase.DeleteCost = testCase.AddCost;

            //2 deletes, 7 changes.
            testCase.Expected = 7 * testCase.ChangeCost + testCase.DeleteCost * 2;

            testCases.push_back(testCase);
        }
        {
            TestCase testCase("Add");
            testCase.AddCost = 1;
            testCase.ChangeCost = 2;
            testCase.DeleteCost = 4;

            testCase.Source = "acf";
            testCase.Target = "abcf";
            testCase.Expected = testCase.AddCost;

            testCases.push_back(testCase);
        }
        {
            TestCase testCase("Simple2");
            testCase.AddCost = 1;
            testCase.ChangeCost = 2;
            testCase.DeleteCost = 4;

            // "sdh f s kj dh";
            // "lje f a kj   ";
            testCase.Source = "AK_sdhfskjdh FSD";
            testCase.Target = "AK_ljefakj FSD";
            testCase.Expected = (4 * testCase.ChangeCost) + testCase.DeleteCost * 2;

            testCases.push_back(testCase);
        } {//It must follow the previous case.
            const TestCase& previousTestCase = VectorUtilities::Last(testCases);

            TestCase testCase = previousTestCase;
            testCase.set_Name("Simple2_Reversed");

            testCase.Source = previousTestCase.Target;
            testCase.Target = previousTestCase.Source;
            testCase.Expected = (4 * testCase.ChangeCost) + testCase.AddCost * 2;

            testCases.push_back(testCase);
        }
    }

    void RunTestCase(const TestCase& testCase)
    {
        const auto actual = EditDistance<>(
            testCase.Source, testCase.Target,
            testCase.AddCost, testCase.ChangeCost, testCase.DeleteCost);

        Assert::AreEqual(testCase.Expected, actual, testCase.get_Name());
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
                void EditDistanceTests()
                {
                    test_utilities<TestCase>::run_tests(RunTestCase, GenerateTestCases);
                }
            }
        }
    }
}